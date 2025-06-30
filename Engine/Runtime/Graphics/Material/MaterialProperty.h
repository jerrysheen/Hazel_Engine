#pragma once

#include "hzpch.h"
#include <variant>
#include <glm/glm.hpp>
#include "Runtime/Graphics/Texture/Texture.h"

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
		// 构造函数
		MaterialProperty();
		MaterialProperty(float value);
		MaterialProperty(const glm::vec2& value);
		MaterialProperty(const glm::vec3& value);
		MaterialProperty(const glm::vec4& value);
		MaterialProperty(int value);
		MaterialProperty(bool value);
		MaterialProperty(const Ref<Texture2D>& value);
		MaterialProperty(const glm::mat4& value);
        
		// 复制构造函数和赋值运算符
		MaterialProperty(const MaterialProperty& other);
		MaterialProperty& operator=(const MaterialProperty& other);
        
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
		
		// 使用std::variant存储不同类型的值
		std::variant<
			std::monostate,
			float, glm::vec2, glm::vec3, glm::vec4,
			int, bool,
			Ref<Texture2D>,
			glm::mat4
		> m_Value;
	};

	// 材质属性块 - 对应着色器中的一个常量缓冲区
	struct MaterialPropertyBlock {
		std::vector<float> RawData;        // 内存块，按照着色器布局优化
		uint32_t Size;                     // 缓冲区大小
		uint32_t BindPoint;                // 绑定点（寄存器号）
		uint32_t BindSpace;                // 绑定空间
		bool Dirty;                        // 脏标记，表示数据需要更新
        
		// 记录属性名称到原始数据偏移量的映射
		std::unordered_map<std::string, uint32_t> PropertyOffsets;
	};

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
} 