#pragma once

#include "Hazel/Renderer/VertexArray.h"

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

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; };
	private:

		uint32_t m_RendererID;
		// VertexBuffer里面的数据回头是可以分成多个Slot，反正D3D12支持，分成position only + 必然的数据，更加cache友好。
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;

	};
}