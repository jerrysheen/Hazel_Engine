#pragma once

#include "IDescritorAllocator.h"
#include <vector>
#include <memory>

namespace Hazel {

    // Frame-level temporary descriptor allocator
    // Provides fast temporary descriptor allocation for a single frame
    class PerFrameDescriptorAllocator {
    public:
        PerFrameDescriptorAllocator(DescriptorHeapType heapType, uint32_t maxDescriptors = 1024);
        virtual ~PerFrameDescriptorAllocator() = default;

        // Initialize the allocator
        virtual void Initialize() = 0;

        // Allocate temporary descriptors (valid only for current frame)
        virtual DescriptorAllocation Allocate(uint32_t count = 1) = 0;

        // Reset the allocator for a new frame
        virtual void Reset() = 0;

        // Get the heap type this allocator manages
        DescriptorHeapType GetHeapType() const { return m_HeapType; }

        // Get the underlying heap
        virtual void* GetHeap() const = 0;

        // Check if we have enough space
        virtual bool HasSpace(uint32_t count) const = 0;

    protected:
        DescriptorHeapType m_HeapType;
        uint32_t m_MaxDescriptors;
        uint32_t m_CurrentOffset;
        uint32_t m_DescriptorSize;
    };

} 