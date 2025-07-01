# Buffer系统重构计划

## 设计理念转变

### 问题识别
之前的Buffer设计存在**过度复杂化**和**过度抽象化**的问题。通过研究Unity和Unreal的实际架构，发现它们更注重**实用性**而不是**理论完美性**。

### 参考架构分析

#### Unity的简化方法
```cpp
// Unity的做法很直接
class GraphicsBuffer {  // 一个类处理所有Buffer类型
    enum Target { Vertex, Index, Constant, Structured, ... };
};

class RenderTexture : public Texture { }; // 渲染目标就是特殊纹理
class Texture2D : public Texture { };     // 普通纹理
```

#### Unreal的功能域划分
```cpp
// Unreal按功能分类，不强求统一基类
class UTexture2D { };        // 纹理资源
class UStaticMesh { };       // 网格资源  
class UMaterial { };        // 材质资源
class UActorComponent { };   // 组件系统
```

## 重构设计方案

### 1. 简化的Buffer基类设计

```cpp
// 只包含真正共同的功能
class Buffer {
public:
    virtual ~Buffer() = default;
    
    // 只包含所有Buffer真正共有的功能
    virtual void* GetNativeResource() const = 0;
    virtual uint32_t GetSize() const = 0;
    virtual boost::uuids::uuid GetUUID() const { return m_UUID; }
    
    // 平台相关的资源获取 - 但简化版本
    template<typename T>
    T GetPlatformResource() const;

protected:
    boost::uuids::uuid m_UUID;
    uint32_t m_Size = 0;
};
```

### 2. 具体Buffer类型实现

#### VertexBuffer
```cpp
class VertexBuffer : public Buffer {
public:
    virtual void Bind() const = 0;
    virtual uint32_t GetStride() const = 0;
    virtual uint32_t GetVertexCount() const = 0;
    
    static Ref<VertexBuffer> Create(const void* data, uint32_t size, uint32_t stride);
};
```

#### IndexBuffer
```cpp
class IndexBuffer : public Buffer {
public:
    virtual void Bind() const = 0;
    virtual uint32_t GetIndexCount() const = 0;
    virtual bool Is16Bit() const = 0; // 16位还是32位索引
    
    static Ref<IndexBuffer> Create(const void* indices, uint32_t count, bool is16Bit = true);
};
```

#### ConstantBuffer
```cpp
class ConstantBuffer : public Buffer {
public:
    virtual void SetData(const void* data, uint32_t size) = 0;
    virtual void* Map() = 0;
    virtual void Unmap() = 0;
    
    // 类型安全的模板方法
    template<typename T>
    void SetData(const T& data) {
        SetData(&data, sizeof(T));
    }
    
    static Ref<ConstantBuffer> Create(uint32_t size);
};
```

### 3. 渲染目标独立设计

```cpp
// 渲染目标单独处理 - 不强行归入Buffer类别
class RenderTarget {
public:
    virtual void Bind() = 0;
    virtual void Clear(const glm::vec4& color = glm::vec4(0.0f)) = 0;
    virtual void Resize(uint32_t width, uint32_t height) = 0;
    
    // 获取作为纹理使用的接口
    virtual Ref<Texture2D> GetColorTexture() const = 0;
    virtual Ref<Texture2D> GetDepthTexture() const = 0;
    
    static Ref<RenderTarget> Create(uint32_t width, uint32_t height);
};
```

### 4. 简化的资源管理器

```cpp
class ResourceManager {
public:
    static ResourceManager& Get();
    
    // 只管理真正需要跨系统访问的资源
    void RegisterBuffer(const Ref<Buffer>& buffer);
    void UnregisterBuffer(const boost::uuids::uuid& uuid);
    
    template<typename T>
    Ref<T> GetBuffer(const boost::uuids::uuid& uuid) {
        return std::dynamic_pointer_cast<T>(GetBufferInternal(uuid));
    }
    
private:
    std::unordered_map<boost::uuids::uuid, std::weak_ptr<Buffer>, boost::hash<boost::uuids::uuid>> m_Buffers;
    Ref<Buffer> GetBufferInternal(const boost::uuids::uuid& uuid);
};
```

## 关键改进点

### 1. 职责明确，避免过度抽象
- Buffer基类只包含真正共同的功能
- 每个子类专注于自己的职责
- RenderTarget单独设计，不强行归类到Buffer系统

### 2. 实用的接口设计
```cpp
// 使用示例 - 简单直观
auto vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices), sizeof(Vertex));
auto indexBuffer = IndexBuffer::Create(indices, indexCount, true);
auto constantBuffer = ConstantBuffer::Create(sizeof(CameraData));

// 类型安全的数据设置
CameraData cameraData = GetCameraData();
constantBuffer->SetData(cameraData);

// 渲染目标独立管理
auto renderTarget = RenderTarget::Create(1920, 1080);
renderTarget->Bind();
renderTarget->Clear(glm::vec4(0.2f, 0.3f, 0.5f, 1.0f));
```

### 3. 避免过度设计原则
- ❌ 不把所有GPU资源都塞进一个基类
- ❌ 不创建不必要的抽象层
- ✅ 专注于解决实际问题
- ✅ 参考成熟引擎的设计思路

### 4. 可选的Unity风格统一Buffer
```cpp
// 可以考虑Unity的统一Buffer方法作为备选方案
class GraphicsBuffer {
public:
    enum class Type { Vertex, Index, Constant, Structured };
    
    GraphicsBuffer(Type type, uint32_t size, const void* data = nullptr);
    
private:
    Type m_Type;
};
```

## 设计哲学总结

这次重构的核心思想是：**实用性驱动的设计**

- **简单优于复杂**：只在真正需要的地方添加抽象
- **实用优于完美**：解决实际问题比理论完美更重要
- **借鉴成熟方案**：学习Unity和Unreal的成功经验
- **渐进式改进**：可以逐步优化，不追求一步到位

这样的设计更符合实际需求，更容易维护，也更容易理解和使用。
