#pragma once

#include "Hazel/Renderer/Shader.h"
#include <glm/glm.hpp>

typedef unsigned int GLenum;

namespace Hazel {

// OpenGLShaderReflection类实现ShaderReflection接口
class OpenGLShaderReflection : public ShaderReflection
{
public:
	OpenGLShaderReflection(uint32_t program);
	virtual ~OpenGLShaderReflection() = default;
	
	// 实现ShaderReflection的方法
	virtual BufferLayout ReflectVertexInputLayout() override;
	virtual std::vector<ShaderRegisterBlock> ReflectRegisterBlocks() override;
	virtual std::vector<ResourceBinding> ReflectResourceBindings() override;

	// 通过名称获取寄存器块
	virtual Ref<ShaderRegisterBlock> GetRegisterBlockByName(const std::string& name) override;
	
	// 通过绑定点获取寄存器块
	virtual Ref<ShaderRegisterBlock> GetRegisterBlockByBindPoint(uint32_t bindPoint, uint32_t space = 0) override;
	
	// 通过名称获取参数
	virtual Ref<ShaderParameter> GetParameterByName(const std::string& name) override;

private:
	uint32_t m_RendererID;
	BufferLayout m_InputLayout;
	std::vector<ShaderRegisterBlock> m_RegisterBlocks;
	std::vector<ResourceBinding> m_ResourceBindings;

	// 缓存映射
	std::unordered_map<std::string, size_t> m_RegisterBlockNameToIndex;
	std::unordered_map<uint64_t, size_t> m_RegisterBlockBindPointToIndex;
	std::unordered_map<std::string, ShaderParameter> m_ParameterCache;

	// 是否已反射
	bool m_HasReflectedInputLayout = false;
	bool m_HasReflectedRegisterBlocks = false;
	bool m_HasReflectedResourceBindings = false;
};

class OpenGLShader : public Shader
{
public:
	OpenGLShader(const std::string& filepath);
	OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	virtual ~OpenGLShader();

	virtual const std::string& GetName() const override { return m_Name; };
	virtual void Bind() const override;
	virtual void UnBind() const override;

	void UploadUniformInt(const std::string& name, int value);
	virtual void SetInt(const std::string& name, int value) override;
	virtual void SetFloat(const std::string& name, float value) override;
	virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
	virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
	virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
	virtual void SetMat4(const std::string& name, const glm::mat4& value) override;
	virtual void SetMat3(const std::string& name, const glm::mat3& value) override;

	void UploadUniformFloat(const std::string& name, float value);
	void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
	void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
	void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

	void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
	void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
	
	// 实现Shader类中的剩余方法
	virtual const BufferLayout& GetInputLayout() const override { return m_InputLayout; }
	virtual Ref<ShaderReflection> GetReflection() const override { return m_Reflection; }
	virtual const void* GetByteCode() const override { return m_ByteCode.data(); }
	virtual size_t GetByteCodeSize() const override { return m_ByteCode.size(); }

private:
	std::string ReadFile(const std::string& filepath);
	std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
	void Compile(std::unordered_map<GLenum, std::string>& shaderSources);
	void CreateReflection();
	
private:
	uint32_t m_RendererID;
	std::string m_Name;
	
	// 新增成员变量
	BufferLayout m_InputLayout;
	Ref<OpenGLShaderReflection> m_Reflection;
	std::vector<uint8_t> m_ByteCode; // 存储着色器字节码
};

}