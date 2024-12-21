#include "hzpch.h"
#include "D3D12Shader.h"

namespace Hazel
{
	D3D12Shader::D3D12Shader(const std::string& filepath)
	{
		std::wstring wstr(filepath.begin(), filepath.end());  // 直接使用构造函数
		m_VsByteCode = d3dUtil::CompileShader(wstr, nullptr, "VS", "vs_5_0");
		m_PsByteCode = d3dUtil::CompileShader(wstr, nullptr, "PS", "ps_5_0");

	}

	D3D12Shader::D3D12Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
	}

	D3D12Shader::~D3D12Shader()
	{
	}

	void D3D12Shader::Bind() const
	{
	}

	void D3D12Shader::UnBind() const
	{
	}

	void D3D12Shader::SetInt(const std::string& name, int value)
	{
	}

	void D3D12Shader::SetFloat(const std::string& name, float value)
	{
	}

	void D3D12Shader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
	}

	void D3D12Shader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
	}

	void D3D12Shader::SetMat4(const std::string& name, const glm::mat4& value)
	{
	}

	void D3D12Shader::SetMat3(const std::string& name, const glm::mat3& value)
	{
	}
}