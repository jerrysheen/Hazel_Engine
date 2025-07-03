#pragma once
#include "hzpch.h"
#include "Runtime/Graphics/Shader/Shader.h"
#include "Runtime/Graphics/Shader/ShaderReflection.h"
#include <d3d12.h>
#include <d3dcompiler.h>
using Microsoft::WRL::ComPtr;

namespace Hazel 
{
	// D3D12 特定的着色器反射实现
	class D3D12ShaderReflection : public ShaderReflection
	{
	public:
		D3D12ShaderReflection();
		virtual ~D3D12ShaderReflection() override;

		// 新增：Stage-based 接口实现
		virtual std::vector<StageResourceInfo> ReflectStageResources() override;
		virtual StageResourceInfo* GetStageResources(ShaderStage stage) const override;
		virtual bool HasStage(ShaderStage stage) const override;
		virtual std::vector<ShaderStage> GetAvailableStages() const override;

		// InputLayout 接口实现（只属于顶点着色器）
		virtual BufferLayout ReflectVertexInputLayout() override;
		
		// Stage-specific 查询接口
		virtual Ref<ShaderRegisterBlock> GetRegisterBlockByName(ShaderStage stage, const std::string& name) override;
		virtual Ref<ShaderRegisterBlock> GetRegisterBlockByBindPoint(ShaderStage stage, uint32_t bindPoint, uint32_t space = 0) override;
		virtual Ref<ShaderParameter> GetParameterByName(ShaderStage stage, const std::string& name) override;

		// 新的核心方法：添加Stage反射（替换MergeReflection）
		void AddStageReflection(ShaderStage stage, ID3DBlob* shaderBytecode);
		
		// 清除所有Stage数据
		void Clear();
		
		// 获取特定Stage的原始D3D反射接口（用于高级操作）
		ComPtr<ID3D12ShaderReflection> GetD3DReflection(ShaderStage stage) const;

	private:
		// Stage-based 数据存储
		std::array<std::unique_ptr<StageResourceInfo>, static_cast<size_t>(ShaderStage::Count)> m_StageResources;
		std::array<ComPtr<ID3D12ShaderReflection>, static_cast<size_t>(ShaderStage::Count)> m_D3DReflections;
		


		// 私有辅助方法
		void ReflectStageShader(ShaderStage stage, ID3DBlob* shaderBytecode);
		StageResourceInfo CreateStageResourceInfo(ShaderStage stage, ID3D12ShaderReflection* reflection);
		

		
		// 计算绑定点键值
		static uint64_t CalculateBindPointKey(uint32_t bindPoint, uint32_t space) {
			return (static_cast<uint64_t>(space) << 32) | bindPoint;
		}
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