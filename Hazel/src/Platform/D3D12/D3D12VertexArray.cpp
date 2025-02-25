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
		// add的时候，相当于将数据上传gpu的过程，这个地方应不应该写一个view呢？ 应该是要的，
		// 后面bind的时候其实就是bind desc
		m_VertexBuffers.push_back(vertexBuffer);
	}

	void D3D12VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
	}
}