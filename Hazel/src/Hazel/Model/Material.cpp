#include "hzpch.h"
#include "Material.h"
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <variant>
#include <cstdio>

namespace Hazel 
{
	// MaterialProperty实现
	MaterialProperty::MaterialProperty()
		: m_Type(MaterialPropertyType::None), m_Size(0), m_Value(std::monostate{})
	{
	}

	MaterialProperty::MaterialProperty(float value)
		: m_Type(MaterialPropertyType::Float), m_Value(value), m_Size(sizeof(float))
	{
	}

	MaterialProperty::MaterialProperty(const glm::vec2& value)
		: m_Type(MaterialPropertyType::Float2), m_Value(value), m_Size(sizeof(glm::vec2))
	{
	}

	MaterialProperty::MaterialProperty(const glm::vec3& value)
		: m_Type(MaterialPropertyType::Float3), m_Value(value), m_Size(sizeof(glm::vec3))
	{
	}

	MaterialProperty::MaterialProperty(const glm::vec4& value)
		: m_Type(MaterialPropertyType::Float4), m_Value(value), m_Size(sizeof(glm::vec4))
	{
	}

	MaterialProperty::MaterialProperty(int value)
		: m_Type(MaterialPropertyType::Int), m_Value(value), m_Size(sizeof(int))
	{
	}

	MaterialProperty::MaterialProperty(bool value)
		: m_Type(MaterialPropertyType::Bool), m_Value(value), m_Size(sizeof(bool))
	{
	}

	MaterialProperty::MaterialProperty(const Ref<Texture2D>& value)
		: m_Type(MaterialPropertyType::Texture2D), m_Value(value), m_Size(sizeof(Ref<Texture2D>))
	{
	}

	MaterialProperty::MaterialProperty(const glm::mat4& value)
		: m_Type(MaterialPropertyType::Matrix4), m_Value(value), m_Size(sizeof(glm::mat4))
	{
	}

	// 复制构造函数
	MaterialProperty::MaterialProperty(const MaterialProperty& other)
		: m_Type(other.m_Type), m_Size(other.m_Size), m_Value(other.m_Value)
	{
	}

	// 赋值运算符
	MaterialProperty& MaterialProperty::operator=(const MaterialProperty& other)
	{
		if (this != &other)
		{
			m_Type = other.m_Type;
			m_Size = other.m_Size;
			m_Value = other.m_Value;
		}
		return *this;
	}

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

	Ref<Material> Material::CreateFromMeta(const std::string& path)
	{
		auto mat = Material::DeserializeFromJSON(path);
		
		return mat;
	}

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

	const Ref<Shader>& Material::GetShader() const
	{
		return m_Shader;
	}

	void Material::SerializeToJSON(const std::string& filepath)
	{
		FILE* file = nullptr;
		fopen_s(&file, filepath.c_str(), "w");
		if (!file)
		{
			// 错误处理：无法打开文件
			return;
		}

		// 写入基本信息
		fprintf(file, "{\n");
		fprintf(file, "    \"shader\": \"%s\",\n", m_Shader->GetName().c_str());
		fprintf(file, "    \"properties\": {\n");

		// 写入属性
		size_t propertyCount = m_Properties.size();
		size_t currentProperty = 0;

		for (auto& [name, property] : m_Properties)
		{
			fprintf(file, "        \"%s\": {\n", name.c_str());

			// 写入类型和值
			switch (property.GetType())
			{
			case MaterialPropertyType::Float:
			{
				fprintf(file, "            \"type\": \"float\",\n");
				fprintf(file, "            \"size\": %u,\n", property.GetSize());
				fprintf(file, "            \"value\": %f\n", property.GetValue<float>());
				break;
			}
			case MaterialPropertyType::Float2:
			{
				auto value = property.GetValue<glm::vec2>();
				fprintf(file, "            \"type\": \"vec2\",\n");
				fprintf(file, "            \"size\": %u,\n", property.GetSize());
				fprintf(file, "            \"value\": [%f, %f]\n", value.x, value.y);
				break;
			}
			case MaterialPropertyType::Float3:
			{
				auto value = property.GetValue<glm::vec3>();
				fprintf(file, "            \"type\": \"vec3\",\n");
				fprintf(file, "            \"size\": %u,\n", property.GetSize());
				fprintf(file, "            \"value\": [%f, %f, %f]\n", value.x, value.y, value.z);
				break;
			}
			case MaterialPropertyType::Float4:
			{
				auto value = property.GetValue<glm::vec4>();
				fprintf(file, "            \"type\": \"vec4\",\n");
				fprintf(file, "            \"size\": %u,\n", property.GetSize());
				fprintf(file, "            \"value\": [%f, %f, %f, %f]\n", value.x, value.y, value.z, value.w);
				break;
			}
			case MaterialPropertyType::Int:
			{
				fprintf(file, "            \"type\": \"int\",\n");
				fprintf(file, "            \"size\": %u,\n", property.GetSize());
				fprintf(file, "            \"value\": %d\n", property.GetValue<int>());
				break;
			}
			case MaterialPropertyType::Bool:
			{
				fprintf(file, "            \"type\": \"bool\",\n");
				fprintf(file, "            \"size\": %u,\n", property.GetSize());
				fprintf(file, "            \"value\": %s\n", property.GetValue<bool>() ? "true" : "false");
				break;
			}
			case MaterialPropertyType::Matrix4:
			{
				auto value = property.GetValue<glm::mat4>();
				fprintf(file, "            \"type\": \"mat4\",\n");
				fprintf(file, "            \"size\": %u,\n", property.GetSize());
				fprintf(file, "            \"value\": [\n");
				fprintf(file, "                [%f, %f, %f, %f],\n", value[0][0], value[0][1], value[0][2], value[0][3]);
				fprintf(file, "                [%f, %f, %f, %f],\n", value[1][0], value[1][1], value[1][2], value[1][3]);
				fprintf(file, "                [%f, %f, %f, %f],\n", value[2][0], value[2][1], value[2][2], value[2][3]);
				fprintf(file, "                [%f, %f, %f, %f]\n", value[3][0], value[3][1], value[3][2], value[3][3]);
				fprintf(file, "            ]\n");
				break;
			}
			case MaterialPropertyType::Texture2D:
			{
				fprintf(file, "            \"type\": \"texture2d\",\n");
				fprintf(file, "            \"size\": %u,\n", property.GetSize());
				auto texture = property.GetValue<Ref<Texture2D>>();
				if (texture)
					fprintf(file, "            \"value\": \"%s\"\n", texture->GetPath().c_str());
				else
					fprintf(file, "            \"value\": \"\"\n");
				break;
			}
			default:
				// 不支持的类型
				fprintf(file, "            \"type\": \"unknown\",\n");
				fprintf(file, "            \"value\": null\n");
				break;
			}

			// 判断是否是最后一个属性
			if (++currentProperty < propertyCount)
				fprintf(file, "        },\n");
			else
				fprintf(file, "        }\n");
		}

		fprintf(file, "    }\n");
		fprintf(file, "}\n");

		fclose(file);
	}

	Ref<Material> Material::DeserializeFromJSON(const std::string& filepath)
	{
		// 读取整个文件内容
		FILE* file = nullptr;
		fopen_s(&file, filepath.c_str(), "rb");
		if (!file)
		{
			HZ_CORE_ERROR("Material: Cannot open file {0}", filepath);
			return nullptr;
		}

		// 获取文件大小
		fseek(file, 0, SEEK_END);
		long fileSize = ftell(file);
		fseek(file, 0, SEEK_SET);

		// 读取文件内容到缓冲区
		std::string content;
		content.resize(fileSize);
		size_t bytesRead = fread(&content[0], 1, fileSize, file);
		fclose(file);

		if (bytesRead != static_cast<size_t>(fileSize))
		{
			HZ_CORE_ERROR("Material: Failed to read file {0}", filepath);
			return nullptr;
		}

		// 解析着色器路径
		size_t shaderStart = content.find("\"shader\"");
		if (shaderStart == std::string::npos) {
			HZ_CORE_ERROR("Material: Failed to find shader path in material file {0}", filepath);
			return nullptr;
		}

		size_t shaderValueStart = content.find("\"", shaderStart + 9);
		if (shaderValueStart == std::string::npos) {
			HZ_CORE_ERROR("Material: Invalid JSON format in {0}: missing shader path", filepath);
			return nullptr;
		}
		shaderValueStart++; // 跳过引号

		size_t shaderValueEnd = content.find("\"", shaderValueStart);
		if (shaderValueEnd == std::string::npos) {
			HZ_CORE_ERROR("Material: Invalid JSON format in {0}: unclosed shader path", filepath);
			return nullptr;
		}

		std::string shaderPath = content.substr(shaderValueStart, shaderValueEnd - shaderValueStart);
		HZ_CORE_TRACE("Material: Loading shader {0} for material", shaderPath);

		// 加载着色器
		Ref<Shader> shader = Shader::Create(shaderPath);
		if (!shader)
		{
			HZ_CORE_ERROR("Material: Failed to load shader {0}", shaderPath);
			return nullptr;
		}

		// 创建材质 - 这会自动调用SyncWithShaderReflection创建所有必要的属性和PropertyBlock
		Ref<Material> material = CreateRef<Material>(shader);
		
		// 找到属性部分并解析
		size_t propertiesStart = content.find("\"properties\"");
		if (propertiesStart == std::string::npos) {
			HZ_CORE_WARN("Material: No properties found in {0}", filepath);
			return material; // 没有属性部分，返回仅带着色器的材质
		}

		size_t propertiesObj = content.find("{", propertiesStart);
		if (propertiesObj == std::string::npos) {
			HZ_CORE_WARN("Material: Invalid properties format in {0}", filepath);
			return material; // 格式错误，但仍返回材质
		}

		// 找到属性部分结束的大括号（考虑嵌套）
		size_t depth = 1;
		size_t propertiesEnd = propertiesObj + 1;
		
		while (depth > 0 && propertiesEnd < content.length()) {
			if (content[propertiesEnd] == '{') {
				depth++;
			} else if (content[propertiesEnd] == '}') {
				depth--;
			}
			propertiesEnd++;
		}
		
		if (depth > 0) {
			HZ_CORE_WARN("Material: Unclosed properties object in {0}", filepath);
			return material; // 格式错误，但仍返回材质
		}
		
		propertiesEnd--; // 调整到最后一个大括号位置
		std::string propertiesContent = content.substr(propertiesObj + 1, propertiesEnd - propertiesObj - 1);

		// 解析每个属性
		size_t pos = 0;
		while (pos < propertiesContent.length()) {
			// 查找属性名开始的引号
			size_t nameStart = propertiesContent.find("\"", pos);
			if (nameStart == std::string::npos) break;
			
			nameStart++; // 跳过引号
			
			// 查找属性名结束的引号
			size_t nameEnd = propertiesContent.find("\"", nameStart);
			if (nameEnd == std::string::npos) break;
			
			std::string name = propertiesContent.substr(nameStart, nameEnd - nameStart);
			
			// 检查材质中是否有此属性 - 只设置Shader中存在的属性
			if (!material->HasProperty(name)) {
				HZ_CORE_TRACE("Material: Property '{0}' in file not found in shader, skipping", name);
				
				// 跳过此属性的剩余部分，找到下一个属性开始
				size_t nextPropStart = propertiesContent.find("\"", nameEnd + 1);
				if (nextPropStart == std::string::npos) break;
				
				pos = nextPropStart;
				continue;
			}
			
			// 跳过冒号
			size_t colonPos = propertiesContent.find(":", nameEnd);
			if (colonPos == std::string::npos) break;
			
			// 查找属性对象开始的大括号
			size_t propObjStart = propertiesContent.find("{", colonPos);
			if (propObjStart == std::string::npos) break;
			
			// 查找属性对象结束的大括号
			size_t propObjEnd = propObjStart + 1;
			depth = 1;
			
			while (depth > 0 && propObjEnd < propertiesContent.length()) {
				if (propertiesContent[propObjEnd] == '{') {
					depth++;
				} else if (propertiesContent[propObjEnd] == '}') {
					depth--;
				}
				propObjEnd++;
			}
			
			if (depth > 0) break; // 没有找到匹配的结束括号
			
			propObjEnd--; // 调整到最后一个大括号位置
			
			std::string propContent = propertiesContent.substr(propObjStart + 1, propObjEnd - propObjStart - 1);
			
			// 解析类型
			size_t typeStart = propContent.find("\"type\"");
			if (typeStart == std::string::npos) {
				pos = propObjEnd + 1;
				continue;
			}
			
			size_t typeValueStart = propContent.find("\"", typeStart + 7);
			if (typeValueStart == std::string::npos) {
				pos = propObjEnd + 1;
				continue;
			}
			
			typeValueStart++; // 跳过引号
			
			size_t typeValueEnd = propContent.find("\"", typeValueStart);
			if (typeValueEnd == std::string::npos) {
				pos = propObjEnd + 1;
				continue;
			}
			
			std::string type = propContent.substr(typeValueStart, typeValueEnd - typeValueStart);
			
			// 解析值
			size_t valueStart = propContent.find("\"value\"");
			if (valueStart == std::string::npos) {
				pos = propObjEnd + 1;
				continue;
			}

			if (type == "float")
			{
				size_t valueNumStart = propContent.find_first_of("0123456789.-", valueStart);
				if (valueNumStart == std::string::npos) {
					pos = propObjEnd + 1;
					continue;
				}
				
				size_t valueNumEnd = propContent.find_first_not_of("0123456789.-", valueNumStart);
				if (valueNumEnd == std::string::npos) {
					valueNumEnd = propContent.length();
				}
				
				std::string valueStr = propContent.substr(valueNumStart, valueNumEnd - valueNumStart);
				float value = std::stof(valueStr);
				material->Set(name, value);
				HZ_CORE_TRACE("Material: Set property '{0}' to float value {1}", name, value);
			}
			else if (type == "vec2")
			{
				size_t valueArrayStart = propContent.find("[", valueStart);
				if (valueArrayStart == std::string::npos) {
					pos = propObjEnd + 1;
					continue;
				}
				
				size_t valueArrayEnd = propContent.find("]", valueArrayStart);
				if (valueArrayEnd == std::string::npos) {
					pos = propObjEnd + 1;
					continue;
				}
				
				std::string arrayContent = propContent.substr(valueArrayStart + 1, valueArrayEnd - valueArrayStart - 1);
				
				size_t commaPos = arrayContent.find(",");
				if (commaPos == std::string::npos) {
					pos = propObjEnd + 1;
					continue;
				}
				
				std::string xStr = arrayContent.substr(0, commaPos);
				std::string yStr = arrayContent.substr(commaPos + 1);
				
				// 去除前后空格
				xStr.erase(0, xStr.find_first_not_of(" \t"));
				xStr.erase(xStr.find_last_not_of(" \t") + 1);
				yStr.erase(0, yStr.find_first_not_of(" \t"));
				yStr.erase(yStr.find_last_not_of(" \t") + 1);
				
				float x = std::stof(xStr);
				float y = std::stof(yStr);
				
				material->Set(name, glm::vec2(x, y));
				HZ_CORE_TRACE("Material: Set property '{0}' to vec2 value ({1}, {2})", name, x, y);
			}
			else if (type == "vec3")
			{
				size_t valueArrayStart = propContent.find("[", valueStart);
				if (valueArrayStart == std::string::npos) {
					pos = propObjEnd + 1;
					continue;
				}
				
				size_t valueArrayEnd = propContent.find("]", valueArrayStart);
				if (valueArrayEnd == std::string::npos) {
					pos = propObjEnd + 1;
					continue;
				}
				
				std::string arrayContent = propContent.substr(valueArrayStart + 1, valueArrayEnd - valueArrayStart - 1);
				
				size_t commaPos1 = arrayContent.find(",");
				if (commaPos1 == std::string::npos) {
					pos = propObjEnd + 1;
					continue;
				}
				
				size_t commaPos2 = arrayContent.find(",", commaPos1 + 1);
				if (commaPos2 == std::string::npos) {
					pos = propObjEnd + 1;
					continue;
				}
				
				std::string xStr = arrayContent.substr(0, commaPos1);
				std::string yStr = arrayContent.substr(commaPos1 + 1, commaPos2 - commaPos1 - 1);
				std::string zStr = arrayContent.substr(commaPos2 + 1);
				
				// 去除前后空格
				xStr.erase(0, xStr.find_first_not_of(" \t"));
				xStr.erase(xStr.find_last_not_of(" \t") + 1);
				yStr.erase(0, yStr.find_first_not_of(" \t"));
				yStr.erase(yStr.find_last_not_of(" \t") + 1);
				zStr.erase(0, zStr.find_first_not_of(" \t"));
				zStr.erase(zStr.find_last_not_of(" \t") + 1);
				
				float x = std::stof(xStr);
				float y = std::stof(yStr);
				float z = std::stof(zStr);
				
				material->Set(name, glm::vec3(x, y, z));
				HZ_CORE_TRACE("Material: Set property '{0}' to vec3 value ({1}, {2}, {3})", name, x, y, z);
			}
			else if (type == "vec4")
			{
				size_t valueArrayStart = propContent.find("[", valueStart);
				if (valueArrayStart == std::string::npos) {
					pos = propObjEnd + 1;
					continue;
				}
				
				size_t valueArrayEnd = propContent.find("]", valueArrayStart);
				if (valueArrayEnd == std::string::npos) {
					pos = propObjEnd + 1;
					continue;
				}
				
				std::string arrayContent = propContent.substr(valueArrayStart + 1, valueArrayEnd - valueArrayStart - 1);
				
				size_t commaPos1 = arrayContent.find(",");
				if (commaPos1 == std::string::npos) {
					pos = propObjEnd + 1;
					continue;
				}
				
				size_t commaPos2 = arrayContent.find(",", commaPos1 + 1);
				if (commaPos2 == std::string::npos) {
					pos = propObjEnd + 1;
					continue;
				}
				
				size_t commaPos3 = arrayContent.find(",", commaPos2 + 1);
				if (commaPos3 == std::string::npos) {
					pos = propObjEnd + 1;
					continue;
				}
				
				std::string xStr = arrayContent.substr(0, commaPos1);
				std::string yStr = arrayContent.substr(commaPos1 + 1, commaPos2 - commaPos1 - 1);
				std::string zStr = arrayContent.substr(commaPos2 + 1, commaPos3 - commaPos2 - 1);
				std::string wStr = arrayContent.substr(commaPos3 + 1);
				
				// 去除前后空格
				xStr.erase(0, xStr.find_first_not_of(" \t"));
				xStr.erase(xStr.find_last_not_of(" \t") + 1);
				yStr.erase(0, yStr.find_first_not_of(" \t"));
				yStr.erase(yStr.find_last_not_of(" \t") + 1);
				zStr.erase(0, zStr.find_first_not_of(" \t"));
				zStr.erase(zStr.find_last_not_of(" \t") + 1);
				wStr.erase(0, wStr.find_first_not_of(" \t"));
				wStr.erase(wStr.find_last_not_of(" \t") + 1);
				
				float x = std::stof(xStr);
				float y = std::stof(yStr);
				float z = std::stof(zStr);
				float w = std::stof(wStr);
				
				material->Set(name, glm::vec4(x, y, z, w));
				HZ_CORE_TRACE("Material: Set property '{0}' to vec4 value ({1}, {2}, {3}, {4})", name, x, y, z, w);
			}
			else if (type == "int")
			{
				size_t valueNumStart = propContent.find_first_of("0123456789-", valueStart);
				if (valueNumStart == std::string::npos) {
					pos = propObjEnd + 1;
					continue;
				}
				
				size_t valueNumEnd = propContent.find_first_not_of("0123456789-", valueNumStart);
				if (valueNumEnd == std::string::npos) {
					valueNumEnd = propContent.length();
				}
				
				std::string valueStr = propContent.substr(valueNumStart, valueNumEnd - valueNumStart);
				int value = std::stoi(valueStr);
				material->Set(name, value);
				HZ_CORE_TRACE("Material: Set property '{0}' to int value {1}", name, value);
			}
			else if (type == "bool")
			{
				size_t valueBoolStart = propContent.find_first_of("tf", valueStart);
				if (valueBoolStart == std::string::npos) {
					pos = propObjEnd + 1;
					continue;
				}
				
				bool value = (propContent[valueBoolStart] == 't');
				material->Set(name, value);
				HZ_CORE_TRACE("Material: Set property '{0}' to bool value {1}", name, value ? "true" : "false");
			}
			else if (type == "mat4")
			{
				size_t valueArrayStart = propContent.find("[", valueStart);
				if (valueArrayStart == std::string::npos) {
					// 如果无法解析，设置为单位矩阵
					material->Set(name, glm::mat4(1.0f));
					HZ_CORE_TRACE("Material: Set property '{0}' to identity matrix (fallback)", name);
					pos = propObjEnd + 1;
					continue;
				}
				
				// 由于矩阵解析复杂，这里简化处理
				// TODO: 实现完整的4x4矩阵解析
				material->Set(name, glm::mat4(1.0f));
				HZ_CORE_TRACE("Material: Set property '{0}' to identity matrix", name);
			}
			else if (type == "texture2d")
			{
				size_t valuePathStart = propContent.find("\"", valueStart + 8);
				if (valuePathStart == std::string::npos) {
					pos = propObjEnd + 1;
					continue;
				}
				
				valuePathStart++; // 跳过引号
				
				size_t valuePathEnd = propContent.find("\"", valuePathStart);
				if (valuePathEnd == std::string::npos) {
					pos = propObjEnd + 1;
					continue;
				}
				
				std::string path = propContent.substr(valuePathStart, valuePathEnd - valuePathStart);
				
				if (!path.empty())
				{
					Ref<Texture2D> texture = Texture2D::Create(path);
					material->Set(name, texture);
					HZ_CORE_TRACE("Material: Set property '{0}' to texture '{1}'", name, path);
				}
			}

			// 移动到下一个属性
			pos = propObjEnd + 1;
		}

		// 同步数据到优化的内存布局
		material->SyncToRawData();

		return material;
	}

	// MaterialLibrary实现
	MaterialLibrary& MaterialLibrary::Get()
	{
		static MaterialLibrary instance;
		return instance;
	}

	void MaterialLibrary::Register(const std::string& name, const Ref<Material>& material)
	{
		m_Materials[name] = material;
	}

	Ref<Material> MaterialLibrary::Get(const std::string& name)
	{
		if (Exists(name))
			return m_Materials[name];
		return nullptr;
	}

	bool MaterialLibrary::Exists(const std::string& name) const
	{
		return m_Materials.find(name) != m_Materials.end();
	}

	bool MaterialLibrary::Save(const std::string& name, const std::string& filepath)
	{
		if (!Exists(name))
			return false;
			
		m_Materials[name]->SerializeToJSON(filepath);
		return true;
	}

	Ref<Material> MaterialLibrary::Load(const std::string& filepath)
	{
		Ref<Material> material = Material::DeserializeFromJSON(filepath);
		if (material)
		{
			// 使用文件名作为材质名称
			size_t lastSlash = filepath.find_last_of("/\\");
			size_t lastDot = filepath.find_last_of(".");
			std::string name = filepath.substr(lastSlash + 1, lastDot - lastSlash - 1);
			
			Register(name, material);
		}
		return material;
	}

	// MaterialProperty::GetValue<float> 实现
	template<> 
	float& MaterialProperty::GetValue<float>()
	{
		if (m_Type != MaterialPropertyType::Float)
			throw std::runtime_error("MaterialProperty: Type mismatch");
		return std::get<float>(m_Value);
	}

	template<> 
	const float& MaterialProperty::GetValue<float>() const
	{
		if (m_Type != MaterialPropertyType::Float)
			throw std::runtime_error("MaterialProperty: Type mismatch");
		return std::get<float>(m_Value);
	}

	template<> 
	glm::vec2& MaterialProperty::GetValue<glm::vec2>()
	{
		if (m_Type != MaterialPropertyType::Float2)
			throw std::runtime_error("MaterialProperty: Type mismatch");
		return std::get<glm::vec2>(m_Value);
	}

	template<> 
	const glm::vec2& MaterialProperty::GetValue<glm::vec2>() const
	{
		if (m_Type != MaterialPropertyType::Float2)
			throw std::runtime_error("MaterialProperty: Type mismatch");
		return std::get<glm::vec2>(m_Value);
	}

	template<> 
	glm::vec3& MaterialProperty::GetValue<glm::vec3>()
	{
		if (m_Type != MaterialPropertyType::Float3)
			throw std::runtime_error("MaterialProperty: Type mismatch");
		return std::get<glm::vec3>(m_Value);
	}

	template<> 
	const glm::vec3& MaterialProperty::GetValue<glm::vec3>() const
	{
		if (m_Type != MaterialPropertyType::Float3)
			throw std::runtime_error("MaterialProperty: Type mismatch");
		return std::get<glm::vec3>(m_Value);
	}

	template<> 
	glm::vec4& MaterialProperty::GetValue<glm::vec4>()
	{
		if (m_Type != MaterialPropertyType::Float4)
			throw std::runtime_error("MaterialProperty: Type mismatch");
		return std::get<glm::vec4>(m_Value);
	}

	template<> 
	const glm::vec4& MaterialProperty::GetValue<glm::vec4>() const
	{
		if (m_Type != MaterialPropertyType::Float4)
			throw std::runtime_error("MaterialProperty: Type mismatch");
		return std::get<glm::vec4>(m_Value);
	}

	template<> 
	int& MaterialProperty::GetValue<int>()
	{
		if (m_Type != MaterialPropertyType::Int)
			throw std::runtime_error("MaterialProperty: Type mismatch");
		return std::get<int>(m_Value);
	}

	template<> 
	const int& MaterialProperty::GetValue<int>() const
	{
		if (m_Type != MaterialPropertyType::Int)
			throw std::runtime_error("MaterialProperty: Type mismatch");
		return std::get<int>(m_Value);
	}

	template<> 
	bool& MaterialProperty::GetValue<bool>()
	{
		if (m_Type != MaterialPropertyType::Bool)
			throw std::runtime_error("MaterialProperty: Type mismatch");
		return std::get<bool>(m_Value);
	}

	template<> 
	const bool& MaterialProperty::GetValue<bool>() const
	{
		if (m_Type != MaterialPropertyType::Bool)
			throw std::runtime_error("MaterialProperty: Type mismatch");
		return std::get<bool>(m_Value);
	}

	template<> 
	Ref<Texture2D>& MaterialProperty::GetValue<Ref<Texture2D>>()
	{
		if (m_Type != MaterialPropertyType::Texture2D)
			throw std::runtime_error("MaterialProperty: Type mismatch");
		return std::get<Ref<Texture2D>>(m_Value);
	}

	template<> 
	const Ref<Texture2D>& MaterialProperty::GetValue<Ref<Texture2D>>() const
	{
		if (m_Type != MaterialPropertyType::Texture2D)
			throw std::runtime_error("MaterialProperty: Type mismatch");
		return std::get<Ref<Texture2D>>(m_Value);
	}

	template<> 
	glm::mat4& MaterialProperty::GetValue<glm::mat4>()
	{
		if (m_Type != MaterialPropertyType::Matrix4)
			throw std::runtime_error("MaterialProperty: Type mismatch");
		return std::get<glm::mat4>(m_Value);
	}

	template<> 
	const glm::mat4& MaterialProperty::GetValue<glm::mat4>() const
	{
		if (m_Type != MaterialPropertyType::Matrix4)
			throw std::runtime_error("MaterialProperty: Type mismatch");
		return std::get<glm::mat4>(m_Value);
	}

	// Material::Set 模板特化
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

	// Material::Get 模板特化
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

	// 创建属性块
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
			propertyBlock.RawData.resize(block.Size / 4, 0); // 初始化为0

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

	// 同步属性到优化的内存布局
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
				uint32_t offset = param.Offset;
				
				// 根据属性类型复制数据
				const auto& property = m_Properties[param.Name];
				
				switch (property.GetType())
				{
				case MaterialPropertyType::Float:
				{
					float value = property.GetValue<float>();
					memcpy(propertyBlock.RawData.data() + offset, &value, sizeof(float));
					break;
				}
				case MaterialPropertyType::Float2:
				{
					const glm::vec2& value = property.GetValue<glm::vec2>();
					memcpy(propertyBlock.RawData.data() + offset, &value, sizeof(glm::vec2));
					break;
				}
				case MaterialPropertyType::Float3:
				{
					const glm::vec3& value = property.GetValue<glm::vec3>();
					memcpy(propertyBlock.RawData.data() + offset, &value, sizeof(glm::vec3));
					break;
				}
				case MaterialPropertyType::Float4:
				{
					const glm::vec4& value = property.GetValue<glm::vec4>();
					memcpy(propertyBlock.RawData.data() + offset, &value, sizeof(glm::vec4));
					break;
				}
				case MaterialPropertyType::Int:
				{
					int value = property.GetValue<int>();
					memcpy(propertyBlock.RawData.data() + offset, &value, sizeof(int));
					break;
				}
				case MaterialPropertyType::Bool:
				{
					bool value = property.GetValue<bool>();
					memcpy(propertyBlock.RawData.data() + offset, &value, sizeof(bool));
					break;
				}
				case MaterialPropertyType::Matrix4:
				{
					const glm::mat4& value = property.GetValue<glm::mat4>();
					memcpy(propertyBlock.RawData.data() + offset, &value, sizeof(glm::mat4));
					break;
				}
				case MaterialPropertyType::Texture2D:
				{
					// 纹理引用不需要复制到常量缓冲区，它们会单独绑定
					// 这里可以存储一个纹理索引或标识符
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

	// 计算属性块键值
	uint64_t Material::CalculateBlockKey(uint32_t bindPoint, uint32_t bindSpace) const
	{
		// 使用bindPoint和bindSpace组合成一个唯一的键值
		return (static_cast<uint64_t>(bindSpace) << 32) | bindPoint;
	}

	// 获取属性块
	const MaterialPropertyBlock* Material::GetPropertyBlock(uint32_t bindPoint, uint32_t bindSpace) const
	{
		uint64_t blockKey = CalculateBlockKey(bindPoint, bindSpace);
		
		auto it = m_PropertyBlocks.find(blockKey);
		if (it != m_PropertyBlocks.end())
			return &(it->second);
		
		return nullptr;
	}

	// 检查属性块是否存在
	bool Material::HasPropertyBlock(uint32_t bindPoint, uint32_t bindSpace) const
	{
		uint64_t blockKey = CalculateBlockKey(bindPoint, bindSpace);
		return m_PropertyBlocks.find(blockKey) != m_PropertyBlocks.end();
	}

	// 标记属性为脏，需要更新
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
}