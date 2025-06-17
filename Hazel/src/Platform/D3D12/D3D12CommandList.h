#pragma once
#include "Hazel/Gfx/CommandList.h"
#include "Platform/D3D12/d3dx12.h"
#include "Platform/D3D12/d3dUtil.h"
#include "Hazel/Renderer/TextureStruct.h"


namespace Hazel 
{
	class D3D12CommandList : public CommandList
	{
	public :
		D3D12CommandList();
		virtual ~D3D12CommandList();
		virtual void Reset() override;
		virtual void ClearRenderTargetView(const Ref<TextureBuffer>& buffer, const glm::vec4& color) override;
		virtual void ChangeResourceState(const Ref<TextureBuffer>& texture, const TextureRenderUsage& fromFormat, const TextureRenderUsage& toFormat) override;
		//virtual void BindCbvHeap(const Ref<GfxDescHeap>& cbvHeap) override;
		virtual void Close() override;
		virtual void Release() override;
		virtual void Execute(ID3D12CommandQueue*& queue, ID3D12Fence* fence) override;

	private :
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& m_CommandAllocatorLocal;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& m_CommandListLocal;
	};
}