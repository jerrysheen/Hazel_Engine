#pragma once

#include "ICommandListAllocator.h"
#include <vector>
#include <atomic>

namespace Hazel {

    // 帧级别的CommandList分配器 - 使用Ring Buffer设计
    class PerFrameCommandListAllocator : public ICommandListAllocator {
    public:
        struct Config {
            uint32_t maxGraphicsCommandLists = 16;
            uint32_t maxComputeCommandLists = 8;
            uint32_t maxCopyCommandLists = 4;
            uint32_t framesInFlight = 3;  // 缓冲帧数
        };
        
        PerFrameCommandListAllocator(const Config& config = {});
        virtual ~PerFrameCommandListAllocator() = default;
        
        // 初始化
        virtual void Initialize() = 0;
        
        // ICommandListAllocator接口实现
        CommandListAllocation Allocate(CommandListType type = CommandListType::Graphics) override;
        void Free(const CommandListHandle& handle) override;
        
        void BeginFrame(uint64_t frameId) override;
        void EndFrame() override;
        void Reset() override;
        
        uint32_t GetActiveCount(CommandListType type) const override;
        uint32_t GetAvailableCount(CommandListType type) const override;
        bool HasSpace(CommandListType type) const override;
        
    protected:
        Config m_Config;
        uint64_t m_CurrentFrameId = 0;
        
        // Ring Buffer结构
        struct FrameData {
            std::vector<CommandListHandle> graphics;
            std::vector<CommandListHandle> compute;
            std::vector<CommandListHandle> copy;
            std::atomic<uint32_t> graphicsOffset{0};
            std::atomic<uint32_t> computeOffset{0};
            std::atomic<uint32_t> copyOffset{0};
            
            // 提供移动构造函数和移动赋值操作符
            FrameData() = default;
            
            FrameData(FrameData&& other) noexcept
                : graphics(std::move(other.graphics))
                , compute(std::move(other.compute))
                , copy(std::move(other.copy))
                , graphicsOffset(other.graphicsOffset.load())
                , computeOffset(other.computeOffset.load())
                , copyOffset(other.copyOffset.load()) {
            }
            
            FrameData& operator=(FrameData&& other) noexcept {
                if (this != &other) {
                    graphics = std::move(other.graphics);
                    compute = std::move(other.compute);
                    copy = std::move(other.copy);
                    graphicsOffset.store(other.graphicsOffset.load());
                    computeOffset.store(other.computeOffset.load());
                    copyOffset.store(other.copyOffset.load());
                }
                return *this;
            }
            
            // 禁止拷贝构造和拷贝赋值
            FrameData(const FrameData&) = delete;
            FrameData& operator=(const FrameData&) = delete;
        };
        
        std::vector<FrameData> m_FrameRingBuffer;
        uint32_t m_CurrentFrameIndex = 0;
        
        // 线程安全的分配
        CommandListHandle AllocateFromRingBuffer(CommandListType type);
        FrameData& GetCurrentFrameData() { return m_FrameRingBuffer[m_CurrentFrameIndex]; }
        const FrameData& GetCurrentFrameData() const { return m_FrameRingBuffer[m_CurrentFrameIndex]; }
        
        // 抽象方法 - 由平台实现
        virtual CommandListHandle CreateCommandListHandle(CommandListType type) = 0;
        virtual void ResetCommandListHandle(const CommandListHandle& handle) = 0;
    };

} // namespace Hazel 