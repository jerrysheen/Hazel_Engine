# 材质系统重构总结

## 重构原因

原有的 `Material.h` 文件存在以下问题：
1. **职责过于复杂** - 一个文件包含了材质属性、材质核心逻辑、序列化和资源管理等多种职责
2. **代码维护困难** - 1300+ 行的实现文件难以维护和扩展
3. **目录结构不合理** - Material 和相关功能散布在不同目录
4. **可读性差** - 功能混杂在一起，新开发者难以理解

## 重构后的目录结构

```
Hazel/src/Hazel/
├── Asset/                    # 资源管理系统
│   └── MaterialLibrary.h    # 材质库 - 专门负责材质资源管理
├── Material/                 # 材质系统
│   ├── Material.h           # 核心材质类 - 只包含材质核心逻辑
│   ├── MaterialProperty.h   # 材质属性 - 独立的属性系统
│   ├── MaterialSerializer.h # 序列化功能 - 专门负责JSON读写
│   └── MaterialSystem.h     # 统一入口 - 方便使用的头文件
└── Renderer/                # 渲染系统
    └── Shader.h             # 着色器保持在渲染系统中
```

## 职责分离

### 1. MaterialProperty.h
- **职责**: 材质属性值的存储和管理
- **内容**: 
  - `MaterialPropertyType` 枚举
  - `MaterialProperty` 类
  - `MaterialPropertyBlock` 结构体

### 2. Material.h  
- **职责**: 材质的核心逻辑
- **内容**:
  - 核心 `Material` 类
  - 属性设置/获取接口
  - 与着色器的交互逻辑

### 3. MaterialSerializer.h
- **职责**: 材质的序列化和反序列化
- **内容**:
  - JSON 文件读写
  - 字符串序列化
  - 序列化辅助方法

### 4. MaterialLibrary.h
- **职责**: 材质资源的管理和缓存
- **内容**:
  - 材质注册/获取
  - 文件加载/保存
  - 资源生命周期管理

## 使用方式

### 推荐使用方式
```cpp
// 使用统一入口，包含所有材质功能
#include "Hazel/Material/MaterialSystem.h"

// 创建材质
auto material = Material::Create(shader);

// 使用材质库
MaterialLib::Get().Register("myMaterial", material);

// 序列化材质
MaterialSerializer::SerializeToJSON(material, "material.json");
```

### 按需包含
```cpp
// 只需要核心材质功能
#include "Hazel/Material/Material.h"

// 只需要序列化功能
#include "Hazel/Material/MaterialSerializer.h"

// 只需要材质库
#include "Hazel/Asset/MaterialLibrary.h"
```

## 向后兼容性

- 原有的 `Hazel/Model/Material.h` 文件已重构为向后兼容的包装器
- 现有代码无需修改即可继续工作
- 建议逐步迁移到新的材质系统

## 关于目录选择的建议

### Material vs Shader 的位置
- **Shader**: 保持在 `Renderer/` 目录
  - 理由: Shader 是渲染管线的核心组件，与渲染 API 紧密相关
  
- **Material**: 移到独立的 `Material/` 目录  
  - 理由: Material 是高层的图形抽象，使用 Shader 但不依赖特定的渲染 API

### 为什么 MaterialLibrary 在 Asset/ 目录
- MaterialLibrary 本质上是一个资源管理器
- 可以与其他资源管理器（TextureLibrary、MeshLibrary 等）保持一致的架构
- 便于将来实现统一的资源管理系统

## 性能优化

重构后的系统具有以下优化：
1. **编译时间** - 按需包含减少编译依赖
2. **内存使用** - 独立的组件可以更好地控制生命周期
3. **可扩展性** - 每个组件都可以独立扩展而不影响其他部分

## 后续改进建议

1. **实现对应的 .cpp 文件** - 将模板特化和实现代码分离到对应的 .cpp 文件
2. **添加单元测试** - 为每个独立组件编写测试
3. **考虑异步加载** - MaterialLibrary 可以支持异步材质加载
4. **实现材质继承体系** - 如 PBRMaterial、UnlitMaterial 等 