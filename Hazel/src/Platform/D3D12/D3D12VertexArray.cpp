#include "hzpch.h"
#include "D3D12VertexArray.h"

namespace Hazel 
{
	D3D12VertexArray::D3D12VertexArray()
		:m_RendererID(0)
	{
	}

	D3D12VertexArray::~D3D12VertexArray()
	{
	}

	void D3D12VertexArray::Bind() const
	{
	}

	void D3D12VertexArray::Unbind() const
	{
	}

	void D3D12VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
	}

	void D3D12VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
	}
}