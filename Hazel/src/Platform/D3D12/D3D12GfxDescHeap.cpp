#include "hzpch.h"
#include "D3D12GfxDescHeap.h"
#include "Platform/D3D12/D3D12RenderAPIManager.h"
#include "Hazel.h"

namespace Hazel
{

	D3D12GfxDescHeap::D3D12GfxDescHeap(const DescriptorType& type):
		m_HeapLocal(std::get<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>>(m_Heap))
	{

		D3D12RenderAPIManager* renderAPIManager = static_cast<D3D12RenderAPIManager*>(Application::Get().GetRenderAPIManager().get());
		Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();
		m_Type = type;


		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		switch (type)
		{
		case DescriptorType::DESCRIPTOR_TYPE_CBV:
		case DescriptorType::DESCRIPTOR_TYPE_UAV:
		case DescriptorType::DESCRIPTOR_TYPE_SRV:
			m_DescriptorCount = 1; // 默认有一个，匹配imgui中需要有一个descriptor？
			cbvHeapDesc.NumDescriptors = 10;
			cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_HeapLocal));
			m_HeapLocal->SetName(L"DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV");
			break;
		case DescriptorType::DESCRIPTOR_TYPE_RTV:
			rtvHeapDesc.NumDescriptors = 10;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_HeapLocal));
			m_HeapLocal->SetName(L"DESCRIPTOR_TYPE_RTV");
			break;
		case DescriptorType::DESCRIPTOR_TYPE_DSV:
			dsvHeapDesc.NumDescriptors = 10;
			dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_HeapLocal));
			m_HeapLocal->SetName(L"DESCRIPTOR_TYPE_DSV");
			break;
		default:
			break;
		}
	
	}

	D3D12GfxDescHeap::~D3D12GfxDescHeap()
	{
		m_HeapLocal->Release();
	}

	Ref<GfxDesc> D3D12GfxDescHeap::GetOrCreateDesc(const Ref<TextureBuffer> textureBuffer, const DescriptorType& decritorType)
	{
		// 先try get, 这边我heap里有一个map，取到desc返回
		// 加了一层， 这边会取到一个 std::map<DescriptorType, Ref<GfxDesc>>，然后根据这个map再去取。
		boost::uuids::uuid keyToSearch = textureBuffer->GetUUID();
		auto it = m_DescMap.find(keyToSearch);
		if (it != m_DescMap.end()) {
			// 找到了uuid， 继续找 std::map<DescriptorType, Ref<GfxDesc>>
			std::map<DescriptorType, Ref<GfxDesc>>  descMap = it->second;
			auto it = descMap.find(decritorType);
			if (it != descMap.end()) {
				// 找到了对应的Desc， 直接return。
				return it->second;
			}
			else 
			{
				// 只是没有desc， uuid是有的。
			}
		}
		else 
		{
			m_DescMap[keyToSearch] = std::map<DescriptorType, Ref<GfxDesc>>();
		}
		// 现在一定有uuid， 但是没有对应的desc， 继续创建之路。。

		Ref<GfxDesc> newDesc = std::make_shared<GfxDesc>();
		D3D12_CPU_DESCRIPTOR_HANDLE handle;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
		// 然后用这个handle创建描述符就好了。。
		auto d3dTextureResource = textureBuffer->getResource<Microsoft::WRL::ComPtr<ID3D12Resource>>();
		D3D12RenderAPIManager* renderAPIManager = static_cast<D3D12RenderAPIManager*>(Application::Get().GetRenderAPIManager().get());
		Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();
		
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		SIZE_T descriptorSize;
		switch (decritorType)
		{
		case DescriptorType::DESCRIPTOR_TYPE_SRV:
			descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			handle = { m_HeapLocal->GetCPUDescriptorHandleForHeapStart().ptr + descriptorSize * m_DescriptorCount };
			gpuHandle = { m_HeapLocal->GetGPUDescriptorHandleForHeapStart().ptr + descriptorSize * m_DescriptorCount };
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = d3dTextureResource->GetDesc().Format;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = d3dTextureResource->GetDesc().MipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0; // 最详细的MIP级别
			srvDesc.Texture2D.PlaneSlice = 0;
			srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
			device->CreateShaderResourceView(d3dTextureResource.Get(), &srvDesc, handle);
			m_DescMap[keyToSearch][decritorType] = newDesc;
			newDesc->GetCPUHandlerVariant() = handle;
			newDesc->GetGPUHandlerVariant() = gpuHandle;
			return newDesc;
			break;
		case DescriptorType::DESCRIPTOR_TYPE_RTV:
			descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			handle = { m_HeapLocal->GetCPUDescriptorHandleForHeapStart().ptr + descriptorSize * m_DescriptorCount };
			rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 确保格式与纹理的格式相匹配
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2D纹理
			rtvDesc.Texture2D.MipSlice = 0; // 通常使用第0层mip
			// 创建渲染目标视图
			device->CreateRenderTargetView(d3dTextureResource.Get(), &rtvDesc, handle);
			m_DescMap[keyToSearch][decritorType] = newDesc;
			m_DescriptorCount++;
			newDesc->GetCPUHandlerVariant() = handle;
			return newDesc;
			break;
		case DescriptorType::DESCRIPTOR_TYPE_DSV:
			descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
			handle = { m_HeapLocal->GetCPUDescriptorHandleForHeapStart().ptr + descriptorSize * m_DescriptorCount };

			dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 通常用于深度模板的格式
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2D深度模板
			dsvDesc.Texture2D.MipSlice = 0; // 第0层mip
			dsvDesc.Flags = D3D12_DSV_FLAG_NONE; // 可以设置为 D3D12_DSV_FLAG_READ_ONLY_DEPTH 或 D3D12_DSV_FLAG_READ_ONLY_STENCIL

			// 创建深度模板视图
			device->CreateDepthStencilView(d3dTextureResource.Get(), &dsvDesc, handle);
			m_DescMap[keyToSearch][decritorType] = newDesc;
			m_DescriptorCount++;
			newDesc->GetCPUHandlerVariant() = handle;
			return newDesc;
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