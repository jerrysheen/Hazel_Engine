#include "hzpch.h"
#include "MaterialSerializer.h"
#include "Material.h"
#include "MaterialProperty.h"
#include "Runtime/Graphics/Renderer/Shader.h"
#include "Runtime/Graphics/Renderer/Texture.h"
#include <fstream>
#include <sstream>
#include <cstdio>

namespace Hazel 
{
	void MaterialSerializer::SerializeToJSON(const Ref<Material>& material, const std::string& filepath)
	{
		FILE* file = nullptr;
		fopen_s(&file, filepath.c_str(), "w");
		if (!file)
		{
			HZ_CORE_ERROR("MaterialSerializer: Cannot open file {0} for writing", filepath);
			return;
		}

		// 写入基本信息
		fprintf(file, "{\n");
		fprintf(file, "    \"shader\": \"%s\",\n", material->GetShader()->GetName().c_str());
		fprintf(file, "    \"properties\": {\n");

		// 写入属性
		size_t propertyCount = material->m_Properties.size();
		size_t currentProperty = 0;

		for (auto& [name, property] : material->m_Properties)
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

	Ref<Material> MaterialSerializer::DeserializeFromJSON(const std::string& filepath)
	{
		// 读取整个文件内容
		FILE* file = nullptr;
		fopen_s(&file, filepath.c_str(), "rb");
		if (!file)
		{
			HZ_CORE_ERROR("MaterialSerializer: Cannot open file {0}", filepath);
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
			HZ_CORE_ERROR("MaterialSerializer: Failed to read file {0}", filepath);
			return nullptr;
		}

		return DeserializeFromString(content);
	}

	std::string MaterialSerializer::SerializeToString(const Ref<Material>& material)
	{
		// 创建临时文件进行序列化，然后读取内容
		std::string tempPath = "temp_material.json";
		SerializeToJSON(material, tempPath);
		
		// 读取临时文件内容
		std::ifstream file(tempPath);
		if (!file.is_open())
			return "";
		
		std::stringstream buffer;
		buffer << file.rdbuf();
		file.close();
		
		// 删除临时文件
		std::remove(tempPath.c_str());
		
		return buffer.str();
	}

	Ref<Material> MaterialSerializer::DeserializeFromString(const std::string& jsonString)
	{
		const std::string& content = jsonString;

		// 解析着色器路径
		size_t shaderStart = content.find("\"shader\"");
		if (shaderStart == std::string::npos) {
			HZ_CORE_ERROR("MaterialSerializer: Failed to find shader path in JSON");
			return nullptr;
		}

		size_t shaderValueStart = content.find("\"", shaderStart + 9);
		if (shaderValueStart == std::string::npos) {
			HZ_CORE_ERROR("MaterialSerializer: Invalid JSON format: missing shader path");
			return nullptr;
		}
		shaderValueStart++; // 跳过引号

		size_t shaderValueEnd = content.find("\"", shaderValueStart);
		if (shaderValueEnd == std::string::npos) {
			HZ_CORE_ERROR("MaterialSerializer: Invalid JSON format: unclosed shader path");
			return nullptr;
		}

		std::string shaderPath = content.substr(shaderValueStart, shaderValueEnd - shaderValueStart);
		HZ_CORE_TRACE("MaterialSerializer: Loading shader {0} for material", shaderPath);

		// 加载着色器
		Ref<Shader> shader = Shader::Create(shaderPath);
		if (!shader)
		{
			HZ_CORE_ERROR("MaterialSerializer: Failed to load shader {0}", shaderPath);
			return nullptr;
		}

		// 创建材质 - 这会自动调用SyncWithShaderReflection创建所有必要的属性和PropertyBlock
		Ref<Material> material = CreateRef<Material>(shader);
		
		// 解析属性 (详细的属性解析逻辑...)
		ParseProperties(content, material);

		// 同步数据到优化的内存布局
		material->SyncToRawData();

		return material;
	}

	void MaterialSerializer::SerializeProperty(const std::string& name, const MaterialProperty& property, void* jsonObject)
	{
		// 这里可以实现更高级的JSON序列化逻辑
		// 目前使用简单的文件输出方式
	}

	void MaterialSerializer::DeserializeProperty(const std::string& name, MaterialProperty& property, void* jsonObject)
	{
		// 这里可以实现更高级的JSON反序列化逻辑
		// 目前使用简单的字符串解析方式
	}

	void MaterialSerializer::ParseProperties(const std::string& content, Ref<Material> material)
	{
		// 找到属性部分并解析
		size_t propertiesStart = content.find("\"properties\"");
		if (propertiesStart == std::string::npos) {
			HZ_CORE_WARN("MaterialSerializer: No properties found in JSON");
			return; // 没有属性部分
		}

		size_t propertiesObj = content.find("{", propertiesStart);
		if (propertiesObj == std::string::npos) {
			HZ_CORE_WARN("MaterialSerializer: Invalid properties format in JSON");
			return; // 格式错误
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
			HZ_CORE_WARN("MaterialSerializer: Unclosed properties object in JSON");
			return; // 格式错误
		}
		
		propertiesEnd--; // 调整到最后一个大括号位置
		std::string propertiesContent = content.substr(propertiesObj + 1, propertiesEnd - propertiesObj - 1);

		// 解析每个属性
		ParseIndividualProperties(propertiesContent, material);
	}

	void MaterialSerializer::ParseIndividualProperties(const std::string& propertiesContent, Ref<Material> material)
	{
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
				HZ_CORE_TRACE("MaterialSerializer: Property '{0}' in file not found in shader, skipping", name);
				
				// 跳过此属性的剩余部分，找到下一个属性开始
				size_t nextPropStart = propertiesContent.find("\"", nameEnd + 1);
				if (nextPropStart == std::string::npos) break;
				
				pos = nextPropStart;
				continue;
			}
			
			// 解析属性值
			pos = ParseSingleProperty(propertiesContent, material, name, nameEnd);
		}
	}

	size_t MaterialSerializer::ParseSingleProperty(const std::string& propertiesContent, Ref<Material> material, 
		const std::string& name, size_t nameEnd)
	{
		// 跳过冒号
		size_t colonPos = propertiesContent.find(":", nameEnd);
		if (colonPos == std::string::npos) return propertiesContent.length();
		
		// 查找属性对象开始的大括号
		size_t propObjStart = propertiesContent.find("{", colonPos);
		if (propObjStart == std::string::npos) return propertiesContent.length();
		
		// 查找属性对象结束的大括号
		size_t propObjEnd = propObjStart + 1;
		size_t depth = 1;
		
		while (depth > 0 && propObjEnd < propertiesContent.length()) {
			if (propertiesContent[propObjEnd] == '{') {
				depth++;
			} else if (propertiesContent[propObjEnd] == '}') {
				depth--;
			}
			propObjEnd++;
		}
		
		if (depth > 0) return propertiesContent.length(); // 没有找到匹配的结束括号
		
		propObjEnd--; // 调整到最后一个大括号位置
		
		std::string propContent = propertiesContent.substr(propObjStart + 1, propObjEnd - propObjStart - 1);
		
		// 解析类型和值
		ParsePropertyTypeAndValue(propContent, material, name);
		
		return propObjEnd + 1;
	}

	void MaterialSerializer::ParsePropertyTypeAndValue(const std::string& propContent, Ref<Material> material, 
		const std::string& name)
	{
		// 解析类型
		size_t typeStart = propContent.find("\"type\"");
		if (typeStart == std::string::npos) return;
		
		size_t typeValueStart = propContent.find("\"", typeStart + 7);
		if (typeValueStart == std::string::npos) return;
		
		typeValueStart++; // 跳过引号
		
		size_t typeValueEnd = propContent.find("\"", typeValueStart);
		if (typeValueEnd == std::string::npos) return;
		
		std::string type = propContent.substr(typeValueStart, typeValueEnd - typeValueStart);
		
		// 解析值
		size_t valueStart = propContent.find("\"value\"");
		if (valueStart == std::string::npos) return;

		// 根据类型解析值
		if (type == "float") {
			ParseFloatValue(propContent, material, name, valueStart);
		} else if (type == "vec2") {
			ParseVec2Value(propContent, material, name, valueStart);
		} else if (type == "vec3") {
			ParseVec3Value(propContent, material, name, valueStart);
		} else if (type == "vec4") {
			ParseVec4Value(propContent, material, name, valueStart);
		} else if (type == "int") {
			ParseIntValue(propContent, material, name, valueStart);
		} else if (type == "bool") {
			ParseBoolValue(propContent, material, name, valueStart);
		} else if (type == "mat4") {
			ParseMat4Value(propContent, material, name, valueStart);
		} else if (type == "texture2d") {
			ParseTextureValue(propContent, material, name, valueStart);
		}
	}

	void MaterialSerializer::ParseFloatValue(const std::string& propContent, Ref<Material> material, 
		const std::string& name, size_t valueStart)
	{
		size_t valueNumStart = propContent.find_first_of("0123456789.-", valueStart);
		if (valueNumStart == std::string::npos) return;
		
		size_t valueNumEnd = propContent.find_first_not_of("0123456789.-", valueNumStart);
		if (valueNumEnd == std::string::npos) {
			valueNumEnd = propContent.length();
		}
		
		std::string valueStr = propContent.substr(valueNumStart, valueNumEnd - valueNumStart);
		float value = std::stof(valueStr);
		material->Set(name, value);
		HZ_CORE_TRACE("MaterialSerializer: Set property '{0}' to float value {1}", name, value);
	}

	void MaterialSerializer::ParseVec2Value(const std::string& propContent, Ref<Material> material, 
		const std::string& name, size_t valueStart)
	{
		size_t valueArrayStart = propContent.find("[", valueStart);
		if (valueArrayStart == std::string::npos) return;
		
		size_t valueArrayEnd = propContent.find("]", valueArrayStart);
		if (valueArrayEnd == std::string::npos) return;
		
		std::string arrayContent = propContent.substr(valueArrayStart + 1, valueArrayEnd - valueArrayStart - 1);
		
		size_t commaPos = arrayContent.find(",");
		if (commaPos == std::string::npos) return;
		
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
		HZ_CORE_TRACE("MaterialSerializer: Set property '{0}' to vec2 value ({1}, {2})", name, x, y);
	}

	void MaterialSerializer::ParseVec3Value(const std::string& propContent, Ref<Material> material, 
		const std::string& name, size_t valueStart)
	{
		size_t valueArrayStart = propContent.find("[", valueStart);
		if (valueArrayStart == std::string::npos) return;
		
		size_t valueArrayEnd = propContent.find("]", valueArrayStart);
		if (valueArrayEnd == std::string::npos) return;
		
		std::string arrayContent = propContent.substr(valueArrayStart + 1, valueArrayEnd - valueArrayStart - 1);
		
		// 找到两个逗号分隔的三个值
		size_t firstComma = arrayContent.find(",");
		if (firstComma == std::string::npos) return;
		
		size_t secondComma = arrayContent.find(",", firstComma + 1);
		if (secondComma == std::string::npos) return;
		
		std::string xStr = arrayContent.substr(0, firstComma);
		std::string yStr = arrayContent.substr(firstComma + 1, secondComma - firstComma - 1);
		std::string zStr = arrayContent.substr(secondComma + 1);
		
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
		HZ_CORE_TRACE("MaterialSerializer: Set property '{0}' to vec3 value ({1}, {2}, {3})", name, x, y, z);
	}

	void MaterialSerializer::ParseVec4Value(const std::string& propContent, Ref<Material> material, 
		const std::string& name, size_t valueStart)
	{
		size_t valueArrayStart = propContent.find("[", valueStart);
		if (valueArrayStart == std::string::npos) return;
		
		size_t valueArrayEnd = propContent.find("]", valueArrayStart);
		if (valueArrayEnd == std::string::npos) return;
		
		std::string arrayContent = propContent.substr(valueArrayStart + 1, valueArrayEnd - valueArrayStart - 1);
		
		// 找到三个逗号分隔的四个值
		size_t firstComma = arrayContent.find(",");
		if (firstComma == std::string::npos) return;
		
		size_t secondComma = arrayContent.find(",", firstComma + 1);
		if (secondComma == std::string::npos) return;
		
		size_t thirdComma = arrayContent.find(",", secondComma + 1);
		if (thirdComma == std::string::npos) return;
		
		std::string xStr = arrayContent.substr(0, firstComma);
		std::string yStr = arrayContent.substr(firstComma + 1, secondComma - firstComma - 1);
		std::string zStr = arrayContent.substr(secondComma + 1, thirdComma - secondComma - 1);
		std::string wStr = arrayContent.substr(thirdComma + 1);
		
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
		HZ_CORE_TRACE("MaterialSerializer: Set property '{0}' to vec4 value ({1}, {2}, {3}, {4})", name, x, y, z, w);
	}

	void MaterialSerializer::ParseIntValue(const std::string& propContent, Ref<Material> material, 
		const std::string& name, size_t valueStart)
	{
		size_t valueNumStart = propContent.find_first_of("0123456789-", valueStart);
		if (valueNumStart == std::string::npos) return;
		
		size_t valueNumEnd = propContent.find_first_not_of("0123456789-", valueNumStart);
		if (valueNumEnd == std::string::npos) {
			valueNumEnd = propContent.length();
		}
		
		std::string valueStr = propContent.substr(valueNumStart, valueNumEnd - valueNumStart);
		int value = std::stoi(valueStr);
		material->Set(name, value);
		HZ_CORE_TRACE("MaterialSerializer: Set property '{0}' to int value {1}", name, value);
	}

	void MaterialSerializer::ParseBoolValue(const std::string& propContent, Ref<Material> material, 
		const std::string& name, size_t valueStart)
	{
		size_t truePos = propContent.find("true", valueStart);
		size_t falsePos = propContent.find("false", valueStart);
		
		bool value = false;
		if (truePos != std::string::npos && (falsePos == std::string::npos || truePos < falsePos)) {
			value = true;
		} else if (falsePos != std::string::npos) {
			value = false;
		} else {
			return; // 找不到有效的布尔值
		}
		
		material->Set(name, value);
		HZ_CORE_TRACE("MaterialSerializer: Set property '{0}' to bool value {1}", name, value ? "true" : "false");
	}

	void MaterialSerializer::ParseMat4Value(const std::string& propContent, Ref<Material> material, 
		const std::string& name, size_t valueStart)
	{
		size_t valueArrayStart = propContent.find("[", valueStart);
		if (valueArrayStart == std::string::npos) return;
		
		size_t valueArrayEnd = propContent.find_last_of("]");
		if (valueArrayEnd == std::string::npos) return;
		
		std::string arrayContent = propContent.substr(valueArrayStart + 1, valueArrayEnd - valueArrayStart - 1);
		
		// 解析4x4矩阵，每行是一个数组
		glm::mat4 matrix(1.0f);
		size_t pos = 0;
		
		for (int row = 0; row < 4; row++) {
			size_t rowStart = arrayContent.find("[", pos);
			if (rowStart == std::string::npos) return;
			
			size_t rowEnd = arrayContent.find("]", rowStart);
			if (rowEnd == std::string::npos) return;
			
			std::string rowContent = arrayContent.substr(rowStart + 1, rowEnd - rowStart - 1);
			
			// 解析行中的4个值
			size_t valuePos = 0;
			for (int col = 0; col < 4; col++) {
				size_t nextComma = rowContent.find(",", valuePos);
				std::string valueStr;
				
				if (col == 3 || nextComma == std::string::npos) {
					valueStr = rowContent.substr(valuePos);
				} else {
					valueStr = rowContent.substr(valuePos, nextComma - valuePos);
				}
				
				// 去除前后空格
				valueStr.erase(0, valueStr.find_first_not_of(" \t"));
				valueStr.erase(valueStr.find_last_not_of(" \t") + 1);
				
				matrix[row][col] = std::stof(valueStr);
				
				if (nextComma != std::string::npos) {
					valuePos = nextComma + 1;
				}
			}
			
			pos = rowEnd + 1;
		}
		
		material->Set(name, matrix);
		HZ_CORE_TRACE("MaterialSerializer: Set property '{0}' to mat4 value", name);
	}

	void MaterialSerializer::ParseTextureValue(const std::string& propContent, Ref<Material> material, 
		const std::string& name, size_t valueStart)
	{
		size_t valueStrStart = propContent.find("\"", valueStart);
		if (valueStrStart == std::string::npos) return;
		
		valueStrStart++; // 跳过引号
		
		size_t valueStrEnd = propContent.find("\"", valueStrStart);
		if (valueStrEnd == std::string::npos) return;
		
		std::string texturePath = propContent.substr(valueStrStart, valueStrEnd - valueStrStart);
		
		if (!texturePath.empty()) {
			Ref<Texture2D> texture = Texture2D::Create(texturePath);
			if (texture) {
				material->Set(name, texture);
				HZ_CORE_TRACE("MaterialSerializer: Set property '{0}' to texture '{1}'", name, texturePath);
			} else {
				HZ_CORE_WARN("MaterialSerializer: Failed to load texture '{0}' for property '{1}'", texturePath, name);
			}
		} else {
			// 空路径，设置为nullptr
			material->Set(name, Ref<Texture2D>());
			HZ_CORE_TRACE("MaterialSerializer: Set property '{0}' to null texture", name);
		}
	}
} 