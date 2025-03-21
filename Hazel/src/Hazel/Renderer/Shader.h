#pragma once

#include <string>
#include "Hazel/Core/Core.h"
#include <unordered_map>
#include "glm/glm.hpp"
#include "Buffer.h"

namespace Hazel {
	
	// ShaderParameter类，表示着色器中的常量参数
	struct ShaderParameter
	{
		std::string Name;
		uint32_t Size;
		uint32_t Offset;
		uint32_t BindPoint;
		// 可以根据需要扩展更多属性，如类型、寄存器等
	};

	// 资源绑定描述
	struct ResourceBinding
	{
		std::string Name;
		uint32_t BindPoint;
		uint32_t BindSpace;
		// 可扩展更多属性，如类型（SRV、UAV、CBV等）
	};

	// ShaderReflection类，用于分析着色器并提取元数据
	class ShaderReflection
	{
	public:
		virtual ~ShaderReflection() = default;
		
		// 反射顶点输入布局
		virtual BufferLayout ReflectVertexInputLayout() = 0;
		
		// 反射着色器参数
		virtual std::vector<ShaderParameter> ReflectParameters() = 0;
		
		// 反射资源绑定
		virtual std::vector<ResourceBinding> ReflectResourceBindings() = 0;
	};

	class Shader 
	{
	public:
		virtual ~Shader() {};

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;
		virtual void SetMat3(const std::string& name, const glm::mat3& value) = 0;

		virtual const std::string& GetName() const = 0;
		
		// 获取顶点输入布局
		virtual const BufferLayout& GetInputLayout() const = 0;
		
		// 获取着色器反射
		virtual Ref<ShaderReflection> GetReflection() const = 0;
		
		// 获取着色器字节码（平台相关）
		virtual const void* GetByteCode() const = 0;
		virtual size_t GetByteCodeSize() const = 0;

		static Ref<Shader> Create(const std::string& filepath);
		static Ref<Shader> Create(const std::string& name,const std::string& vertexSrc, const std::string& fragmentSrc);

	};

	class ShaderLibrary
	{
	public:
		void Add(const Ref<Shader>& shader);
		void Add(const std::string& name, const Ref<Shader>& shader);
		Ref<Shader> Load(const std::string& filepath); // assets/Texture.glsl
		Ref<Shader> Load(const std::string& name, const std::string& filepath);
		
		Ref<Shader> Get(const std::string& name);
		bool Exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}