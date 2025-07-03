#include "hzpch.h"
#include "D3D12Shader.h"
#include "Runtime/Core/Log/Log.h"
#include "D3D12Utils.h"

namespace Hazel
{
	// D3D12ShaderReflection 实现

	D3D12ShaderReflection::D3D12ShaderReflection()
	{
		// 初始化所有Stage数据为空
		for (auto& stageRes : m_StageResources) {
			stageRes = nullptr;
		}
	}

	D3D12ShaderReflection::~D3D12ShaderReflection()
	{
		Clear();
	}

	void D3D12ShaderReflection::AddStageReflection(ShaderStage stage, ID3DBlob* shaderBytecode)
	{
		if (!shaderBytecode) {
			HZ_CORE_WARN("D3D12ShaderReflection: 尝试添加空的着色器字节码到Stage {0}", ShaderStageToString(stage));
			return;
		}

		size_t stageIndex = static_cast<size_t>(stage);
		if (stageIndex >= static_cast<size_t>(ShaderStage::Count)) {
			HZ_CORE_ERROR("D3D12ShaderReflection: 无效的Shader Stage");
			return;
		}

		// 清除该Stage之前的数据
		m_StageResources[stageIndex] = nullptr;
		m_D3DReflections[stageIndex] = nullptr;

		// 创建D3D12反射接口
		ComPtr<ID3D12ShaderReflection> d3dReflection;
		HRESULT hr = D3DReflect(
			shaderBytecode->GetBufferPointer(),
			shaderBytecode->GetBufferSize(),
			IID_PPV_ARGS(&d3dReflection)
		);

		if (FAILED(hr)) {
			HZ_CORE_ERROR("D3D12ShaderReflection: 创建{0} Stage反射失败", ShaderStageToString(stage));
			return;
		}

		// 存储D3D反射接口
		m_D3DReflections[stageIndex] = d3dReflection;

		// 创建Stage资源信息
		m_StageResources[stageIndex] = std::make_unique<StageResourceInfo>(
			CreateStageResourceInfo(stage, d3dReflection.Get())
		);

		HZ_CORE_INFO("D3D12ShaderReflection: 成功添加{0} Stage反射", ShaderStageToString(stage));
	}

	void D3D12ShaderReflection::Clear()
	{
		for (auto& stageRes : m_StageResources) {
			stageRes = nullptr;
		}
		for (auto& d3dRef : m_D3DReflections) {
			d3dRef = nullptr;
		}
	}

	ComPtr<ID3D12ShaderReflection> D3D12ShaderReflection::GetD3DReflection(ShaderStage stage) const
	{
		size_t stageIndex = static_cast<size_t>(stage);
		if (stageIndex < static_cast<size_t>(ShaderStage::Count)) {
			return m_D3DReflections[stageIndex];
		}
		return nullptr;
	}

	// === Stage-based 接口实现 ===

	std::vector<StageResourceInfo> D3D12ShaderReflection::ReflectStageResources()
	{
		std::vector<StageResourceInfo> result;
		for (const auto& stageRes : m_StageResources) {
			if (stageRes) {
				result.push_back(*stageRes);
			}
		}
		return result;
	}

	StageResourceInfo* D3D12ShaderReflection::GetStageResources(ShaderStage stage) const
	{
		size_t stageIndex = static_cast<size_t>(stage);
		if (stageIndex < static_cast<size_t>(ShaderStage::Count) && m_StageResources[stageIndex]) {
			return m_StageResources[stageIndex].get();
		}
		return nullptr;
	}

	bool D3D12ShaderReflection::HasStage(ShaderStage stage) const
	{
		size_t stageIndex = static_cast<size_t>(stage);
		return stageIndex < static_cast<size_t>(ShaderStage::Count) && 
			   m_StageResources[stageIndex] != nullptr;
	}

	std::vector<ShaderStage> D3D12ShaderReflection::GetAvailableStages() const
	{
		std::vector<ShaderStage> stages;
		for (size_t i = 0; i < static_cast<size_t>(ShaderStage::Count); ++i) {
			if (m_StageResources[i]) {
				stages.push_back(static_cast<ShaderStage>(i));
			}
		}
		return stages;
	}

	// === Stage-specific 查询接口 ===

	Ref<ShaderRegisterBlock> D3D12ShaderReflection::GetRegisterBlockByName(ShaderStage stage, const std::string& name)
	{
		StageResourceInfo* stageInfo = GetStageResources(stage);
		if (!stageInfo) return nullptr;

		const auto* block = stageInfo->GetRegisterBlock(name);
		if (block) {
			return CreateRef<ShaderRegisterBlock>(*block);
		}
		return nullptr;
	}

	Ref<ShaderRegisterBlock> D3D12ShaderReflection::GetRegisterBlockByBindPoint(ShaderStage stage, uint32_t bindPoint, uint32_t space)
	{
		StageResourceInfo* stageInfo = GetStageResources(stage);
		if (!stageInfo) return nullptr;

		const auto* block = stageInfo->GetRegisterBlock(bindPoint, space);
		if (block) {
			return CreateRef<ShaderRegisterBlock>(*block);
		}
		return nullptr;
	}

	Ref<ShaderParameter> D3D12ShaderReflection::GetParameterByName(ShaderStage stage, const std::string& name)
	{
		StageResourceInfo* stageInfo = GetStageResources(stage);
		if (!stageInfo) return nullptr;

		const auto* param = stageInfo->GetParameter(name);
		if (param) {
			return CreateRef<ShaderParameter>(*param);
		}
		return nullptr;
	}

	// === InputLayout 接口实现（只属于顶点着色器） ===

	BufferLayout D3D12ShaderReflection::ReflectVertexInputLayout()
	{
		std::vector<BufferElement> elements;
		
		// InputLayout 只属于顶点着色器
		if (HasStage(ShaderStage::Vertex)) {
			ComPtr<ID3D12ShaderReflection> vsReflection = GetD3DReflection(ShaderStage::Vertex);
			if (vsReflection) {
				D3D12_SHADER_DESC shaderDesc;
				vsReflection->GetDesc(&shaderDesc);

				for (UINT i = 0; i < shaderDesc.InputParameters; ++i) {
					D3D12_SIGNATURE_PARAMETER_DESC paramDesc;
					vsReflection->GetInputParameterDesc(i, &paramDesc);

					BufferElement element;
					element.Name = paramDesc.SemanticName;
					element.CoordIndex = paramDesc.SemanticIndex;

					// 根据ComponentType和Mask确定类型
					if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) {
						UINT componentCount = 0;
						if (paramDesc.Mask & 0x1) componentCount++;
						if (paramDesc.Mask & 0x2) componentCount++;
						if (paramDesc.Mask & 0x4) componentCount++;
						if (paramDesc.Mask & 0x8) componentCount++;

						switch (componentCount) {
							case 1: element.Type = ShaderDataType::Float; break;
							case 2: element.Type = ShaderDataType::Float2; break;
							case 3: element.Type = ShaderDataType::Float3; break;
							case 4: element.Type = ShaderDataType::Float4; break;
						}
					}

					elements.push_back(element);
				}
			}
		}
		
		return BufferLayout(elements);
	}





	// === 私有辅助方法 ===

	StageResourceInfo D3D12ShaderReflection::CreateStageResourceInfo(ShaderStage stage, ID3D12ShaderReflection* reflection)
	{
		StageResourceInfo stageInfo;
		stageInfo.stage = stage;

		D3D12_SHADER_DESC shaderDesc;
		reflection->GetDesc(&shaderDesc);

		// 反射常量缓冲区
		for (UINT i = 0; i < shaderDesc.ConstantBuffers; ++i) {
			ID3D12ShaderReflectionConstantBuffer* cbuffer = reflection->GetConstantBufferByIndex(i);
			if (!cbuffer) continue;

			D3D12_SHADER_BUFFER_DESC bufferDesc;
			cbuffer->GetDesc(&bufferDesc);

			ShaderRegisterBlock registerBlock;
			registerBlock.Name = bufferDesc.Name;
			registerBlock.Size = bufferDesc.Size;
			registerBlock.Stage = stage;

			// 获取绑定信息
			D3D12_SHADER_INPUT_BIND_DESC bindDesc;
			if (SUCCEEDED(reflection->GetResourceBindingDescByName(bufferDesc.Name, &bindDesc))) {
				registerBlock.BindPoint = bindDesc.BindPoint;
				registerBlock.BindSpace = bindDesc.Space;
			}

			// 反射参数
			for (UINT j = 0; j < bufferDesc.Variables; ++j) {
				ID3D12ShaderReflectionVariable* variable = cbuffer->GetVariableByIndex(j);
				if (!variable) continue;

				D3D12_SHADER_VARIABLE_DESC varDesc;
				variable->GetDesc(&varDesc);

				ShaderParameter param;
				param.Name = varDesc.Name;
				param.Size = varDesc.Size;
				param.Offset = varDesc.StartOffset;

				registerBlock.Parameters.push_back(param);
				
				// 添加到Stage参数映射 (格式: "BlockName.ParamName")
				std::string fullName = registerBlock.Name + "." + param.Name;
				stageInfo.parameters[fullName] = param;
			}

			stageInfo.registerBlocks.push_back(registerBlock);
		}

		// 反射所有资源绑定
		for (UINT i = 0; i < shaderDesc.BoundResources; ++i) {
			D3D12_SHADER_INPUT_BIND_DESC bindDesc;
			reflection->GetResourceBindingDesc(i, &bindDesc);

			ResourceBinding binding;
			binding.Name = bindDesc.Name;
			binding.BindPoint = bindDesc.BindPoint;
			binding.BindSpace = bindDesc.Space;
			binding.Stage = stage;

			// 确定资源类型
			switch (bindDesc.Type) {
				case D3D_SIT_CBUFFER:
					binding.Type = ResourceType::ConstantBuffer;
					// 查找对应的寄存器块索引
					for (size_t j = 0; j < stageInfo.registerBlocks.size(); ++j) {
						if (stageInfo.registerBlocks[j].Name == binding.Name) {
							binding.RegisterBlockIndex = static_cast<int>(j);
							break;
						}
					}
					break;
				case D3D_SIT_TEXTURE:
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
					continue; // 跳过未知类型
			}

			stageInfo.resourceBindings.push_back(binding);
		}

		return stageInfo;
	}





	// === D3D12Shader 实现 ===

	D3D12Shader::D3D12Shader(const std::string& filepath)
	{
		std::wstring wstr(filepath.begin(), filepath.end());
		m_VsByteCode = d3dUtil::CompileShader(wstr, nullptr, "VS", "vs_5_0");
		m_PsByteCode = d3dUtil::CompileShader(wstr, nullptr, "PS", "ps_5_0");

		// 创建反射
		CreateReflection();
	}

	D3D12Shader::D3D12Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_Name(name)
	{
		// 编译着色器
		Compile(vertexSrc, "VS", "vs_5_0", m_VsByteCode);
		Compile(fragmentSrc, "PS", "ps_5_0", m_PsByteCode);

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
		// 创建新的Stage-based反射系统
		auto stageReflection = CreateRef<D3D12ShaderReflection>();
		m_Reflection = stageReflection;

		// 添加顶点着色器反射
		if (m_VsByteCode) {
			stageReflection->AddStageReflection(ShaderStage::Vertex, m_VsByteCode.Get());
			
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

		// 添加像素着色器反射
		if (m_PsByteCode) {
			stageReflection->AddStageReflection(ShaderStage::Pixel, m_PsByteCode.Get());
		}

		HZ_CORE_INFO("D3D12Shader: Shader反射创建完成，包含{0}个Stage", 
			stageReflection->GetAvailableStages().size());
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