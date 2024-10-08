#pragma once
#include "Hazel/Gfx/CommandList.h"
#include "Platform/D3D12/d3dx12.h"
#include "Platform/D3D12/d3dUtil.h"


namespace Hazel 
{
	class D3D12CommandList : public CommandList
	{
	public :
		D3D12CommandList();
		virtual ~D3D12CommandList();
		virtual void Reset() override;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;
	};
}