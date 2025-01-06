#pragma once
#include "Hazel/Core/Core.h"
#include "Hazel/Renderer/TextureBuffer.h"
#include "Hazel/Renderer/Buffer.h"
#include "Platform/D3D12/d3dx12.h"
#include "Platform/D3D12/d3dUtil.h"
namespace Hazel 
{
	enum DescriptorType
	{
		DESCRIPTOR_TYPE_CBV,
		DESCRIPTOR_TYPE_SRV,
		DESCRIPTOR_TYPE_UAV,
		DESCRIPTOR_TYPE_SAMPLER,
		DESCRIPTOR_TYPE_RTV,
		DESCRIPTOR_TYPE_DSV,
		DESCRIPTOR_TYPE_NUM
	};


	class GfxDesc
	{
	public:
		GfxDesc();
		~GfxDesc() {};
		//void OnUpdate() {};
		//static Ref<GfxDesc> Create(const DescriptorType& type);

		//virtual void Reset() = 0;
		template<typename T>
		T GetCPUDescHandle() const {
			if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T, D3D12_CPU_DESCRIPTOR_HANDLE>) {
				return std::get<T>(m_CPUHandle);  // 尝试获取 T 类型的值
			}
			else {
				static_assert(false, "T must be either uint32_t or D3D12_CPU_DESCRIPTOR_HANDLE");
			}
		}

		template<typename T>
		T GetGPUDescHandle() const {
			if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T, D3D12_GPU_DESCRIPTOR_HANDLE>) {
				return std::get<T>(m_GPUHandle);  // 尝试获取 T 类型的值
			}
			else {
				static_assert(false, "T must be either uint32_t or D3D12_GPU_DESCRIPTOR_HANDLE");
			}
		}

		inline std::variant<uint32_t, D3D12_CPU_DESCRIPTOR_HANDLE>& GetCPUHandlerVariant() { return m_CPUHandle; };
		inline std::variant<uint32_t, D3D12_GPU_DESCRIPTOR_HANDLE>& GetGPUHandlerVariant() { return m_GPUHandle; };
	protected:
		// 不一定都有CPU GPU handle，像UAV SRV这种都有
		// RTV DSV 就没有，要用rt做贴图，就得切换成UAV， 这个地方应该做一个保护。。
		std::variant<uint32_t, D3D12_CPU_DESCRIPTOR_HANDLE> m_CPUHandle;
		std::variant<uint32_t, D3D12_GPU_DESCRIPTOR_HANDLE> m_GPUHandle;
	};


	class GfxDescHeap
	{
	public:
		GfxDescHeap() 
		{
#ifdef RENDER_API_OPENGL
			m_Heap = 0;
#elif RENDER_API_DIRECTX12 // RENDER_API_OPENGL
			m_Heap = Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>{};
#endif // DEBUG
		};
		virtual ~GfxDescHeap() {};
		virtual void OnUpdate() {};
		static Ref<GfxDescHeap> Create(const DescriptorType &type);

		virtual Ref<GfxDesc> GetOrCreateDesc(const Ref<TextureBuffer> textureBuffer, const DescriptorType& type) = 0;
		virtual Ref<GfxDesc> GetOrCreateDesc(const Ref<ConstantBuffer> constantBuffer) = 0;
		DescriptorType m_Type;
		int m_DescriptorCount = 0;

		template<typename T>
		T getHeap() const {
			if constexpr (std::is_same_v<T, uint32_t> || 
				std::is_same_v<T, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap >>) {
				return std::get<T>(m_Heap);  // 尝试获取 T 类型的值
			}
			else {
				static_assert(false, "T must be either uint32_t or CD3DX12_CPU_DESCRIPTOR_HANDLE");
			}
		}
	protected:

		std::variant<uint32_t, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> m_Heap;
	};

}