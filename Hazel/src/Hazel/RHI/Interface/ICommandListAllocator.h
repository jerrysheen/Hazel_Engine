#pragma once

#include "Hazel/Core/Core.h"
#include <cstdint>

namespace Hazel {

    // CommandList类型
    enum class CommandListType {
        Graphics,   // 图形命令
        Compute,    // 计算命令  
        Copy,       // 拷贝命令
        Bundle      // 命令包（D3D12专用）
    };

    // CommandList句柄 - 直接使用指针，简洁高效
    struct CommandListHandle {
        void* commandList = nullptr;        // 原生CommandList指针
        void* commandAllocator = nullptr;   // 原生CommandAllocator指针
        uint32_t allocatorIndex = 0;        // 分配器索引
        uint64_t frameId = 0;               // 分配时的帧ID
        bool isValid = false;               // 是否有效
        
        bool IsValid() const { return isValid && commandList != nullptr; }
    };

    // CommandList分配结果
    struct CommandListAllocation {
        CommandListHandle handle;
        CommandListType type;
        
        bool IsValid() const { return handle.IsValid(); }
        
        // 隐式转换为句柄
        operator CommandListHandle() const { return handle; }
    };

    // CommandList分配器接口
    class ICommandListAllocator {
    public:
        virtual ~ICommandListAllocator() = default;
        
        // 分配CommandList
        virtual CommandListAllocation Allocate(CommandListType type = CommandListType::Graphics) = 0;
        
        // 释放CommandList
        virtual void Free(const CommandListHandle& handle) = 0;
        
        // 帧管理
        virtual void BeginFrame(uint64_t frameId) = 0;
        virtual void EndFrame() = 0;
        
        // 重置分配器（通常在帧结束时调用）
        virtual void Reset() = 0;
        
        // 获取统计信息
        virtual uint32_t GetActiveCount(CommandListType type) const = 0;
        virtual uint32_t GetAvailableCount(CommandListType type) const = 0;
        
        // 检查是否有空间
        virtual bool HasSpace(CommandListType type) const = 0;
    };

} // namespace Hazel 