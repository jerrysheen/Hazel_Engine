#include "hzpch.h"
#include "PerFrameCommandListAllocator.h"
#include "Hazel/Core/Log.h"

namespace Hazel {

    PerFrameCommandListAllocator::PerFrameCommandListAllocator(const Config& config)
        : m_Config(config) {
        // 初始化Ring Buffer
        m_FrameRingBuffer.reserve(m_Config.framesInFlight);
        
        for (uint32_t i = 0; i < m_Config.framesInFlight; ++i) {
            m_FrameRingBuffer.emplace_back();
            auto& frameData = m_FrameRingBuffer.back();
            frameData.graphics.reserve(m_Config.maxGraphicsCommandLists);
            frameData.compute.reserve(m_Config.maxComputeCommandLists);
            frameData.copy.reserve(m_Config.maxCopyCommandLists);
        }
    }

    CommandListAllocation PerFrameCommandListAllocator::Allocate(CommandListType type) {
        CommandListAllocation allocation = {};
        
        // 检查是否有可用空间
        if (!HasSpace(type)) {
            HZ_CORE_WARN("[PerFrameCommandListAllocator] No space available for command list type {}", (int)type);
            return allocation;
        }

        // 尝试从Ring Buffer分配
        CommandListHandle handle = AllocateFromRingBuffer(type);
        if (handle.IsValid()) {
            allocation.handle = handle;
            allocation.type = type;
        } else {
            // Ring Buffer中没有预分配的，创建新的
            handle = CreateCommandListHandle(type);
            if (handle.IsValid()) {
                allocation.handle = handle;
                allocation.type = type;
                
                // 添加到当前帧的Ring Buffer中
                auto& frameData = GetCurrentFrameData();
                switch (type) {
                    case CommandListType::Graphics:
                        frameData.graphics.push_back(handle);
                        break;
                    case CommandListType::Compute:
                        frameData.compute.push_back(handle);
                        break;
                    case CommandListType::Copy:
                        frameData.copy.push_back(handle);
                        break;
                    default:
                        break;
                }
            }
        }

        return allocation;
    }

    void PerFrameCommandListAllocator::Free(const CommandListHandle& handle) {
        if (!handle.IsValid()) {
            HZ_CORE_WARN("[PerFrameCommandListAllocator] Attempting to free invalid handle");
            return;
        }

        // 重置CommandList以备下次使用
        ResetCommandListHandle(handle);
    }

    void PerFrameCommandListAllocator::BeginFrame(uint64_t frameId) {
        m_CurrentFrameId = frameId;
        m_CurrentFrameIndex = frameId % m_Config.framesInFlight;
        
        // 重置当前帧的偏移量
        auto& frameData = GetCurrentFrameData();
        frameData.graphicsOffset.store(0);
        frameData.computeOffset.store(0);
        frameData.copyOffset.store(0);
    }

    void PerFrameCommandListAllocator::EndFrame() {
        // Ring Buffer会自动循环，这里不需要特别处理
    }

    void PerFrameCommandListAllocator::Reset() {
        // 重置所有帧的偏移量
        for (auto& frameData : m_FrameRingBuffer) {
            frameData.graphicsOffset.store(0);
            frameData.computeOffset.store(0);
            frameData.copyOffset.store(0);
        }
        
        m_CurrentFrameId = 0;
        m_CurrentFrameIndex = 0;
    }

    uint32_t PerFrameCommandListAllocator::GetActiveCount(CommandListType type) const {
        const auto& frameData = GetCurrentFrameData();
        
        switch (type) {
            case CommandListType::Graphics:
                return frameData.graphicsOffset.load();
            case CommandListType::Compute:
                return frameData.computeOffset.load();
            case CommandListType::Copy:
                return frameData.copyOffset.load();
            default:
                return 0;
        }
    }

    uint32_t PerFrameCommandListAllocator::GetAvailableCount(CommandListType type) const {
        const auto& frameData = GetCurrentFrameData();
        
        switch (type) {
            case CommandListType::Graphics:
                return static_cast<uint32_t>(frameData.graphics.size()) - frameData.graphicsOffset.load();
            case CommandListType::Compute:
                return static_cast<uint32_t>(frameData.compute.size()) - frameData.computeOffset.load();
            case CommandListType::Copy:
                return static_cast<uint32_t>(frameData.copy.size()) - frameData.copyOffset.load();
            default:
                return 0;
        }
    }

    bool PerFrameCommandListAllocator::HasSpace(CommandListType type) const {
        const auto& frameData = GetCurrentFrameData();
        
        switch (type) {
            case CommandListType::Graphics:
                return frameData.graphicsOffset.load() < m_Config.maxGraphicsCommandLists;
            case CommandListType::Compute:
                return frameData.computeOffset.load() < m_Config.maxComputeCommandLists;
            case CommandListType::Copy:
                return frameData.copyOffset.load() < m_Config.maxCopyCommandLists;
            default:
                return false;
        }
    }

    CommandListHandle PerFrameCommandListAllocator::AllocateFromRingBuffer(CommandListType type) {
        auto& frameData = GetCurrentFrameData();
        
        switch (type) {
            case CommandListType::Graphics: {
                uint32_t index = frameData.graphicsOffset.fetch_add(1);
                if (index < frameData.graphics.size()) {
                    return frameData.graphics[index];
                }
                frameData.graphicsOffset.fetch_sub(1); // 回滚
                break;
            }
            case CommandListType::Compute: {
                uint32_t index = frameData.computeOffset.fetch_add(1);
                if (index < frameData.compute.size()) {
                    return frameData.compute[index];
                }
                frameData.computeOffset.fetch_sub(1); // 回滚
                break;
            }
            case CommandListType::Copy: {
                uint32_t index = frameData.copyOffset.fetch_add(1);
                if (index < frameData.copy.size()) {
                    return frameData.copy[index];
                }
                frameData.copyOffset.fetch_sub(1); // 回滚
                break;
            }
            default:
                break;
        }
        
        return {}; // 返回无效句柄
    }

} // namespace Hazel 