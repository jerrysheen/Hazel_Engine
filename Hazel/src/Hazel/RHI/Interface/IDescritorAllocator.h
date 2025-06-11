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
        uint32_t count = 1;           // 分配的描述符数量，默认为1支持单个描述符
        uint32_t heapIndex = 0;       // 所属堆的索引
        uint32_t descriptorSize = 0;  // 单个描述符的大小
        
        bool IsValid() const { return baseHandle.IsValid() && count > 0; }
        
        // 获取指定偏移量的句柄（按需计算，简单直接）
        DescriptorHandle GetHandle(uint32_t index = 0) const;
        
        // 从当前分配中切片出一部分
        DescriptorAllocation Slice(uint32_t offset, uint32_t newCount) const;
        
        // 判断是否为单个描述符分配
        bool IsSingle() const { return count == 1; }
        
        // 隐式转换为单个句柄（当count=1时）
        operator DescriptorHandle() const {
            return (count == 1 && IsValid()) ? baseHandle : DescriptorHandle{};
        }
        
        // === 便利的GPU绑定接口 ===
        
        // 获取用于GPU绑定的起始CPU句柄
        uint64_t GetCPUDescriptorTableStart() const { return baseHandle.cpuHandle; }
        
        // 获取用于GPU绑定的起始GPU句柄  
        uint64_t GetGPUDescriptorTableStart() const { return baseHandle.gpuHandle; }
        
        // 获取描述符表大小（用于绑定）
        uint32_t GetDescriptorTableSize() const { return count; }
        
        // 检查是否支持GPU访问
        bool HasGPUHandle() const { return baseHandle.gpuHandle != 0; }
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
