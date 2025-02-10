#pragma once

#include "hzpch.h"
#include "Hazel/Core/Core.h"
#include "glm/gtc/type_ptr.hpp"
#include "Hazel/Renderer/TextureStruct.h"


namespace Hazel {

	// TextureBuffer 包含 rendertarget 和 rendertexture, 需要去包含texture2d吗，因为看起来会有点奇怪？
	// 这个地方就不包括texture2d了，额外写一套texture2d，texture2d就是写死只有shader可读的texture。
	class TextureBuffer
	{
	public:
		virtual ~TextureBuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void RebindColorAttachment(uint32_t colorAttachmentID, TextureBufferSpecification spec) = 0;
		virtual void RebindDepthAttachment(uint32_t depthAttachmentID, TextureBufferSpecification spec) = 0;
		virtual void RebindColorAndDepthAttachment(uint32_t colorAttachmentID, uint32_t depthAttachmentID, TextureBufferSpecification spec) = 0;

		virtual void Resize(const glm::vec2& viewportSize) = 0;
		virtual uint32_t GetColorAttachmentRendererID() const = 0;
		virtual uint32_t GetDepthAttachmentRendererID() const = 0;
		virtual std::any GetRendererID() const = 0;
		//virtual uint32_t GetRendererID() const = 0;

		virtual const TextureBufferSpecification& GetSpecification() const = 0;
		static Ref<TextureBuffer> Create(const TextureBufferSpecification& spec);


		inline boost::uuids::uuid GetUUID() const { return m_UUID; }
		inline TextureRenderUsage GetTextureRenderUsage() const { return m_TextureRenderUsage; }		
		inline void SetTextureRenderUsage(TextureRenderUsage usage) { m_TextureRenderUsage = usage; }

		template<typename T>
		T getCpuHandle() const {
			if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T, CD3DX12_CPU_DESCRIPTOR_HANDLE>) {
				return std::get<T>(m_CpuHandle);  // 尝试获取 T 类型的值
			}
			else {
				static_assert(false, "T must be either uint32_t or CD3DX12_CPU_DESCRIPTOR_HANDLE");
			}
		}

		template<typename T>
		T getResource() const {
			if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T, Microsoft::WRL::ComPtr<ID3D12Resource>>) {
				return std::get<T>(m_BufferResource);  // 尝试获取 T 类型的值
			}
			else {
				static_assert(false, "T must be either uint32_t or ID3D12Resource");
			}
		}


		std::variant<uint32_t, CD3DX12_CPU_DESCRIPTOR_HANDLE> m_CpuHandle;

		std::variant<uint32_t, Microsoft::WRL::ComPtr<ID3D12Resource>> m_BufferResource;
		//Microsoft::WRL::ComPtr<ID3D12Resource> m_BufferResource;
	protected:
		// 先想一下在TextureBuffer层，需要做哪些操作的
		// 首先上层会创建一个TextureBuffer，然后这个texturebuffer有对应的资源以及描述符。
		// 上层还要做的一个操作是bind，commandlist.settexture类似于这种操作，那么也就是说textureBuffer需要有一个资源描述符，来进行绑定操作，在绑定的时候只需要将对应的描述符的地址传递过来就好了。
		// 也就是说，至少在textureBuffer层，我需要持有对应的资源，以及描述符。
		boost::uuids::uuid m_UUID;
		TextureRenderUsage m_TextureRenderUsage;
	};

}