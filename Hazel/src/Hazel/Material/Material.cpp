#include "hzpch.h"
#include "Material.h"
#include "MaterialSerializer.h"

namespace Hazel 
{
	// Material实现
	Material::Material(const Ref<Shader>& shader)
		: m_Shader(shader)
	{
		// 自动从着色器反射数据同步属性
		if (shader)
			SyncWithShaderReflection();
	}

	Ref<Material> Material::Create(const Ref<Shader>& shader)
	{
		return CreateRef<Material>(shader);
	}

	void Material::Bind() const
	{
		m_Shader->Bind();
		
		// 绑定所有属性到着色器
		for (auto& [name, property] : m_Properties)
		{
			switch (property.GetType())
			{
			case MaterialPropertyType::Float:
				m_Shader->SetFloat(name, property.GetValue<float>());
				break;
			case MaterialPropertyType::Float2:
				m_Shader->SetFloat2(name, property.GetValue<glm::vec2>());
				break;
			case MaterialPropertyType::Float3:
				m_Shader->SetFloat3(name, property.GetValue<glm::vec3>());
				break;
			case MaterialPropertyType::Float4:
				m_Shader->SetFloat4(name, property.GetValue<glm::vec4>());
				break;
			case MaterialPropertyType::Int:
				m_Shader->SetInt(name, property.GetValue<int>());
				break;
			case MaterialPropertyType::Bool:
				m_Shader->SetInt(name, property.GetValue<bool>() ? 1 : 0);
				break;
			case MaterialPropertyType::Matrix4:
				m_Shader->SetMat4(name, property.GetValue<glm::mat4>());
				break;
			case MaterialPropertyType::Texture2D:
			{
				auto texture = property.GetValue<Ref<Texture2D>>();
				if (texture)
				{
					// 这里假设你的Shader类有一个设置纹理的方法
					//m_Shader->SetTexture(name, texture);
				}
				break;
			}
			}
		}
	}

	Ref<Material> Material::Clone() const
	{
		Ref<Material> material = CreateRef<Material>(m_Shader);
		material->m_Properties = m_Properties;
		return material;
	}

	bool Material::HasProperty(const std::string& name) const
	{
		return m_Properties.find(name) != m_Properties.end();
	}

	void Material::SyncToRawData()
	{
		if (!m_Shader)
			return;

		// 获取寄存器块信息
		const auto& registerBlocks = m_Shader->GetReflection()->ReflectRegisterBlocks();
		
		for (const auto& block : registerBlocks)
		{
			// 计算块键值
			uint64_t blockKey = CalculateBlockKey(block.BindPoint, block.BindSpace);
			
			// 检查块是否存在
			if (m_PropertyBlocks.find(blockKey) == m_PropertyBlocks.end())
				continue;
			
			auto& propertyBlock = m_PropertyBlocks[blockKey];
			
			// 遍历块中的所有参数
			for (const auto& param : block.Parameters)
			{
				// 检查属性是否存在
				if (!HasProperty(param.Name))
					continue;
				
				// 获取偏移量
				uint32_t offset = param.Offset / sizeof(float);
				
				// 根据属性类型复制数据
				const auto& property = m_Properties[param.Name];
				
				switch (property.GetType())
				{
				case MaterialPropertyType::Float:
				{
					float value = property.GetValue<float>();
					propertyBlock.RawData[offset] = value;
					break;
				}
				case MaterialPropertyType::Float2:
				{
					const glm::vec2& value = property.GetValue<glm::vec2>();
					propertyBlock.RawData[offset] = value.x;
					propertyBlock.RawData[offset + 1] = value.y;
					break;
				}
				case MaterialPropertyType::Float3:
				{
					const glm::vec3& value = property.GetValue<glm::vec3>();
					propertyBlock.RawData[offset] = value.x;
					propertyBlock.RawData[offset + 1] = value.y;
					propertyBlock.RawData[offset + 2] = value.z;
					break;
				}
				case MaterialPropertyType::Float4:
				{
					const glm::vec4& value = property.GetValue<glm::vec4>();
					propertyBlock.RawData[offset] = value.x;
					propertyBlock.RawData[offset + 1] = value.y;
					propertyBlock.RawData[offset + 2] = value.z;
					propertyBlock.RawData[offset + 3] = value.w;
					break;
				}
				case MaterialPropertyType::Int:
				{
					int value = property.GetValue<int>();
					*reinterpret_cast<int*>(&propertyBlock.RawData[offset]) = value;
					break;
				}
				case MaterialPropertyType::Bool:
				{
					bool value = property.GetValue<bool>();
					propertyBlock.RawData[offset] = value ? 1.0f : 0.0f;
					break;
				}
				case MaterialPropertyType::Matrix4:
				{
					const glm::mat4& value = property.GetValue<glm::mat4>();
					// GLM矩阵是列主序的, 确保按照着色器期望的方式复制
					for (int col = 0; col < 4; col++) {
						for (int row = 0; row < 4; row++) {
							propertyBlock.RawData[offset + col * 4 + row] = value[col][row];
						}
					}
					break;
				}
				case MaterialPropertyType::Texture2D:
				{
					// 纹理引用不需要复制到常量缓冲区，它们会单独绑定
					break;
				}
				default:
					HZ_CORE_WARN("Material: Property type {0} not supported for raw data sync", (int)property.GetType());
					break;
				}
			}
			
			// 设置为已更新
			propertyBlock.Dirty = false;
		}
	}

	const MaterialPropertyBlock* Material::GetPropertyBlock(uint32_t bindPoint, uint32_t bindSpace) const
	{
		uint64_t blockKey = CalculateBlockKey(bindPoint, bindSpace);
		
		auto it = m_PropertyBlocks.find(blockKey);
		if (it != m_PropertyBlocks.end())
			return &(it->second);
		
		return nullptr;
	}

	bool Material::HasPropertyBlock(uint32_t bindPoint, uint32_t bindSpace) const
	{
		uint64_t blockKey = CalculateBlockKey(bindPoint, bindSpace);
		return m_PropertyBlocks.find(blockKey) != m_PropertyBlocks.end();
	}

	// 私有方法实现
	void Material::SyncWithShaderReflection()
	{
		if (!m_Shader)
			return;

		// 清除所有现有属性，以确保只包含着色器中定义的属性
		m_Properties.clear();

		// 从着色器反射数据中获取寄存器块信息
		const auto& registerBlocks = m_Shader->GetReflection()->ReflectRegisterBlocks();
		
		// 创建材质需要的所有属性
		for (const auto& block : registerBlocks)
		{
			for (const auto& param : block.Parameters)
			{
				// 根据反射数据创建属性
				CreatePropertyFromReflection(param.Name, param.Size);
				HZ_CORE_TRACE("Material: Created property '{0}' from shader reflection", param.Name);
			}
		}

		// 创建属性块 - 根据着色器布局优化内存
		CreatePropertyBlocks();
	}

	void Material::CreatePropertyFromReflection(const std::string& name, uint32_t size)
	{
		if (!m_Shader)
			return;

		// 尝试从Shader反射数据中找到此属性的更详细信息
		const auto& registerBlocks = m_Shader->GetReflection()->ReflectRegisterBlocks();
		ShaderDataType dataType = ShaderDataType::None;
		
		// 如果找不到详细类型信息，则根据大小推断类型
		if (dataType == ShaderDataType::None)
		{
			dataType = InferShaderDataTypeFromSize(size);
		}
		
		// 根据数据类型创建适当的MaterialProperty
		switch (dataType)
		{
		case ShaderDataType::Float:
			Set(name, 0.0f);
			break;
		case ShaderDataType::Float2:
			Set(name, glm::vec2(0.0f));
			break;
		case ShaderDataType::Float3:
			Set(name, glm::vec3(0.0f));
			break;
		case ShaderDataType::Float4:
			Set(name, glm::vec4(0.0f));
			break;
		case ShaderDataType::Int:
			Set(name, 0);
			break;
		case ShaderDataType::Bool:
			Set(name, false);
			break;
		case ShaderDataType::Mat4:
			Set(name, glm::mat4(1.0f)); // 初始化为单位矩阵
			break;
		default:
			HZ_CORE_WARN("Material: Cannot create property '{0}', type {1} is not supported.", name, (int)dataType);
			break;
		}
	}

	void Material::CreatePropertyBlocks()
	{
		if (!m_Shader)
			return;

		// 清除现有属性块
		m_PropertyBlocks.clear();

		// 获取寄存器块信息
		const auto& registerBlocks = m_Shader->GetReflection()->ReflectRegisterBlocks();
		
		for (const auto& block : registerBlocks)
		{
			// 计算块键值
			uint64_t blockKey = CalculateBlockKey(block.BindPoint, block.BindSpace);
			
			// 创建新的属性块
			MaterialPropertyBlock propertyBlock;
			propertyBlock.BindPoint = block.BindPoint;
			propertyBlock.BindSpace = block.BindSpace;
			propertyBlock.Size = block.Size;
			propertyBlock.Dirty = true;
			// 一个float占用4个字节，RawData大小为块大小除以4
			propertyBlock.RawData.resize(block.Size / sizeof(float), 0); // 初始化为0

			// 记录每个参数在块中的偏移
			for (const auto& param : block.Parameters)
			{
				propertyBlock.PropertyOffsets[param.Name] = param.Offset;
			}

			// 存储属性块
			m_PropertyBlocks[blockKey] = propertyBlock;
			
			HZ_CORE_INFO("Material: Created property block for register b{0}, space {1}, size {2} bytes", 
				block.BindPoint, block.BindSpace, block.Size);
		}
	}

	void Material::MarkPropertyDirty(const std::string& name)
	{
		if (!m_Shader)
			return;
		
		// 查找所有包含此属性的属性块
		for (auto& [key, block] : m_PropertyBlocks)
		{
			if (block.PropertyOffsets.find(name) != block.PropertyOffsets.end())
			{
				block.Dirty = true;
			}
		}
	}

	uint64_t Material::CalculateBlockKey(uint32_t bindPoint, uint32_t bindSpace) const
	{
		// 使用bindPoint和bindSpace组合成一个唯一的键值
		return (static_cast<uint64_t>(bindSpace) << 32) | bindPoint;
	}

	ShaderDataType Material::InferShaderDataTypeFromSize(uint32_t size)
	{
		// 根据大小推断类型
		switch (size)
		{
		case 4:
			return ShaderDataType::Float;
		case 8:
			return ShaderDataType::Float2;
		case 12:
			return ShaderDataType::Float3;
		case 16:
			return ShaderDataType::Float4;
		case 36:
			return ShaderDataType::Mat3;
		case 64:
			return ShaderDataType::Mat4;
		default:
			HZ_CORE_WARN("Material: Cannot infer type from size {0}", size);
			return ShaderDataType::None;
		}
	}

	uint32_t Material::GetSizeFromMaterialPropertyType(MaterialPropertyType type)
	{
		switch (type)
		{
		case MaterialPropertyType::Float:
			return 4; // sizeof(float)
		case MaterialPropertyType::Float2:
			return 8; // sizeof(glm::vec2)
		case MaterialPropertyType::Float3:
			return 12; // sizeof(glm::vec3)
		case MaterialPropertyType::Float4:
			return 16; // sizeof(glm::vec4)
		case MaterialPropertyType::Int:
			return 4; // sizeof(int)
		case MaterialPropertyType::Int2:
			return 8; // sizeof(glm::ivec2)
		case MaterialPropertyType::Int3:
			return 12; // sizeof(glm::ivec3)
		case MaterialPropertyType::Int4:
			return 16; // sizeof(glm::ivec4)
		case MaterialPropertyType::Bool:
			return 1; // sizeof(bool)
		case MaterialPropertyType::Matrix3:
			return 36; // sizeof(glm::mat3)
		case MaterialPropertyType::Matrix4:
			return 64; // sizeof(glm::mat4)
		case MaterialPropertyType::Texture2D:
		case MaterialPropertyType::TextureCube:
		case MaterialPropertyType::Texture3D:
			return sizeof(void*); // Texture references are pointer-sized
		default:
			return 0;
		}
	}

	bool Material::IsPropertyTypeMatchingShaderType(MaterialPropertyType propType, const std::string& paramName)
	{
		// Find parameter size from shader reflection
		uint32_t paramSize = 0;
		const auto& registerBlocks = m_Shader->GetReflection()->ReflectRegisterBlocks();
		
		for (const auto& block : registerBlocks)
		{
			for (const auto& param : block.Parameters)
			{
				if (param.Name == paramName)
				{
					paramSize = param.Size;
					break;
				}
			}
			if (paramSize != 0)
				break;
		}
		
		if (paramSize == 0)
			return false; // Parameter not found
		
		// Compare material property size with shader parameter size
		uint32_t propSize = GetSizeFromMaterialPropertyType(propType);
		return propSize == paramSize;
	}

	// Material::Set 模板特化实现
	template<> 
	void Material::Set<float>(const std::string& name, const float& value)
	{
		m_Properties[name] = MaterialProperty(value);
		MarkPropertyDirty(name);
	}

	template<> 
	void Material::Set<glm::vec2>(const std::string& name, const glm::vec2& value)
	{
		m_Properties[name] = MaterialProperty(value);
		MarkPropertyDirty(name);
	}

	template<> 
	void Material::Set<glm::vec3>(const std::string& name, const glm::vec3& value)
	{
		m_Properties[name] = MaterialProperty(value);
		MarkPropertyDirty(name);
	}

	template<> 
	void Material::Set<glm::vec4>(const std::string& name, const glm::vec4& value)
	{
		m_Properties[name] = MaterialProperty(value);
		MarkPropertyDirty(name);
	}

	template<> 
	void Material::Set<int>(const std::string& name, const int& value)
	{
		m_Properties[name] = MaterialProperty(value);
		MarkPropertyDirty(name);
	}

	template<> 
	void Material::Set<bool>(const std::string& name, const bool& value)
	{
		m_Properties[name] = MaterialProperty(value);
		MarkPropertyDirty(name);
	}

	template<> 
	void Material::Set<Ref<Texture2D>>(const std::string& name, const Ref<Texture2D>& value)
	{
		m_Properties[name] = MaterialProperty(value);
		MarkPropertyDirty(name);
	}

	template<> 
	void Material::Set<glm::mat4>(const std::string& name, const glm::mat4& value)
	{
		m_Properties[name] = MaterialProperty(value);
		MarkPropertyDirty(name);
	}

	// Material::Get 模板特化实现
	template<> 
	float Material::Get<float>(const std::string& name) const
	{
		if (!HasProperty(name))
			return 0.0f;
		return m_Properties.at(name).GetValue<float>();
	}

	template<> 
	glm::vec2 Material::Get<glm::vec2>(const std::string& name) const
	{
		if (!HasProperty(name))
			return glm::vec2(0.0f);
		return m_Properties.at(name).GetValue<glm::vec2>();
	}

	template<> 
	glm::vec3 Material::Get<glm::vec3>(const std::string& name) const
	{
		if (!HasProperty(name))
			return glm::vec3(0.0f);
		return m_Properties.at(name).GetValue<glm::vec3>();
	}

	template<> 
	glm::vec4 Material::Get<glm::vec4>(const std::string& name) const
	{
		if (!HasProperty(name))
			return glm::vec4(0.0f);
		return m_Properties.at(name).GetValue<glm::vec4>();
	}

	template<> 
	int Material::Get<int>(const std::string& name) const
	{
		if (!HasProperty(name))
			return 0;
		return m_Properties.at(name).GetValue<int>();
	}

	template<> 
	bool Material::Get<bool>(const std::string& name) const
	{
		if (!HasProperty(name))
			return false;
		return m_Properties.at(name).GetValue<bool>();
	}

	template<> 
	Ref<Texture2D> Material::Get<Ref<Texture2D>>(const std::string& name) const
	{
		if (!HasProperty(name))
			return nullptr;
		return m_Properties.at(name).GetValue<Ref<Texture2D>>();
	}

	template<> 
	glm::mat4 Material::Get<glm::mat4>(const std::string& name) const
	{
		if (!HasProperty(name))
			return glm::mat4(1.0f); // 返回单位矩阵作为默认值
		return m_Properties.at(name).GetValue<glm::mat4>();
	}
} 