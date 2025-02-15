#pragma once
#include "Hazel/Core/Core.h"
#include "hzpch.h"
#include "Hazel/Gfx/CommandList.h"

namespace Hazel {
    class CommandPool {
        private:
            static Ref<CommandPool> instance;
            static std::mutex mutex;
            
            // 私有构造函数和析构函数
            CommandPool() 
            {
                //Init(); 
            }
            ~CommandPool() { std::cout << "CommandPool Destroyed\n"; }

            std::stack<Ref<CommandList>> m_IdleCommandListStack;
            //std::stack<Ref<CommandList>> m_BusyCommandListStack;
        public:
            // 删除拷贝构造函数和赋值操作符
            CommandPool(const CommandPool&) = delete;
            CommandPool& operator=(const CommandPool&) = delete;

            // getInstance 方法
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
