#pragma once
#include "Hazel/Core/Core.h"
#include <iostream>
#include <memory>
#include <mutex>
#include "Hazel/Gfx/CommandList.h"

namespace Hazel {
    class CommandPool {
        private:
            static Ref<CommandPool> instance;
            static std::mutex mutex;

            // ˽�й��캯������������
            CommandPool() { Init(); }
            ~CommandPool() { std::cout << "CommandPool Destroyed\n"; }

            std::vector<Ref<CommandList>> m_CommandLists;

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

            void getCommand() {
                std::cout << "Getting command...\n";
                // ������ط���ȡһ������ʹ�õ�commandList;
            }
    };

}
