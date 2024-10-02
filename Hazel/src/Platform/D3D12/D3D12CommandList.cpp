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

		ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CommandAllocator.GetAddressOf())));


		ThrowIfFailed(device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			CommandAllocator.Get(), // Associated command allocator
			nullptr,                   // Initial PipelineStateObject
			IID_PPV_ARGS(mCommandList.GetAddressOf())));

		// Start off in a closed state.  This is because the first time we refer 
		// to the command list we will Reset it, and it needs to be closed before
		// calling Reset.
		mCommandList->Close();
	}


	D3D12CommandList::~D3D12CommandList()
	{
	}


	void D3D12CommandList::Reset()
	{
		// Reset the command allocator.
		ThrowIfFailed(CommandAllocator->Reset());

		// Reset the command list
		ThrowIfFailed(mCommandList->Reset(CommandAllocator.Get(), nullptr));
	}
}