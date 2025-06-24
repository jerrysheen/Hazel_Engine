#pragma once
#include "Hazel/RHI/Core/CommandList.h"
#include "Platform/D3D12/d3dx12.h"
#include "Platform/D3D12/d3dUtil.h"
#include "Hazel/Renderer/TextureStruct.h"

namespace Hazel 
{
	class D3D12CommandList : public CommandList
	{
	public:
		D3D12CommandList(CommandListType type = CommandListType::Graphics);
		virtual ~D3D12CommandList();
		
		// CommandList接口实现
		virtual void Reset() override;
		virtual void Close() override;
		virtual void Execute() override;
		virtual void ClearRenderTargetView(const Ref<TextureBuffer>& buffer, const glm::vec4& color) override;
		virtual void ChangeResourceState(const Ref<TextureBuffer>& texture, 
		                               const TextureRenderUsage& fromFormat, 
		                               const TextureRenderUsage& toFormat) override;
		
		// D3D12特定方法
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetD3D12CommandList() const;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> GetD3D12Allocator() const;
		
		// 用于管理器设置原生对象
		void SetD3D12Objects(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator, 
		                    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);

	private:
		// 直接存储D3D12对象的指针，方便使用
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;
		
		// 初始化D3D12对象
		void InitializeD3D12Resources();
		
		// 同步原生句柄
		void UpdateNativeHandle();
	};
}