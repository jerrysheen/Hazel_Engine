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

	std::vector<ShaderParameter> D3D12ShaderReflection::ReflectParameters()
	{
		// 如果已经反射过，则直接返回缓存的结果
		if (m_HasReflectedParameters)
			return m_Parameters;

		// 获取着色器描述
		D3D12_SHADER_DESC shaderDesc;
		m_Reflection->GetDesc(&shaderDesc);

		// 清空参数列表
		m_Parameters.clear();

		// 遍历常量缓冲区
		for (UINT i = 0; i < shaderDesc.ConstantBuffers; i++)
		{
			ID3D12ShaderReflectionConstantBuffer* cbReflection = m_Reflection->GetConstantBufferByIndex(i);
			D3D12_SHADER_BUFFER_DESC cbDesc;
			cbReflection->GetDesc(&cbDesc);

			// 遍历常量缓冲区中的变量
			for (UINT j = 0; j < cbDesc.Variables; j++)
			{
				ID3D12ShaderReflectionVariable* varReflection = cbReflection->GetVariableByIndex(j);
				D3D12_SHADER_VARIABLE_DESC varDesc;
				varReflection->GetDesc(&varDesc);

				ShaderParameter param;
				param.Name = varDesc.Name;
				param.Size = varDesc.Size;
				param.Offset = varDesc.StartOffset;
				// 注意：这里的BindPoint可能需要根据绑定点类型进行转换
				param.BindPoint = 0; // 需要根据实际情况设置

				m_Parameters.push_back(param);
			}
		}

		m_HasReflectedParameters = true;
		return m_Parameters;
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

		// 遍历绑定资源
		for (UINT i = 0; i < shaderDesc.BoundResources; i++)
		{
			D3D12_SHADER_INPUT_BIND_DESC bindDesc;
			m_Reflection->GetResourceBindingDesc(i, &bindDesc);

			ResourceBinding binding;
			binding.Name = bindDesc.Name;
			binding.BindPoint = bindDesc.BindPoint;
			binding.BindSpace = bindDesc.Space;

			m_ResourceBindings.push_back(binding);
		}

		m_HasReflectedResourceBindings = true;
		return m_ResourceBindings;
	}

	// D3D12Shader实现
	D3D12Shader::D3D12Shader(const std::string& filepath)
	{
		std::wstring wstr(filepath.begin(), filepath.end());  // ֱ��ʹ�ù��캯��
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
			// 创建着色器反射
			m_Reflection = CreateRef<D3D12ShaderReflection>(m_VsByteCode.Get());
			
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