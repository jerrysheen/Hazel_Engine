#pragma once

#include "hzpch.h"
#include "Hazel/Core/Core.h"
#include "glm/gtc/type_ptr.hpp"
#include "Hazel/Renderer/TextureStruct.h"


namespace Hazel {

	// TextureBuffer ���� rendertarget �� rendertexture, ��Ҫȥ����texture2d����Ϊ���������е���֣�
	// ����ط��Ͳ�����texture2d�ˣ�����дһ��texture2d��texture2d����д��ֻ��shader�ɶ���texture��
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
				return std::get<T>(m_CpuHandle);  // ���Ի�ȡ T ���͵�ֵ
			}
			else {
				static_assert(false, "T must be either uint32_t or CD3DX12_CPU_DESCRIPTOR_HANDLE");
			}
		}

		template<typename T>
		T getResource() const {
			if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T, Microsoft::WRL::ComPtr<ID3D12Resource>>) {
				return std::get<T>(m_BufferResource);  // ���Ի�ȡ T ���͵�ֵ
			}
			else {
				static_assert(false, "T must be either uint32_t or ID3D12Resource");
			}
		}

		virtual void* GetNativeResource() const = 0;
		std::variant<uint32_t, CD3DX12_CPU_DESCRIPTOR_HANDLE> m_CpuHandle;

		std::variant<uint32_t, Microsoft::WRL::ComPtr<ID3D12Resource>> m_BufferResource;
	protected:
		boost::uuids::uuid m_UUID;
		TextureRenderUsage m_TextureRenderUsage;
	};

}