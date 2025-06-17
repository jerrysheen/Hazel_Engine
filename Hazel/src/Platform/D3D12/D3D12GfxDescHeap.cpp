#include "hzpch.h"
#include "D3D12GfxDescHeap.h"
#include "Platform/D3D12/D3D12RenderAPIManager.h"
#include "Hazel.h"

namespace Hazel
{

	//D3D12GfxDescHeap::D3D12GfxDescHeap(const DescriptorType& type):
	//	m_HeapLocal(std::get<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>>(m_Heap))
	//{

	//	D3D12RenderAPIManager* renderAPIManager = dynamic_cast<D3D12RenderAPIManager*>(RenderAPIManager::getInstance()->GetManager().get());
	//	Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();
	//	m_Type = type;


	//	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	//	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	//	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	//	switch (type)
	//	{
	//	case DescriptorType::DESCRIPTOR_TYPE_CBV:
	//		m_DescriptorCount = 0; // 
	//		cbvHeapDesc.NumDescriptors = 10;
	//		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	//		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	//		device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_HeapLocal));
	//		m_HeapLocal->SetName(L"DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV");
	//		break;
	//	case DescriptorType::DESCRIPTOR_TYPE_UAV:
	//	case DescriptorType::DESCRIPTOR_TYPE_SRV:
	//		m_DescriptorCount = 1; // Ĭ����һ����ƥ��imgui����Ҫ��һ��descriptor��
	//		cbvHeapDesc.NumDescriptors = 10;
	//		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	//		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	//		device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_HeapLocal));
	//		m_HeapLocal->SetName(L"DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV");
	//		break;
	//	case DescriptorType::DESCRIPTOR_TYPE_RTV:
	//		rtvHeapDesc.NumDescriptors = 10;
	//		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	//		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	//		device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_HeapLocal));
	//		m_HeapLocal->SetName(L"DESCRIPTOR_TYPE_RTV");
	//		break;
	//	case DescriptorType::DESCRIPTOR_TYPE_DSV:
	//		dsvHeapDesc.NumDescriptors = 10;
	//		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	//		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	//		device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_HeapLocal));
	//		m_HeapLocal->SetName(L"DESCRIPTOR_TYPE_DSV");
	//		break;
	//	default:
	//		break;
	//	}
	//
	//}

	//D3D12GfxDescHeap::~D3D12GfxDescHeap()
	//{
	//	m_HeapLocal->Release();
	//}

	//Ref<GfxDesc> D3D12GfxDescHeap::GetOrCreateDesc(const Ref<TextureBuffer> textureBuffer, const DescriptorType& decritorType)
	//{
	//	// ��try get, �����heap����һ��map��ȡ��desc����
	//	// ����һ�㣬 ��߻�ȡ��һ�� std::map<DescriptorType, Ref<GfxDesc>>��Ȼ��������map��ȥȡ��
	//	boost::uuids::uuid keyToSearch = textureBuffer->GetUUID();
	//	auto it = m_DescMap.find(keyToSearch);
	//	if (it != m_DescMap.end()) {
	//		// �ҵ���uuid�� ������ std::map<DescriptorType, Ref<GfxDesc>>
	//		std::map<DescriptorType, Ref<GfxDesc>>  descMap = it->second;
	//		auto it = descMap.find(decritorType);
	//		if (it != descMap.end()) {
	//			// �ҵ��˶�Ӧ��Desc�� ֱ��return��
	//			return it->second;
	//		}
	//		else 
	//		{
	//			// ֻ��û��desc�� uuid���еġ�
	//		}
	//	}
	//	else 
	//	{
	//		m_DescMap[keyToSearch] = std::map<DescriptorType, Ref<GfxDesc>>();
	//	}
	//	// ����һ����uuid�� ����û�ж�Ӧ��desc�� ��������֮·����

	//	Ref<GfxDesc> newDesc = std::make_shared<GfxDesc>();
	//	D3D12_CPU_DESCRIPTOR_HANDLE handle;
	//	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
	//	// Ȼ�������handle�����������ͺ��ˡ���
	//	auto d3dTextureResource = textureBuffer->getResource<Microsoft::WRL::ComPtr<ID3D12Resource>>();
	//	D3D12RenderAPIManager* renderAPIManager = dynamic_cast<D3D12RenderAPIManager*>(RenderAPIManager::getInstance()->GetManager().get());
	//	Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();
	//	
	//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	//	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	//	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	//	SIZE_T descriptorSize;
	//	switch (decritorType)
	//	{
	//	case DescriptorType::DESCRIPTOR_TYPE_SRV:
	//		descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//		handle = { m_HeapLocal->GetCPUDescriptorHandleForHeapStart().ptr + descriptorSize * m_DescriptorCount };
	//		gpuHandle = { m_HeapLocal->GetGPUDescriptorHandleForHeapStart().ptr + descriptorSize * m_DescriptorCount };
	//		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//		srvDesc.Format = d3dTextureResource->GetDesc().Format;
	//		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//		srvDesc.Texture2D.MipLevels = d3dTextureResource->GetDesc().MipLevels;
	//		srvDesc.Texture2D.MostDetailedMip = 0; // ����ϸ��MIP����
	//		srvDesc.Texture2D.PlaneSlice = 0;
	//		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	//		device->CreateShaderResourceView(d3dTextureResource.Get(), &srvDesc, handle);
	//		m_DescMap[keyToSearch][decritorType] = newDesc;
	//		newDesc->GetCPUHandlerVariant() = handle;
	//		newDesc->GetGPUHandlerVariant() = gpuHandle;
	//		return newDesc;
	//		break;
	//	case DescriptorType::DESCRIPTOR_TYPE_RTV:
	//		descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//		handle = { m_HeapLocal->GetCPUDescriptorHandleForHeapStart().ptr + descriptorSize * m_DescriptorCount };
	//		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // ȷ����ʽ������ĸ�ʽ��ƥ��
	//		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2D����
	//		rtvDesc.Texture2D.MipSlice = 0; // ͨ��ʹ�õ�0��mip
	//		// ������ȾĿ����ͼ
	//		device->CreateRenderTargetView(d3dTextureResource.Get(), &rtvDesc, handle);
	//		m_DescMap[keyToSearch][decritorType] = newDesc;
	//		m_DescriptorCount++;
	//		newDesc->GetCPUHandlerVariant() = handle;
	//		return newDesc;
	//		break;
	//	case DescriptorType::DESCRIPTOR_TYPE_DSV:
	//		descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	//		handle = { m_HeapLocal->GetCPUDescriptorHandleForHeapStart().ptr + descriptorSize * m_DescriptorCount };

	//		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // ͨ���������ģ��ĸ�ʽ
	//		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2D���ģ��
	//		dsvDesc.Texture2D.MipSlice = 0; // ��0��mip
	//		dsvDesc.Flags = D3D12_DSV_FLAG_NONE; // ��������Ϊ D3D12_DSV_FLAG_READ_ONLY_DEPTH �� D3D12_DSV_FLAG_READ_ONLY_STENCIL

	//		// �������ģ����ͼ
	//		device->CreateDepthStencilView(d3dTextureResource.Get(), &dsvDesc, handle);
	//		m_DescMap[keyToSearch][decritorType] = newDesc;
	//		m_DescriptorCount++;
	//		newDesc->GetCPUHandlerVariant() = handle;
	//		return newDesc;
	//	default:
	//		break;
	//	}
	//}



	//Ref<GfxDesc> D3D12GfxDescHeap::GetOrCreateDesc(const Ref<ConstantBuffer> constantBuffer)
	//{
	//	DescriptorType decritorType = DescriptorType::DESCRIPTOR_TYPE_CBV;
	//	// ��try get, �����heap����һ��map��ȡ��desc����
	//	// ����һ�㣬 ��߻�ȡ��һ�� std::map<DescriptorType, Ref<GfxDesc>>��Ȼ��������map��ȥȡ��
	//	boost::uuids::uuid keyToSearch = constantBuffer->GetUUID();
	//	auto it = m_DescMap.find(keyToSearch);
	//	if (it != m_DescMap.end()) {
	//		// �ҵ���uuid�� ������ std::map<DescriptorType, Ref<GfxDesc>>
	//		std::map<DescriptorType, Ref<GfxDesc>>  descMap = it->second;
	//		auto it = descMap.find(decritorType);
	//		if (it != descMap.end()) {
	//			// �ҵ��˶�Ӧ��Desc�� ֱ��return��
	//			return it->second;
	//		}
	//		else
	//		{
	//			// ֻ��û��desc�� uuid���еġ�
	//			// todo ����ط��߼��ƺ�������
	//		}
	//	}
	//	else
	//	{
	//		m_DescMap[keyToSearch] = std::map<DescriptorType, Ref<GfxDesc>>();
	//	}
	//	// ����һ����uuid�� ����û�ж�Ӧ��desc�� ��������֮·����

	//	Ref<GfxDesc> newDesc = std::make_shared<GfxDesc>();
	//	D3D12_CPU_DESCRIPTOR_HANDLE handle;
	//	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
	//	// Ȼ�������handle�����������ͺ��ˡ���
	//	auto constantResource = constantBuffer->getResource<Microsoft::WRL::ComPtr<ID3D12Resource>>();
	//	D3D12RenderAPIManager* renderAPIManager = dynamic_cast<D3D12RenderAPIManager*>(RenderAPIManager::getInstance()->GetManager().get());
	//	Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();

	//	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = constantResource.Get()->GetGPUVirtualAddress();
	//	// Offset to the ith object constant buffer in the buffer.
	//	int boxCBufIndex = 0;
	//	//cbAddress += boxCBufIndex * objCBByteSize;

	//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	//	cbvDesc.BufferLocation = cbAddress;
	//	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(constantBuffer->GetBufferSize());

	//	UINT descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//	handle = { m_HeapLocal->GetCPUDescriptorHandleForHeapStart().ptr + descriptorSize * m_DescriptorCount };
	//	gpuHandle = { m_HeapLocal->GetGPUDescriptorHandleForHeapStart().ptr + descriptorSize * m_DescriptorCount };
	//	device->CreateConstantBufferView(
	//		&cbvDesc,
	//		handle);
	//	// ����heap��ַ�� todo�����Ӧ�øĳ�д�������������� �Զ�����һ�����������������Ѿ���2��������ǵ�����������ȡӦ���ò�����
	//	// ��Ϊcpu ��gpu��ַ�Զ��������Ѿ���
	//	m_DescriptorCount++;
	//	m_DescMap[keyToSearch][decritorType] = newDesc;
	//	newDesc->GetCPUHandlerVariant() = handle;
	//	newDesc->GetGPUHandlerVariant() = gpuHandle;
	//	return newDesc;
	//	
	//}
}