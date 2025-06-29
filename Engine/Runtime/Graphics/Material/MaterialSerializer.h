#pragma once

#include "Runtime/Core/Core.h"
#include <string>

namespace Hazel 
{
	class Material;
	class MaterialProperty;

	// 材质序列化器 - 负责材质的保存和加载
	class MaterialSerializer {
	public:
		MaterialSerializer() = default;
		
		// 序列化材质到 JSON 文件
		static void SerializeToJSON(const Ref<Material>& material, const std::string& filepath);
		
		// 从 JSON 文件反序列化材质
		static Ref<Material> DeserializeFromJSON(const std::string& filepath);
		
		// 序列化到字符串
		static std::string SerializeToString(const Ref<Material>& material);
		
		// 从字符串反序列化
		static Ref<Material> DeserializeFromString(const std::string& jsonString);
		
	private:
		// 内部序列化辅助方法
		static void SerializeProperty(const std::string& name, const MaterialProperty& property, void* jsonObject);
		static void DeserializeProperty(const std::string& name, MaterialProperty& property, void* jsonObject);
		
		// JSON解析辅助方法
		static void ParseProperties(const std::string& content, Ref<Material> material);
		static void ParseIndividualProperties(const std::string& propertiesContent, Ref<Material> material);
		static size_t ParseSingleProperty(const std::string& propertiesContent, Ref<Material> material, 
			const std::string& name, size_t nameEnd);
		static void ParsePropertyTypeAndValue(const std::string& propContent, Ref<Material> material, 
			const std::string& name);
			
		// 类型特定的值解析方法
		static void ParseFloatValue(const std::string& propContent, Ref<Material> material, 
			const std::string& name, size_t valueStart);
		static void ParseVec2Value(const std::string& propContent, Ref<Material> material, 
			const std::string& name, size_t valueStart);
		static void ParseVec3Value(const std::string& propContent, Ref<Material> material, 
			const std::string& name, size_t valueStart);
		static void ParseVec4Value(const std::string& propContent, Ref<Material> material, 
			const std::string& name, size_t valueStart);
		static void ParseIntValue(const std::string& propContent, Ref<Material> material, 
			const std::string& name, size_t valueStart);
		static void ParseBoolValue(const std::string& propContent, Ref<Material> material, 
			const std::string& name, size_t valueStart);
		static void ParseMat4Value(const std::string& propContent, Ref<Material> material, 
			const std::string& name, size_t valueStart);
		static void ParseTextureValue(const std::string& propContent, Ref<Material> material, 
			const std::string& name, size_t valueStart);
	};
} 