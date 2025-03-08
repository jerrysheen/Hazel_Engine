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

## 完整架构图
```mermaid
graph LR;
    %% 引入各个子模块
    subgraph 核心系统
        RenderAPI[RenderAPI抽象基类]
        GraphicsDevice[GraphicsDevice抽象基类]
        SwapChain[SwapChain抽象基类]
    end

    subgraph 命令管理
        CommandQueue[CommandQueue抽象基类]
        CommandList[CommandList抽象基类]
        Fence[Fence抽象基类]
    end

    subgraph 管线系统
        GraphicsPipeline[GraphicsPipeline抽象基类]
        RootSignature[RootSignature抽象基类]
        Shader[Shader抽象基类]
        RenderState[RenderState抽象基类]
    end

    subgraph 状态管理
        BlendState[BlendState]
        DepthStencilState[DepthStencilState]
        RasterizerState[RasterizerState]
    end

    subgraph 资源系统
        Buffer[Buffer抽象基类]
        Texture[Texture抽象基类]
        ResourceBinding[ResourceBinding抽象基类]
        DescriptorHeap[DescriptorHeap抽象基类]
    end

    subgraph 具体缓冲区
        VertexBuffer[VertexBuffer]
        IndexBuffer[IndexBuffer]
        ConstantBuffer[ConstantBuffer/UniformBuffer]
    end

    subgraph 具体纹理
        Texture2D[Texture2D]
        TextureCube[TextureCube]
        RenderTarget[RenderTarget]
    end

    subgraph 渲染上下文
        RenderContext[RenderContext抽象基类]
        RenderQueue[RenderQueue]
        OpaqueQueue[OpaqueQueue]
        TransparentQueue[TransparentQueue]
    end

    %% 建立继承关系
    Buffer --> VertexBuffer
    Buffer --> IndexBuffer
    Buffer --> ConstantBuffer
    
    Texture --> Texture2D
    Texture --> TextureCube
    Texture --> RenderTarget
    
    RenderState --> BlendState
    RenderState --> DepthStencilState
    RenderState --> RasterizerState
    
    RenderQueue --> OpaqueQueue
    RenderQueue --> TransparentQueue
    
    %% 模块间关系
    RenderAPI -- 创建 --> GraphicsDevice
    GraphicsDevice -- 创建 --> CommandQueue
    GraphicsDevice -- 创建 --> SwapChain
    GraphicsDevice -- 创建 --> GraphicsPipeline
    GraphicsDevice -- 创建 --> RootSignature
    GraphicsDevice -- 创建 --> DescriptorHeap
    GraphicsDevice -- 创建 --> Buffer
    GraphicsDevice -- 创建 --> Texture
    
    CommandQueue -- 执行 --> CommandList
    CommandQueue -- 使用 --> Fence
    
    CommandList -- 设置 --> GraphicsPipeline
    CommandList -- 设置 --> RootSignature
    CommandList -- 绑定 --> Buffer
    CommandList -- 绑定 --> Texture
    CommandList -- 绑定 --> RenderState
    
    GraphicsPipeline -- 包含 --> RootSignature
    GraphicsPipeline -- 包含 --> Shader
    GraphicsPipeline -- 包含 --> RenderState
    
    RootSignature -- 定义 --> ResourceBinding
    
    RenderContext -- 管理 --> CommandList
    RenderContext -- 使用 --> GraphicsPipeline
    RenderContext -- 管理 --> ResourceBinding
    RenderContext -- 管理 --> RenderState
    RenderContext -- 处理 --> RenderQueue
    
    ResourceBinding -- 绑定 --> ConstantBuffer
    ResourceBinding -- 绑定 --> Texture
    ResourceBinding -- 使用 --> DescriptorHeap
    
    SwapChain -- 使用 --> RenderTarget