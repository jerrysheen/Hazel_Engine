#pragma once
#include "hzpch.h"
#include "Hazel/Renderer/Buffer.h"

namespace Hazel 
{
	class D3D12Buffer : public ConstantBuffer
	{
	public:
		D3D12Buffer(uint32_t elementSize);
		virtual ~D3D12Buffer();
		virtual void SetData(void* srcData, int length) override;
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource>& mUploadBuffer;
		BYTE* mMappedData = nullptr;
	};


	class D3D12VertexBuffer : public VertexBuffer
	{
	public:
		D3D12VertexBuffer(float* vertices, uint32_t size);
		virtual ~D3D12VertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		//virtual const BufferLayout& GetLayout()  const override { return m_Layout; };
		virtual void SetLayout(const BufferLayout& layout) override { SetD3D12InputLayout(); };
		DXGI_FORMAT GetLayOutFormat(const ShaderDataType& type);
	private:
		void SetD3D12InputLayout();
		//BufferLayout m_Layout;
		// 这个Buffer具体的layout， 根据上层的BufferLayout进行抽象。
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_D3DInputLayout;
		uint32_t m_RendererID;
	};

	class D3D12IndexBuffer : public IndexBuffer
	{
	public:
		D3D12IndexBuffer(uint32_t* indices, uint32_t size);
		virtual ~D3D12IndexBuffer();

		virtual void Bind() const;
		virtual void Unbind() const;

		virtual uint32_t GetCount() const { return m_Count; };
	private:
		uint32_t m_Count;
		uint32_t m_RendererID;
	};


}