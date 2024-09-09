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

		virtual void RebindColorAttachment(uint32_t colorAttachmentID, FramebufferSpecification Spec) override;
		virtual void RebindDepthAttachment(uint32_t depthAttachmentID, FramebufferSpecification Spec) override;
		virtual void RebindColorAndDepthAttachment(uint32_t colorAttachmentID, uint32_t depthAttachmentID, FramebufferSpecification Spec) override;
		virtual void Resize(const glm::vec2 &viewportSize) override;
		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specifications; };
		
		virtual uint32_t GetColorAttachmentRendererID() const override { return m_ColorAttachment; }
		virtual uint32_t GetDepthAttachmentRendererID() const override { return m_DepthAttachment; }
		virtual std::any GetRendererID() const override { return m_RendererID; }


	private:

		std::any m_RendererID = 0;
		uint32_t m_NativeRendererID = 0;
		uint32_t m_ColorAttachment = 0, m_DepthAttachment = 0;
		FramebufferSpecification m_Specifications;
	};

}