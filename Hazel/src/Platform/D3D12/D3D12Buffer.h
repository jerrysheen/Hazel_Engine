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
		virtual void SetLayout(const BufferLayout& layout) override;
		DXGI_FORMAT GetLayOutFormat(const ShaderDataType& type);
	public:
		// 一个GPU buffer，一个Upload buffer来上传数据
		Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;

		// Data about the buffers.
		UINT VertexByteStride = 0;
		UINT VertexBufferByteSize = 0;

		// 这个Buffer具体的layout， 根据上层的BufferLayout进行抽象。
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_D3DInputLayout;
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