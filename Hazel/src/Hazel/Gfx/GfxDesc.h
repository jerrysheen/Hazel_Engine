#pragma once
#include "Hazel/Core/Core.h"
#include "Hazel/Renderer/TextureBuffer.h"

namespace Hazel 
{
	enum DescriptorType
	{
		DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		DESCRIPTOR_TYPE_SAMPLER,
		DESCRIPTOR_TYPE_RTV,
		DESCRIPTOR_TYPE_DSV,
		DESCRIPTOR_TYPE_NUM
	};


	class GfxDesc
	{
	public:
		GfxDesc() {};
		~GfxDesc() {};
		//void OnUpdate() {};
		//static Ref<GfxDesc> Create(const DescriptorType& type);

		//virtual void Reset() = 0;
		template<typename T>
		T getDescHandle() const {
			if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T, D3D12_CPU_DESCRIPTOR_HANDLE>) {
				return std::get<T>(m_Handle);  // 尝试获取 T 类型的值
			}
			else {
				static_assert(false, "T must be either uint32_t or CD3DX12_CPU_DESCRIPTOR_HANDLE");
			}
		}
	private:

		std::variant<uint32_t, D3D12_CPU_DESCRIPTOR_HANDLE> m_Handle;
	};


	class GfxDescHeap
	{
	public:
		virtual ~GfxDescHeap() {};
		virtual void OnUpdate() {};
		static Ref<GfxDescHeap> Create(const DescriptorType &type);

		virtual void Reset() = 0;
		virtual Ref<GfxDesc> GetOrCreateDesc(const TextureBuffer& textureBuffer) = 0;
		DescriptorType m_Type;
		// 记录当前已经有的描述符数量，用来计算下一个的位置。
		int m_DescriptorCount = 0;
	};

}