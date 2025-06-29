#pragma once

#include "Runtime/Graphics/RHI/Interface/ICommandListManager.h"
#include "Runtime/Graphics/RHI/Core/CommandList.h"

namespace Hazel {

    // RAII封装的CommandList - 类似智能指针的使用体验
    class ScopedCommandList {
    private:
        Ref<CommandList> m_CommandList;
        ICommandListManager* m_Manager;
        
    public:
        explicit ScopedCommandList(CommandListType type = CommandListType::Graphics)
            : m_Manager(&ICommandListManager::Get()) {
            m_CommandList = m_Manager->AcquireCommandList(type);
        }
        
        ~ScopedCommandList() {
            if (m_CommandList && m_Manager) {
                m_Manager->ReleaseCommandList(m_CommandList);
            }
        }
        
        // 禁止拷贝，允许移动
        ScopedCommandList(const ScopedCommandList&) = delete;
        ScopedCommandList& operator=(const ScopedCommandList&) = delete;
        
        ScopedCommandList(ScopedCommandList&& other) noexcept
            : m_CommandList(std::move(other.m_CommandList))
            , m_Manager(other.m_Manager) {
            other.m_Manager = nullptr;
        }
        
        ScopedCommandList& operator=(ScopedCommandList&& other) noexcept {
            if (this != &other) {
                Release();
                m_CommandList = std::move(other.m_CommandList);
                m_Manager = other.m_Manager;
                other.m_Manager = nullptr;
            }
            return *this;
        }
        
        // 访问接口
        Ref<CommandList> Get() const { return m_CommandList; }
        CommandList* operator->() const { return m_CommandList.get(); }
        CommandList& operator*() const { return *m_CommandList; }
        
        // 手动释放
        void Release() {
            if (m_CommandList && m_Manager) {
                m_Manager->ReleaseCommandList(m_CommandList);
                m_CommandList.reset();
            }
        }
        
        // 检查有效性
        bool IsValid() const { return m_CommandList != nullptr; }
        operator bool() const { return IsValid(); }
        
        // 获取原生句柄（高性能场景）
        void* GetNativeCommandList() const {
            return m_CommandList ? m_CommandList->GetNativeHandle().commandList : nullptr;
        }
    };

} // namespace Hazel 