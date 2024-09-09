#include "hzpch.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>
namespace Hazel 
{
	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: m_Specifications(spec), m_RendererID(0)
	{
		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_NativeRendererID);
		glDeleteTextures(1, &m_ColorAttachment);
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void OpenGLFramebuffer::Invalidate()
	{
		if (!m_NativeRendererID) 
		{
			glDeleteFramebuffers(1, &m_NativeRendererID);
			glDeleteTextures(1, &m_ColorAttachment);
			glDeleteTextures(1, &m_DepthAttachment);
		}
		glCreateFramebuffers(1, &m_NativeRendererID);
		m_RendererID = m_NativeRendererID;
		glBindFramebuffer(GL_FRAMEBUFFER, m_NativeRendererID);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specifications.Width, m_Specifications.Height, 0, 
			GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);
	
		//glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
		//glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
		//glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specifications.Width, m_Specifications.Height);
		////glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Specifications.Width, m_Specifications.Height, 0,
		//// GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

		HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void  OpenGLFramebuffer::Bind() 
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_NativeRendererID);

	}
	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void OpenGLFramebuffer::Resize(const glm::vec2& viewportSize)
	{
		m_Specifications.Width = viewportSize.x;
		m_Specifications.Height = viewportSize.y;
		Invalidate();
	}	

	void OpenGLFramebuffer::RebindColorAttachment(uint32_t colorAttachmentID, FramebufferSpecification Spec)
	{
		m_ColorAttachment = colorAttachmentID;
		glBindFramebuffer(GL_FRAMEBUFFER, m_NativeRendererID);
		//glBindTexture(GL_TEXTURE_2D, colorAttachmentID);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specifications.Width, m_Specifications.Height, 0,
		//	GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachmentID, 0);
		// recreate depth/stencil buffer
		//默认，如果只绑定一个color attachment，那么我们就取消绑定depth/stencil buffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);

		//glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
		//glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
		//glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, Spec.Width, Spec.Height);
		////glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Specifications.Width, m_Specifications.Height, 0,
		//// GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
		////glDeleteTextures(1, &m_DepthAttachment);
		HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void OpenGLFramebuffer::RebindDepthAttachment(uint32_t depthAttachmentID, FramebufferSpecification Spec)
	{

		m_DepthAttachment = depthAttachmentID;
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		//glBindFramebuffer(GL_FRAMEBUFFER, m_NativeRendererID);
		//glDeleteTextures(1, &m_ColorAttachment);
		//glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
		//glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Spec.Width, Spec.Height, 0,
		//	GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, depthAttachmentID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAttachmentID, 0);
		HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}	

	void OpenGLFramebuffer::RebindColorAndDepthAttachment(uint32_t colorAttachmentID, uint32_t depthAttachmentID, FramebufferSpecification Spec)
	{
		m_ColorAttachment = colorAttachmentID;
		m_DepthAttachment = depthAttachmentID;
		glBindFramebuffer(GL_FRAMEBUFFER, m_NativeRendererID);
		glBindTexture(GL_TEXTURE_2D, colorAttachmentID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachmentID, 0);
		glBindTexture(GL_TEXTURE_2D, depthAttachmentID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAttachmentID, 0);
		HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}