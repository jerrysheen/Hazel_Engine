# Hazel Engine 架构改造分析与建议

## 当前发现的核心问题

### 1. ImGui 滥用问题

**问题描述：**
- ImGui 被硬编码到 Engine 核心层（`Application.h` 中的 `ImGuiLayer* m_ImGuiLayer`）
- 这违反了分层原则，运行时不需要 ImGui，应该只在编辑器中使用

**影响：**
- 运行时打包会包含不必要的 ImGui 代码
- 增加了运行时的内存占用和二进制大小
- 破坏了 Engine 和 Editor 的分离

**改造建议：**
```
Current: Application -> ImGuiLayer (硬耦合)
Target:  Editor -> ImGuiLayer, Application (解耦)
```

### 2. 架构内容缺失

**当前层级结构：**
- ✅ RHI 层：`Engine/Runtime/Graphics/RHI/`
- ✅ Graphics 层：`Hazel/src/Hazel/Graphics/`
- ✅ Renderer 层：`Hazel/src/Hazel/Renderer/`
- ✅ Scene 层：`Engine/Runtime/Scene/`

**缺失的关键系统：**

#### 2.1 AssetManager 系统
**当前状态：** 已有 `MaterialLibrary` 实现，但缺乏通用的 AssetManager
**现有实现：** `Engine/Runtime/Asset/Core/MaterialLibrary.cpp`
```cpp
// 现有的 MaterialLibrary 功能
MaterialLibrary& MaterialLibrary::Get() // 单例模式
Ref<Material> LoadMaterial(const std::string& path) // 材质加载
void Register(const std::string& name, const Ref<Material>& material) // 注册
```

**需要扩展为通用 AssetManager：**
- 支持多种资产类型（纹理、模型、声音、场景等）
- 资产依赖关系管理
- 异步资产加载系统
- 资产热重加载机制
- 统一的资产ID系统

#### 2.2 Memory Allocator 系统
**当前状态：** 依赖系统默认分配器，缺乏自定义内存管理
**需要实现：**
- 池化内存分配器
- 栈式分配器（用于临时对象）
- 线性分配器（用于帧数据）
- 内存跟踪和调试工具

### 3. 代码重复和架构混乱问题

**重复代码分析：**

#### 3.1 渲染器重复
- `Hazel/Core/Application.h` - 包含渲染 API 管理
- `Hazel/Gfx/Renderer.h` - 基础渲染器
- `Hazel/Graphics/RenderAPI.h` - 渲染 API 抽象
- `Hazel/Renderer/Renderer3D.h` - 3D 渲染器

**重复功能：**
- 渲染初始化逻辑分散在多个类中
- 相似的渲染状态管理
- 重复的资源管理代码

#### 3.2 SceneViewLayer 架构混乱
**严重问题：** `Editor/SceneViewLayer.h` 违反了分层原则
```cpp
// 问题代码示例：UI层直接包含底层D3D12代码
#include "Platform/D3D12/d3dx12.h"
#include "Platform/D3D12/d3dUtil.h"

class SceneViewLayer : public Layer {
    // UI层不应该有这些底层渲染对象
    Microsoft::WRL::ComPtr<ID3D12Resource> colorBuffer;
    ComPtr<ID3D12PipelineState> mPSO = nullptr;
    ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
    ComPtr<ID3D12CommandQueue> mCommandQueue;
};
```

**影响：**
- UI层和渲染层强耦合
- 违反单一职责原则
- 难以测试和维护
- 无法复用渲染逻辑

#### 3.2 建议的统一架构
```
Application (Core)
    ↓
Graphics (RHI + Resource Management)
    ↓
Renderer (High-level Rendering)
    ↓
Scene (Entity-Component System)
```

## 改造实施建议

### 🚨 紧急优先级：SceneViewLayer 重构
**当前问题极其严重，需要立即处理：**

1. **分离渲染逻辑**
   ```cpp
   // 目标架构
   class SceneRenderer {  // 纯渲染逻辑
       void SetupRenderTarget();
       void RenderScene(const Scene& scene);
       TextureHandle GetRenderResult();
   };
   
   class SceneViewLayer {  // 纯UI逻辑
       void OnImGuiRender() override;
       void DisplaySceneTexture(TextureHandle texture);
   };
   ```

2. **移除底层API依赖**
   - 从 SceneViewLayer 中移除所有 D3D12 特定代码
   - 通过抽象接口与渲染系统通信
   - 使用事件系统传递渲染命令

### 阶段一：ImGui 解耦
1. 创建 `EditorApplication` 继承 `Application`
2. 将 `ImGuiLayer` 移动到 `EditorApplication` 中
3. 修改 Editor 项目使用 `EditorApplication`
4. 确保运行时使用纯净的 `Application`

### 阶段二：系统架构重构
1. **AssetManager 实现**
   ```cpp
   class AssetManager {
   public:
       template<typename T>
       Ref<T> Load(const std::string& path);
       
       template<typename T>
       void Preload(const std::string& path);
       
       void UnloadUnused();
       
   private:
       std::unordered_map<std::string, Ref<Asset>> m_LoadedAssets;
       ThreadPool m_LoadingThreads;
   };
   ```

2. **Memory System 实现**
   ```cpp
   class MemoryManager {
   public:
       static void Initialize();
       
       template<typename T>
       static T* Allocate(size_t count = 1);
       
       template<typename T>
       static void Deallocate(T* ptr);
       
       static MemoryStats GetStats();
   };
   ```

### 阶段三：代码去重
1. **统一渲染器架构**
   - 保留 `RenderAPI` 作为底层抽象
   - 合并 `Renderer` 和 `Renderer3D` 功能
   - 移除 `Application` 中的渲染逻辑

2. **清理重复的头文件包含**
   - 统一 graphics 相关的包含路径
   - 减少循环依赖

### 阶段四：Editor 专用功能隔离
1. 将 `SceneViewLayer` 等编辑器组件移动到 `Editor/` 目录
2. 实现打包时自动排除 Editor 代码的机制
3. 确保运行时和编辑器使用相同的底层系统但不同的上层接口

## 预期收益

**性能提升：**
- 运行时内存占用减少 20-30%
- 打包体积减少 15-25%
- 启动时间优化

**开发效率：**
- 代码复用率提升
- 维护成本降低
- 新功能开发更加便利

**架构清晰度：**
- 层级分离明确
- 职责单一原则
- 便于单元测试和调试

## 下一步行动建议

### 立即开始（本周内）：
1. **重构 SceneViewLayer** - 这是最紧急的问题
2. **创建 SceneRenderer 抽象类** - 分离渲染逻辑
3. **移除 Application 中的 ImGui 依赖** - 解耦编辑器和引擎

### 短期目标（2-4周）：
1. **基于 MaterialLibrary 扩展 AssetManager**
2. **统一渲染器架构** - 合并重复代码
3. **实现基础内存管理系统**

### 长期目标（1-2月）：
1. **完善资产管理系统**
2. **优化打包流程** - 自动排除编辑器代码
3. **性能优化和测试**

## 风险提示

⚠️ **重构 SceneViewLayer 时的注意事项：**
- 确保渲染结果正确传递到 ImGui
- 保持现有的交互功能
- 谨慎处理 D3D12 资源的生命周期管理

⚠️ **架构变更的影响：**
- 可能需要修改现有的编辑器代码
- 确保运行时性能不受影响
- 保持 API 的向后兼容性（在可能的情况下）
