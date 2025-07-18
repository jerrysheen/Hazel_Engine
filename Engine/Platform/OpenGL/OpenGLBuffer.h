#pragma once
#include "hzpch.h"
#include "Runtime/Graphics/RHI/Core/Buffer.h"

namespace Hazel {

	class OpenGLVertexBuffer : public VertexBuffer 
	{
		public:
			OpenGLVertexBuffer(float * vertices, uint32_t size, uint32_t stride);
			virtual ~OpenGLVertexBuffer();

			virtual void Bind() const override;
			virtual void Unbind() const override;

			//virtual const BufferLayout& GetLayout()  const override { return m_Layout; };
			//virtual void SetLayout(const BufferLayout& layout) override {};

		private:
			uint32_t m_RendererID;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
		public:
			OpenGLIndexBuffer(uint16_t* indices, uint32_t size);
			virtual ~OpenGLIndexBuffer();

			virtual void Bind() const;
			virtual void Unbind() const;

			virtual uint32_t GetCount() const { return m_Count; };
		private:
			uint32_t m_Count;
			uint32_t m_RendererID;
	};
}