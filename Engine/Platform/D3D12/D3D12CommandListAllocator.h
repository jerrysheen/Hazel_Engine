#pragma once

#include "Runtime/Graphics/RHI/Interface/PerFrameCommandListAllocator.h"
#include "Platform/D3D12/d3dx12.h"
#include "Platform/D3D12/d3dUtil.h"
#include <wrl/client.h>

namespace Hazel {

    class D3D12CommandListAllocator : public PerFrameCommandListAllocator {
    public:
        D3D12CommandListAllocator(const Config& config = {});
        virtual ~D3D12CommandListAllocator();
        
        // 初始化
        void Initialize() override;
        
    protected:
        // 实现抽象方法
        CommandListHandle CreateCommandListHandle(CommandListType type) override;
        void ResetCommandListHandle(const CommandListHandle& handle) override;
        
    private:
        Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
        
        // 预分配的CommandAllocator池
        struct AllocatorPool {
            std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> allocators;
            std::vector<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>> commandLists;
        };
        
        std::vector<AllocatorPool> m_GraphicsPools;  // 每帧一个池
        std::vector<AllocatorPool> m_ComputePools;
        std::vector<AllocatorPool> m_CopyPools;
        
        // 辅助方法
        D3D12_COMMAND_LIST_TYPE GetD3D12CommandListType(CommandListType type) const;
        void CreateAllocatorPool(AllocatorPool& pool, D3D12_COMMAND_LIST_TYPE type, uint32_t count);
        void ResetAllocatorPool(AllocatorPool& pool);
    };

} // namespace Hazel 