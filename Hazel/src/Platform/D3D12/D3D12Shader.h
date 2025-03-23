#pragma once
#include "hzpch.h"
#include "Hazel/Renderer/Shader.h"
#include <d3d12.h>
#include <d3dcompiler.h>
using Microsoft::WRL::ComPtr;

namespace Hazel 
{
	// D3D12 特定的着色器反射实现
	class D3D12ShaderReflection : public ShaderReflection
	{
	public:
		D3D12ShaderReflection(ID3DBlob* shaderBytecode);
		virtual ~D3D12ShaderReflection();

		// 实现ShaderReflection接口
		virtual BufferLayout ReflectVertexInputLayout() override;
		virtual std::vector<ShaderParameter> ReflectParameters() override;
		virtual std::vector<ResourceBinding> ReflectResourceBindings() override;

	private:
		ComPtr<ID3D12ShaderReflection> m_Reflection;
		BufferLayout m_InputLayout; // 缓存反射结果
		std::vector<ShaderParameter> m_Parameters; // 缓存反射结果
		std::vector<ResourceBinding> m_ResourceBindings; // 缓存反射结果

		// 是否已反射
		bool m_HasReflectedInputLayout = false;
		bool m_HasReflectedParameters = false;
		bool m_HasReflectedResourceBindings = false;
	};

	class D3D12Shader : public Shader
	{
		public:
		D3D12Shader(const std::string& filepath);
		D3D12Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~D3D12Shader();

		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;
		virtual void SetMat3(const std::string& name, const glm::mat3& value) override;

		virtual const std::string& GetName() const override { return m_Name; }
		
		// 实现新增的接口
		virtual const BufferLayout& GetInputLayout() const override { return m_InputLayout; }
		virtual Ref<ShaderReflection> GetReflection() const override { return m_Reflection; }
		virtual const void* GetByteCode() const override { return m_VsByteCode ? m_VsByteCode->GetBufferPointer() : nullptr; }
		virtual size_t GetByteCodeSize() const override { return m_VsByteCode ? m_VsByteCode->GetBufferSize() : 0; }

		inline ComPtr<ID3DBlob> GetVSByteCode() const { return m_VsByteCode; }
		inline ComPtr<ID3DBlob> GetPSByteCode() const { return m_PsByteCode; }
		inline const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetD3D12InputLayout() const { return m_D3D12InputLayout; }

		private:
		void Compile(const std::string& source, const std::string& entryPoint, const std::string& target, ComPtr<ID3DBlob>& outByteCode);
		void CreateReflection();
		static DXGI_FORMAT GetDXGIFormat(ShaderDataType type);

		private:
		std::string m_Name;

		ComPtr<ID3DBlob> m_VsByteCode = nullptr;
		ComPtr<ID3DBlob> m_PsByteCode = nullptr;

		std::vector<D3D12_INPUT_ELEMENT_DESC> m_D3D12InputLayout;
		BufferLayout m_InputLayout; // 与D3D12_INPUT_ELEMENT_DESC对应的BufferLayout
		
		Ref<ShaderReflection> m_Reflection;
	};

}