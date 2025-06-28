# MaterialBatchManager重构设计

## 一、功能职责与边界划分

### 1.1 MaterialBatchManager核心职责
MaterialBatchManager是材质批处理管理器，专注于以下核心职责：
1. **材质批次管理**：组织和管理材质批次，实现批量渲染
2. **状态管理**：优化材质状态切换和绑定顺序
3. **实例分组**：对使用相同或相似材质的对象进行分组
4. **排序与优化**：最小化渲染状态切换和资源绑定

### 1.2 与其他系统的边界
为避免职责重叠，明确划分系统边界：

1. **MaterialBatchManager负责**：
   - 材质批次的创建和组织
   - 批次的排序和优化
   - 材质状态设置的封装 
   - 材质变体管理

2. **不属于MaterialBatchManager的职责**：
   - 描述符分配（由DescriptorManager负责）
   - 资源上传和内存管理（由FrameResourceManager负责）
   - 材质资源创建（由Material类或ResourceManager负责）
   - 渲染命令生成（由CommandList负责）

## 二、解耦设计

### 2.1 材质批次信息结构

```cpp
// 材质批次ID结构
struct MaterialBatchId {
    uint64_t id = 0;
    
    bool IsValid() const { return id != 0; }
    
    bool operator==(const MaterialBatchId& other) const {
        return id == other.id;
    }
    
    // 哈希支持
    struct Hasher {
        size_t operator()(const MaterialBatchId& batchId) const {
            return std::hash<uint64_t>()(batchId.id);
        }
    };
};

// 材质批次参数（用于材质分组）
struct MaterialBatchParameters {
    uint64_t shaderId = 0;         // 着色器ID
    uint32_t renderStateFlags = 0; // 渲染状态标记
    std::vector<uint64_t> textureIds; // 纹理ID
    
    // 生成批次ID
    MaterialBatchId GenerateBatchId() const {
        // 简单哈希算法示例
        uint64_t hash = shaderId;
        hash = hash * 31 + renderStateFlags;
        for (const auto& texId : textureIds) {
            hash = hash * 31 + texId;
        }
        return MaterialBatchId{ hash };
    }
};

// 材质在批次中的实例信息
struct MaterialInstanceInfo {
    MaterialBatchId batchId;        // 所属批次ID
    uint32_t instanceIndex = 0;     // 在批次中的索引
    uint32_t dataOffset = 0;        // 材质数据偏移量
    
    bool IsValid() const { return batchId.IsValid() && instanceIndex != UINT32_MAX; }
};
```

### 2.2 材质批处理管理器接口

```cpp
class IMaterialBatchManager {
public:
    virtual ~IMaterialBatchManager() = default;
    
    // 批次管理
    virtual MaterialBatchId CreateBatch(const MaterialBatchParameters& params) = 0;
    virtual MaterialInstanceInfo RegisterMaterialInstance(const MaterialBatchId& batchId, const Ref<Material>& material) = 0;
    
    // 材质实例更新
    virtual void UpdateMaterialData(const MaterialInstanceInfo& instanceInfo, const void* data, uint32_t size) = 0;
    
    // 批次渲染
    virtual void SortBatches() = 0;
    virtual void BindBatch(const MaterialBatchId& batchId, void* cmdList) = 0;
    virtual void RenderBatch(const MaterialBatchId& batchId, void* cmdList) = 0;
    
    // 帧管理
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    
    // 批次查询
    virtual std::vector<MaterialBatchId> GetBatchesUsingShader(uint64_t shaderId) const = 0;
    virtual std::vector<MaterialBatchId> GetActiveBatches() const = 0;
    
    // 调试统计
    virtual uint32_t GetTotalBatchCount() const = 0;
    virtual uint32_t GetActiveInstanceCount() const = 0;
};
```

### 2.3 高级批处理系统实现

```cpp
class MaterialBatchManager : public IMaterialBatchManager {
public:
    // 单例访问
    static MaterialBatchManager& Get();

    // IMaterialBatchManager接口实现
    virtual MaterialBatchId CreateBatch(const MaterialBatchParameters& params) override;
    virtual MaterialInstanceInfo RegisterMaterialInstance(const MaterialBatchId& batchId, const Ref<Material>& material) override;
    virtual void UpdateMaterialData(const MaterialInstanceInfo& instanceInfo, const void* data, uint32_t size) override;
    virtual void SortBatches() override;
    virtual void BindBatch(const MaterialBatchId& batchId, void* cmdList) override;
    virtual void RenderBatch(const MaterialBatchId& batchId, void* cmdList) override;
    virtual void BeginFrame() override;
    virtual void EndFrame() override;
    virtual std::vector<MaterialBatchId> GetBatchesUsingShader(uint64_t shaderId) const override;
    virtual std::vector<MaterialBatchId> GetActiveBatches() const override;
    virtual uint32_t GetTotalBatchCount() const override;
    virtual uint32_t GetActiveInstanceCount() const override;

private:
    // 批次数据结构
    struct BatchData {
        MaterialBatchParameters params;
        uint32_t maxInstanceCount = 0;
        uint32_t activeInstanceCount = 0;
        uint32_t materialDataStride = 0;
        uint64_t frameLastUsed = 0;
        
        // 与渲染相关的信息
        PipelineStateHandle pipelineState;
        std::vector<std::pair<uint32_t, RootParameterType>> rootParameters;
        
        // 实例管理
        std::vector<uint32_t> instanceIndices;    // 活跃实例索引
        std::vector<bool> instanceActive;         // 实例活跃状态
        
        // 外部资源引用
        DescriptorAllocation sharedDescriptors;   // 由DescriptorManager分配
        RingBufferAllocation constantBuffer;      // 由FrameResourceManager分配
    };
    
    // 材质实例数据
    struct InstanceData {
        MaterialBatchId batchId;
        uint32_t instanceIndex;
        uint32_t dataSize;
        bool needsUpdate;
        void* userData;
    };
    
    // 批次存储
    std::unordered_map<MaterialBatchId, BatchData, MaterialBatchId::Hasher> m_Batches;
    
    // 实例存储
    std::unordered_map<uint64_t, InstanceData> m_Instances;  // 材质ID -> 实例数据
    
    // 活跃批次跟踪
    std::vector<MaterialBatchId> m_ActiveBatches;
    std::vector<MaterialBatchId> m_BatchesNeedingSort;
    
    // 外部系统引用
    IFrameResourceManager* m_FrameResourceManager = nullptr;
    IDescriptorManager* m_DescriptorManager = nullptr;
    
    // 批次实现方法
    BatchData& GetOrCreateBatch(const MaterialBatchParameters& params);
    uint32_t AllocateInstanceInBatch(BatchData& batch);
    void PrepareInstanceData(BatchData& batch);
    void ReleaseBatchResources(BatchData& batch);
    
    // 帧管理
    uint64_t m_CurrentFrame = 0;
    std::mutex m_Mutex;
};
```

## 三、与其他系统的集成

### 3.1 与描述符系统的解耦集成

关键变化：MaterialBatchManager不再直接管理描述符分配，而是通过接口使用DescriptorManager提供的服务。

```cpp
void MaterialBatchManager::PrepareInstanceData(BatchData& batch) {
    // 1. 请求FrameResourceManager分配常量缓冲区
    uint32_t totalSize = batch.materialDataStride * batch.maxInstanceCount;
    batch.constantBuffer = m_FrameResourceManager->AllocateBuffer(
        totalSize, 
        BufferType::ConstantBuffer,
        "MaterialBatch"
    );
    
    // 2. 请求DescriptorManager创建描述符表
    DescriptorTableDesc tableDesc;
    tableDesc.cbvCount = 1;  // 指向材质常量缓冲区
    
    // 对于批次中的共享资源（如纹理），添加额外描述符
    for (const auto& texId : batch.params.textureIds) {
        tableDesc.srvCount++;
    }
    
    // 由DescriptorManager负责分配连续描述符空间
    batch.sharedDescriptors = m_DescriptorManager->AllocateDescriptorTable(tableDesc);
    
    // 3. 设置CBV（指向材质常量缓冲区）
    if (batch.constantBuffer.IsValid() && batch.sharedDescriptors.IsValid()) {
        DescriptorWriteDesc cbvWrite;
        cbvWrite.type = DescriptorType::CBV;
        cbvWrite.handle = batch.sharedDescriptors.GetDescriptor(0); // 第一个描述符
        cbvWrite.bufferLocation = batch.constantBuffer.gpuAddress;
        cbvWrite.bufferSize = totalSize;
        
        m_DescriptorManager->UpdateDescriptor(cbvWrite);
        
        // 4. 设置SRV（纹理）
        uint32_t index = 1; // CBV之后
        for (const auto& texId : batch.params.textureIds) {
            auto texture = ResourceManager::Get().GetTexture(texId);
            if (texture) {
                DescriptorWriteDesc srvWrite;
                srvWrite.type = DescriptorType::SRV;
                srvWrite.handle = batch.sharedDescriptors.GetDescriptor(index++);
                srvWrite.resource = texture->GetNativeResource();
                
                m_DescriptorManager->UpdateDescriptor(srvWrite);
            }
        }
    }
}
```

### 3.2 与渲染系统的集成

```cpp
// 渲染器如何使用MaterialBatchManager
void Renderer::RenderScene(const Scene& scene) {
    auto& batchManager = MaterialBatchManager::Get();
    
    // 1. 注册和更新材质
    for (const auto& entity : scene.GetEntities()) {
        if (entity.HasComponent<MeshComponent>() && entity.HasComponent<MaterialComponent>()) {
            auto& mesh = entity.GetComponent<MeshComponent>().mesh;
            auto& material = entity.GetComponent<MaterialComponent>().material;
            
            // 创建材质批次参数
            MaterialBatchParameters params;
            params.shaderId = material->GetShaderId();
            params.renderStateFlags = material->GetRenderStateFlags();
            
            for (const auto& tex : material->GetTextures()) {
                params.textureIds.push_back(tex->GetId());
            }
            
            // 获取或创建批次
            MaterialBatchId batchId = batchManager.CreateBatch(params);
            
            // 注册材质实例
            MaterialInstanceInfo instanceInfo = batchManager.RegisterMaterialInstance(batchId, material);
            
            // 如果材质数据已更改，更新数据
            if (material->IsDirty()) {
                const void* data = material->GetConstantData();
                uint32_t size = material->GetConstantDataSize();
                
                batchManager.UpdateMaterialData(instanceInfo, data, size);
                material->ClearDirty();
            }
            
            // 存储渲染项
            RenderItem item;
            item.mesh = mesh;
            item.materialInstanceInfo = instanceInfo;
            m_RenderItems.push_back(item);
        }
    }
    
    // 2. 对批次排序（可由多种策略控制）
    batchManager.SortBatches();
    
    // 3. 按批次渲染
    CommandList* cmdList = m_CommandQueue->GetCommandList();
    
    MaterialBatchId currentBatch;
    for (const auto& item : m_RenderItems) {
        // 如果切换到新批次，绑定批次状态
        if (!(currentBatch == item.materialInstanceInfo.batchId)) {
            currentBatch = item.materialInstanceInfo.batchId;
            batchManager.BindBatch(currentBatch, cmdList);
        }
        
        // 设置mesh和绘制
        item.mesh->Bind(cmdList);
        item.mesh->Draw(cmdList);
    }
}
```

## 四、批次渲染流程优化

### 4.1 批次排序策略

MaterialBatchManager的主要优化来自于批次排序，大幅减少状态切换：

```cpp
void MaterialBatchManager::SortBatches() {
    // 获取需要排序的批次
    std::vector<MaterialBatchId> batchesToSort = m_BatchesNeedingSort;
    m_BatchesNeedingSort.clear();
    
    // 优先根据以下条件排序:
    
    // 1. 着色器程序（最高优先级）
    std::sort(batchesToSort.begin(), batchesToSort.end(), 
        [this](const MaterialBatchId& a, const MaterialBatchId& b) {
            return m_Batches[a].params.shaderId < m_Batches[b].params.shaderId;
        }
    );
    
    // 2. 在着色器组内，按渲染状态排序
    // PSO切换通常比着色器切换开销小，但仍然昂贵
    auto shaderGroupStart = batchesToSort.begin();
    while (shaderGroupStart != batchesToSort.end()) {
        auto shaderGroupEnd = std::find_if(shaderGroupStart + 1, batchesToSort.end(),
            [this, shaderGroupStart](const MaterialBatchId& batch) {
                return m_Batches[batch].params.shaderId != m_Batches[*shaderGroupStart].params.shaderId;
            }
        );
        
        std::sort(shaderGroupStart, shaderGroupEnd,
            [this](const MaterialBatchId& a, const MaterialBatchId& b) {
                return m_Batches[a].params.renderStateFlags < m_Batches[b].params.renderStateFlags;
            }
        );
        
        shaderGroupStart = shaderGroupEnd;
    }
    
    // 3. 在渲染状态组内，最后按纹理排序
    // 这是最细粒度的排序，最小化纹理切换
    auto stateGroupStart = batchesToSort.begin();
    while (stateGroupStart != batchesToSort.end()) {
        auto stateGroupEnd = std::find_if(stateGroupStart + 1, batchesToSort.end(),
            [this, stateGroupStart](const MaterialBatchId& batch) {
                return m_Batches[batch].params.shaderId != m_Batches[*stateGroupStart].params.shaderId ||
                       m_Batches[batch].params.renderStateFlags != m_Batches[*stateGroupStart].params.renderStateFlags;
            }
        );
        
        std::sort(stateGroupStart, stateGroupEnd,
            [this](const MaterialBatchId& a, const MaterialBatchId& b) {
                const auto& texA = m_Batches[a].params.textureIds;
                const auto& texB = m_Batches[b].params.textureIds;
                return std::lexicographical_compare(texA.begin(), texA.end(), texB.begin(), texB.end());
            }
        );
        
        stateGroupStart = stateGroupEnd;
    }
    
    // 更新活跃批次列表
    m_ActiveBatches = std::move(batchesToSort);
}
```

### 4.2 材质实例管理

解耦后的实例管理专注于跟踪和组织材质实例：

```cpp
MaterialInstanceInfo MaterialBatchManager::RegisterMaterialInstance(const MaterialBatchId& batchId, const Ref<Material>& material) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    
    uint64_t materialId = material->GetId();
    
    // 检查材质是否已注册到批次
    auto it = m_Instances.find(materialId);
    if (it != m_Instances.end() && it->second.batchId == batchId) {
        // 已存在且在同一批次中，直接返回
        return MaterialInstanceInfo {
            batchId,
            it->second.instanceIndex,
            it->second.instanceIndex * m_Batches[batchId].materialDataStride
        };
    }
    
    // 获取批次数据
    auto batchIt = m_Batches.find(batchId);
    if (batchIt == m_Batches.end()) {
        return MaterialInstanceInfo{}; // 无效批次
    }
    
    BatchData& batch = batchIt->second;
    
    // 分配新实例
    uint32_t instanceIndex = AllocateInstanceInBatch(batch);
    if (instanceIndex == UINT32_MAX) {
        return MaterialInstanceInfo{}; // 分配失败
    }
    
    // 创建实例数据
    InstanceData instanceData;
    instanceData.batchId = batchId;
    instanceData.instanceIndex = instanceIndex;
    instanceData.dataSize = material->GetConstantDataSize();
    instanceData.needsUpdate = true;
    instanceData.userData = nullptr;
    
    // 如果材质已存在但在不同批次，先从旧批次中移除
    if (it != m_Instances.end()) {
        BatchData& oldBatch = m_Batches[it->second.batchId];
        oldBatch.instanceActive[it->second.instanceIndex] = false;
        oldBatch.activeInstanceCount--;
        
        // 将旧批次标记为需要排序
        m_BatchesNeedingSort.push_back(it->second.batchId);
    }
    
    // 注册新实例
    m_Instances[materialId] = instanceData;
    
    // 更新批次状态
    batch.frameLastUsed = m_CurrentFrame;
    batch.instanceActive[instanceIndex] = true;
    batch.activeInstanceCount++;
    
    // 将批次标记为需要排序
    if (std::find(m_BatchesNeedingSort.begin(), m_BatchesNeedingSort.end(), batchId) == m_BatchesNeedingSort.end()) {
        m_BatchesNeedingSort.push_back(batchId);
    }
    
    // 返回实例信息
    return MaterialInstanceInfo {
        batchId,
        instanceIndex,
        instanceIndex * batch.materialDataStride
    };
}
```

## 五、设计优势

### 5.1 解耦的好处

1. **职责分离**
   - MaterialBatchManager：专注于批次组织和优化
   - DescriptorManager：负责描述符分配和管理
   - FrameResourceManager：负责内存分配和资源上传
   - 每个系统有明确边界，减少耦合

2. **灵活性增强**
   - 批次创建和实例管理逻辑独立
   - 可以轻松支持不同的批次策略
   - 资源分配策略可独立于批次逻辑变化

3. **易于维护和扩展**
   - 修改描述符分配策略无需改动批次管理
   - 添加新的批次排序算法不影响底层资源管理
   - 单元测试可以针对独立组件进行

### 5.2 性能优势

1. **批次粒度控制**
   - 可灵活选择批次划分粒度
   - 在状态变化和批次数量之间取得平衡
   - 支持动态调整批次大小

2. **多层次排序优化**
   - 按成本递减顺序排序：着色器 > PSO > 纹理 > 常量
   - 最小化昂贵的状态切换操作
   - 预先知道完整的渲染序列，可全局优化

3. **实例重用**
   - 实例数据布局优化为GPU访问模式
   - 批次内实例共享描述符资源
   - 材质变化可以保留在同一批次内

## 六、总结

这个重新设计的MaterialBatchManager具有以下特点：

1. **清晰的职责边界**：专注于批次组织和优化，而不是资源管理
2. **解耦的系统协作**：通过定义良好的接口与其他系统集成
3. **高性能批处理**：多层次排序和优化的实例管理
4. **灵活的批次策略**：支持不同的批次创建和排序策略
5. **简化的实现**：每个组件职责单一，实现简单明了

这种设计避免了与描述符系统和资源管理的职责重叠，并专注于MaterialBatchManager的核心功能：组织和优化材质批次以实现高性能渲染。
