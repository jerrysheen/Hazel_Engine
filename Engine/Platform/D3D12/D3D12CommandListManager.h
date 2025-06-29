#pragma once

#include "Runtime/Graphics/RHI/Interface/ICommandListManager.h"
#include "D3D12CommandListAllocator.h"
#include <memory>
#include <unordered_map>
#include <thread>
#include <mutex>

namespace Hazel {

    class D3D12CommandListManager : public ICommandListManager {
    public:
        D3D12CommandListManager();
        virtual ~D3D12CommandListManager();
        
        // === 初始化 ===
        void Initialize() override;
        
        // === 高级接口 ===
        Ref<CommandList> AcquireCommandList(CommandListType type = CommandListType::Graphics) override;
        void ReleaseCommandList(Ref<CommandList> commandList) override;
        
        // === 低级接口 ===
        CommandListHandle AcquireHandle(CommandListType type = CommandListType::Graphics) override;
        void ReleaseHandle(const CommandListHandle& handle) override;
        
        // === 批量操作 ===
        std::vector<Ref<CommandList>> AcquireBatch(CommandListType type, uint32_t count) override;
        void ReleaseBatch(const std::vector<Ref<CommandList>>& commandLists) override;
        
        // === 帧管理 ===
        void BeginFrame(uint64_t frameId) override;
        void EndFrame() override;
        
        // === 线程管理 ===
        void RegisterWorkerThread() override;
        void UnregisterWorkerThread() override;
        
        // === 异步执行支持 ===
        void ExecuteAsync(Ref<CommandList> commandList, std::function<void()> callback = nullptr) override;
        void WaitForCompletion(Ref<CommandList> commandList) override;
        
        // === 统计和调试 ===
        void PrintStatistics() const override;
        uint32_t GetTotalActiveCount() const override;
        
    private:
        std::unique_ptr<D3D12CommandListAllocator> m_Allocator;
        
        // CommandList包装器映射
        std::unordered_map<uint64_t, CommandListHandle> m_ActiveCommandLists;
        mutable std::mutex m_ActiveListsMutex;
        
        // 线程管理
        std::unordered_map<std::thread::id, std::vector<CommandListHandle>> m_ThreadLocalHandles;
        mutable std::mutex m_ThreadMutex;
        
        // 当前帧ID
        uint64_t m_CurrentFrameId = 0;
        
        // 辅助方法
        Ref<CommandList> WrapHandle(const CommandListHandle& handle, CommandListType type);
        void UnwrapCommandList(Ref<CommandList> commandList);
    };

} // namespace Hazel 