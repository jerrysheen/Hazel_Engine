#include "hzpch.h"
#include "D3D12Shader.h"
#include "d3dUtil.h"
#include "D3D12RenderAPIManager.h"
#include "d3dx12.h"

namespace Hazel
{
	// D3D12ShaderReflection实现
	D3D12ShaderReflection::D3D12ShaderReflection(ID3DBlob* shaderBytecode)
	{
		// 使用D3D编译器的反射API创建反射接口
		HRESULT hr = D3DReflect(
			shaderBytecode->GetBufferPointer(),
			shaderBytecode->GetBufferSize(),
			IID_PPV_ARGS(&m_Reflection)
		);

		if (FAILED(hr))
		{
			HZ_CORE_ERROR("D3D12ShaderReflection: Failed to create reflection interface");
		}
	}

	D3D12ShaderReflection::~D3D12ShaderReflection()
	{
	}

	BufferLayout D3D12ShaderReflection::ReflectVertexInputLayout()
	{
		// 如果已经反射过，则直接返回缓存的结果
		if (m_HasReflectedInputLayout)
			return m_InputLayout;

		// 获取着色器描述
		D3D12_SHADER_DESC shaderDesc;
		m_Reflection->GetDesc(&shaderDesc);

		// 用于构建BufferLayout的元素
		std::vector<BufferElement> elements;

		// 遍历输入参数
		for (UINT i = 0; i < shaderDesc.InputParameters; i++)
		{
			D3D12_SIGNATURE_PARAMETER_DESC paramDesc;
			m_Reflection->GetInputParameterDesc(i, &paramDesc);

			// 根据语义名确定ShaderDataType
			ShaderDataType type = ShaderDataType::None;
			if (strcmp(paramDesc.SemanticName, "POSITION") == 0)
			{
				type = ShaderDataType::Float3;
			}
			else if (strcmp(paramDesc.SemanticName, "NORMAL") == 0)
			{
				type = ShaderDataType::Float3;
			}
			else if (strcmp(paramDesc.SemanticName, "TEXCOORD") == 0)
			{
				type = ShaderDataType::Float2;
			}
			else if (strcmp(paramDesc.SemanticName, "COLOR") == 0)
			{
				type = ShaderDataType::Float4;
			}
			else if (strcmp(paramDesc.SemanticName, "TANGENT") == 0)
			{
				type = ShaderDataType::Float3;
			}
			else if (strcmp(paramDesc.SemanticName, "BINORMAL") == 0 || strcmp(paramDesc.SemanticName, "BITANGENT") == 0)
			{
				type = ShaderDataType::Float3;
			}
			// 可以根据需要添加更多语义类型的处理

			// 如果识别出类型，则添加到布局中
			if (type != ShaderDataType::None)
			{
				elements.push_back({ type, paramDesc.SemanticName, paramDesc.SemanticIndex });
			}
		}

		m_InputLayout = BufferLayout(elements);
		m_HasReflectedInputLayout = true;
		return m_InputLayout;
	}

	std::vector<ShaderRegisterBlock> D3D12ShaderReflection::ReflectRegisterBlocks()
	{
		// 如果已经反射过，则直接返回缓存的结果
		if (m_HasReflectedRegisterBlocks)
			return m_RegisterBlocks;

		// 获取着色器描述
		D3D12_SHADER_DESC shaderDesc;
		m_Reflection->GetDesc(&shaderDesc);

		// 清空寄存器块列表和映射
		m_RegisterBlocks.clear();
		m_RegisterBlockNameToIndex.clear();
		m_RegisterBlockBindPointToIndex.clear();

		// 用于存储资源绑定描述
		std::vector<D3D12_SHADER_INPUT_BIND_DESC> cbvBindDescs;

		// 第一步: 收集所有CBV资源绑定(这些是register(bX)绑定)
		for (UINT i = 0; i < shaderDesc.BoundResources; i++)
		{
			D3D12_SHADER_INPUT_BIND_DESC bindDesc;
			m_Reflection->GetResourceBindingDesc(i, &bindDesc);

			// 只处理常量缓冲区视图(cbuffer)
			if (bindDesc.Type == D3D_SIT_CBUFFER)
			{
				cbvBindDescs.push_back(bindDesc);
			}
		}

		// 第二步: 遍历所有已发现的常量缓冲区，创建寄存器块
		for (const auto& bindDesc : cbvBindDescs)
		{
			// 1. 获取常量缓冲区反射
			ID3D12ShaderReflectionConstantBuffer* cbReflection = 
				m_Reflection->GetConstantBufferByName(bindDesc.Name);
			
			// 2. 获取常量缓冲区描述
			D3D12_SHADER_BUFFER_DESC cbDesc;
			cbReflection->GetDesc(&cbDesc);

			// 3. 创建寄存器块
			ShaderRegisterBlock block;
			block.Name = bindDesc.Name;
			block.BindPoint = bindDesc.BindPoint;
			block.BindSpace = bindDesc.Space;
			block.Size = cbDesc.Size;

			// 4. 遍历常量缓冲区中的变量，提取参数
			for (UINT j = 0; j < cbDesc.Variables; j++)
			{
				ID3D12ShaderReflectionVariable* varReflection = cbReflection->GetVariableByIndex(j);
				D3D12_SHADER_VARIABLE_DESC varDesc;
				varReflection->GetDesc(&varDesc);

				// 5. 创建参数
				ShaderParameter param;
				param.Name = varDesc.Name;
				param.Size = varDesc.Size;
				param.Offset = varDesc.StartOffset;

				// 6. 添加参数到寄存器块
				block.Parameters.push_back(param);

				// 7. 缓存参数到映射表中
				m_ParameterCache[block.Name + "." + param.Name] = param;
			}

			// 8. 添加寄存器块到列表
			size_t index = m_RegisterBlocks.size();
			m_RegisterBlocks.push_back(block);

			// 9. 更新映射关系
			m_RegisterBlockNameToIndex[block.Name] = index;
			
			// 创建一个64位键，包含绑定点和空间
			uint64_t bindKey = ((uint64_t)block.BindSpace << 32) | block.BindPoint;
			m_RegisterBlockBindPointToIndex[bindKey] = index;
		}

		m_HasReflectedRegisterBlocks = true;
		return m_RegisterBlocks;
	}

	std::vector<ResourceBinding> D3D12ShaderReflection::ReflectResourceBindings()
	{
		// 如果已经反射过，则直接返回缓存的结果
		if (m_HasReflectedResourceBindings)
			return m_ResourceBindings;

		// 获取着色器描述
		D3D12_SHADER_DESC shaderDesc;
		m_Reflection->GetDesc(&shaderDesc);

		// 清空资源绑定列表
		m_ResourceBindings.clear();

		// 确保寄存器块已反射
		auto registerBlocks = ReflectRegisterBlocks();

		// 遍历绑定资源
		for (UINT i = 0; i < shaderDesc.BoundResources; i++)
		{
			D3D12_SHADER_INPUT_BIND_DESC bindDesc;
			m_Reflection->GetResourceBindingDesc(i, &bindDesc);

			ResourceBinding binding;
			binding.Name = bindDesc.Name;
			binding.BindPoint = bindDesc.BindPoint;
			binding.BindSpace = bindDesc.Space;

			// 设置资源类型
			switch (bindDesc.Type)
			{
			case D3D_SIT_CBUFFER:
				binding.Type = ResourceType::ConstantBuffer;
				// 查找对应的寄存器块
				if (m_RegisterBlockNameToIndex.find(bindDesc.Name) != m_RegisterBlockNameToIndex.end())
				{
					binding.RegisterBlockIndex = (int)m_RegisterBlockNameToIndex[bindDesc.Name];
				}
				break;
			case D3D_SIT_TEXTURE:
			case D3D_SIT_STRUCTURED:
			case D3D_SIT_BYTEADDRESS:
				binding.Type = ResourceType::ShaderResource;
				break;
			case D3D_SIT_UAV_RWTYPED:
			case D3D_SIT_UAV_RWSTRUCTURED:
			case D3D_SIT_UAV_RWBYTEADDRESS:
			case D3D_SIT_UAV_APPEND_STRUCTURED:
			case D3D_SIT_UAV_CONSUME_STRUCTURED:
			case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
				binding.Type = ResourceType::UnorderedAccess;
				break;
			case D3D_SIT_SAMPLER:
				binding.Type = ResourceType::Sampler;
				break;
			default:
				// 未处理的类型
				break;
			}

			m_ResourceBindings.push_back(binding);
		}

		m_HasReflectedResourceBindings = true;
		return m_ResourceBindings;
	}

	void D3D12ShaderReflection::MergeReflection(const Ref<D3D12ShaderReflection>& other)
	{
		// 获取其他反射的寄存器块和资源绑定
		auto otherRegisterBlocks = other->ReflectRegisterBlocks();
		auto otherResourceBindings = other->ReflectResourceBindings();

		// 确保当前反射的数据已加载
		auto currentRegisterBlocks = ReflectRegisterBlocks();
		auto currentResourceBindings = ReflectResourceBindings();

		// 合并寄存器块
		for (const auto& otherBlock : otherRegisterBlocks)
		{
			// 检查是否已存在同名的寄存器块
			auto it = m_RegisterBlockNameToIndex.find(otherBlock.Name);
			if (it == m_RegisterBlockNameToIndex.end())
			{
				// 不存在，添加这个寄存器块
				size_t index = m_RegisterBlocks.size();
				m_RegisterBlocks.push_back(otherBlock);
				m_RegisterBlockNameToIndex[otherBlock.Name] = index;
				
				// 更新绑定点索引
				uint64_t bindKey = ((uint64_t)otherBlock.BindSpace << 32) | otherBlock.BindPoint;
				m_RegisterBlockBindPointToIndex[bindKey] = index;

				// 添加参数到映射表
				for (const auto& param : otherBlock.Parameters)
				{
					m_ParameterCache[otherBlock.Name + "." + param.Name] = param;
				}
			}
			else
			{
				// 已存在同名寄存器块，合并参数（如果有新的）
				size_t index = it->second;
				auto& existingBlock = m_RegisterBlocks[index];
				
				// 遍历其他块的参数
				for (const auto& otherParam : otherBlock.Parameters)
				{
					// 检查参数是否已存在
					bool exists = false;
					for (const auto& existingParam : existingBlock.Parameters)
					{
						if (existingParam.Name == otherParam.Name)
						{
							exists = true;
							break;
						}
					}
					
					// 如果不存在，添加参数
					if (!exists)
					{
						existingBlock.Parameters.push_back(otherParam);
						m_ParameterCache[otherBlock.Name + "." + otherParam.Name] = otherParam;
					}
				}
			}
		}

		// 合并资源绑定
		for (const auto& otherBinding : otherResourceBindings)
		{
			// 检查是否已存在相同的资源绑定
			bool exists = false;
			for (const auto& existingBinding : m_ResourceBindings)
			{
				if (existingBinding.Name == otherBinding.Name && 
					existingBinding.BindPoint == otherBinding.BindPoint &&
					existingBinding.BindSpace == otherBinding.BindSpace)
				{
					exists = true;
					break;
				}
			}
			
			// 如果不存在，添加资源绑定
			if (!exists)
			{
				ResourceBinding binding = otherBinding;
				
				// 如果是常量缓冲区类型，更新RegisterBlockIndex
				if (binding.Type == ResourceType::ConstantBuffer)
				{
					auto it = m_RegisterBlockNameToIndex.find(binding.Name);
					if (it != m_RegisterBlockNameToIndex.end())
					{
						binding.RegisterBlockIndex = (int)it->second;
					}
				}
				
				m_ResourceBindings.push_back(binding);
			}
		}
	}

	Ref<ShaderRegisterBlock> D3D12ShaderReflection::GetRegisterBlockByName(const std::string& name)
	{
		// 确保寄存器块已反射
		if (!m_HasReflectedRegisterBlocks)
			ReflectRegisterBlocks();

		// 查找名称
		auto it = m_RegisterBlockNameToIndex.find(name);
		if (it != m_RegisterBlockNameToIndex.end())
		{
			// 创建一个新的ShaderRegisterBlock并复制内容
			Ref<ShaderRegisterBlock> block = CreateRef<ShaderRegisterBlock>();
			*block = m_RegisterBlocks[it->second]; // 复制内容
			return block;
		}
		
		return nullptr;
	}

	Ref<ShaderRegisterBlock> D3D12ShaderReflection::GetRegisterBlockByBindPoint(uint32_t bindPoint, uint32_t space)
	{
		// 确保寄存器块已反射
		if (!m_HasReflectedRegisterBlocks)
			ReflectRegisterBlocks();

		// 创建绑定键
		uint64_t bindKey = ((uint64_t)space << 32) | bindPoint;
		
		// 查找绑定点
		auto it = m_RegisterBlockBindPointToIndex.find(bindKey);
		if (it != m_RegisterBlockBindPointToIndex.end())
		{
			// 创建一个新的ShaderRegisterBlock并复制内容
			Ref<ShaderRegisterBlock> block = CreateRef<ShaderRegisterBlock>();
			*block = m_RegisterBlocks[it->second]; // 复制内容
			return block;
		}
		
		return nullptr;
	}

	Ref<ShaderParameter> D3D12ShaderReflection::GetParameterByName(const std::string& name)
	{
		// 确保寄存器块已反射
		if (!m_HasReflectedRegisterBlocks)
			ReflectRegisterBlocks();

		// 查找参数名称
		auto it = m_ParameterCache.find(name);
		if (it != m_ParameterCache.end())
		{
			// 创建一个新的ShaderParameter并复制内容
			Ref<ShaderParameter> param = CreateRef<ShaderParameter>();
			*param = it->second; // 复制内容
			return param;
		}
		
		return nullptr;
	}

	// D3D12Shader实现
	D3D12Shader::D3D12Shader(const std::string& filepath)
	{
		std::wstring wstr(filepath.begin(), filepath.end());
		m_VsByteCode = d3dUtil::CompileShader(wstr, nullptr, "VS", "vs_5_0");
		m_PsByteCode = d3dUtil::CompileShader(wstr, nullptr, "PS", "ps_5_0");

		// 创建反射
		CreateReflection();
	}

	D3D12Shader::D3D12Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		// 实现源码编译逻辑...
		// 这里保留原来的实现，只添加CreateReflection调用
		
		// 创建反射
		CreateReflection();
	}

	D3D12Shader::~D3D12Shader()
	{
	}

	void D3D12Shader::Bind() const
	{
	}

	void D3D12Shader::UnBind() const
	{
	}

	void D3D12Shader::SetInt(const std::string& name, int value)
	{
	}

	void D3D12Shader::SetFloat(const std::string& name, float value)
	{
	}

	void D3D12Shader::SetFloat2(const std::string& name, const glm::vec2& value)
	{
	}

	void D3D12Shader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
	}

	void D3D12Shader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
	}

	void D3D12Shader::SetMat4(const std::string& name, const glm::mat4& value)
	{
	}

	void D3D12Shader::SetMat3(const std::string& name, const glm::mat3& value)
	{
	}

	void D3D12Shader::Compile(const std::string& source, const std::string& entryPoint, const std::string& target, ComPtr<ID3DBlob>& outByteCode)
	{
		// 编译着色器
		UINT compileFlags = 0;
#if defined(HZ_DEBUG)
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		ComPtr<ID3DBlob> errors;
		HRESULT hr = D3DCompile(
			source.c_str(),
			source.length(),
			nullptr,
			nullptr,
			nullptr,
			entryPoint.c_str(),
			target.c_str(),
			compileFlags,
			0,
			&outByteCode,
			&errors
		);

		if (FAILED(hr))
		{
			if (errors)
			{
				HZ_CORE_ERROR("D3D12Shader: {0}", (char*)errors->GetBufferPointer());
			}
			HZ_CORE_ASSERT(false, "Shader compilation failed!");
		}
	}

	void D3D12Shader::CreateReflection()
	{
		// 确保顶点着色器字节码存在
		if (m_VsByteCode)
		{
			// 创建顶点着色器反射
			auto vsReflection = CreateRef<D3D12ShaderReflection>(m_VsByteCode.Get());
			m_Reflection = vsReflection;
			
			// 获取输入布局
			m_InputLayout = m_Reflection->ReflectVertexInputLayout();
			
			// 创建D3D12输入布局描述
			m_D3D12InputLayout.clear();
			
			int count = 0;
			// 将BufferLayout转换为D3D12_INPUT_ELEMENT_DESC
			for (const auto& element : m_InputLayout)
			{
				D3D12_INPUT_ELEMENT_DESC inputElement = {};
				inputElement.SemanticName = element.Name.c_str();
				inputElement.SemanticIndex = element.CoordIndex;
				inputElement.Format = GetDXGIFormat(element.Type);
				inputElement.InputSlot = count;
				inputElement.AlignedByteOffset = 0;
				inputElement.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
				inputElement.InstanceDataStepRate = 0;
				
				m_D3D12InputLayout.push_back(inputElement);
				count++;
			}
		}
		
		// 如果像素着色器字节码存在且与顶点着色器不同，需要合并反射结果
		if (m_PsByteCode && (m_VsByteCode != m_PsByteCode))
		{
			// 创建像素着色器反射
			auto psReflection = CreateRef<D3D12ShaderReflection>(m_PsByteCode.Get());
			
			// 如果没有顶点着色器反射，直接使用像素着色器反射
			if (!m_Reflection)
			{
				m_Reflection = psReflection;
				return;
			}
			
			// 合并VS和PS的反射信息
			auto vsReflection = std::dynamic_pointer_cast<D3D12ShaderReflection>(m_Reflection);
			if (vsReflection)
			{
				// 将PS反射合并到VS反射中
				vsReflection->MergeReflection(psReflection);
			}
		}
	}

	// 辅助函数：将ShaderDataType转换为DXGI_FORMAT
	DXGI_FORMAT D3D12Shader::GetDXGIFormat(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return DXGI_FORMAT_R32_FLOAT;
		case ShaderDataType::Float2:   return DXGI_FORMAT_R32G32_FLOAT;
		case ShaderDataType::Float3:   return DXGI_FORMAT_R32G32B32_FLOAT;
		case ShaderDataType::Float4:   return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case ShaderDataType::Int:      return DXGI_FORMAT_R32_SINT;
		case ShaderDataType::Int2:     return DXGI_FORMAT_R32G32_SINT;
		case ShaderDataType::Int3:     return DXGI_FORMAT_R32G32B32_SINT;
		case ShaderDataType::Int4:     return DXGI_FORMAT_R32G32B32A32_SINT;
		case ShaderDataType::Bool:     return DXGI_FORMAT_R8_UINT;
		}
		HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return DXGI_FORMAT_UNKNOWN;
	}
}