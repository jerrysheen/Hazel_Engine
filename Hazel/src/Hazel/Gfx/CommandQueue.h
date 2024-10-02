#pragma once
#include "Hazel/Core/Core.h"


namespace Hazel 
{
	class CommandQueue 
	{
        // 私有构造函数和析构函数
        CommandQueue() { Init(); }
        ~CommandQueue() { }

    public:
        // 删除拷贝构造函数和赋值操作符
        CommandQueue(const CommandQueue&) = delete;
        CommandQueue& operator=(const CommandQueue&) = delete;

        void Init();

        Ref<CommandQueue> GetCommandQueue();
	
	
	
	};
}