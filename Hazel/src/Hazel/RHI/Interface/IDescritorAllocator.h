#pragma once

//#include "Hazel/Core/Base.h"
#include "DescriptorTypes.h"

namespace Hazel {

    struct DescriptorHandle {
        uint64_t cpuHandle = 0;  // CPU可访问句柄
        uint64_t gpuHandle = 0;  // GPU可访问句柄（若支持）
        uint32_t heapIndex = 0;  // 在堆中的索引
        bool     isValid = false; // 是否有效
        
        bool IsValid() const { return isValid; }
    };
    
    struct DescriptorAllocation {
        DescriptorHandle baseHandle;  // 基础句柄
        uint32_t count = 0;           // 分配的描述符数量
        uint32_t heapIndex = 0;       // 所属堆的索引
        uint32_t descriptorSize = 0;  // 单个描述符的大小
        
        bool IsValid() const { return baseHandle.IsValid() && count > 0; }
        
        // 获取指定偏移量的句柄
        DescriptorHandle GetHandle(uint32_t index) const;
        
        // 从当前分配中切片出一部分
        DescriptorAllocation Slice(uint32_t offset, uint32_t newCount) const;
    };
    
    class IDescriptorAllocator {
    public:
        virtual ~IDescriptorAllocator() = default;
        
        // 分配描述符
        virtual DescriptorAllocation Allocate(uint32_t count = 1) = 0;
        
        // 释放描述符
        virtual void Free(const DescriptorAllocation& allocation) = 0;
        
        // 重置分配器
        virtual void Reset() = 0;
        
        // 获取堆类型
        virtual DescriptorHeapType GetHeapType() const = 0;
        
        // 获取描述符大小
        virtual uint32_t GetDescriptorSize() const = 0;
    };

}
