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
		// 可以根据需要扩展更多属性，如类型、寄存器等
	};

	// 着色器寄存器块描述 - 表示一个寄存器(如b0)所对应的所有数据
	struct ShaderRegisterBlock
	{
		std::string Name;         // cbuffer的名称
		uint32_t BindPoint;       // 绑定点(比如b0中的0)
		uint32_t BindSpace;       // 绑定空间
		uint32_t Size;            // 缓冲区大小
		std::vector<ShaderParameter> Parameters; // 缓冲区中包含的参数
	};

	// 资源类型枚举
	enum class ResourceType
	{
		ConstantBuffer,    // 常量缓冲区(b)
		ShaderResource,    // 着色器资源视图(t)
		UnorderedAccess,   // 无序访问视图(u)
		Sampler           // 采样器(s)
	};

	// 资源绑定描述
	struct ResourceBinding
	{
		std::string Name;
		ResourceType Type;
		uint32_t BindPoint;
		uint32_t BindSpace;
		// 如果是ConstantBuffer类型，对应的寄存器块索引
		int RegisterBlockIndex = -1;
	};

	// ShaderReflection类，用于分析着色器并提取元数据
	class ShaderReflection
	{
	public:
		virtual ~ShaderReflection() = default;
		
		// 反射顶点输入布局
		virtual BufferLayout ReflectVertexInputLayout() = 0;
		
		// 反射寄存器块
		virtual std::vector<ShaderRegisterBlock> ReflectRegisterBlocks() = 0;
		
		// 反射资源绑定
		virtual std::vector<ResourceBinding> ReflectResourceBindings() = 0;

		// 通过名称获取寄存器块
		virtual Ref<ShaderRegisterBlock> GetRegisterBlockByName(const std::string& name) = 0;
		
		// 通过绑定点获取寄存器块
		virtual Ref<ShaderRegisterBlock> GetRegisterBlockByBindPoint(uint32_t bindPoint, uint32_t space = 0) = 0;
		
		// 通过名称获取参数
		virtual Ref<ShaderParameter> GetParameterByName(const std::string& name) = 0;
	};

	class Shader 
	{
	public:
		virtual ~Shader() {};

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
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