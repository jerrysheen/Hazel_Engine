#pragma once
#include "hzpch.h"
#include "Runtime/Graphics/RHI/Core/Buffer.h"

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
		D3D12VertexBuffer(float* vertices, uint32_t size, uint32_t stride);
		virtual ~D3D12VertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		//virtual const BufferLayout& GetLayout()  const override { return m_Layout; };
		//virtual void SetLayout(const BufferLayout& layout) override;
	public:
		// һ��GPU buffer��һ��Upload buffer���ϴ�����
		Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;

		// Data about the buffers.
		//UINT VertexByteStride = 0;
		//UINT VertexBufferByteSize = 0;


	};

	class D3D12IndexBuffer : public IndexBuffer
	{
	public:
		D3D12IndexBuffer(uint16_t* indices, uint32_t size);
		virtual ~D3D12IndexBuffer();

		virtual void Bind() const;
		virtual void Unbind() const;

		virtual uint32_t GetCount() const { return m_Count; };

		Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;
		inline DXGI_FORMAT GetIndexFormat() { return DXGI_FORMAT_R16_UINT; }
		inline uint32_t GetIndexBufferSize() { return IndexBufferByteSize; }
	private:
		uint32_t m_Count;
		uint32_t m_RendererID;

		// һ��GPU buffer��һ��Upload buffer���ϴ�����
		Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;
		DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
		UINT IndexBufferByteSize = 0;
		// Data about the buffers.
		//UINT VertexByteStride = 0;
		//UINT VertexBufferByteSize = 0;
	};


}