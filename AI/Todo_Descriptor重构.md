# Descriptor系统重构计划

## 大致思路
DescritorAllocator是一个描述符分配器，它返回一个DescriptorAllocation，用来记录分配的结果。
DescritorAllocator分配时，只需要传入需要分配的描述符数量。
在DescriptorAllocation中，记录一个DescritorHandle和一个count信息。
DescriptorHandle记录这个Handle的GPU和CPU地址。

## 一、当前描述符系统分析

### 1.1 现有架构
当前描述符系统采用以下类管理：
- `GfxDesc`: 表示单个描述符，包含CPU和GPU句柄
- `GfxDescHeap`: 管理描述符堆，负责创建和获取描述符
- `GfxViewManager`: 单例模式，管理不同类型的描述符堆和视图获取

### 1.2 现有问题
1. **连续性问题**: 缺乏对描述符的连续分配管理，导致在使用`SetGraphicsRootDescriptorTable`时效率低下
2. **资源追踪**: 没有完善的资源生命周期管理机制
3. **堆管理**: 描述符堆大小固定(10)，缺乏动态扩展能力
4. **内存碎片**: 缺少对描述符空间的回收和再利用机制
5. **多线程安全**: 当前实现不是线程安全的
6. **缓存一致性**: 缺乏帧间缓存管理机制
7. **平台抽象**: 部分接口直接依赖于特定API (DirectX12)，缺乏统一的抽象层

## 二、描述符系统重构设计

### 2.1 描述符系统抽象接口

#### 2.1.1 描述符类型枚举

```cpp
enum class DescriptorType {
    CBV,        // 常量缓冲区视图
    SRV,        // 着色器资源视图
    UAV,        // 无序访问视图
    Sampler,    // 采样器
    RTV,        // 渲染目标视图
    DSV         // 深度模板视图
};

enum class DescriptorHeapType {
    CbvSrvUav,  // 组合堆
    Sampler,    // 采样器堆
    Rtv,        // 渲染目标堆
    Dsv         // 深度模板堆
};
```

#### 2.1.2 描述符句柄

```cpp
// 平台无关的描述符句柄
struct DescriptorHandle {
    uint64_t cpuHandle = 0;  // CPU可访问句柄
    uint64_t gpuHandle = 0;  // GPU可访问句柄（若支持）
    uint32_t heapIndex = 0;  // 在堆中的索引
    bool     isValid = false; // 是否有效
    
    bool IsValid() const { return isValid; }
};
```

#### 2.1.3 描述符分配结果

```cpp
struct DescriptorAllocation {
    DescriptorHandle baseHandle;  // 基础句柄
    uint32_t count = 0;           // 分配的描述符数量
    uint32_t heapIndex = 0;       // 所属堆的索引
    uint32_t descriptorSize = 0;  // 单个描述符的大小
    
    bool IsValid() const { return baseHandle.IsValid() && count > 0; }
    
    // 获取指定偏移量的句柄
    DescriptorHandle GetHandle(uint32_t index) const {
        if (index >= count)
            return {};
            
        DescriptorHandle handle;
        handle.cpuHandle = baseHandle.cpuHandle + (index * descriptorSize);
        handle.gpuHandle = baseHandle.gpuHandle + (index * descriptorSize);
        handle.heapIndex = baseHandle.heapIndex + index;
        handle.isValid = true;
        return handle;
    }
    
    // 从当前分配中切片出一部分
    DescriptorAllocation Slice(uint32_t offset, uint32_t newCount) const {
        if (offset + newCount > count)
            return {};
            
        DescriptorAllocation result;
        result.baseHandle = GetHandle(offset);
        result.count = newCount;
        result.heapIndex = heapIndex;
        result.descriptorSize = descriptorSize;
        return result;
    }
};
```

#### 2.1.4 描述符分配器接口

```cpp
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
```

#### 2.1.5 描述符堆管理器接口

```cpp
class IDescriptorHeapManager {
public:
    virtual ~IDescriptorHeapManager() = default;
    
    // 初始化
    virtual void Initialize() = 0;
    
    // 获取分配器
    virtual IDescriptorAllocator& GetAllocator(DescriptorHeapType type) = 0;
    
    // 创建特定类型的视图
    virtual DescriptorHandle CreateView(DescriptorType type, const void* resourcePtr, const void* viewDesc = nullptr) = 0;
    
    // 复制描述符
    virtual void CopyDescriptors(
        uint32_t numDescriptors,
        const DescriptorHandle* srcHandles,
        const DescriptorHandle& dstHandleStart) = 0;
    
    // 获取特定类型的堆
    virtual void* GetHeap(DescriptorHeapType type) const = 0;
};
```

### 2.2 视图管理器核心接口

```cpp
class IGfxViewManager {
public:
    virtual ~IGfxViewManager() = default;
    
    // 初始化
    virtual void Initialize() = 0;
    
    // 资源视图创建
    // 【注意】：这些接口不保证得到连续的描述符，不适合用于描述符表
    virtual DescriptorHandle CreateRenderTargetView(const Ref<TextureBuffer>& texture) = 0;
    virtual DescriptorHandle CreateDepthStencilView(const Ref<TextureBuffer>& texture) = 0;
    virtual DescriptorHandle CreateShaderResourceView(const Ref<TextureBuffer>& texture) = 0;
    virtual DescriptorHandle CreateConstantBufferView(const Ref<ConstantBuffer>& buffer) = 0;
    
    // 连续描述符分配和视图创建 - 针对描述符表优化
    virtual DescriptorAllocation AllocateDescriptors(uint32_t count, DescriptorHeapType type) = 0;
    virtual void CreateShaderResourceView(const Ref<TextureBuffer>& texture, const DescriptorHandle& targetHandle) = 0;
    virtual void CreateConstantBufferView(const Ref<ConstantBuffer>& buffer, const DescriptorHandle& targetHandle) = 0;
    
    // 批量创建连续视图的便捷方法
    virtual DescriptorAllocation CreateConsecutiveShaderResourceViews(
        const std::vector<Ref<TextureBuffer>>& textures) = 0;
    
    virtual DescriptorAllocation CreateConsecutiveConstantBufferViews(
        const std::vector<Ref<ConstantBuffer>>& buffers) = 0;
    
    // 帧管理
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual PerFrameDescriptorAllocator& GetFrameAllocator(DescriptorHeapType type) = 0;
    
    // 资源生命周期管理
    virtual void OnResourceDestroyed(const boost::uuids::uuid& resourceId) = 0;
    virtual DescriptorHandle GetCachedView(const boost::uuids::uuid& resourceId, DescriptorType type) = 0;
    virtual void GarbageCollect() = 0;
    
    // 获取特定类型的堆
    virtual void* GetHeap(DescriptorHeapType type) const = 0;
    
    // 获取当前视图管理器实例
    static IGfxViewManager& Get();
};
```

## 三、帧缓冲区与连续描述符设计

### 3.1 帧环形缓冲区分配器

```cpp
class PerFrameDescriptorAllocator {
public:
    PerFrameDescriptorAllocator(IDescriptorAllocator& allocator, uint32_t ringBufferSize)
        : m_Allocator(allocator), m_RingBufferSize(ringBufferSize) {
        // 预分配大块连续空间
        m_RingBufferAllocation = allocator.Allocate(ringBufferSize);
        HZ_CORE_ASSERT(m_RingBufferAllocation.IsValid(), "Failed to allocate ring buffer space!");
    }
    
    ~PerFrameDescriptorAllocator() {
        // 释放环形缓冲区
        m_Allocator.Free(m_RingBufferAllocation);
    }
    
    // 从环形缓冲区分配临时描述符
    DescriptorAllocation AllocateTemporary(uint32_t count) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        
        if (m_CurrentOffset + count > m_RingBufferSize) {
            HZ_CORE_WARN("Per-frame descriptor ring buffer out of space! Consider increasing size.");
            return {};
        }
        
        // 分配子区域
        DescriptorAllocation result = m_RingBufferAllocation.Slice(m_CurrentOffset, count);
        m_CurrentOffset += count;
        return result;
    }
    
    // 每帧开始时重置
    void Reset() {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_CurrentOffset = 0;
    }
    
private:
    IDescriptorAllocator& m_Allocator;
    DescriptorAllocation m_RingBufferAllocation;
    uint32_t m_CurrentOffset = 0;
    uint32_t m_RingBufferSize;
    std::mutex m_Mutex;
};
```

### 3.2 多帧描述符管理

```cpp
// 多帧描述符管理器，用于处理资源跨多帧共享的情况
class MultiFrameDescriptorManager {
public:
    MultiFrameDescriptorManager(uint32_t frameCount = 3)
        : m_FrameCount(frameCount) {
        m_FrameAllocators.resize(frameCount);
    }
    
    void Initialize(IDescriptorHeapManager* heapManager, uint32_t allocationsPerFrame = 1024) {
        for (uint32_t i = 0; i < m_FrameCount; i++) {
            m_FrameAllocators[i] = std::make_unique<PerFrameDescriptorAllocator>(
                heapManager->GetAllocator(DescriptorHeapType::CbvSrvUav),
                allocationsPerFrame
            );
        }
    }
    
    void NewFrame() {
        m_CurrentFrame = (m_CurrentFrame + 1) % m_FrameCount;
        m_FrameAllocators[m_CurrentFrame]->Reset();
    }
    
    PerFrameDescriptorAllocator& GetCurrentFrameAllocator() {
        return *m_FrameAllocators[m_CurrentFrame];
    }
    
private:
    std::vector<std::unique_ptr<PerFrameDescriptorAllocator>> m_FrameAllocators;
    uint32_t m_FrameCount = 3;
    uint32_t m_CurrentFrame = 0;
};
```

## 四、资源设计模式与最佳实践

### 4.1 描述符表模式（高性能固定资源）

适合那些资源组合固定的场景，如材质系统。

```cpp
// 材质中预分配连续描述符
class Material {
public:
    void Initialize() {
        // 获取材质需要的所有纹理
        std::vector<Ref<TextureBuffer>> textures = { m_AlbedoMap, m_NormalMap, m_RoughnessMap };
        
        // 分配连续描述符并创建SRV
        m_TextureViews = IGfxViewManager::Get().CreateConsecutiveShaderResourceViews(textures);
    }
    
    void Bind(CommandList* cmdList) {
        // 设置着色器和常量
        cmdList->SetPipelineState(m_PipelineState);
        cmdList->SetGraphicsRootConstantBufferView(0, m_ConstantBuffer->GetGPUVirtualAddress());
        
        // 绑定描述符表（一次性绑定所有纹理）
        cmdList->SetGraphicsRootDescriptorTable(1, m_TextureViews.baseHandle.gpuHandle);
    }
    
private:
    Ref<TextureBuffer> m_AlbedoMap;
    Ref<TextureBuffer> m_NormalMap;
    Ref<TextureBuffer> m_RoughnessMap;
    DescriptorAllocation m_TextureViews;
};
```

### 4.2 环形缓冲区模式（灵活动态资源）

适合那些需要频繁切换资源的场景，如动态加载的内容。

```cpp
// DrawCall处理系统
class RenderSystem {
public:
    // 处理一个材质的渲染，需要多个纹理
    void ProcessMaterialDrawCall(Material* material, CommandList* cmdList) {
        // 1. 获取材质需要的纹理列表
        auto& textures = material->GetTextures();
        
        // 2. 从帧分配器中获取临时空间
        auto& frameAllocator = IGfxViewManager::Get().GetFrameAllocator(DescriptorHeapType::CbvSrvUav);
        auto allocation = frameAllocator.AllocateTemporary(textures.size());
        
        // 3. 有两种方式填充这个临时空间:
        
        // 方法A: 在临时空间创建新的视图
        for (uint32_t i = 0; i < textures.size(); i++) {
            IGfxViewManager::Get().CreateShaderResourceView(textures[i], allocation.GetHandle(i));
        }
        
        // 方法B: 复制已有描述符（如果资源已在其他地方创建了描述符）
        std::vector<DescriptorHandle> srcHandles;
        for (auto& texture : textures) {
            srcHandles.push_back(IGfxViewManager::Get().GetCachedView(texture->GetUUID(), DescriptorType::SRV));
        }
        
        IGfxViewManager::Get().GetHeapManager().CopyDescriptors(
            textures.size(),
            srcHandles.data(),
            allocation.baseHandle
        );
        
        // 4. 设置描述符表 - 告诉GPU使用新的连续描述符区域
        cmdList->SetGraphicsRootDescriptorTable(0, allocation.baseHandle.gpuHandle);
        
        // 5. 执行绘制
        cmdList->DrawIndexed(material->GetIndexCount(), 1, 0, 0, 0);
    }

private:
    IDescriptorHeapManager& m_HeapManager;
};
```

### 4.3 描述符表切换的工作原理

当使用描述符表时，要理解以下关键点:

1. **根签名定义了资源布局**，但不指定具体资源
```cpp
// 根签名设置示例
D3D12_DESCRIPTOR_RANGE descRange = {};
descRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // 着色器资源视图
descRange.NumDescriptors = 3;  // 需要3个连续的贴图描述符
descRange.BaseShaderRegister = 0;  // t0, t1, t2
// ... 其他设置 ...

// 根参数设置
D3D12_ROOT_PARAMETER rootParam = {};
rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
rootParam.DescriptorTable.NumDescriptorRanges = 1;
rootParam.DescriptorTable.pDescriptorRanges = &descRange;
// ... 创建根签名 ...
```

2. **每个DrawCall前更新描述符表**，不需要修改根签名
```cpp
// 每个DrawCall前获取临时空间
DescriptorAllocation tempAllocation = frameAllocator.AllocateTemporary(3);

// 将新贴图的描述符复制或创建到这块空间
device->CopyDescriptors(
    1,                          // 目标范围数
    &tempAllocation.cpuHandle,  // 目标句柄（连续空间的开始）
    &3,                         // 目标范围大小
    3,                          // 源描述符数量
    srcHandles,                 // 源句柄数组
    nullptr,                    // 描述符大小（使用默认）
    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV // 堆类型
);

// 更新描述符表指针 - 告诉GPU使用新位置的描述符
commandList->SetGraphicsRootDescriptorTable(0, tempAllocation.gpuHandle);

// 执行绘制
commandList->DrawIndexed(...);
```

3. **在着色器中的使用**保持不变，无需修改着色器代码
```hlsl
// 在着色器中，通过寄存器索引访问贴图
Texture2D albedoMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D roughnessMap : register(t2);
```

### 4.4 性能考虑

1. **批处理相同材质/贴图**的绘制调用，减少描述符表的切换
2. **预分配连续描述符**给常用资源组合
3. **使用环形缓冲区**管理临时描述符，避免频繁分配
4. **复制描述符**而不是重新创建，可以减少CPU开销

## 五、应用层初始化设计

### 5.1 Renderer初始化

```cpp
class Renderer {
public:
    static void Initialize() {
        // 创建图形设备和命令队列
        GraphicsDevice::CreateInstance();
        
        // 创建并初始化描述符系统
        InitializeDescriptorSystem();
        
        // 初始化其他渲染系统组件
        // ...
    }

private:
    static void InitializeDescriptorSystem() {
        // 获取当前图形API类型
        GraphicsAPI api = GraphicsDevice::Get().GetGraphicsAPI();
        
        // 使用工厂创建适合当前API的描述符系统
        m_DescriptorHeapManager = DescriptorSystemFactory::CreateHeapManager(api);
        m_ViewManager = DescriptorSystemFactory::CreateViewManager(api);
        
        // 初始化描述符堆管理器
        DescriptorHeapConfig heapConfig;
        heapConfig.cbvSrvUavHeapSize = 5000;  // 默认大小
        heapConfig.samplerHeapSize = 1000;
        heapConfig.rtvHeapSize = 100;
        heapConfig.dsvHeapSize = 100;
        
        m_DescriptorHeapManager->Initialize(heapConfig);
        
        // 初始化视图管理器
        m_ViewManager->Initialize(m_DescriptorHeapManager.get());
        
        // 注册资源管理器的回调，用于资源生命周期管理
        ResourceManager::Get().RegisterResourceDestroyedCallback(
            [this](const boost::uuids::uuid& resourceId) {
                m_ViewManager->OnResourceDestroyed(resourceId);
            }
        );
    }
    
    static Scope<IDescriptorHeapManager> m_DescriptorHeapManager;
    static Scope<IGfxViewManager> m_ViewManager;
};
```

### 5.2 描述符堆配置

```cpp
// 描述符堆配置结构
struct DescriptorHeapConfig {
    uint32_t cbvSrvUavHeapSize = 5000;  // CBV/SRV/UAV组合堆大小
    uint32_t samplerHeapSize = 1000;    // 采样器堆大小
    uint32_t rtvHeapSize = 100;         // 渲染目标堆大小
    uint32_t dsvHeapSize = 100;         // 深度模板堆大小
    uint32_t cbvSrvUavRingBufferSize = 1024; // 每帧临时描述符环形缓冲区大小
    uint32_t samplerRingBufferSize = 256;    // 采样器环形缓冲区大小
    bool enableGpuVisibleHeaps = true;  // 是否启用GPU可见堆
};
```

## 六、结论

通过重构描述符系统，我们解决了以下关键问题：

1. **连续描述符分配**：实现了连续描述符的分配和管理，提高了描述符表的使用效率
2. **帧缓冲区管理**：添加了环形缓冲区机制，优化了每帧临时资源的分配
3. **资源生命周期**：完善了资源追踪和描述符回收机制
4. **批量操作**：支持批量创建和复制描述符，减少API调用
5. **多线程安全**：添加了适当的线程保护机制
6. **平台抽象**：设计了与平台无关的接口，便于跨平台扩展

这种设计既支持静态资源的高效缓存，也支持动态资源的灵活绑定，为渲染系统提供了高效的资源管理机制。
