#pragma once
#include "CommandList.h"
#include <mutex>
namespace Hazel 
{
	class CommandPool 
	{
		// 这个地方可以做成单例类，只需要在RenderingAPI Manager 单例初始化完之后再初始化这个就好了吧

        private:
            // 私有静态指针，用于存储类的唯一实例
            static Ref<CommandPool> instance;
            static std::mutex mutex;

            // 将构造函数和析构函数设置为protected防止外部构造和析构
            CommandPool() {}  // 构造函数（可以添加你的初始化代码）
            ~CommandPool() {}  // 析构函数
            // 私有类，用于启用 make_shared
            struct CommandPoolEnabler : public CommandPool {};
        public:
            // 删除拷贝构造函数和赋值操作符，确保它们不会被复制
            CommandPool(const CommandPool&) = delete;
            CommandPool& operator=(const CommandPool&) = delete;

            // 提供全局访问点
            static Ref<CommandPool> getInstance() {
                std::lock_guard<std::mutex> lock(mutex);
                if (instance == nullptr) {
                    instance = std::make_shared<CommandPoolEnabler>();
                }
                return instance;
            }

            // 初始化方法
            void init() {
                std::cout << "Initializing command pool..." << std::endl;
                // 添加初始化代码
            }

            // 获取命令方法
            void getCommand() {
                std::cout << "Getting command..." << std::endl;
                // 添加获取命令的逻辑
            }
        };


}