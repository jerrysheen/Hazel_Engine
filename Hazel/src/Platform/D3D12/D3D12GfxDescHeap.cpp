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
		// 先try get, 这边我heap里有一个map，取到desc返回
		boost::uuids::uuid keyToSearch = textureBuffer.GetUUID();
		auto it = m_DescMap.find(keyToSearch);
		if (it != m_DescMap.end()) {
			// 找到了，直接return。
			return it->second;
		}
		// 没return， 创建一个新的desc.
		// desc本身好像并不需要派生类。。
		Ref<GfxDesc> newDesc = std::make_shared<GfxDesc>();
		auto handle = newDesc->getDescHandle<D3D12_CPU_DESCRIPTOR_HANDLE>();
		// 然后用这个handle创建描述符就好了。。
		auto d3dTextureResource = textureBuffer.getResource<Microsoft::WRL::ComPtr<ID3D12Resource>>();

		D3D12RenderAPIManager* renderAPIManager = static_cast<D3D12RenderAPIManager*>(Application::Get().GetRenderAPIManager().get());
		Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();
		// 获得当前对应的stride，以及开始的位置，然后在针对性的位置创建一个描述符，做成一个GfxDesc返回。
		


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
			// 创建一个RTV描述符堆
			m_Type = DescriptorType::DESCRIPTOR_TYPE_RTV;
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = 1;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_Heap));
			m_Heap->SetName(L"DESCRIPTOR_TYPE_RTV");
				break;
		case DescriptorType::DESCRIPTOR_TYPE_DSV:
			// 创建一个DSV描述符堆
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
	//	// 在这个heap对应的位置创建
	//}
	//D3D12GfxDesc::~D3D12GfxDesc()
	//{
	//}
}