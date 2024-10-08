#pragma once
#include "hzpch.h"
#include "Hazel/Gfx/GfxDesc.h"
#include "Platform/D3D12/d3dx12.h"
#include "Platform/D3D12/d3dUtil.h"

namespace Hazel
{
	class D3D12GfxDescHeap : public GfxDescHeap
	{
	public:
		D3D12GfxDescHeap(const DescriptorType& type);
		virtual ~D3D12GfxDescHeap();
		virtual void Reset() override;
		virtual Ref<GfxDesc> GetOrCreateDesc(const TextureBuffer& textureBuffer) override;

	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_Heap;
		std::map<boost::uuids::uuid, Ref<GfxDesc>> m_DescMap;
	};




	//class D3D12GfxDesc : public GfxDesc
	//{
	//public:
	//	D3D12GfxDesc(const DescriptorType& type, const GfxDescHeap& heap);
	//	virtual ~D3D12GfxDesc();
	//	//virtual void Reset() override;
	//};
}