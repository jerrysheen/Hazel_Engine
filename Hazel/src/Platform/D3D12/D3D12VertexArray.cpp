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
		// add��ʱ���൱�ڽ������ϴ�gpu�Ĺ��̣�����ط�Ӧ��Ӧ��дһ��view�أ� Ӧ����Ҫ�ģ�
		// ����bind��ʱ����ʵ����bind desc
		m_VertexBuffers.push_back(vertexBuffer);
	}

	void D3D12VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		m_IndexBuffer = indexBuffer;
	}

	void D3D12VertexArray::SetLayout(const BufferLayout& layout)
	{
		m_Layout = layout;
		VertexByteStride = layout.GetStride();
		// Data about the buffers.
		for (int i = 0; i < m_Layout.GetCount(); i++)
		{
			auto& element = m_Layout.GetElements()[i];
			m_D3DInputLayout.push_back(D3D12_INPUT_ELEMENT_DESC{ element.Name.c_str(), 0, GetLayOutFormat(element.Type), 0, element.Offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA ,0 });
		}
	}

	DXGI_FORMAT D3D12VertexArray::GetLayOutFormat(const ShaderDataType& type)
	{
		switch (type)
		{
		case ShaderDataType::Float2: return DXGI_FORMAT_R32G32_FLOAT;
		case ShaderDataType::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
		case ShaderDataType::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		default:
			HZ_CORE_ERROR("This Format are not implemented yet.");
			break;
		}
		return DXGI_FORMAT();
	}

}