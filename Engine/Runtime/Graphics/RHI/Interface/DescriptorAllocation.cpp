#include "hzpch.h"
#include "IDescritorAllocator.h"

namespace Hazel {

    DescriptorHandle DescriptorAllocation::GetHandle(uint32_t index) const {
        if (index >= count || !IsValid()) {
            return DescriptorHandle{}; // 返回无效句柄
        }
        
        // 对于第0个，直接返回基础句柄
        if (index == 0) {
            return baseHandle;
        }
        
        // 按需计算偏移后的句柄
        DescriptorHandle handle = baseHandle;
        handle.cpuHandle += index * descriptorSize;
        if (handle.gpuHandle != 0) {
            handle.gpuHandle += index * descriptorSize;
        }
        return handle;
    }

    DescriptorAllocation DescriptorAllocation::Slice(uint32_t offset, uint32_t newCount) const {
        if (offset + newCount > count || !IsValid()) {
            return DescriptorAllocation{}; // 返回无效分配
        }
        
        DescriptorAllocation slice;
        slice.baseHandle = GetHandle(offset);
        slice.count = newCount;
        slice.heapIndex = heapIndex;
        slice.descriptorSize = descriptorSize;
        
        return slice;
    }

} // namespace Hazel 