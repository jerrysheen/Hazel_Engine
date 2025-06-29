#include "hzpch.h"
#include "D3D12CommandListAllocator.h"
#include "D3D12RenderAPIManager.h"
#include "Runtime/Graphics/RenderAPIManager.h"
#include "Runtime/Core/Log.h"
#include "D3D12Utils.h"

namespace Hazel {
    
    // 使用公共的D3D12Utils命名空间
    using namespace D3D12Utils;

    D3D12CommandListAllocator::D3D12CommandListAllocator(const Config& config)
        : PerFrameCommandListAllocator(config) {
    }

    D3D12CommandListAllocator::~D3D12CommandListAllocator() {
        // 释放所有分配的资源
        for (auto& pool : m_GraphicsPools) {
            ResetAllocatorPool(pool);
        }
        for (auto& pool : m_ComputePools) {
            ResetAllocatorPool(pool);
        }
        for (auto& pool : m_CopyPools) {
            ResetAllocatorPool(pool);
        }
    }

    void D3D12CommandListAllocator::Initialize() {
        // 获取D3D12设备
        D3D12RenderAPIManager* renderAPIManager = dynamic_cast<D3D12RenderAPIManager*>(
            RenderAPIManager::getInstance()->GetManager().get());
        HZ_CORE_ASSERT(renderAPIManager, "Failed to get D3D12RenderAPIManager");
        
        m_Device = renderAPIManager->GetD3DDevice();
        HZ_CORE_ASSERT(m_Device, "Failed to get D3D12 device");

        // 为每个帧创建分配器池
        m_GraphicsPools.resize(m_Config.framesInFlight);
        m_ComputePools.resize(m_Config.framesInFlight);
        m_CopyPools.resize(m_Config.framesInFlight);

        // 预分配CommandList和Allocator
        for (uint32_t frameIndex = 0; frameIndex < m_Config.framesInFlight; ++frameIndex) {
            CreateAllocatorPool(m_GraphicsPools[frameIndex], D3D12_COMMAND_LIST_TYPE_DIRECT, 
                              m_Config.maxGraphicsCommandLists);
            CreateAllocatorPool(m_ComputePools[frameIndex], D3D12_COMMAND_LIST_TYPE_COMPUTE, 
                              m_Config.maxComputeCommandLists);
            CreateAllocatorPool(m_CopyPools[frameIndex], D3D12_COMMAND_LIST_TYPE_COPY, 
                              m_Config.maxCopyCommandLists);
        }

        HZ_CORE_INFO("[D3D12CommandListAllocator] Initialized with {} frames in flight, {} graphics, {} compute, {} copy command lists per frame",
            m_Config.framesInFlight, m_Config.maxGraphicsCommandLists, 
            m_Config.maxComputeCommandLists, m_Config.maxCopyCommandLists);
    }

    CommandListHandle D3D12CommandListAllocator::CreateCommandListHandle(CommandListType type) {
        CommandListHandle handle = {};
        
        // 获取当前帧数据
        auto& frameData = GetCurrentFrameData();
        
        // 根据类型获取对应的池
        AllocatorPool* pool = nullptr;
        std::atomic<uint32_t>* offset = nullptr;
        uint32_t maxCount = 0;
        
        switch (type) {
            case CommandListType::Graphics:
                pool = &m_GraphicsPools[m_CurrentFrameIndex];
                offset = &frameData.graphicsOffset;
                maxCount = m_Config.maxGraphicsCommandLists;
                break;
            case CommandListType::Compute:
                pool = &m_ComputePools[m_CurrentFrameIndex];
                offset = &frameData.computeOffset;
                maxCount = m_Config.maxComputeCommandLists;
                break;
            case CommandListType::Copy:
                pool = &m_CopyPools[m_CurrentFrameIndex];
                offset = &frameData.copyOffset;
                maxCount = m_Config.maxCopyCommandLists;
                break;
            default:
                HZ_CORE_ERROR("[D3D12CommandListAllocator] Unsupported command list type: {}", (int)type);
                return handle;
        }

        // 获取可用的索引
        uint32_t index = offset->fetch_add(1);
        if (index >= maxCount) {
            HZ_CORE_ERROR("[D3D12CommandListAllocator] No available command lists of type {}", (int)type);
            offset->fetch_sub(1); // 回滚
            return handle;
        }

        // 填充句柄
        handle.commandList = pool->commandLists[index].Get();
        handle.commandAllocator = pool->allocators[index].Get();
        handle.allocatorIndex = index;
        handle.frameId = m_CurrentFrameId;
        handle.isValid = true;

        return handle;
    }

    void D3D12CommandListAllocator::ResetCommandListHandle(const CommandListHandle& handle) {
        if (!handle.IsValid()) {
            HZ_CORE_WARN("[D3D12CommandListAllocator] Attempting to reset invalid command list handle");
            return;
        }

        // 重置CommandAllocator
        auto allocator = static_cast<ID3D12CommandAllocator*>(handle.commandAllocator);
        HRESULT hr = allocator->Reset();
        if (FAILED(hr)) {
            HZ_CORE_ERROR("[D3D12CommandListAllocator] Failed to reset command allocator: {}", HRESULTToString(hr));
            return;
        }

        // 重置CommandList
        auto commandList = static_cast<ID3D12GraphicsCommandList*>(handle.commandList);
        hr = commandList->Reset(allocator, nullptr);
        if (FAILED(hr)) {
            HZ_CORE_ERROR("[D3D12CommandListAllocator] Failed to reset command list: {}", HRESULTToString(hr));
        }
    }

    D3D12_COMMAND_LIST_TYPE D3D12CommandListAllocator::GetD3D12CommandListType(CommandListType type) const {
        switch (type) {
            case CommandListType::Graphics:
                return D3D12_COMMAND_LIST_TYPE_DIRECT;
            case CommandListType::Compute:
                return D3D12_COMMAND_LIST_TYPE_COMPUTE;
            case CommandListType::Copy:
                return D3D12_COMMAND_LIST_TYPE_COPY;
            case CommandListType::Bundle:
                return D3D12_COMMAND_LIST_TYPE_BUNDLE;
            default:
                HZ_CORE_ERROR("[D3D12CommandListAllocator] Unknown command list type: {}", (int)type);
                return D3D12_COMMAND_LIST_TYPE_DIRECT;
        }
    }

    void D3D12CommandListAllocator::CreateAllocatorPool(AllocatorPool& pool, D3D12_COMMAND_LIST_TYPE type, uint32_t count) {
        pool.allocators.resize(count);
        pool.commandLists.resize(count);

        for (uint32_t i = 0; i < count; ++i) {
            // 创建CommandAllocator
            HRESULT hr = m_Device->CreateCommandAllocator(type, IID_PPV_ARGS(pool.allocators[i].GetAddressOf()));
            if (FAILED(hr)) {
                HZ_CORE_ERROR("[D3D12CommandListAllocator] Failed to create command allocator {}: {}", i, HRESULTToString(hr));
                continue;
            }

            // 创建CommandList
            hr = m_Device->CreateCommandList(0, type, pool.allocators[i].Get(), nullptr, 
                                           IID_PPV_ARGS(pool.commandLists[i].GetAddressOf()));
            if (FAILED(hr)) {
                HZ_CORE_ERROR("[D3D12CommandListAllocator] Failed to create command list {}: {}", i, HRESULTToString(hr));
                continue;
            }

            // CommandList创建时是开放状态，需要先关闭
            pool.commandLists[i]->Close();

            // 设置调试名称
            std::string allocatorName = "CommandAllocator_" + std::to_string(i);
            std::string commandListName = "CommandList_" + std::to_string(i);
            SetDebugName(pool.allocators[i].Get(), allocatorName.c_str());
            SetDebugName(pool.commandLists[i].Get(), commandListName.c_str());
        }
    }

    void D3D12CommandListAllocator::ResetAllocatorPool(AllocatorPool& pool) {
        // 释放所有资源
        for (auto& allocator : pool.allocators) {
            if (allocator) {
                allocator.Reset();
            }
        }
        for (auto& commandList : pool.commandLists) {
            if (commandList) {
                commandList.Reset();
            }
        }
        pool.allocators.clear();
        pool.commandLists.clear();
    }

} // namespace Hazel 