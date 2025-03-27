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
		: m_Type(MaterialPropertyType::None)
	{
	}

	MaterialProperty::MaterialProperty(float value)
		: m_Type(MaterialPropertyType::Float), m_Value(value)
	{
	}

	MaterialProperty::MaterialProperty(const glm::vec2& value)
		: m_Type(MaterialPropertyType::Float2), m_Value(value)
	{
	}

	MaterialProperty::MaterialProperty(const glm::vec3& value)
		: m_Type(MaterialPropertyType::Float3), m_Value(value)
	{
	}

	MaterialProperty::MaterialProperty(const glm::vec4& value)
		: m_Type(MaterialPropertyType::Float4), m_Value(value)
	{
	}

	MaterialProperty::MaterialProperty(int value)
		: m_Type(MaterialPropertyType::Int), m_Value(value)
	{
	}

	MaterialProperty::MaterialProperty(bool value)
		: m_Type(MaterialPropertyType::Bool), m_Value(value)
	{
	}

	MaterialProperty::MaterialProperty(const Ref<Texture2D>& value)
		: m_Type(MaterialPropertyType::Texture2D), m_Value(value)
	{
	}

	// Material实现
	Material::Material(const Ref<Shader>& shader)
		: m_Shader(shader)
	{
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
				fprintf(file, "            \"value\": %f\n", property.GetValue<float>());
				break;
			}
			case MaterialPropertyType::Float2:
			{
				auto value = property.GetValue<glm::vec2>();
				fprintf(file, "            \"type\": \"vec2\",\n");
				fprintf(file, "            \"value\": [%f, %f]\n", value.x, value.y);
				break;
			}
			case MaterialPropertyType::Float3:
			{
				auto value = property.GetValue<glm::vec3>();
				fprintf(file, "            \"type\": \"vec3\",\n");
				fprintf(file, "            \"value\": [%f, %f, %f]\n", value.x, value.y, value.z);
				break;
			}
			case MaterialPropertyType::Float4:
			{
				auto value = property.GetValue<glm::vec4>();
				fprintf(file, "            \"type\": \"vec4\",\n");
				fprintf(file, "            \"value\": [%f, %f, %f, %f]\n", value.x, value.y, value.z, value.w);
				break;
			}
			case MaterialPropertyType::Int:
			{
				fprintf(file, "            \"type\": \"int\",\n");
				fprintf(file, "            \"value\": %d\n", property.GetValue<int>());
				break;
			}
			case MaterialPropertyType::Bool:
			{
				fprintf(file, "            \"type\": \"bool\",\n");
				fprintf(file, "            \"value\": %s\n", property.GetValue<bool>() ? "true" : "false");
				break;
			}
			case MaterialPropertyType::Texture2D:
			{
				fprintf(file, "            \"type\": \"texture2d\",\n");
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
			// 错误处理：无法打开文件
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
			// 错误处理：读取文件失败
			return nullptr;
		}

		// 解析着色器路径
		size_t shaderStart = content.find("\"shader\"");
		if (shaderStart == std::string::npos) {
			HZ_ASSERT(false, "Failed to find shader path in material file");
			return nullptr;
		}

		size_t shaderValueStart = content.find("\"", shaderStart + 9);
		if (shaderValueStart == std::string::npos) {
			HZ_ASSERT(false, "Invalid JSON format: missing shader path");
			return nullptr;
		}
		shaderValueStart++; // 跳过引号

		size_t shaderValueEnd = content.find("\"", shaderValueStart);
		if (shaderValueEnd == std::string::npos) {
			HZ_ASSERT(false, "Invalid JSON format: unclosed shader path");
			return nullptr;
		}

		std::string shaderPath = content.substr(shaderValueStart, shaderValueEnd - shaderValueStart);

		// 加载着色器
		Ref<Shader> shader = Shader::Create(shaderPath);
		if (!shader)
		{
			// 错误处理：无法加载着色器
			HZ_ASSERT(false, "Failed to load shader {0}", shaderPath);
			return nullptr;
		}

		// 创建材质
		Ref<Material> material = CreateRef<Material>(shader);

		// 找到属性部分
		size_t propertiesStart = content.find("\"properties\"");
		if (propertiesStart == std::string::npos) {
			return material; // 没有属性部分，返回仅带着色器的材质
		}

		size_t propertiesObj = content.find("{", propertiesStart);
		if (propertiesObj == std::string::npos) {
			return material; // 格式错误，但仍返回材质
		}

		// 找到属性部分结束的大括号
		// 需要考虑嵌套的大括号情况
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
			HZ_ASSERT(false, "Invalid JSON format: unclosed properties object");
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
				}
			}

			// 移动到下一个属性（跳过当前属性对象的结束大括号）
			pos = propObjEnd + 1;
		}

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

	// MaterialProperty模板特化
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

	// Material::Set 模板特化
	template<> 
	void Material::Set<float>(const std::string& name, const float& value)
	{
		m_Properties[name] = MaterialProperty(value);
	}

	template<> 
	void Material::Set<glm::vec2>(const std::string& name, const glm::vec2& value)
	{
		m_Properties[name] = MaterialProperty(value);
	}

	template<> 
	void Material::Set<glm::vec3>(const std::string& name, const glm::vec3& value)
	{
		m_Properties[name] = MaterialProperty(value);
	}

	template<> 
	void Material::Set<glm::vec4>(const std::string& name, const glm::vec4& value)
	{
		m_Properties[name] = MaterialProperty(value);
	}

	template<> 
	void Material::Set<int>(const std::string& name, const int& value)
	{
		m_Properties[name] = MaterialProperty(value);
	}

	template<> 
	void Material::Set<bool>(const std::string& name, const bool& value)
	{
		m_Properties[name] = MaterialProperty(value);
	}

	template<> 
	void Material::Set<Ref<Texture2D>>(const std::string& name, const Ref<Texture2D>& value)
	{
		m_Properties[name] = MaterialProperty(value);
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
}