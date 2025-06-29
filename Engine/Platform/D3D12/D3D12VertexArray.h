#pragma once

#include "Runtime/Graphics/Renderer/VertexArray.h"
#include "Platform/D3D12/d3dx12.h"
namespace Hazel {

	class D3D12VertexArray : public VertexArray
	{
	public:
		D3D12VertexArray();

		virtual ~D3D12VertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		//virtual void SetLayout(const BufferLayout& layout) override;

		//virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; };

		//virtual const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputLayout() const { return m_D3DInputLayout; }
	private:
		DXGI_FORMAT GetLayOutFormat(const ShaderDataType& type);

		uint32_t m_RendererID;
		// VertexBuffer��������ݻ�ͷ�ǿ��Էֳɶ��Slot������D3D12֧�֣��ֳ�position only + ��Ȼ�����ݣ�����cache�Ѻá�
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;

		//// Inputlay ������ VertexArray���档
		//std::vector<D3D12_INPUT_ELEMENT_DESC> m_D3DInputLayout;
		UINT VertexByteStride = 0;

	};
}