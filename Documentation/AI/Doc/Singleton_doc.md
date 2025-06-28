# CRTP单例模式介绍与使用

## 什么是CRTP单例模式

CRTP（Curiously Recurring Template Pattern，奇异递归模板模式）单例模式是一种设计模式，它利用C++模板特性来实现类型安全的单例类。这种模式可以将单例逻辑封装在基类模板中，使得继承该模板的子类自动获得单例功能，避免了在每个需要单例的类中重复编写相同的单例逻辑。

## 改进的CRTP单例模式实现

下面是一个使用智能指针的CRTP单例模式改进实现，支持继承：

```cpp
#include <memory>

template<typename T>
class Singleton
{
protected:
    // 构造和析构函数设为保护的，防止外部直接创建实例
    Singleton() = default;
    virtual ~Singleton() = default;
    
    // 禁止拷贝和移动
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    
public:
    // 获取单例实例的接口
    static T& Get()
    {
        if (!s_Instance)
            s_Instance = CreateInstance();
        return *s_Instance;
    }
    
    // 获取原始指针（不转移所有权）
    static T* GetPtr()
    {
        return Get() ? s_Instance.get() : nullptr;
    }
    
    // 释放单例实例
    static void Release()
    {
        s_Instance.reset();
    }
    
protected:
    // 可以被子类重写的创建实例方法，允许继承层次中的自定义实例化
    static std::shared_ptr<T> CreateInstance()
    {
        return std::make_shared<T>();
    }
    
private:
    static std::shared_ptr<T> s_Instance;
};

// 在全局范围内初始化静态成员
template<typename T>
std::shared_ptr<T> Singleton<T>::s_Instance = nullptr;
```

## 模板类特化的机制

在CRTP单例模式中，模板特化机制是确保每个派生类拥有独立单例实例的核心。下面详细解释其工作原理：

### 1. 模板代码为何放在头文件中

C++中的模板与普通类有重要区别：
- **模板不是实际的代码**：模板只是生成代码的"蓝图"
- **延迟实例化**：直到模板被具体类型实例化时，编译器才会生成实际的代码
- **需要完整定义**：使用模板的翻译单元需要看到模板的完整定义

因此，模板代码通常需要放在头文件中，这样任何包含该头文件的翻译单元都能看到完整的模板定义并正确实例化。

### 2. 静态成员变量的特化

特别注意这行代码：
```cpp
template<typename T>
std::shared_ptr<T> Singleton<T>::s_Instance = nullptr;
```

当不同的类继承自`Singleton<T>`时：
- 每个不同的T类型都会触发模板的一次特化
- 每个特化都会有自己独立的静态成员`s_Instance`
- 这些静态成员不会相互干扰

### 3. 模板实例化过程示例

假设有以下两个类继承自Singleton模板：
```cpp
class AudioSystem : public Singleton<AudioSystem> { /* ... */ };
class RenderSystem : public Singleton<RenderSystem> { /* ... */ };
```

编译器会为这两个类生成以下特化：

**对于AudioSystem**:
```cpp
// 简化的示意代码，展示编译器实际生成的内容
class Singleton<AudioSystem> {
private:
    static std::shared_ptr<AudioSystem> s_Instance; // 独立的静态成员
public:
    static AudioSystem& Get() {
        if (!s_Instance)
            s_Instance = CreateInstance();
        return *s_Instance;
    }
    // ... 其他成员
};
std::shared_ptr<AudioSystem> Singleton<AudioSystem>::s_Instance = nullptr;
```

**对于RenderSystem**:
```cpp
class Singleton<RenderSystem> {
private:
    static std::shared_ptr<RenderSystem> s_Instance; // 另一个独立的静态成员
public:
    static RenderSystem& Get() {
        if (!s_Instance)
            s_Instance = CreateInstance();
        return *s_Instance;
    }
    // ... 其他成员
};
std::shared_ptr<RenderSystem> Singleton<RenderSystem>::s_Instance = nullptr;
```

### 4. 内存布局

在运行时，每个特化的静态成员在内存中占据不同的位置：

```
内存布局:
+-----------------------------------------------+
| Singleton<AudioSystem>::s_Instance | nullptr  | ← 初始状态
+-----------------------------------------------+
| Singleton<RenderSystem>::s_Instance | nullptr | ← 初始状态
+-----------------------------------------------+
```

首次调用后：
```
+--------------------------------------------------------+
| Singleton<AudioSystem>::s_Instance | AudioSystem实例指针 | ← 首次调用AudioSystem::Get()
+--------------------------------------------------------+
| Singleton<RenderSystem>::s_Instance | nullptr          | ← 尚未调用
+--------------------------------------------------------+
```

两者都调用后：
```
+--------------------------------------------------------+
| Singleton<AudioSystem>::s_Instance | AudioSystem实例指针 |
+--------------------------------------------------------+
| Singleton<RenderSystem>::s_Instance | RenderSystem实例指针 |
+--------------------------------------------------------+
```

### 5. 链接时的处理

在链接阶段：
- 每个翻译单元可能独立实例化模板
- 链接器会识别并合并相同特化的多个定义
- 每种类型的静态成员只会保留一份

## 基本使用CRTP单例模式

要使用这个单例模式，只需让您的类继承自`Singleton<YourClass>`：

```cpp
class RenderSystem : public Singleton<RenderSystem>
{
    // 通过友元声明，允许基类访问构造函数
    friend class Singleton<RenderSystem>;
    // 如果需要自定义CreateInstance，还需要声明这个友元
    friend std::shared_ptr<RenderSystem> Singleton<RenderSystem>::CreateInstance();
    
private:
    // 私有构造函数，防止外部直接创建实例
    RenderSystem() { /* 初始化代码 */ }
    
public:
    void Initialize() { /* ... */ }
    void Shutdown() { /* ... */ }
    
    // 渲染系统特有的方法
    void Render() { /* ... */ }
};
```

## 支持继承的使用

CRTP单例模式可以通过重写CreateInstance方法支持继承和工厂模式：

```cpp
// 抽象渲染系统基类
class AbstractRenderSystem
{
public:
    virtual ~AbstractRenderSystem() = default;
    virtual void Initialize() = 0;
    virtual void Render() = 0;
    virtual void Shutdown() = 0;
};

// 具体渲染系统实现
class OpenGLRenderSystem : public AbstractRenderSystem
{
public:
    OpenGLRenderSystem() { /* 构造函数 */ }
    
    void Initialize() override { /* OpenGL初始化 */ }
    void Render() override { /* OpenGL渲染 */ }
    void Shutdown() override { /* OpenGL关闭 */ }
};

class DirectXRenderSystem : public AbstractRenderSystem
{
public:
    DirectXRenderSystem() { /* 构造函数 */ }
    
    void Initialize() override { /* DirectX初始化 */ }
    void Render() override { /* DirectX渲染 */ }
    void Shutdown() override { /* DirectX关闭 */ }
};

// 渲染系统管理器作为单例
class RenderSystemManager : public Singleton<RenderSystemManager>
{
    friend class Singleton<RenderSystemManager>;
    
private:
    enum class API { OpenGL, DirectX } m_CurrentAPI;
    std::shared_ptr<AbstractRenderSystem> m_RenderSystem;
    
    RenderSystemManager() : m_CurrentAPI(API::OpenGL) { }
    
protected:
    // 重写CreateInstance方法
    static std::shared_ptr<RenderSystemManager> CreateInstance()
    {
        return std::make_shared<RenderSystemManager>();
    }
    
public:
    void SetAPI(API api)
    {
        m_CurrentAPI = api;
        if (m_RenderSystem)
            m_RenderSystem->Shutdown();
            
        if (api == API::OpenGL)
            m_RenderSystem = std::make_shared<OpenGLRenderSystem>();
        else
            m_RenderSystem = std::make_shared<DirectXRenderSystem>();
    }
    
    void Initialize()
    {
        if (!m_RenderSystem)
            SetAPI(m_CurrentAPI);
        m_RenderSystem->Initialize();
    }
    
    void Render()
    {
        if (m_RenderSystem)
            m_RenderSystem->Render();
    }
    
    void Shutdown()
    {
        if (m_RenderSystem)
        {
            m_RenderSystem->Shutdown();
            m_RenderSystem.reset();
        }
    }
    
    AbstractRenderSystem* GetCurrentRenderSystem()
    {
        return m_RenderSystem.get();
    }
};
```

## 如何使用这些单例类

```cpp
void Application::Initialize()
{
    // 使用管理器类
    auto& renderManager = RenderSystemManager::Get();
    renderManager.SetAPI(RenderSystemManager::API::DirectX);
    renderManager.Initialize();
}

void Application::Run()
{
    // 使用管理器类渲染
    RenderSystemManager::Get().Render();
}

void Application::Shutdown()
{
    // 关闭和释放资源
    RenderSystemManager::Get().Shutdown();
    RenderSystemManager::Release();
}
```

## 工作原理

改进的CRTP单例模式有以下关键点：

1. **智能指针管理**：使用`std::shared_ptr`代替原始指针，自动管理内存
2. **可重写的CreateInstance方法**：允许子类自定义实例创建过程
3. **友元声明**：确保单例基类可以访问派生类的私有构造函数

每个不同类型T的特化都有自己的静态成员`s_Instance`：

- 当程序开始执行时，所有单例实例均为`nullptr`
- 首次调用`Get()`时，会通过`CreateInstance()`创建实例并存储在`s_Instance`中
- 通过重写`CreateInstance()`方法，可以实现工厂模式或其他复杂的实例化逻辑

## 优势

1. **类型安全**：获取的单例实例类型是确定的，不需要类型转换
2. **内存安全**：使用智能指针自动管理内存，避免内存泄漏
3. **灵活继承**：支持继承层次结构，可以与多态结合使用
4. **可扩展性**：通过重写`CreateInstance()`可以实现自定义实例化逻辑
5. **代码复用**：单例逻辑只需在基类模板中实现一次
6. **统一接口**：为所有单例类提供一致的接口
7. **零负担抽象**：CRTP的模板实例化在编译期完成，几乎没有运行时开销