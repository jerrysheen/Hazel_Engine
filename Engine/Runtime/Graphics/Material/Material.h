#pragma once

#include "hzpch.h"
#include "MaterialProperty.h"
#include "Runtime/Graphics/Renderer/Shader.h"
#include <unordered_map>

namespace Hazel 
{
	// 材质核心类 - 负责材质的核心逻辑
	class Material {
	public:
		// 创建方法
		static Ref<Material> Create(const Ref<Shader>& shader);
		
		// 构造函数
		Material(const Ref<Shader>& shader);
		
		// 核心功能
		virtual void Bind() const;
		virtual Ref<Material> Clone() const;
		
		// 属性设置和获取方法
		template<typename T>
		void Set(const std::string& name, const T& value);
		
		template<typename T>
		T Get(const std::string& name) const;
		
		bool HasProperty(const std::string& name) const;
		const Ref<Shader>& GetShader() const { return m_Shader; }
		
		// 同步属性到优化的内存布局
		void SyncToRawData();
        
		// 获取原始数据，供渲染系统使用
		const MaterialPropertyBlock* GetPropertyBlock(uint32_t bindPoint, uint32_t bindSpace = 0) const;
		bool HasPropertyBlock(uint32_t bindPoint, uint32_t bindSpace = 0) const;
		
		// 材质标识符
		const std::string& GetMaterialID() const { return m_MaterialID; }
		void SetMaterialID(const std::string& id) { m_MaterialID = id; }

	protected:
		Ref<Shader> m_Shader;
		std::unordered_map<std::string, MaterialProperty> m_Properties;
		
		// 常量缓冲区块集合，按bindPoint组织
		std::unordered_map<uint64_t, MaterialPropertyBlock> m_PropertyBlocks;
		
		// 材质的唯一标识符（基于文件路径的哈希值）
		std::string m_MaterialID;
		
	private:
		// 从着色器反射中同步属性
		void SyncWithShaderReflection();
		
		// 根据反射数据创建属性
		void CreatePropertyFromReflection(const std::string& name, uint32_t size);
        
		// 根据反射数据创建属性块
		void CreatePropertyBlocks();
        
		// 标记属性为脏，需要更新
		void MarkPropertyDirty(const std::string& name);
        
		// 计算属性块键值
		uint64_t CalculateBlockKey(uint32_t bindPoint, uint32_t bindSpace) const;
		
		// 从属性大小推断着色器数据类型
		ShaderDataType InferShaderDataTypeFromSize(uint32_t size);
		
		// 获取材质属性类型对应的大小
		uint32_t GetSizeFromMaterialPropertyType(MaterialPropertyType type);
		
		// 检查属性类型是否与着色器类型匹配
		bool IsPropertyTypeMatchingShaderType(MaterialPropertyType propType, const std::string& paramName);
		
		// 友元类，允许序列化器和材质库访问私有成员
		friend class MaterialSerializer;
		friend class MaterialLibrary;
	};

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