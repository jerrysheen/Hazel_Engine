#include "hzpch.h"
#include "CommandPool.h"

namespace Hazel 
{
    // ��ʼ����̬��Ա
    Ref<CommandPool> CommandPool::instance = nullptr;
    std::mutex CommandPool::mutex;
}