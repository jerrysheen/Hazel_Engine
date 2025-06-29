#pragma once

#include "IDescritorAllocator.h"
#include "DescriptorTypes.h"

namespace Hazel {

    class IDescriptorHeapManager {
    public:
        virtual ~IDescriptorHeapManager() = default;
        
        // 初始化
        virtual void Initialize() = 0;
        
        // 获取分配器
        virtual IDescriptorAllocator& GetAllocator(DescriptorHeapType type) = 0;
        
        // 创建特定类型的视图 - 使用抽象的ViewDescription
        virtual DescriptorAllocation CreateView(DescriptorType type, const void* resourcePtr, const ViewDescription* viewDesc = nullptr) = 0;
        
        // 复制描述符
        virtual void CopyDescriptors(
            uint32_t numDescriptors,
            const DescriptorHandle* srcHandles,
            const DescriptorHandle& dstHandleStart) = 0;
        
        // 获取特定类型的堆
        virtual void* GetHeap(DescriptorHeapType type) const = 0;
    };

}
