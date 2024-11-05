#pragma once
#include "hzpch.h"
#include "Hazel/Renderer/Shader.h"

namespace Hazel 
{
	class D3D12Shader : public Shader
	{
		public:
		D3D12Shader(const std::string& filepath);
		D3D12Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		~D3D12Shader();

		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;
		virtual void SetMat3(const std::string& name, const glm::mat3& value) override;

		const std::string& GetName() const { return m_Name; }

		private:
		std::string m_Name;
	};

}