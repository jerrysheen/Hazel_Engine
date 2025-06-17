#pragma once
#include "hzpch.h"
#include "Hazel/Gfx/GfxDesc.h"
#include "Platform/D3D12/d3dx12.h"
#include "Platform/D3D12/d3dUtil.h"
#include "Hazel/Renderer/Buffer.h"

namespace Hazel
{
	//class D3D12GfxDescHeap : public GfxDescHeap
	//{
	//public:
	//	D3D12GfxDescHeap(const DescriptorType& type);
	//	virtual ~D3D12GfxDescHeap();
	//	//virtual void Reset() override;
	//	virtual Ref<GfxDesc> GetOrCreateDesc(const Ref<TextureBuffer> textureBuffer, const DescriptorType& type) override;
	//	virtual Ref<GfxDesc> GetOrCreateDesc(const Ref<ConstantBuffer> constantBuffer) override;

	//private:
	//	// ���D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV �������ͣ����ܻ����һ�Զ���������
	//	// ����Ӧ�ò���Ҫ�ˣ���Դ��heap����һһ��Ӧ��
	//	std::map<boost::uuids::uuid, std::map<DescriptorType, Ref<GfxDesc>>> m_DescMap;
	//	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& m_HeapLocal;
	//};




	////class D3D12GfxDesc : public GfxDesc
	////{
	////public:
	////	D3D12GfxDesc(const DescriptorType& type, const GfxDescHeap& heap);
	////	virtual ~D3D12GfxDesc();
	////	//virtual void Reset() override;
	////};
}