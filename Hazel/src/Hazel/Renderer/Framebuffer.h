#pragma once

#include "hzpch.h"
#include "Hazel/Core/Core.h"
#include "glm/gtc/type_ptr.hpp"

namespace Hazel {

	struct FramebufferSpecification
	{
		uint32_t Width, Height;
		//FramebufferFormat Format;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};
	class Framebuffer 
	{
	public:
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void RebindColorAttachment(uint32_t colorAttachmentID, FramebufferSpecification spec) = 0;
		virtual void RebindDepthAttachment(uint32_t depthAttachmentID, FramebufferSpecification spec) = 0;
		virtual void RebindColorAndDepthAttachment(uint32_t colorAttachmentID, uint32_t depthAttachmentID, FramebufferSpecification spec) = 0;

		virtual void Resize(const glm::vec2& viewportSize) = 0;
		virtual uint32_t GetColorAttachmentRendererID() const = 0;
		virtual uint32_t GetDepthAttachmentRendererID() const = 0;
		virtual std::any GetRendererID() const = 0;
		//virtual uint32_t GetRendererID() const = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};

}