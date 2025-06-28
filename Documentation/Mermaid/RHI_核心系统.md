```mermaid
graph LR;
    %% 核心系统模块
    RenderAPI[RenderAPI抽象基类]
    GraphicsDevice[GraphicsDevice抽象基类]
    SwapChain[SwapChain抽象基类]
    
    %% 内部关系
    RenderAPI -- 创建 --> GraphicsDevice
    GraphicsDevice -- 创建 --> SwapChain
``` 