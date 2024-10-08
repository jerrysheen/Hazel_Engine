#include "D3D12GfxDescHeap.h"
#include "Platform/D3D12/D3D12RenderAPIManager.h"
#include "Hazel.h"

namespace Hazel
{

	D3D12GfxDescHeap::~D3D12GfxDescHeap()
	{

	}

	Ref<GfxDesc> D3D12GfxDescHeap::GetOrCreateDesc(const TextureBuffer& textureBuffer)
	{
		// ��try get, �����heap����һ��map��ȡ��desc����
		boost::uuids::uuid keyToSearch = textureBuffer.GetUUID();
		auto it = m_DescMap.find(keyToSearch);
		if (it != m_DescMap.end()) {
			// �ҵ��ˣ�ֱ��return��
			return it->second;
		}
		// ûreturn�� ����һ���µ�desc.
		// desc������񲢲���Ҫ�����ࡣ��
		Ref<GfxDesc> newDesc = std::make_shared<GfxDesc>();
		auto handle = newDesc->getDescHandle<D3D12_CPU_DESCRIPTOR_HANDLE>();
		// Ȼ�������handle�����������ͺ��ˡ���
		auto d3dTextureResource = textureBuffer.getResource<Microsoft::WRL::ComPtr<ID3D12Resource>>();

		D3D12RenderAPIManager* renderAPIManager = static_cast<D3D12RenderAPIManager*>(Application::Get().GetRenderAPIManager().get());
		Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();
		// ��õ�ǰ��Ӧ��stride���Լ���ʼ��λ�ã�Ȼ��������Ե�λ�ô���һ��������������һ��GfxDesc���ء�
		


		switch (m_Type)
		{
		case DescriptorType::DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
			SIZE_T descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			handle = { m_Heap->GetCPUDescriptorHandleForHeapStart().ptr + descriptorSize * m_DescriptorCount };
			device->CreateDescriptorHeap(d3dTextureResource.Get(), nullptr, handle);
			m_DescMap[keyToSearch] = newDesc;
			m_DescriptorCount++;
			return newDesc;


			break;
		case DescriptorType::DESCRIPTOR_TYPE_RTV:
			SIZE_T descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			handle = { m_Heap->GetCPUDescriptorHandleForHeapStart().ptr + descriptorSize * m_DescriptorCount };
			device->CreateRenderTargetView(d3dTextureResource.Get(), nullptr, handle);
			m_DescMap[keyToSearch] = newDesc;
			m_DescriptorCount++;
			return newDesc;
			break;
		case DescriptorType::DESCRIPTOR_TYPE_DSV:
			break;
		default:
			break;
		}
	}

	D3D12GfxDescHeap::D3D12GfxDescHeap(const DescriptorType& type)
	{

		D3D12RenderAPIManager* renderAPIManager = static_cast<D3D12RenderAPIManager*>(Application::Get().GetRenderAPIManager().get());
		Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();
		switch (type)
		{
		case DescriptorType::DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
			m_Type = DescriptorType::DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
			cbvHeapDesc.NumDescriptors = 1;
			cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_Heap));
			m_Heap->SetName(L"DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV");
			break;
		case DescriptorType::DESCRIPTOR_TYPE_RTV:
			// ����һ��RTV��������
			m_Type = DescriptorType::DESCRIPTOR_TYPE_RTV;
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = 1;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_Heap));
			m_Heap->SetName(L"DESCRIPTOR_TYPE_RTV");
				break;
		case DescriptorType::DESCRIPTOR_TYPE_DSV:
			// ����һ��DSV��������
			m_Type = DescriptorType::DESCRIPTOR_TYPE_DSV;
			D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
			dsvHeapDesc.NumDescriptors = 1;
			dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_Heap));
			m_Heap->SetName(L"DESCRIPTOR_TYPE_DSV");
			break;
		default:
			break;
		}
	}


	//D3D12GfxDesc::D3D12GfxDesc(const DescriptorType& type, const GfxDescHeap& heap)
	//{
	//	// �����heap��Ӧ��λ�ô���
	//}
	//D3D12GfxDesc::~D3D12GfxDesc()
	//{
	//}
}