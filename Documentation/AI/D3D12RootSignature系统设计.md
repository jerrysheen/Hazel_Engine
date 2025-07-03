# D3D12RootSignature系统设计文档

## 概述

D3D12RootSignature系统是一个用于管理DirectX 12根签名和资源绑定的高效解决方案。该系统参考了Unity的设计理念，通过预先分析着色器需求，自动创建和管理根签名，并提供简洁的资源绑定接口。

## 设计理念

### 1. Unity风格的资源管理
- **预计算资源需求**：在创建根签名时，预先计算每个着色器阶段需要的各类资源数量
- **槽位映射**：为不同的资源类型和着色器阶段分配固定的槽位
- **高效绑定**：在渲染时根据预计算的槽位索引快速绑定资源

### 2. 缓存机制
- **避免重复创建**：相同的根签名描述只创建一次
- **内存效率**：通过共享根签名对象节省内存
- **性能优化**：减少根签名创建的开销

### 3. 类型安全
- **强类型枚举**：使用枚举定义资源类型和槽位，避免错误绑定
- **编译时检查**：在编译时发现类型错误
- **清晰的接口**：简洁明了的API设计

## 核心组件

### 1. 资源类型枚举

```cpp
// 根签名条目类型
enum D3DRootSigEntry
{
    kD3DRootSigEntrySRV = 0,    // 着色器资源视图
    kD3DRootSigEntryCBV,        // 常量缓冲区视图
    kD3DRootSigEntrySMP,        // 采样器
    kD3DRootSigEntryCount,
    kD3DRootSigEntryUAVShared = kD3DRootSigEntryCount,
};

// 根签名槽位
enum D3DRootSigSlot
{
    kD3DRootSigSlotSRV = 0,     // 包含SRV
    kD3DRootSigSlotCBV,         // 包含CBV
    kD3DRootSigSlotSMP,         // 包含采样器
    kD3DRootSigSlotCount,
    kD3DRootSigSlotUAVShared = kD3DRootSigSlotCount,
};
```

### 2. 根签名描述

```cpp
struct D3D12RootSignatureDesc
{
    // 每个着色器阶段的每种资源类型的数量
    uint8_t count[kD3DRootSigEntryCount][static_cast<size_t>(D3D12ShaderStage::Count)];
    uint8_t countUAVs : 7;      // UAV数量
    uint8_t compute : 1;        // 是否为计算着色器
    uint8_t countBICs;          // 内建常量数量
};
```

### 3. 根签名类

```cpp
class D3D12RootSignature
{
public:
    // 从着色器反射创建根签名
    static std::unique_ptr<D3D12RootSignature> CreateFromShaderReflection(
        ID3D12Device* device,
        const std::vector<Ref<ShaderReflection>>& reflections);
    
    // 获取根参数索引
    uint8_t GetRootParameterIndex(D3DRootSigSlot slot, D3D12ShaderStage stage) const;
    
    // 获取根签名对象
    ID3D12RootSignature* GetRootSignature() const;
    
    // 检查是否有效
    bool IsValid() const;
};
```

### 4. 根签名管理器

```cpp
class D3D12RootSignatureManager
{
public:
    // 获取或创建根签名（带缓存）
    std::shared_ptr<D3D12RootSignature> GetOrCreateRootSignature(
        const D3D12RootSignatureDesc& desc);
    
    // 从着色器反射获取或创建根签名
    std::shared_ptr<D3D12RootSignature> GetOrCreateRootSignature(
        const std::vector<Ref<ShaderReflection>>& reflections);
};
```

## 使用流程

### 1. 创建根签名

```cpp
// 方法1：从着色器反射创建
auto shader = Shader::Create("path/to/shader.hlsl");
auto d3d12Shader = std::static_pointer_cast<D3D12Shader>(shader);
auto reflection = d3d12Shader->GetReflection();
std::vector<Ref<ShaderReflection>> reflections = { reflection };
auto rootSignature = D3D12RootSignature::CreateFromShaderReflection(device, reflections);

// 方法2：手动创建描述
D3D12RootSignatureDesc desc;
desc.count[kD3DRootSigEntryCBV][static_cast<size_t>(D3D12ShaderStage::Vertex)] = 1;
desc.count[kD3DRootSigEntrySRV][static_cast<size_t>(D3D12ShaderStage::Pixel)] = 2;
auto rootSignature = std::make_unique<D3D12RootSignature>(device, desc);
```

### 2. 绑定资源

```cpp
// 设置根签名
cmdList->SetGraphicsRootSignature(rootSignature->GetRootSignature());

// 获取根参数索引
uint8_t cbvIndex = rootSignature->GetRootParameterIndex(
    kD3DRootSigSlotCBV, D3D12ShaderStage::Vertex);

// 绑定常量缓冲区
if (cbvIndex != D3D12RootSignature::kInvalidIndex) {
    cmdList->SetGraphicsRootDescriptorTable(cbvIndex, descriptorHandle);
}
```

### 3. 使用管理器

```cpp
// 创建管理器
D3D12RootSignatureManager manager(device);

// 获取根签名（自动缓存）
auto rootSig1 = manager.GetOrCreateRootSignature(desc);
auto rootSig2 = manager.GetOrCreateRootSignature(desc); // 从缓存获取

// 验证缓存
assert(rootSig1 == rootSig2); // 同一个对象
```

## 与D3D12GraphicsPipeline的集成

### 修改后的D3D12GraphicsPipeline

```cpp
class D3D12GraphicsPipeline : public IGraphicsPipeline 
{
private:
    std::shared_ptr<D3D12RootSignature> m_RootSignature;
    
    void CreateRootSignature()
    {
        auto d3d12Shader = std::static_pointer_cast<D3D12Shader>(m_Description.shader);
        auto reflection = d3d12Shader->GetReflection();
        std::vector<Ref<ShaderReflection>> reflections = { reflection };
        m_RootSignature = D3D12RootSignature::CreateFromShaderReflection(m_Device.Get(), reflections);
    }
    
public:
    void Bind() const override
    {
        if (cmdList && m_RootSignature) {
            cmdList->SetPipelineState(m_PipelineState.Get());
            cmdList->SetGraphicsRootSignature(m_RootSignature->GetRootSignature());
        }
    }
};
```

## 优势

### 1. 性能优势
- **预计算**：避免运行时的资源分析
- **缓存机制**：减少重复创建的开销
- **内存效率**：共享相同的根签名对象

### 2. 易用性
- **自动化**：从着色器反射自动创建根签名
- **类型安全**：强类型枚举避免错误
- **清晰的API**：简洁的接口设计

### 3. 可维护性
- **模块化**：清晰的组件划分
- **可扩展**：容易添加新的资源类型
- **调试友好**：提供详细的布局信息

### 4. 兼容性
- **Unity风格**：借鉴成熟的设计理念
- **D3D12标准**：完全兼容D3D12 API
- **跨平台**：可以扩展到其他图形API

## 最佳实践

### 1. 使用管理器进行缓存
```cpp
// 推荐：使用管理器
D3D12RootSignatureManager manager(device);
auto rootSig = manager.GetOrCreateRootSignature(reflections);

// 不推荐：直接创建
auto rootSig = D3D12RootSignature::CreateFromShaderReflection(device, reflections);
```

### 2. 检查有效性
```cpp
if (!rootSignature || !rootSignature->IsValid()) {
    HZ_CORE_ERROR("Failed to create root signature");
    return;
}
```

### 3. 使用调试信息
```cpp
#ifdef HZ_DEBUG
rootSignature->GetDesc().PrintLayout();
#endif
```

## 总结

D3D12RootSignature系统提供了一个高效、易用、类型安全的根签名管理解决方案。通过借鉴Unity的设计理念，该系统能够自动分析着色器需求，创建优化的根签名，并提供简洁的资源绑定接口。这不仅提高了开发效率，还保证了运行时的性能。 