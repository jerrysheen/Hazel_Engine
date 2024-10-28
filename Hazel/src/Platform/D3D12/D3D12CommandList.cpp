#include "hzpch.h"
#include "D3D12CommandList.h"
#include "Platform/D3D12/D3D12RenderAPIManager.h"
#include "Hazel.h"
#include "Hazel/Gfx/GfxViewManager.h"
#include "Platform/D3D12/d3dx12.h"
#include "Platform/D3D12/d3dUtil.h"


namespace Hazel 
{
	D3D12CommandList::D3D12CommandList():
		m_CommandAllocatorLocal(std::get<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>(m_CommandAllocator)),
		m_CommandListLocal(std::get<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>>(m_CommandList))
	{



		D3D12RenderAPIManager* renderAPIManager = static_cast<D3D12RenderAPIManager*>(Application::Get().GetRenderAPIManager().get());
		Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();

		ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CommandAllocatorLocal.GetAddressOf())));


		ThrowIfFailed(device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_CommandAllocatorLocal.Get(), // Associated command allocator
			nullptr,                   // Initial PipelineStateObject
			IID_PPV_ARGS(m_CommandListLocal.GetAddressOf())));

		// Start off in a closed state.  This is because the first time we refer 
		// to the command list we will Reset it, and it needs to be closed before
		// calling Reset.
		m_CommandListLocal->Close();
	}


	D3D12CommandList::~D3D12CommandList()
	{
		m_CommandAllocatorLocal->Release();
		m_CommandListLocal->Release();
	}


	void D3D12CommandList::Reset()
	{
		// Reset the command allocator.
		ThrowIfFailed(m_CommandAllocatorLocal->Reset());

		// Reset the command list
		ThrowIfFailed(m_CommandListLocal->Reset(m_CommandAllocatorLocal.Get(), nullptr));

	}
	void D3D12CommandList::ClearRenderTargetView(const Ref<GfxDesc>& desc, const glm::vec4& color)
	{
		auto handle = desc->GetCPUDescHandle<D3D12_CPU_DESCRIPTOR_HANDLE>();
		//m_CommandListLocal->ClearRenderTargetView(handle, Colors::Azure, 0, nullptr);
		m_CommandListLocal->ClearRenderTargetView(handle, &color[0], 0, nullptr);

	}


	void D3D12CommandList::ChangeResourceState(const Ref<TextureBuffer>& texture, const TextureRenderUsage& fromFormat, const TextureRenderUsage& toFormat)
	{

		auto bufferResource = texture->getResource<Microsoft::WRL::ComPtr<ID3D12Resource>>();
		D3D12_RESOURCE_STATES fromState = D3D12_RESOURCE_STATE_COMMON;
		D3D12_RESOURCE_STATES toState = D3D12_RESOURCE_STATE_COMMON;

		switch (fromFormat)
		{
		case TextureRenderUsage::RENDER_TARGET:
			fromState = D3D12_RESOURCE_STATE_RENDER_TARGET;
			break;
		case TextureRenderUsage::RENDER_TEXTURE:
			fromState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			break;
		default:
			break;
		}

		switch (toFormat)
		{
		case TextureRenderUsage::RENDER_TARGET:
			toState = D3D12_RESOURCE_STATE_RENDER_TARGET;
			break;
		case TextureRenderUsage::RENDER_TEXTURE:
			toState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			break;
		default:
			break;
		}
		CD3DX12_RESOURCE_BARRIER barrierToSRV = CD3DX12_RESOURCE_BARRIER::Transition(
			bufferResource.Get(),
			fromState, // 渲染结束时是 Render Target 状态
			toState // 切换到纹理资源状态
		);

		m_CommandListLocal->ResourceBarrier(1, &barrierToSRV);

		// 这里应该设置一个同步点。。。

	}

	void D3D12CommandList::BindCbvHeap(const Ref<GfxDescHeap>& cbvHeap)
	{
		auto d3dCbvHeap = cbvHeap->getHeap<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>>();
		ID3D12DescriptorHeap* descriptorHeaps[] = { d3dCbvHeap.Get()};
		m_CommandListLocal->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	}

	void D3D12CommandList::Close()
	{
		HRESULT hr = m_CommandListLocal->Close();
		assert(SUCCEEDED(hr));

	}
	void D3D12CommandList::Release()
	{
		m_CommandListLocal->Release();
		m_CommandAllocatorLocal->Release();
	}

	void D3D12CommandList::Execute(ID3D12CommandQueue*& queue, ID3D12Fence* fence)
	{
	}
}