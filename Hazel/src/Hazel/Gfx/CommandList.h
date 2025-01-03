#pragma once
#include "Hazel/Gfx/GfxDesc.h"
#include "Hazel/Gfx/Color.h"
#include "Hazel/Renderer/TextureStruct.h"


namespace Hazel 
{
	class CommandList 
	{
	public:
		CommandList() {
#ifdef RENDER_API_OPENGL
			m_CommandAllocator = 0;
			m_CommandList = 0;
#elif RENDER_API_DIRECTX12 // RENDER_API_OPENGL
			m_CommandAllocator = Microsoft::WRL::ComPtr<ID3D12CommandAllocator>{};
			m_CommandList = Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>{};
#endif // DEBUG
		};
		virtual ~CommandList() {};
		virtual void OnUpdate() {};
		static Ref<CommandList> Create();

		virtual void Reset() = 0;
		virtual void ClearRenderTargetView(const Ref<GfxDesc>& desc, const glm::vec4& color) = 0;
		virtual void ChangeResourceState(const Ref<TextureBuffer>& texture, const TextureRenderUsage& fromFormat, const TextureRenderUsage& toFormat) = 0;
		virtual void BindCbvHeap(const Ref<GfxDescHeap>& cbvHeap) = 0;
		virtual void Close() = 0;
		virtual void Release() = 0;
		virtual void Execute(ID3D12CommandQueue* & queue, ID3D12Fence* fence) = 0;

		template<typename T>
		T getCommandAllocator() const {
			if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T,Microsoft::WRL::ComPtr<ID3D12CommandAllocator >>) {
				return std::get<T>(m_CommandAllocator);  // 尝试获取 T 类型的值
			}
			else {
				static_assert(false, "T must be either uint32_t or ID3D12CommandAllocator");
			}
		}

		template<typename T>
		T getCommandList() const {
			if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v < T, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>>) {
				return std::get<T>(m_CommandList);  // 尝试获取 T 类型的值
			}
			else {
				static_assert(false, "T must be either Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>");
			}
		}
	protected:

		std::variant<uint32_t, Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> m_CommandAllocator;
		std::variant<uint32_t, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>> m_CommandList;

	};

}