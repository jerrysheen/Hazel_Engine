#include "hzpch.h"
#include "CommandPool.h"
#include "Hazel/Core/Core.h"
#include "Hazel/Gfx/RendererConstant.h"

namespace Hazel 
{
    /// 初始化静态成员
    std::shared_ptr<CommandPool> CommandPool::instance = nullptr;
    std::mutex CommandPool::mutex;

    void CommandPool::Init()
    {
        for (int i = 0; i < MAX_COMMANDLIST_SIZE; i++) 
        {
            Ref<CommandList> command = CommandList::Create();
            m_CommandLists.push_back(command);
        }
        
    }
}