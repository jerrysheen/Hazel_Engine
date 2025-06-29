#include "hzpch.h"
#include "D3D12GfxViewManager.h"
#include "D3D12RenderAPIManager.h"
#include "Runtime/Core/Application.h"
#include "D3D12DescriptorHeapManager.h"
#include "Runtime/Graphics/RenderAPIManager.h"
#include "Runtime/Graphics/RHI/Interface/IDescritorAllocator.h"
#include "Runtime/Graphics/RHI/Interface/DescriptorTypes.h"
#include <boost/uuid/uuid_io.hpp>
namespace Hazel {

    D3D12GfxViewManager::D3D12GfxViewManager() {
        // Get D3D12 device from render API manager
        D3D12RenderAPIManager* renderAPIManager = dynamic_cast<D3D12RenderAPIManager*>(
            RenderAPIManager::getInstance()->GetManager().get());
        m_Device = renderAPIManager->GetD3DDevice();
    }

    D3D12GfxViewManager::~D3D12GfxViewManager() {
        // Cleanup will be handled by smart pointers
    }

    void D3D12GfxViewManager::Initialize() {
        // Initialize heap manager
        // TODO: Create D3D12DescriptorHeapManager implementation
        // 创建堆管理器
        m_HeapManager = std::make_unique<D3D12DescriptorHeapManager>(m_Device);
        m_HeapManager->Initialize();
    
        
        // Initialize frame allocators
        InitializeFrameAllocators();
        
        HZ_CORE_INFO("D3D12GfxViewManager initialized");
    }



    DescriptorAllocation D3D12GfxViewManager::CreateRenderTargetView(const Ref<TextureBuffer>& texture) {
        // 自动提取资源UUID
        boost::uuids::uuid resourceId = texture->GetUUID();
        
        // 检查缓存
        DescriptorAllocation cachedView = GetCachedView(resourceId, DescriptorType::RTV);
        if (cachedView.IsValid()) {
            return cachedView;
        }
        
        // TODO: Implement RTV creation
        // 1. 从heap分配器分配descriptor
        // 2. 使用texture的原生资源创建RTV
        // 3. 缓存结果
        DescriptorAllocation newView{}; // TODO: 实际创建逻辑

        newView = m_HeapManager->CreateView(DescriptorType::RTV, texture.get());

        // 缓存新创建的视图
        m_ViewCache[resourceId][DescriptorType::RTV] = newView;
        
        return newView;
    }

    DescriptorAllocation D3D12GfxViewManager::CreateDepthStencilView(const Ref<TextureBuffer>& texture) {
        // 自动提取资源UUID
        boost::uuids::uuid resourceId = texture->GetUUID();
        
        // 检查缓存
        DescriptorAllocation cachedView = GetCachedView(resourceId, DescriptorType::DSV);
        if (cachedView.IsValid()) {
            return cachedView;
        }
        
        // TODO: Implement DSV creation
        DescriptorAllocation newView{}; // TODO: 实际创建逻辑
        newView = m_HeapManager->CreateView(DescriptorType::DSV, texture.get());
        // 缓存新创建的视图
        m_ViewCache[resourceId][DescriptorType::DSV] = newView;
        
        return newView;
    }

    DescriptorAllocation D3D12GfxViewManager::CreateShaderResourceView(const Ref<TextureBuffer>& texture) {
        // 自动提取资源UUID
        boost::uuids::uuid resourceId = texture->GetUUID();
        
        // 检查缓存
        DescriptorAllocation cachedView = GetCachedView(resourceId, DescriptorType::SRV);
        if (cachedView.IsValid()) {
            return cachedView;
        }
        
        // TODO: Implement SRV creation
        DescriptorAllocation newView{}; // TODO: 实际创建逻辑
		newView = m_HeapManager->CreateView(DescriptorType::SRV, texture.get());

        // 缓存新创建的视图
        m_ViewCache[resourceId][DescriptorType::SRV] = newView;
        
        return newView;
    }

    DescriptorAllocation D3D12GfxViewManager::CreateConstantBufferView(const Ref<ConstantBuffer>& buffer) {
        // 自动提取资源UUID
        boost::uuids::uuid resourceId = buffer->GetUUID();
        
        // 检查缓存
        DescriptorAllocation cachedView = GetCachedView(resourceId, DescriptorType::CBV);
        if (cachedView.IsValid()) {
            return cachedView;
        }
        
        // TODO: Implement CBV creation
        DescriptorAllocation newView{}; // TODO: 实际创建逻辑
        ViewDescription desc = ViewDescription::Buffer(buffer->GetBufferSize(), 1, 0);
        newView = m_HeapManager->CreateView(DescriptorType::CBV, buffer.get(), &desc);
        // 缓存新创建的视图
        m_ViewCache[resourceId][DescriptorType::CBV] = newView;
        
        return newView;
    }

    DescriptorAllocation D3D12GfxViewManager::AllocateDescriptors(uint32_t count, DescriptorHeapType type) {
        // TODO: Implement descriptor allocation
        return DescriptorAllocation{};
    }

    void D3D12GfxViewManager::CreateShaderResourceView(const Ref<TextureBuffer>& texture, const DescriptorAllocation& targetHandle) {
        // TODO: Implement SRV creation at specific handle
        // 在指定的targetHandle位置创建SRV，不需要缓存因为是用户管理的descriptor
        // 可以考虑添加日志：哪个资源在哪个位置创建了视图
        HZ_CORE_TRACE("Creating SRV for resource {} at handle {}", boost::uuids::to_string(texture->GetUUID()), targetHandle.baseHandle.cpuHandle);
    }

    void D3D12GfxViewManager::CreateConstantBufferView(const Ref<ConstantBuffer>& buffer, const DescriptorAllocation& targetHandle) {
        // TODO: Implement CBV creation at specific handle
        // 在指定的targetHandle位置创建CBV，不需要缓存因为是用户管理的descriptor
        HZ_CORE_TRACE("Creating CBV for resource {} at handle {}", boost::uuids::to_string(buffer->GetUUID()), targetHandle.baseHandle.cpuHandle);
    }

    DescriptorAllocation D3D12GfxViewManager::CreateConsecutiveShaderResourceViews(
        const std::vector<Ref<TextureBuffer>>& textures) {
        // TODO: Implement batch SRV creation
        return DescriptorAllocation{};
    }

    void D3D12GfxViewManager::BeginFrame() {
        // Reset frame allocators
        for (auto& [type, allocator] : m_FrameAllocators) {
            allocator->Reset();
        }
    }

    void D3D12GfxViewManager::EndFrame() {
        // Frame cleanup if needed
    }

    PerFrameDescriptorAllocator& D3D12GfxViewManager::GetFrameAllocator(DescriptorHeapType type) {
        auto it = m_FrameAllocators.find(type);
        HZ_CORE_ASSERT(it != m_FrameAllocators.end(), "Frame allocator not found for heap type");
        return *it->second;
    }

    void D3D12GfxViewManager::OnResourceDestroyed(const boost::uuids::uuid& resourceId) {
        // Remove cached views for this resource
        m_ViewCache.erase(resourceId);
    }

    DescriptorAllocation D3D12GfxViewManager::GetCachedView(const boost::uuids::uuid& resourceId, DescriptorType type) {
        auto resourceIt = m_ViewCache.find(resourceId);
        if (resourceIt != m_ViewCache.end()) {
            auto typeIt = resourceIt->second.find(type);
            if (typeIt != resourceIt->second.end()) {
                return typeIt->second;
            }
        }
        return DescriptorAllocation{};
    }

    void D3D12GfxViewManager::GarbageCollect() {
        // TODO: Implement garbage collection for unused descriptors
    }

    void* D3D12GfxViewManager::GetHeap(DescriptorHeapType type) const {
        // TODO: Return D3D12 descriptor heap for the given type
		auto* handler = m_HeapManager.get()->GetHeap(type);
        if (handler) return handler;
        return nullptr;
    }

    DescriptorAllocation D3D12GfxViewManager::CreateViewInternal(const void* resource, DescriptorType type, const void* viewDesc) {
        // TODO: Implement internal view creation
        return DescriptorAllocation{};
    }

    void D3D12GfxViewManager::InitializeFrameAllocators() {
        // TODO: Create frame allocators for each heap type
        // This would create temporary descriptor allocators for each frame
    }

    // ImGui 专用接口实现
    IDescriptorAllocator& D3D12GfxViewManager::GetImGuiAllocator() const {
        D3D12DescriptorHeapManager* heapManager = static_cast<D3D12DescriptorHeapManager*>(m_HeapManager.get());
        return heapManager->GetImGuiAllocator();
    }

    void* D3D12GfxViewManager::GetImGuiHeap() const {
        D3D12DescriptorHeapManager* heapManager = static_cast<D3D12DescriptorHeapManager*>(m_HeapManager.get());
        return heapManager->GetImGuiHeap();
    }

    DescriptorAllocation D3D12GfxViewManager::CreateImGuiSRV(const Ref<TextureBuffer>& texture, const ViewDescription* viewDesc) {
        D3D12DescriptorHeapManager* heapManager = static_cast<D3D12DescriptorHeapManager*>(m_HeapManager.get());
        
        // 为 ImGui 创建 SRV，不使用缓存机制，因为 ImGui 的纹理通常是临时使用
        DescriptorAllocation allocation = heapManager->CreateImGuiSRV(texture.get(), viewDesc);
        
        if (allocation.IsValid()) {
            HZ_CORE_TRACE("Created ImGui SRV for resource {} at handle {}", 
                boost::uuids::to_string(texture->GetUUID()), allocation.baseHandle.cpuHandle);
        } else {
            HZ_CORE_ERROR("Failed to create ImGui SRV for resource {}", 
                boost::uuids::to_string(texture->GetUUID()));
        }
        
        return allocation;
    }

} 