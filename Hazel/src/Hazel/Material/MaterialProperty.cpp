#include "hzpch.h"
#include "MaterialProperty.h"
#include <stdexcept>

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

	// MaterialProperty::GetValue 模板特化实现
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
} 