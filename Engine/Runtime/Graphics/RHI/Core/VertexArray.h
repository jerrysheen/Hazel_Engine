#pragma once

#include <memory>
#include "Runtime/Graphics/RHI/Core/Buffer.h"
#include <map>

namespace Hazel {

	// define vertexProperty class
	enum class VertexProperty
	{
		Position,
		Normal,
		Tangent,
		TexCoord0,
		TexCoord1,
		VertexColor,
	};

	class VertexArray 
	{
	public:
		virtual ~VertexArray() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

		void AddVertexBuffer(const VertexProperty& vertexProperty, const Ref<VertexBuffer>& vertexBuffer);

		//virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
		inline const std::map<VertexProperty, Ref<VertexBuffer>>& GetVertexBuffers() { return m_VertexBuffers;}
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

		static Ref<VertexArray> Create();

		//inline virtual void SetLayout(const BufferLayout& layout) = 0;
	protected:
		std::map<VertexProperty, Ref<VertexBuffer>> m_VertexBuffers;
	};
}