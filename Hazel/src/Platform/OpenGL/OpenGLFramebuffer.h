#pragma once

#include "Hazel/Renderer/Framebuffer.h"
#include "glm/gtc/type_ptr.hpp"

namespace Hazel 
{


	class OpenGLFramebuffer : public Framebuffer 
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void Resize(const glm::vec2 &viewportSize) override;
		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specifications; };
		
		virtual uint32_t GetColorAttachmentRendererID() const override { return m_ColorAttachment; }


	private:

		uint32_t m_RendererID;
		uint32_t m_ColorAttachment, m_DepthAttachment;
		FramebufferSpecification m_Specifications;
	};

}