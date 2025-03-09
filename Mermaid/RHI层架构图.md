# RHI层架构图

## 核心系统
[核心系统模块](RHI_核心系统.md)

## 命令管理
[命令管理模块](RHI_命令管理.md)

## 管线系统
[管线系统模块](RHI_管线系统.md)

## 状态管理
[状态管理模块](RHI_状态管理.md)

## 资源系统
[资源系统模块](RHI_资源系统.md)

## 具体缓冲区
[具体缓冲区模块](RHI_具体缓冲区.md)

## 具体纹理
[具体纹理模块](RHI_具体纹理.md)

## 渲染上下文
[渲染上下文模块](RHI_渲染上下文.md)

## Mesh系统
[Mesh系统模块](RHI_Mesh系统.md)

## Shader系统
[Shader系统模块](RHI_Shader反射系统.md)

## 材质系统
[材质系统模块](RHI_材质系统.md)

## 完整架构图
```mermaid
graph TD
    %% 子模块分组
    subgraph 核心系统
        RenderAPI["RenderAPI抽象基类"]
        GraphicsDevice["GraphicsDevice抽象基类"]
        SwapChain["SwapChain抽象基类"]
    end
    
    subgraph 命令管理
        CommandQueue["CommandQueue抽象基类"]
        CommandList["CommandList抽象基类"]
        Fence["Fence抽象基类"]
    end
    
    subgraph 管线系统
        GraphicsPipeline["GraphicsPipeline抽象基类"]
        RootSignature["RootSignature抽象基类"]
        RenderState["RenderState抽象基类"]
        InputLayout["InputLayout"]
    end
    
    subgraph 状态管理
        BlendState["BlendState"]
        DepthStencilState["DepthStencilState"]
        RasterizerState["RasterizerState"]
    end
    
    subgraph 资源系统
        Buffer["Buffer抽象基类"]
        Texture["Texture抽象基类"]
        ResourceBinding["ResourceBinding抽象基类"]
        DescriptorHeap["DescriptorHeap抽象基类"]
    end
    
    subgraph 具体缓冲区
        VertexBuffer["VertexBuffer"]
        IndexBuffer["IndexBuffer"]
        ConstantBuffer["ConstantBuffer"]
    end
    
    subgraph 具体纹理
        Texture2D["Texture2D"]
        TextureCube["TextureCube"]
        RenderTarget["RenderTarget"]
    end
    
    subgraph 渲染上下文
        RenderContext["RenderContext抽象基类"]
        RenderQueue["RenderQueue"]
        OpaqueQueue["OpaqueQueue"]
        TransparentQueue["TransparentQueue"]
    end
    
    subgraph Shader系统
        Shader["Shader抽象基类"]
        ShaderReflection["ShaderReflection"]
        ShaderParameter["ShaderParameter"]
    end
    
    subgraph Mesh系统
        Mesh["Mesh抽象基类"]
        MeshData["MeshData原始数据"]
        MeshFactory["MeshFactory工厂类"]
        VertexBufferCache["VertexBufferCache"]
    end
    
    subgraph 材质系统
        Material["Material抽象基类"]
        MaterialInstance["MaterialInstance"]
        MaterialParameter["MaterialParameter"]
    end

    %% 继承关系（使用一致的语法简化）
    Buffer --> VertexBuffer & IndexBuffer & ConstantBuffer
    Texture --> Texture2D & TextureCube & RenderTarget
    RenderState --> BlendState & DepthStencilState & RasterizerState
    RenderQueue --> OpaqueQueue & TransparentQueue
    
    %% 核心关系
    RenderAPI --> GraphicsDevice
    GraphicsDevice --> CommandQueue & SwapChain & GraphicsPipeline & RootSignature & DescriptorHeap & Buffer & Texture
    
    %% 命令系统关系
    CommandQueue --> CommandList & Fence
    
    %% 管线系统关系
    GraphicsPipeline --> RootSignature & RenderState & InputLayout
    RootSignature --> ResourceBinding
    
    %% Shader系统关系
    Shader --> ShaderReflection
    ShaderReflection --> InputLayout & ShaderParameter
    
    %% Mesh系统关系
    MeshFactory --> Mesh
    Mesh --> MeshData & VertexBufferCache & VertexBuffer & IndexBuffer
    
    %% 材质系统关系
    Material --> Shader & MaterialInstance
    MaterialInstance --> MaterialParameter
    
    %% 跨系统重要关系（使用不同样式）
    Shader -.-> GraphicsPipeline & Mesh
    MaterialParameter -.-> Texture & ConstantBuffer
    
    %% 渲染上下文协调关系
    RenderContext --> CommandList & GraphicsPipeline & RenderQueue & ResourceBinding & RenderState
    RenderContext -.-> Mesh & Material
    
    %% 资源绑定
    ResourceBinding -.-> ConstantBuffer & Texture & DescriptorHeap
    SwapChain -.-> RenderTarget
    
    %% 命令执行
    CommandList -.-> GraphicsPipeline & RootSignature & Buffer & Texture & RenderState
```

## 连线优化说明

为提高架构图的可读性，主要进行了以下优化：

1. **简化连线**：
   - 使用组合语法（如`A --> B & C & D`）减少连线数量
   - 将类似关系的连线合并
   - 使用实线表示核心/主要关系，虚线表示次要关系

2. **连线设计**：
   - 从上到下的流向，减少交叉连线
   - 系统内部关系用实线，系统间关系大多用虚线
   - 将相关节点放在一起，缩短连线距离

3. **突出重点**：
   - 继承关系和核心系统关系清晰显示
   - 跨系统交互使用不同样式区分
   - 渲染上下文的协调作用通过连线模式体现