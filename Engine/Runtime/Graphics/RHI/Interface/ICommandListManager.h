#pragma once

#include "ICommandListAllocator.h"
#include "Runtime/Graphics/RHI/Core/CommandList.h"
#include <memory>
#include <functional>
#include <vector>

namespace Hazel {

    // 线程安全的CommandList管理器
    class ICommandListManager {
    public:
        virtual ~ICommandListManager() = default;
        
        // === 初始化 ===
        virtual void Initialize() = 0;
        
        // === 高级接口 - 推荐使用 ===
        
        // 获取封装好的CommandList对象
        virtual Ref<CommandList> AcquireCommandList(CommandListType type = CommandListType::Graphics) = 0;
        virtual void ReleaseCommandList(Ref<CommandList> commandList) = 0;
        
        // === 低级接口 - 高性能场景使用 ===
        
        // 直接获取原生句柄（避免封装开销）
        virtual CommandListHandle AcquireHandle(CommandListType type = CommandListType::Graphics) = 0;
        virtual void ReleaseHandle(const CommandListHandle& handle) = 0;
        
        // === 批量操作 ===
        
        virtual std::vector<Ref<CommandList>> AcquireBatch(CommandListType type, uint32_t count) = 0;
        virtual void ReleaseBatch(const std::vector<Ref<CommandList>>& commandLists) = 0;
        
        // === 帧管理 ===
        
        virtual void BeginFrame(uint64_t frameId) = 0;
        virtual void EndFrame() = 0;
        
        // === 线程管理 ===
        
        virtual void RegisterWorkerThread() = 0;
        virtual void UnregisterWorkerThread() = 0;
        
        // === 异步执行支持 ===
        
        virtual void ExecuteAsync(Ref<CommandList> commandList, std::function<void()> callback = nullptr) = 0;
        virtual void WaitForCompletion(Ref<CommandList> commandList) = 0;
        
        // === 统计和调试 ===
        
        virtual void PrintStatistics() const = 0;
        virtual uint32_t GetTotalActiveCount() const = 0;
        
        // === 单例访问 ===
        
        static ICommandListManager& Get();
        static void Shutdown();
        
    protected:
        static std::unique_ptr<ICommandListManager> s_Instance;
    };

} // namespace Hazel 