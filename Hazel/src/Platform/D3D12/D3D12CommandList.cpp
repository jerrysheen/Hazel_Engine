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
	}


	D3D12CommandList::~D3D12CommandList()
	{
	}
}