#pragma once

#include <string>
#include "Hazel/Core/Core.h"
#include "glm/glm.hpp"
#include "Buffer.h"
#include "ShaderReflection.h"

namespace Hazel {

	class Shader 
	{
	public:
		virtual ~Shader() = default;

		// 核心着色器操作
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		// 参数设置接口（用于直接设置着色器参数）
		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;
		virtual void SetMat3(const std::string& name, const glm::mat3& value) = 0;

		// 着色器信息获取
		virtual const std::string& GetName() const = 0;
		
		// 获取顶点输入布局
		virtual const BufferLayout& GetInputLayout() const = 0;
		
		// 获取着色器反射
		virtual Ref<ShaderReflection> GetReflection() const = 0;
		
		// 获取着色器字节码（平台相关）
		virtual const void* GetByteCode() const = 0;
		virtual size_t GetByteCodeSize() const = 0;

		// 静态工厂方法
		static Ref<Shader> Create(const std::string& filepath);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	};

} // namespace Hazel