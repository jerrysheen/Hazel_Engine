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
            m_IdleCommandListStack.push(command);
        }
    }


    Ref<CommandList> CommandPool::GetCommand()
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (m_IdleCommandListStack.empty())
        {
			HZ_CORE_ASSERT(false, "CommandList is not enough");
			return nullptr;
		}
		Ref<CommandList> command = m_IdleCommandListStack.top();
		m_IdleCommandListStack.pop();
		//m_BusyCommandListStack.push(command);

        //command->Reset();
		return command;
    }

    void CommandPool::RecycleCommand(Ref<CommandList> command) 
    {
        m_IdleCommandListStack.push(command);
        command->Close();
    }
}