#pragma once

#include "hzpch.h"
#include "../Renderer/Shader.h"
#include "Hazel/Renderer/Texture.h"
#include <variant>
#include <glm/glm.hpp>

//#include "../Renderer/Renderer3D.h"

namespace Hazel 
{
	// 材质属性类型
	enum class MaterialPropertyType {
		None = 0,
		Float, Float2, Float3, Float4,
		Int, Int2, Int3, Int4,
		Bool,
		Matrix3, Matrix4,
		Texture2D, TextureCube, Texture3D
	};

	// 材质属性值
	class MaterialProperty {
	public:
		// 构造函数和访问器
		MaterialProperty();
		MaterialProperty(float value);
		MaterialProperty(const glm::vec2& value);
		MaterialProperty(const glm::vec3& value);
		MaterialProperty(const glm::vec4& value);
		MaterialProperty(int value);
		MaterialProperty(bool value);
		MaterialProperty(const Ref<Texture2D>& value);
		MaterialProperty(const glm::mat4& value);
        
		// 获取属性类型
		MaterialPropertyType GetType() const { return m_Type; }
        
		// 获取属性大小
		uint32_t GetSize() const { return m_Size; }

		// 获取属性值（类型安全的访问器）
		template<typename T>
		T& GetValue();

		template<typename T>
		const T& GetValue() const;
        
	private:
		MaterialPropertyType m_Type;
		uint32_t m_Size = 0;
		std::variant<
			std::monostate,
			float, glm::vec2, glm::vec3, glm::vec4,
			int, bool,
			Ref<Texture2D>, Ref<Texture3D>,
			glm::mat4
		> m_Value;
	};

	// 材质基类
	class Material {
	public:
		static Ref<Material> Create(const Ref<Shader>& shader);
		static Ref<Material> CreateFromMeta(const std::string& path);
		virtual void Bind() const;
		virtual Ref<Material> Clone() const;
		
		// 属性设置和获取方法
		template<typename T>
		void Set(const std::string& name, const T& value);
		
		template<typename T>
		T Get(const std::string& name) const;
		
		bool HasProperty(const std::string& name) const;
		const Ref<Shader>& GetShader() const;
		
		// 序列化和反序列化
		void SerializeToJSON(const std::string& filepath);
		static Ref<Material> DeserializeFromJSON(const std::string& filepath);
		
		Material(const Ref<Shader>& shader);

	protected:
        
		Ref<Shader> m_Shader;
		std::unordered_map<std::string, MaterialProperty> m_Properties;
		//GraphicsPipelineDesc m_PipelineDesc;
		//Ref<GraphicsPipeline> m_Pipeline;
		
	private:
		// 从着色器反射中同步属性
		void SyncWithShaderReflection();
		
		// 根据反射数据创建属性
		void CreatePropertyFromReflection(const std::string& name, uint32_t size);
		
		// 从属性大小推断着色器数据类型
		ShaderDataType InferShaderDataTypeFromSize(uint32_t size);
		
		// 获取材质属性类型对应的大小
		uint32_t GetSizeFromMaterialPropertyType(MaterialPropertyType type);
		
		// 检查属性类型是否与着色器类型匹配
		bool IsPropertyTypeMatchingShaderType(MaterialPropertyType propType, const std::string& paramName);
	};

	// 材质库
	class MaterialLibrary {
	public:
		static MaterialLibrary& Get();
		
		void Register(const std::string& name, const Ref<Material>& material);
		Ref<Material> Get(const std::string& name);
		bool Exists(const std::string& name) const;
		
		bool Save(const std::string& name, const std::string& filepath);
		Ref<Material> Load(const std::string& filepath);
		
	private:
		std::unordered_map<std::string, Ref<Material>> m_Materials;
	};

// // 特定材质类型
// class PBRMaterial : public Material {
//     // PBR特定属性和方法...
// };

// class UnlitMaterial : public Material {
//     // Unlit特定属性和方法...
// };

// MaterialProperty模板特化声明
template<> float& MaterialProperty::GetValue<float>();
template<> const float& MaterialProperty::GetValue<float>() const;
template<> glm::vec2& MaterialProperty::GetValue<glm::vec2>();
template<> const glm::vec2& MaterialProperty::GetValue<glm::vec2>() const;
template<> glm::vec3& MaterialProperty::GetValue<glm::vec3>();
template<> const glm::vec3& MaterialProperty::GetValue<glm::vec3>() const;
template<> glm::vec4& MaterialProperty::GetValue<glm::vec4>();
template<> const glm::vec4& MaterialProperty::GetValue<glm::vec4>() const;
template<> int& MaterialProperty::GetValue<int>();
template<> const int& MaterialProperty::GetValue<int>() const;
template<> bool& MaterialProperty::GetValue<bool>();
template<> const bool& MaterialProperty::GetValue<bool>() const;
template<> Ref<Texture2D>& MaterialProperty::GetValue<Ref<Texture2D>>();
template<> const Ref<Texture2D>& MaterialProperty::GetValue<Ref<Texture2D>>() const;
template<> glm::mat4& MaterialProperty::GetValue<glm::mat4>();
template<> const glm::mat4& MaterialProperty::GetValue<glm::mat4>() const;

// Material模板特化声明
template<> void Material::Set<float>(const std::string& name, const float& value);
template<> void Material::Set<glm::vec2>(const std::string& name, const glm::vec2& value);
template<> void Material::Set<glm::vec3>(const std::string& name, const glm::vec3& value);
template<> void Material::Set<glm::vec4>(const std::string& name, const glm::vec4& value);
template<> void Material::Set<int>(const std::string& name, const int& value);
template<> void Material::Set<bool>(const std::string& name, const bool& value);
template<> void Material::Set<Ref<Texture2D>>(const std::string& name, const Ref<Texture2D>& value);
template<> void Material::Set<glm::mat4>(const std::string& name, const glm::mat4& value);

template<> float Material::Get<float>(const std::string& name) const;
template<> glm::vec2 Material::Get<glm::vec2>(const std::string& name) const;
template<> glm::vec3 Material::Get<glm::vec3>(const std::string& name) const;
template<> glm::vec4 Material::Get<glm::vec4>(const std::string& name) const;
template<> int Material::Get<int>(const std::string& name) const;
template<> bool Material::Get<bool>(const std::string& name) const;
template<> Ref<Texture2D> Material::Get<Ref<Texture2D>>(const std::string& name) const;
template<> glm::mat4 Material::Get<glm::mat4>(const std::string& name) const;

}