#include "hzpch.h"
#include "D3D12CommandList.h"
#include "Platform/D3D12/D3D12RenderAPIManager.h"
#include "Hazel.h"

namespace Hazel 
{
	D3D12CommandList::D3D12CommandList()
	{
		D3D12RenderAPIManager* renderAPIManager = static_cast<D3D12RenderAPIManager*>(Application::Get().GetRenderAPIManager().get());
		Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();

		ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CommandAllocator.GetAddressOf())));


		ThrowIfFailed(device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_CommandAllocator.Get(), // Associated command allocator
			nullptr,                   // Initial PipelineStateObject
			IID_PPV_ARGS(m_CommandList.GetAddressOf())));

		// Start off in a closed state.  This is because the first time we refer 
		// to the command list we will Reset it, and it needs to be closed before
		// calling Reset.
		m_CommandList->Close();
	}


	D3D12CommandList::~D3D12CommandList()
	{
		m_CommandAllocator->Release();
		m_CommandList->Release();
	}


	void D3D12CommandList::Reset()
	{
		// Reset the command allocator.
		ThrowIfFailed(m_CommandAllocator->Reset());

		// Reset the command list
		ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));
	}
}