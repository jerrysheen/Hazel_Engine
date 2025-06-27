# 材质系统实现文件说明

## 📁 文件结构

基于原始的 `Material.cpp` (1343行)，我们已将其重构为多个独立的实现文件：

```
Hazel/src/Hazel/
├── Material/
│   ├── MaterialProperty.cpp   # 材质属性实现 (~200行)
│   ├── Material.cpp          # 材质核心实现 (~400行)
│   └── MaterialSerializer.cpp # 序列化实现 (~350行)
└── Asset/
    └── MaterialLibrary.cpp   # 材质库实现 (~100行)
```

## 🔧 文件职责分工

### 1. MaterialProperty.cpp
**职责**: 材质属性的基础数据结构
**包含内容**:
- 所有 `MaterialProperty` 构造函数
- 复制构造函数和赋值运算符
- 所有 `GetValue<T>()` 模板特化实现
- 类型安全的数据访问

**关键特性**:
```cpp
// 支持的数据类型
float, glm::vec2, glm::vec3, glm::vec4
int, bool, glm::mat4, Ref<Texture2D>

// 类型安全访问
template<> float& MaterialProperty::GetValue<float>();
template<> const float& MaterialProperty::GetValue<float>() const;
```

### 2. Material.cpp  
**职责**: 材质的核心业务逻辑
**包含内容**:
- `Material` 类的所有公共方法实现
- 着色器反射数据同步
- 属性块管理 (`MaterialPropertyBlock`)
- 所有 `Set<T>()` 和 `Get<T>()` 模板特化
- 内存布局优化 (`SyncToRawData`)

**关键功能**:
```cpp
// 核心API
void Bind() const;                    // 绑定到着色器
Ref<Material> Clone() const;          // 克隆材质
void SyncToRawData();                 // 内存优化

// 私有方法
void SyncWithShaderReflection();     // 着色器反射同步
void CreatePropertyBlocks();         // 创建属性块
void MarkPropertyDirty();            // 脏标记管理
```

### 3. MaterialSerializer.cpp
**职责**: 材质的序列化和持久化
**包含内容**:
- JSON 序列化/反序列化
- 字符串格式转换
- 复杂的JSON解析逻辑
- 错误处理和验证

**API设计**:
```cpp
// 主要接口
static void SerializeToJSON(const Ref<Material>&, const std::string& filepath);
static Ref<Material> DeserializeFromJSON(const std::string& filepath);
static std::string SerializeToString(const Ref<Material>&);
static Ref<Material> DeserializeFromString(const std::string&);
```

### 4. MaterialLibrary.cpp
**职责**: 材质资源的统一管理
**包含内容**:
- 单例模式实现
- 材质注册/获取/卸载
- 批量操作支持
- 生命周期管理

**功能特性**:
```cpp
// 资源管理
void Register(const std::string& name, const Ref<Material>&);
Ref<Material> Get(const std::string& name);
void Unload(const std::string& name);

// 文件操作
Ref<Material> Load(const std::string& filepath);
bool Save(const std::string& name, const std::string& filepath);
Ref<Material> LoadAndRegister(const std::string& name, const std::string& filepath);
```

## 🎯 重构优势

### 1. **职责分离**
- 每个文件专注于单一职责
- 降低了代码复杂度
- 提高了可维护性

### 2. **编译优化**
- 减少了编译依赖
- 支持增量编译
- 更快的构建时间

### 3. **团队协作**
- 不同开发者可以并行开发不同组件
- 减少了代码冲突
- 便于代码审查

### 4. **测试友好**
- 每个组件可以独立测试
- 支持单元测试
- 更好的测试覆盖率

## 📋 模板特化分布

### MaterialProperty.cpp
```cpp
// GetValue 模板特化 (8对，16个函数)
template<> T& GetValue<T>();
template<> const T& GetValue<T>() const;
// 支持: float, vec2, vec3, vec4, int, bool, Texture2D, mat4
```

### Material.cpp
```cpp
// Set 模板特化 (8个函数)
template<> void Set<T>(const std::string&, const T&);

// Get 模板特化 (8个函数)  
template<> T Get<T>(const std::string&) const;
// 支持: float, vec2, vec3, vec4, int, bool, Texture2D, mat4
```

## 🔗 依赖关系

```
MaterialProperty.cpp  ←── (无依赖，基础类型)
        ↑
Material.cpp         ←── MaterialProperty, MaterialSerializer  
        ↑
MaterialSerializer.cpp ←── Material (友元访问)
        ↑  
MaterialLibrary.cpp  ←── Material, MaterialSerializer
```

## ⚡ 性能考量

### 1. **内存布局优化**
- `MaterialPropertyBlock` 提供GPU友好的内存布局
- `SyncToRawData()` 进行批量内存同步
- 减少GPU常量缓冲区更新次数

### 2. **脏标记系统**
- 只更新改变的属性块
- 避免不必要的GPU上传
- 提高渲染性能

### 3. **模板特化**
- 编译时类型检查
- 零开销抽象
- 最优化的代码生成

## 🚀 使用示例

```cpp
// 使用新的材质系统
#include "Hazel/Material/MaterialSystem.h"

// 创建材质
auto shader = Shader::Create("assets/shaders/basic.hlsl");
auto material = Material::Create(shader);

// 设置属性
material->Set("u_Color", glm::vec3(1.0f, 0.0f, 0.0f));
material->Set("u_Metallic", 0.8f);

// 序列化
MaterialSerializer::SerializeToJSON(material, "assets/materials/red_metal.mat");

// 使用材质库
MaterialLib::Get().Register("RedMetal", material);
auto loadedMaterial = MaterialLib::Get().Get("RedMetal");

// 渲染时使用
material->Bind();
// ... 渲染调用 ...
```

## 📝 注意事项

1. **友元访问**: `MaterialSerializer` 是 `Material` 的友元类，可以访问私有成员
2. **错误处理**: 所有文件操作都包含完善的错误处理和日志记录
3. **内存安全**: 使用智能指针管理资源生命周期
4. **线程安全**: MaterialLibrary 的单例实现是线程安全的

这样的重构使得材质系统更加模块化、可维护和可扩展！ 