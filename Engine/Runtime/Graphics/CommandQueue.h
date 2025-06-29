#pragma once
#include "Runtime/Core/Core.h"


namespace Hazel 
{
	class CommandQueue 
	{
        // ˽�й��캯������������
        CommandQueue() { Init(); }
        ~CommandQueue() { }

    public:
        // ɾ���������캯���͸�ֵ������
        CommandQueue(const CommandQueue&) = delete;
        CommandQueue& operator=(const CommandQueue&) = delete;

        void Init();

        Ref<CommandQueue> GetCommandQueue();
	
	
	
	};
}