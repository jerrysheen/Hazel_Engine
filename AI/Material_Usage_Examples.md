# 材质系统使用示例 - 重构后版本

## 🚀 **新的材质管理架构**

### ✅ **清晰的职责分离**

```cpp
// Material类：专注核心功能
auto material = Material::Create(shader);  // 只负责材质核心逻辑
material->Set("u_Color", glm::vec3(1.0f, 0.0f, 0.0f));
material->Bind();

// MaterialLibrary：专门负责资源管理
auto& lib = MaterialLibrary::Get();
auto material = lib.LoadMaterial("assets/materials/pbr.mat");  // 统一加载入口
```

## 🎯 **智能缓存系统**

### **三级缓存架构**

```cpp
auto& lib = MaterialLibrary::Get();

// 第一次加载 - 从文件读取
auto material1 = lib.LoadMaterial("assets/materials/metal.mat");

// 第二次加载 - 弱指针缓存命中
auto material2 = lib.LoadMaterial("assets/materials/metal.mat"); 
// material1 和 material2 指向同一个实例！

// 即使对象被回收，智能缓存仍然有效
{
    auto tempMaterial = lib.LoadMaterial("assets/materials/wood.mat");
} // tempMaterial 超出作用域

// 再次加载 - 智能缓存命中（如果文件未修改）
auto woodMaterial = lib.LoadMaterial("assets/materials/wood.mat");
// 无需重新读取文件！
```

### **文件修改自动检测**

```cpp
// 加载材质并缓存
auto material = lib.LoadMaterial("test.mat");

// 在编辑器中修改了 test.mat 文件...

// 再次加载时会自动检测文件修改
auto updatedMaterial = lib.LoadMaterial("test.mat");
// 自动重新加载，获取最新内容！
```

## 🔧 **新的API用法**

### **基本材质加载**
```cpp
auto& lib = MaterialLibrary::Get();

// ✅ 主要入口：智能缓存加载
auto material = lib.LoadMaterial("assets/materials/pbr.mat");

// ✅ 需要独立实例时
auto uniqueMaterial = lib.CreateUniqueMaterial("assets/materials/pbr.mat");

// ✅ 显式命名管理
lib.Register("DefaultPBR", material);
auto namedMaterial = lib.Get("DefaultPBR");
```

### **缓存管理**
```cpp
auto& lib = MaterialLibrary::Get();

// 检查材质是否已缓存
if (lib.IsCached("assets/materials/stone.mat")) {
    HZ_CORE_INFO("Stone material is cached");
}

// 获取缓存统计信息
HZ_CORE_INFO("Total cache size: {}", lib.GetCacheSize());
HZ_CORE_INFO("Path cache size: {}", lib.GetPathCacheSize());

// 打印详细缓存信息
lib.PrintCacheInfo();

// 分层清理缓存
lib.ClearPathCache();        // 只清理路径缓存
lib.ClearNamedMaterials();   // 只清理命名材质
lib.ClearCache();            // 清理所有缓存
```

### **高级用法示例**
```cpp
// 批量预加载常用材质
std::vector<std::string> commonMaterials = {
    "assets/materials/default.mat",
    "assets/materials/pbr_basic.mat",
    "assets/materials/ui_standard.mat"
};

for (const auto& path : commonMaterials) {
    lib.LoadMaterial(path);  // 预加载到缓存
}

// 场景加载 - 享受缓存优势
void LoadScene() {
    // 所有材质都从缓存中快速获取
    auto woodMaterial = lib.LoadMaterial("assets/materials/wood.mat");
    auto metalMaterial = lib.LoadMaterial("assets/materials/metal.mat");
    auto glassMaterial = lib.LoadMaterial("assets/materials/glass.mat");
    
    // 应用到不同对象
    table->SetMaterial(woodMaterial);
    chair->SetMaterial(woodMaterial);      // 同一材质，共享实例
    door->SetMaterial(woodMaterial);
    
    lib.PrintCacheInfo();  // 监控缓存使用情况
}
```

## 📊 **性能优势对比**

### **旧架构 vs 新架构**

| 场景 | 旧Material静态缓存 | 新MaterialLibrary缓存 |
|------|-------------------|----------------------|
| 首次加载 | 慢（文件I/O） | 慢（文件I/O） |
| 重复加载（对象存在） | 快（弱指针） | 快（弱指针） |
| 重复加载（对象已回收） | 慢（重新加载） | 中等（智能缓存） |
| 文件修改检测 | 无 | 自动检测 |
| 缓存管理 | 分散难管理 | 集中统一管理 |
| 架构清晰度 | 职责混乱 | 清晰分离 |

### **内存使用优化**
```cpp
// 监控缓存大小
void MonitorCacheUsage() {
    auto& lib = MaterialLibrary::Get();
    
    // 定期清理过期条目
    if (lib.GetCacheSize() > 100) {
        lib.ClearPathCache();  // 清理弱指针缓存
        HZ_CORE_INFO("Cleaned up cache, new size: {}", lib.GetCacheSize());
    }
    
    // 在场景切换时清理
    if (sceneChanged) {
        lib.ClearCache();  // 清理所有缓存
    }
}
```

## 🛠️ **调试和监控**

### **缓存状态监控**
```cpp
auto& lib = MaterialLibrary::Get();

// 详细缓存报告
lib.PrintCacheInfo();
/*
输出示例：
MaterialLibrary Cache Info:
  - Path Cache: 15 entries (12 valid)
  - Smart Cache: 8 entries  
  - Named Materials: 5 entries
  - Total Cache Size: 28 entries
*/

// 检查特定材质状态
if (lib.IsCached("pbr.mat")) {
    HZ_CORE_INFO("PBR material is in cache");
}

// 获取所有命名材质
auto materialNames = lib.GetAllMaterialNames();
for (const auto& name : materialNames) {
    HZ_CORE_INFO("Named material: {}", name);
}
```

## ⚡ **最佳实践**

### 1. **使用LoadMaterial作为主要入口**
```cpp
// ✅ 推荐：享受智能缓存优势
auto material = MaterialLibrary::Get().LoadMaterial("assets/materials/default.mat");
```

### 2. **需要修改时使用Clone**
```cpp
// ✅ 推荐：克隆后修改，不影响缓存的原始材质
auto baseMaterial = lib.LoadMaterial("base.mat");
auto customMaterial = baseMaterial->Clone();
customMaterial->Set("u_CustomParam", 1.5f);
```

### 3. **特殊情况使用CreateUniqueMaterial**
```cpp
// ⚠️ 仅在确实需要完全独立实例时使用
auto uniqueMaterial = lib.CreateUniqueMaterial("special.mat");
```

### 4. **合理的缓存管理**
```cpp
// 在适当时机清理缓存
if (applicationMemoryPressure) {
    lib.ClearPathCache();  // 温和清理
}

if (sceneTransition) {
    lib.ClearCache();      // 彻底清理
}
```

## 🎯 **架构优势总结**

### **Material类（单一职责）**
- ✅ 专注材质核心逻辑
- ✅ 属性管理和渲染
- ✅ 着色器交互
- ✅ PropertyBlock管理

### **MaterialLibrary类（资源管理）**
- ✅ 统一的材质加载入口
- ✅ 智能多级缓存系统
- ✅ 文件修改自动检测
- ✅ 灵活的缓存管理策略
- ✅ 显式命名材质管理

这样的重构让每个类都有明确的职责，系统更易维护和扩展，同时提供了更强大的缓存管理能力！ 