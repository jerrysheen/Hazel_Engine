# Hazel引擎架构重构建议文档

## 项目概述

**项目名称**: Hazel_Engine  
**当前状态**: 游戏引擎开发中  
**主要技术栈**: C++, OpenGL/DirectX12, ImGui  
**参考架构**: Unity Engine

## 当前结构分析

### 现有问题
1. **模块边界不清晰**: Gfx、Graphics、Renderer职责重叠
2. **缺乏统一基类系统**: 没有类似Unity Object的基础架构
3. **资源管理分散**: Asset相关代码分布在多个目录
4. **平台代码混杂**: 平台特定代码与通用逻辑耦合
5. **工具链不完整**: 缺乏独立的工具和测试体系

### 现有优势
1. **RHI抽象良好**: 已有较好的渲染API抽象
2. **第三方库集成完善**: vendor目录组织合理
3. **编辑器分离**: Hazelnut独立于引擎核心

## 重构建议

### 1. 顶层目录重组

```
Hazel_Engine/
├── Engine/                      # 引擎核心 (原Hazel/)
│   ├── Runtime/                 # 运行时核心
│   ├── Platform/                # 平台特定实现  
│   └── Bindings/                # 脚本绑定
│
├── Editor/                      # 编辑器 (原Hazelnut/)
│   ├── Core/                    # 编辑器核心
│   ├── Panels/                  # 编辑器面板
│   └── Assets/                  # 编辑器资源
│
├── Tools/                       # 工具集合
│   ├── AssetProcessor/          # 资源处理工具
│   ├── ShaderCompiler/          # 着色器编译器
│   └── BuildTools/              # 构建工具 (原build_scripts/)
│
├── Tests/                       # 测试 (原Sandbox/)
│   ├── Runtime/                 # 运行时测试
│   ├── Integration/             # 集成测试
│   └── Performance/             # 性能测试
│
├── Documentation/               # 文档整合
│   ├── API/                     # API文档
│   ├── Architecture/            # 架构文档 (原AI/Doc/)
│   └── Diagrams/                # 图表 (原Mermaid/)
│
└── ThirdParty/                  # 第三方库 (原vendor/)
```

### 2. Runtime核心模块架构

```
Engine/Runtime/
├── Core/                        # 核心基础设施
│   ├── BaseClasses/             # 基础类系统 (参考Unity Object)
│   │   ├── Object.h/.cpp        # 基础对象类
│   │   ├── NamedObject.h        # 命名对象
│   │   ├── InstanceID.h         # 实例ID系统
│   │   └── PPtr.h               # 智能指针系统
│   │
│   ├── Memory/                  # 内存管理
│   │   ├── Allocator/           # 内存分配器
│   │   ├── MemoryLabel.h        # 内存标签
│   │   └── GarbageCollector.h   # 垃圾回收
│   │
│   ├── Threading/               # 线程系统
│   ├── Serialization/           # 序列化系统
│   ├── Events/                  # 事件系统 (保持现有)
│   ├── Math/                    # 数学库
│   ├── Containers/              # 容器类
│   └── Utility/                 # 工具类
│
├── Asset/                       # 资源管理系统
│   ├── Core/
│   │   ├── AssetDatabase.h      # 资源数据库
│   │   ├── AssetImporter.h      # 资源导入器
│   │   └── AssetReference.h     # 资源引用
│   ├── Loaders/                 # 资源加载器
│   └── Processors/              # 资源处理器
│
├── Graphics/                    # 图形渲染系统
│   ├── RHI/                     # 渲染硬件接口 (保持现有)
│   ├── Renderer/                # 渲染器 (整合现有Renderer/Gfx)
│   ├── Material/                # 材质系统 (保持现有)
│   ├── Mesh/                    # 网格系统 (原Model/)
│   ├── Shader/                  # 着色器系统 (保持现有)
│   ├── Texture/                 # 纹理系统
│   └── Camera/                  # 相机系统
│
├── Scene/                       # 场景系统 (扩展现有)
│   ├── Core/
│   ├── Components/              # 组件系统
│   └── Systems/                 # 系统
│
├── Audio/                       # 音频系统 (新增)
├── Physics/                     # 物理系统 (新增)
├── Scripting/                   # 脚本系统 (新增)
├── Networking/                  # 网络系统 (新增)
└── Debug/                       # 调试系统 (原Debug/)
```

## 关键设计模式

### 1. 统一基础对象系统 (参考Unity Object)

```cpp
// Engine/Runtime/Core/BaseClasses/Object.h
class EXPORT_ENGINE Object : public NonCopyable
{
public:
    Object(MemLabelId label, ObjectCreationMode mode);
    virtual ~Object();
    
    // 生命周期管理
    virtual void AwakeFromLoad(AwakeFromLoadMode mode);
    virtual void Reset();
    virtual void MainThreadCleanup();
    
    // 类型系统
    template<typename T> bool Is() const;
    const Type* GetType() const;
    
    // 实例管理
    InstanceID GetInstanceID() const;
    static Object* IDToPointer(InstanceID id);
    
    // 序列化
    virtual void Serialize(Archive& archive);
    
    // 内存管理
    virtual size_t GetRuntimeMemorySize() const;
    
private:
    InstanceID m_InstanceID;
    UInt32 m_CachedTypeIndex;
    HideFlags m_HideFlags;
    MemLabelId m_MemoryLabel;
};
```

### 2. 资源管理系统

```cpp
// Engine/Runtime/Asset/Core/AssetDatabase.h
class AssetDatabase
{
public:
    // 异步加载资源
    template<typename T>
    static AssetHandle<T> LoadAssetAsync(const AssetID& id);
    
    // 同步加载资源
    template<typename T> 
    static T* LoadAsset(const AssetID& id);
    
    // 注册资源类型
    static void RegisterAssetType<T>();
    
    // 导入资源
    static void ImportAsset(const std::string& path);
    
    // 资源引用计数管理
    static void AddReference(const AssetID& id);
    static void RemoveReference(const AssetID& id);
    
private:
    static std::unordered_map<AssetID, std::unique_ptr<Asset>> s_Assets;
    static std::unordered_map<std::string, AssetImporter*> s_Importers;
};
```

### 3. 模块化初始化系统

```cpp
// Engine/Runtime/Core/ModuleManager.h
class ModuleManager
{
public:
    static void Initialize();
    static void Shutdown();
    
    template<typename T>
    static void RegisterModule();
    
    template<typename T>
    static T* GetModule();
    
private:
    static std::vector<std::unique_ptr<IModule>> s_Modules;
    static std::unordered_map<TypeID, IModule*> s_ModuleMap;
};

// 模块接口
class IModule
{
public:
    virtual ~IModule() = default;
    virtual void Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual void Update(float deltaTime) = 0;
};
```

## 实施步骤

### 阶段1: 基础架构重构 (1-2周)
1. **目录重组**: 按新结构重新组织现有代码
2. **基础对象系统**: 实现Object基类和InstanceID系统
3. **模块管理器**: 建立模块化初始化框架

### 阶段2: 核心系统整合 (2-3周)
1. **图形系统整合**: 合并Gfx、Graphics、Renderer模块
2. **资源系统重构**: 建立统一的AssetDatabase
3. **序列化系统**: 实现统一的序列化框架

### 阶段3: 工具链完善 (1-2周)
1. **编辑器重构**: 基于新架构重构编辑器
2. **构建系统更新**: 更新premake配置
3. **测试框架**: 建立完整的测试体系

### 阶段4: 扩展功能 (按需)
1. **音频系统**: 添加音频管理模块
2. **物理系统**: 集成物理引擎
3. **脚本系统**: 添加脚本绑定支持

## 迁移策略

### 代码迁移映射
```
原路径 -> 新路径
Hazel/src/Hazel/Core/ -> Engine/Runtime/Core/
Hazel/src/Hazel/Gfx/ -> Engine/Runtime/Graphics/Renderer/
Hazel/src/Hazel/Graphics/ -> Engine/Runtime/Graphics/RHI/
Hazel/src/Hazel/Material/ -> Engine/Runtime/Graphics/Material/
Hazel/src/Hazel/Model/ -> Engine/Runtime/Graphics/Mesh/
Hazel/src/Hazel/Renderer/ -> Engine/Runtime/Graphics/Renderer/
Hazel/src/Platform/ -> Engine/Platform/
Hazelnut/ -> Editor/
Sandbox/ -> Tests/Runtime/
```

### 重构优先级
1. **高优先级**: Core、Graphics模块整合
2. **中优先级**: Asset系统、Scene系统
3. **低优先级**: 新增模块 (Audio、Physics等)

## 预期收益

### 1. 可维护性提升
- **模块边界清晰**: 每个模块职责明确
- **依赖关系简化**: 减少模块间耦合
- **代码复用增强**: 统一的基础架构

### 2. 扩展性增强
- **插件化架构**: 新功能作为模块插入
- **平台适配简化**: 平台代码完全分离
- **第三方集成便利**: 标准化的接口

### 3. 开发效率提升
- **并行开发支持**: 模块间相对独立
- **调试能力增强**: 清晰的调用链
- **工具链完整**: 编辑器、构建、测试一体化

## 注意事项

1. **渐进式重构**: 避免一次性大规模改动
2. **向后兼容**: 保持现有API的兼容性
3. **测试覆盖**: 每个阶段都要有充分的测试
4. **文档同步**: 及时更新架构文档

这个重构将使Hazel引擎具备现代游戏引擎的标准架构，为后续功能扩展和团队协作奠定坚实基础。