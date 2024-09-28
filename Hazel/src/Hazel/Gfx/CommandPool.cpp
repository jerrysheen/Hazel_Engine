#include "hzpch.h"
#include "CommandPool.h"

namespace Hazel 
{
    // 初始化静态成员
    Ref<CommandPool> CommandPool::instance = nullptr;
    std::mutex CommandPool::mutex;
}