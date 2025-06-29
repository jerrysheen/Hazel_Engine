#include "hzpch.h"
#include "D3D12CommandListManager.h"
#include "D3D12CommandList.h"
#include "Hazel/Core/Log.h"
#include <future>
#include <chrono>
#include "D3D12Utils.h"
using namespace Hazel::D3D12Utils;

namespace Hazel {

    D3D12CommandListManager::D3D12CommandListManager() {
    }

    D3D12CommandListManager::~D3D12CommandListManager() {
        // 等待所有异步操作完成
        for (auto& [id, handle] : m_ActiveCommandLists) {
            // 简单等待，实际项目中应该有更好的同步机制
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        m_ActiveCommandLists.clear();
        m_ThreadLocalHandles.clear();
    }

    void D3D12CommandListManager::Initialize() {
        // 创建分配器
        m_Allocator = std::make_unique<D3D12CommandListAllocator>();
        m_Allocator->Initialize();
        
        HZ_CORE_INFO("[D3D12CommandListManager] Initialized successfully");
    }

    Ref<CommandList> D3D12CommandListManager::AcquireCommandList(CommandListType type) {
        // 获取原生句柄
        CommandListHandle handle = AcquireHandle(type);
        if (!handle.IsValid()) {
            HZ_CORE_ERROR("[D3D12CommandListManager] Failed to acquire command list handle");
            return nullptr;
        }

        // 包装成高级接口
        return WrapHandle(handle, type);
    }

    void D3D12CommandListManager::ReleaseCommandList(Ref<CommandList> commandList) {
        if (!commandList) {
            HZ_CORE_WARN("[D3D12CommandListManager] Attempting to release null command list");
            return;
        }

        // 提取原生句柄并释放
        UnwrapCommandList(commandList);
        ReleaseHandle(commandList->GetNativeHandle());
    }

    CommandListHandle D3D12CommandListManager::AcquireHandle(CommandListType type) {
        if (!m_Allocator) {
            HZ_CORE_ERROR("[D3D12CommandListManager] Allocator not initialized");
            return {};
        }

        // 从分配器获取句柄
        auto allocation = m_Allocator->Allocate(type);
        if (!allocation.IsValid()) {
            HZ_CORE_ERROR("[D3D12CommandListManager] Failed to allocate command list");
            return {};
        }

        CommandListHandle handle = allocation.handle;
        
        // 记录活跃的CommandList
        {
            std::lock_guard<std::mutex> lock(m_ActiveListsMutex);
            uint64_t id = reinterpret_cast<uint64_t>(handle.commandList);
            m_ActiveCommandLists[id] = handle;
        }

        // 线程本地记录
        {
            std::lock_guard<std::mutex> lock(m_ThreadMutex);
            auto threadId = std::this_thread::get_id();
            m_ThreadLocalHandles[threadId].push_back(handle);
        }

        return handle;
    }

    void D3D12CommandListManager::ReleaseHandle(const CommandListHandle& handle) {
        if (!handle.IsValid()) {
            HZ_CORE_WARN("[D3D12CommandListManager] Attempting to release invalid handle");
            return;
        }

        // 从活跃列表中移除
        {
            std::lock_guard<std::mutex> lock(m_ActiveListsMutex);
            uint64_t id = reinterpret_cast<uint64_t>(handle.commandList);
            m_ActiveCommandLists.erase(id);
        }

        // 从线程本地记录中移除
        {
            std::lock_guard<std::mutex> lock(m_ThreadMutex);
            auto threadId = std::this_thread::get_id();
            auto& handles = m_ThreadLocalHandles[threadId];
            handles.erase(std::remove_if(handles.begin(), handles.end(),
                [&handle](const CommandListHandle& h) {
                    return h.commandList == handle.commandList;
                }), handles.end());
        }

        // 释放到分配器
        if (m_Allocator) {
            m_Allocator->Free(handle);
        }
    }

    std::vector<Ref<CommandList>> D3D12CommandListManager::AcquireBatch(CommandListType type, uint32_t count) {
        std::vector<Ref<CommandList>> result;
        result.reserve(count);

        for (uint32_t i = 0; i < count; ++i) {
            auto commandList = AcquireCommandList(type);
            if (commandList) {
                result.push_back(commandList);
            } else {
                HZ_CORE_WARN("[D3D12CommandListManager] Failed to acquire command list {} of {}", i + 1, count);
                break;
            }
        }

        return result;
    }

    void D3D12CommandListManager::ReleaseBatch(const std::vector<Ref<CommandList>>& commandLists) {
        for (auto& commandList : commandLists) {
            ReleaseCommandList(commandList);
        }
    }

    void D3D12CommandListManager::BeginFrame(uint64_t frameId) {
        m_CurrentFrameId = frameId;
        
        if (m_Allocator) {
            m_Allocator->BeginFrame(frameId);
        }
    }

    void D3D12CommandListManager::EndFrame() {
        if (m_Allocator) {
            m_Allocator->EndFrame();
        }

        // 清理已完成的线程本地句柄
        {
            std::lock_guard<std::mutex> lock(m_ThreadMutex);
            for (auto it = m_ThreadLocalHandles.begin(); it != m_ThreadLocalHandles.end();) {
                if (it->second.empty()) {
                    it = m_ThreadLocalHandles.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }

    void D3D12CommandListManager::RegisterWorkerThread() {
        std::lock_guard<std::mutex> lock(m_ThreadMutex);
        auto threadId = std::this_thread::get_id();
        if (m_ThreadLocalHandles.find(threadId) == m_ThreadLocalHandles.end()) {
            m_ThreadLocalHandles[threadId] = std::vector<CommandListHandle>();
            HZ_CORE_TRACE("[D3D12CommandListManager] Registered worker thread");
        }
    }

    void D3D12CommandListManager::UnregisterWorkerThread() {
        std::lock_guard<std::mutex> lock(m_ThreadMutex);
        auto threadId = std::this_thread::get_id();
        auto it = m_ThreadLocalHandles.find(threadId);
        if (it != m_ThreadLocalHandles.end()) {
            // 释放该线程的所有句柄
            for (const auto& handle : it->second) {
                if (m_Allocator) {
                    m_Allocator->Free(handle);
                }
            }
            m_ThreadLocalHandles.erase(it);
            HZ_CORE_TRACE("[D3D12CommandListManager] Unregistered worker thread");
        }
    }

    void D3D12CommandListManager::ExecuteAsync(Ref<CommandList> commandList, std::function<void()> callback) {
        if (!commandList) {
            HZ_CORE_ERROR("[D3D12CommandListManager] Cannot execute null command list");
            return;
        }

        // 简单的异步执行实现
        // 实际项目中应该使用线程池和更复杂的同步机制
        std::thread([commandList, callback]() {
            commandList->Execute();
            if (callback) {
                callback();
            }
        }).detach();
    }

    void D3D12CommandListManager::WaitForCompletion(Ref<CommandList> commandList) {
        if (!commandList) {
            HZ_CORE_ERROR("[D3D12CommandListManager] Cannot wait for null command list");
            return;
        }

        commandList->WaitForCompletion();
    }

    void D3D12CommandListManager::PrintStatistics() const {
        std::lock_guard<std::mutex> lock(m_ActiveListsMutex);
        
        HZ_CORE_INFO("[D3D12CommandListManager] Statistics:");
        HZ_CORE_INFO("  Active command lists: {}", m_ActiveCommandLists.size());
        HZ_CORE_INFO("  Registered threads: {}", m_ThreadLocalHandles.size());
        
        if (m_Allocator) {
            HZ_CORE_INFO("  Graphics active: {}", m_Allocator->GetActiveCount(CommandListType::Graphics));
            HZ_CORE_INFO("  Compute active: {}", m_Allocator->GetActiveCount(CommandListType::Compute));
            HZ_CORE_INFO("  Copy active: {}", m_Allocator->GetActiveCount(CommandListType::Copy));
        }
    }

    uint32_t D3D12CommandListManager::GetTotalActiveCount() const {
        std::lock_guard<std::mutex> lock(m_ActiveListsMutex);
        return static_cast<uint32_t>(m_ActiveCommandLists.size());
    }

    Ref<CommandList> D3D12CommandListManager::WrapHandle(const CommandListHandle& handle, CommandListType type) {
        // 创建D3D12CommandList包装器
        auto commandList = CreateRef<D3D12CommandList>(type);
        
        // 设置原生句柄
        commandList->SetNativeHandle(handle);
        
        // 设置D3D12对象
        auto d3d12CommandList = static_cast<D3D12CommandList*>(commandList.get());
        auto d3dAllocator = static_cast<ID3D12CommandAllocator*>(handle.commandAllocator);
        auto d3dCommandList = static_cast<ID3D12GraphicsCommandList*>(handle.commandList);
        
        d3d12CommandList->SetD3D12Objects(
            Microsoft::WRL::ComPtr<ID3D12CommandAllocator>(d3dAllocator),
            Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>(d3dCommandList)
        );

        // 🔥 关键修复：确保CommandList处于Closed状态
        // 因为从分配器获取的CommandList应该是Closed状态的
        HRESULT hr = d3dCommandList->Close();
        if (hr != E_INVALIDARG) { // E_INVALIDARG表示已经是Closed状态
            if (FAILED(hr)) {
                HZ_CORE_ERROR("[D3D12CommandListManager] Failed to close command list: {}", HRESULTToString(hr));
            }
        }

        return commandList;
    }

    void D3D12CommandListManager::UnwrapCommandList(Ref<CommandList> commandList) {
        // 清理包装器中的引用
        // 这里主要用于确保生命周期管理正确
        if (commandList) {
            // 可以在这里添加额外的清理逻辑
        }
    }

} // namespace Hazel 