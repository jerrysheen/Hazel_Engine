```mermaid
classDiagram
    %% 资源类层次结构
    class TextureResource {
        <<abstract>>
        +GetWidth() uint32_t
        +GetHeight() uint32_t
        +GetFormat() TextureFormat
        +GetNativeResource() void*
        #m_Width: uint32_t
        #m_Height: uint32_t
        #m_Format: TextureFormat
    }
    
    class Texture2DResource {
        +Create(width, height, format, usageFlags) Ref~Texture2DResource~
        +Create(path) Ref~Texture2DResource~
        +GetNativeResource() void*
    }
    
    class Texture3DResource {
        +Create(width, height, depth, format, usageFlags) Ref~Texture3DResource~
        +GetNativeResource() void*
    }
    
    class TextureCubeResource {
        +Create(size, format, usageFlags) Ref~TextureCubeResource~
        +Create(paths[6]) Ref~TextureCubeResource~
        +GetNativeResource() void*
    }
    
    %% 视图类层次结构
    class TextureView {
        <<abstract>>
        +Bind(slot) void
        +GetResource() Ref~TextureResource~
        #m_Resource: Ref~TextureResource~
    }
    
    class ShaderResourceView {
        +Create(resource) Ref~ShaderResourceView~
        +Bind(slot) void
    }
    
    class RenderTargetView {
        +Create(resource) Ref~RenderTargetView~
        +Bind(slot) void
        +Clear(clearColor) void
    }
    
    class DepthStencilView {
        +Create(resource) Ref~DepthStencilView~
        +Bind(slot) void
        +Clear(depth, stencil) void
    }
    
    %% 高级渲染目标类
    class RenderTarget {
        +RenderTarget(colorAttachments, depthStencil)
        +Bind() void
        +Unbind() void
        +Clear(clearColor) void
        +GetColorAttachmentSRV(index) Ref~ShaderResourceView~
        -m_ColorAttachments: vector~RenderTargetView~
        -m_DepthStencil: Ref~DepthStencilView~
    }
    
    %% 关系
    TextureResource <|-- Texture2DResource
    TextureResource <|-- Texture3DResource
    TextureResource <|-- TextureCubeResource
    
    TextureView <|-- ShaderResourceView
    TextureView <|-- RenderTargetView
    TextureView <|-- DepthStencilView
    
    TextureView o-- TextureResource : references
    RenderTarget o-- RenderTargetView : contains
    RenderTarget o-- DepthStencilView : contains
    RenderTarget ..> ShaderResourceView : creates
```

# 纹理资源架构

上图展示了我们设计的纹理资源架构，采用"资源+视图"的模式。这种设计将纹理系统分为两个核心概念：

## 资源类

- **TextureResource**: 抽象基类，表示GPU上的内存资源
  - **Texture2DResource**: 2D纹理实现
  - **Texture3DResource**: 3D纹理实现
  - **TextureCubeResource**: 立方体贴图实现

## 视图类

- **TextureView**: 抽象基类，定义资源的使用方式
  - **ShaderResourceView**: 用于着色器读取纹理
  - **RenderTargetView**: 用于作为渲染目标
  - **DepthStencilView**: 用于深度/模板测试

## 高级抽象

- **RenderTarget**: 高级类，组合多个视图，简化渲染目标操作

这种设计的优势在于：
1. 符合现代图形API的资源管理模型
2. 支持资源复用（同一纹理可用于不同目的）
3. 职责分离明确
4. 提供类型安全的API

## TextureView与GfxViewManager的关系

```mermaid
classDiagram
    %% 主要类
    class TextureResource {
        <<abstract>>
        +GetNativeResource() void*
    }
    
    class TextureView {
        <<abstract>>
        +Bind(slot) void
        -m_DescriptorHandle
    }
    
    class GfxViewManager {
        <<singleton>>
        +GetOrCreateViewDesc(resource, viewType) Ref~GfxDesc~
        +FindViewDesc(resourceId, viewType) Ref~GfxDesc~
        -m_RtvHeap: Ref~GfxDescHeap~
        -m_SrvHeap: Ref~GfxDescHeap~
        -m_DsvHeap: Ref~GfxDescHeap~
        -m_ViewCache: Map
    }
    
    class GfxDescHeap {
        +Allocate() GfxDesc
        +GetCPUHandle(index) void*
        +GetGPUHandle(index) void*
    }
    
    class GfxDesc {
        +GetCPUHandle() void*
        +GetGPUHandle() void*
    }
    
    %% 具体视图类
    class ShaderResourceView {
        +Create(resource) Ref~ShaderResourceView~
    }
    
    class RenderTargetView {
        +Create(resource) Ref~RenderTargetView~
    }
    
    class DepthStencilView {
        +Create(resource) Ref~DepthStencilView~
    }
    
    %% 关系
    TextureView <|-- ShaderResourceView
    TextureView <|-- RenderTargetView
    TextureView <|-- DepthStencilView
    
    TextureView o-- TextureResource : references
    TextureView o-- GfxDesc : holds descriptor
    
    GfxViewManager o-- GfxDescHeap : manages
    GfxDescHeap *-- GfxDesc : allocates
    
    ShaderResourceView ..> GfxViewManager : requests SRV descriptor
    RenderTargetView ..> GfxViewManager : requests RTV descriptor
    DepthStencilView ..> GfxViewManager : requests DSV descriptor
```

TextureView与GfxViewManager之间存在关键的协作关系：

1. **视图创建过程**
   - 当创建一个新的视图（如ShaderResourceView）时，该视图需要向GfxViewManager请求一个合适的描述符
   - GfxViewManager根据资源和视图类型在对应的描述符堆中分配描述符空间
   - 视图存储获得的描述符句柄，以便后续绑定操作使用

2. **描述符管理**
   - GfxViewManager负责管理各类描述符堆（SRV、RTV、DSV等）
   - 使用资源ID+视图类型作为键，缓存已创建的描述符避免重复创建
   - 处理描述符的生命周期管理

3. **接口协调**
   - 视图对象提供高级抽象API（Bind、Clear等）
   - GfxViewManager提供底层描述符管理功能
   - 两者配合实现资源到GPU管线的绑定

这种设计充分利用了现代图形API（特别是D3D12）的描述符模型，同时通过抽象层次保持了代码的可维护性。
