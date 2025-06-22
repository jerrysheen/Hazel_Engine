#pragma once
#include "Hazel/Core/Core.h"
#include "hzpch.h"
#include "Hazel/Gfx/CommandList.h"

namespace Hazel {
    class CommandPool {
        private:
            static Ref<CommandPool> instance;
            static std::mutex mutex;
            
            // ˽�й��캯������������
            CommandPool() 
            {
                //Init(); 
            }
            ~CommandPool() { std::cout << "CommandPool Destroyed\n"; }

            std::stack<Ref<CommandList>> m_IdleCommandListStack;
            //std::stack<Ref<CommandList>> m_BusyCommandListStack;
        public:
            // ɾ���������캯���͸�ֵ������
            CommandPool(const CommandPool&) = delete;
            CommandPool& operator=(const CommandPool&) = delete;

            // getInstance ����
            static Ref<CommandPool> getInstance() {
                std::lock_guard<std::mutex> lock(mutex);
                if (!instance) {
                    instance = Ref<CommandPool>(new CommandPool(), [](CommandPool* p) {
                        delete p;
                        });
                }
                return instance;
            }

            void Init();

            Ref<CommandList> GetCommand();
            void RecycleCommand(Ref<CommandList> command);
    };

}
