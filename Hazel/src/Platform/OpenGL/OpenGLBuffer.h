#pragma once
#include "hzpch.h"
#include "Hazel/Renderer/Buffer.h"

namespace Hazel {

	class OpenGLVertexBuffer : VertexBuffer 
	{
	public:
		OpenGLVertexBuffer(float *indices, uint32_t size);
		virtual ~OpenGLVertexBuffer() {}

		virtual void Bind() const;
		virtual void Unbind() const;

	private:
		uint32_t m_RendererID;
	};
}