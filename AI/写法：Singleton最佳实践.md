# C++单例模式最佳实践

单例模式是最常用的设计模式之一，尤其在游戏引擎和渲染系统中，常用于管理全局资源或提供统一访问点。本文档总结几种常见的单例实现方式，并探讨它们在渲染引擎中的应用，特别是当需要与继承结合时的解决方案。

## 一、简单单例模式

### 1.1 基本实现（无继承支持）

最简单的单例实现，适用于不需要继承的场景：

```cpp
class SimpleSingleton {
private:
    static SimpleSingleton* s_Instance;
    
    // 私有构造函数防止外部创建实例
    SimpleSingleton() {}
    ~SimpleSingleton() {}

public:
    // 删除拷贝构造函数和赋值运算符
    SimpleSingleton(const SimpleSingleton&) = delete;
    SimpleSingleton& operator=(const SimpleSingleton&) = delete;
    
    // 获取实例的静态方法
    static SimpleSingleton& Get() {
        if (!s_Instance)
            s_Instance = new SimpleSingleton();
        return *s_Instance;
    }
    
    // 清理方法（在应用程序结束时调用）
    static void Destroy() {
        delete s_Instance;
        s_Instance = nullptr;
    }
};

// 静态成员初始化
SimpleSingleton* SimpleSingleton::s_Instance = nullptr;
```

### 1.2 Meyer's Singleton (局部静态变量)

这是C++11后推荐的线程安全单例实现：

```cpp
class MeyersSingleton {
private:
    MeyersSingleton() {}
    ~MeyersSingleton() {}

public:
    MeyersSingleton(const MeyersSingleton&) = delete;
    MeyersSingleton& operator=(const MeyersSingleton&) = delete;
    
    static MeyersSingleton& Get() {
        // C++11保证这是线程安全的
        static MeyersSingleton instance;
        return instance;
    }
};
```

这种方法简洁且线程安全，但不支持继承和多态。

## 二、支持继承的单例模式

在渲染引擎中，经常需要基于平台或API实现不同的子类单例。以下是几种支持继承的单例实现：

### 2.1 工厂函数注册方式（如RenderAPIManager）

这种方式使用工厂函数注册具体实现类：

```cpp
class BaseSingleton {
protected:
    static Ref<BaseSingleton> s_Instance;
    static std::mutex s_Mutex;
    static std::function<Ref<BaseSingleton>()> s_CreateFunction;

    // 保护构造函数，允许子类调用
    BaseSingleton() = default;
    virtual ~BaseSingleton() = default;

public:
    // 删除拷贝构造函数和赋值运算符
    BaseSingleton(const BaseSingleton&) = delete;
    BaseSingleton& operator=(const BaseSingleton&) = delete;

    // 注册创建函数
    template <typename T>
    static void Register() {
        s_CreateFunction = [] { return CreateRef<T>(); };
    }

    // 获取实例
    static Ref<BaseSingleton> GetInstance() {
        std::lock_guard<std::mutex> lock(s_Mutex);
        if (!s_Instance) {
            if (!s_CreateFunction) {
                throw std::runtime_error("No singleton implementation registered.");
            }
            s_Instance = s_CreateFunction();
        }
        return s_Instance;
    }
};

// 静态成员初始化
Ref<BaseSingleton> BaseSingleton::s_Instance = nullptr;
std::mutex BaseSingleton::s_Mutex;
std::function<Ref<BaseSingleton>()> BaseSingleton::s_CreateFunction = nullptr;
```

具体子类实现：

```cpp
class ConcreteImplementation : public BaseSingleton {
public:
    // 可以访问构造函数，但仍由Register方法控制创建
    ConcreteImplementation() {}
    
    // 子类特有方法
    void SpecificMethod() { /* ... */ }
};

// 在应用程序初始化时注册具体实现
void Initialize() {
    BaseSingleton::Register<ConcreteImplementation>();
}
```

### 2.2 CRTP (Curiously Recurring Template Pattern)

使用模板实现静态多态的单例基类：

```cpp
template<typename T>
class Singleton {
protected:
    static T* s_Instance;
    
    // 保护构造函数
    Singleton() {
        assert(!s_Instance);
        s_Instance = static_cast<T*>(this);
    }
    
    ~Singleton() {
        assert(s_Instance);
        s_Instance = nullptr;
    }

public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    
    static T& Get() {
        assert(s_Instance);
        return *s_Instance;
    }
};

// 静态成员初始化
template<typename T>
T* Singleton<T>::s_Instance = nullptr;
```

子类实现：

```cpp
class RenderSystem : public Singleton<RenderSystem> {
    // 允许基类访问私有构造函数
    friend class Singleton<RenderSystem>;
private:
    RenderSystem() {}

public:
    void Initialize() { /* ... */ }
};
```

### 2.3 动态多态与虚拟基类

对于需要运行时决定具体实现的场景：

```cpp
class IRenderAPI {
public:
    virtual ~IRenderAPI() = default;
    virtual void Initialize() = 0;
    
    // 获取实例接口
    static IRenderAPI* Get();
    
    // 设置实例（由派生类调用）
    static void SetInstance(IRenderAPI* instance);

private:
    static IRenderAPI* s_Instance;
};

// 具体实现类
class OpenGLRenderAPI : public IRenderAPI {
public:
    virtual void Initialize() override { /* OpenGL实现 */ }
    
    // 创建并设置为当前实例
    static void Create() {
        OpenGLRenderAPI* api = new OpenGLRenderAPI();
        IRenderAPI::SetInstance(api);
    }
};

class D3D12RenderAPI : public IRenderAPI {
public:
    virtual void Initialize() override { /* D3D12实现 */ }
    
    static void Create() {
        D3D12RenderAPI* api = new D3D12RenderAPI();
        IRenderAPI::SetInstance(api);
    }
};
```

## 三、渲染引擎中的实际应用

### 3.1 GfxViewManager示例（无继承单例）

```cpp
class GfxViewManager {
private:
    static Ref<GfxViewManager> s_Instance;
    static std::mutex s_Mutex;

    // 私有构造函数
    GfxViewManager() {}
    ~GfxViewManager() {}

public:
    // 删除拷贝构造函数和赋值运算符
    GfxViewManager(const GfxViewManager&) = delete;
    GfxViewManager& operator=(const GfxViewManager&) = delete;

    // 获取实例
    static Ref<GfxViewManager> GetInstance() {
        std::lock_guard<std::mutex> lock(s_Mutex);
        if (!s_Instance) {
            s_Instance = Ref<GfxViewManager>(new GfxViewManager(), [](GfxViewManager* p) {
                delete p;
            });
        }
        return s_Instance;
    }

    // 业务方法
    void Initialize() { /* ... */ }
    Ref<GfxDesc> GetRtvHandle(const Ref<TextureBuffer>& texture) { /* ... */ }
    // ...
};
```

### 3.2 RenderAPIManager示例（支持继承的单例）

```cpp
class RenderAPIManager {
protected:
    static Ref<RenderAPIManager> s_Instance;
    static std::mutex s_Mutex;
    static std::function<Ref<RenderAPIManager>()> s_CreateFunction;

    // 保护构造函数
    RenderAPIManager() = default;
    virtual ~RenderAPIManager() = default;

public:
    // 删除拷贝构造函数和赋值运算符
    RenderAPIManager(const RenderAPIManager&) = delete;
    RenderAPIManager& operator=(const RenderAPIManager&) = delete;

    // 注册具体API实现
    template <typename T>
    static void Register() {
        s_CreateFunction = [] { return CreateRef<T>(); };
    }

    // 获取实例
    static Ref<RenderAPIManager> GetInstance() {
        std::lock_guard<std::mutex> lock(s_Mutex);
        if (!s_Instance) {
            if (!s_CreateFunction) {
                throw std::runtime_error("No render API manager registered.");
            }
            s_Instance = s_CreateFunction();
        }
        return s_Instance;
    }
    
    // 获取管理器实例
    Ref<RenderAPIManager> GetManager() { return s_Instance; }
    
    // 虚拟方法，允许子类特化
    virtual void Initialize() = 0;
};

// 具体D3D12实现
class D3D12RenderAPIManager : public RenderAPIManager {
public:
    virtual void Initialize() override {
        // 初始化D3D12设备和资源
    }
};
```

## 四、最佳实践建议

1. **单一职责原则**：单例应只负责一个特定功能，避免"上帝类"

2. **线程安全**：确保单例初始化和访问是线程安全的，使用互斥锁或C++11局部静态变量

3. **内存管理**：使用智能指针(Ref/Scope)管理单例生命周期，避免内存泄漏

4. **继承支持选择**：
   - 不需要继承时，使用Meyer's单例或简单单例
   - 需要继承和多态时，使用工厂注册模式或CRTP

5. **延迟初始化**：单例应该在第一次使用时才被创建(懒加载)

6. **显式销毁**：提供显式的销毁方法，避免程序结束时的未定义行为

7. **命名统一**：
   - 使用`GetInstance()`或`Get()`作为获取实例的方法名
   - 使用`s_`前缀表示静态成员

8. **初始化顺序**：注意静态初始化顺序问题，特别是单例间相互依赖的情况

## 五、不同应用场景的选择

| 场景 | 推荐实现 | 优点 |
|------|---------|------|
| 简单全局管理器 | Meyer's Singleton | 简洁、线程安全、无需显式销毁 |
| 平台特定API抽象 | 工厂注册模式 | 支持运行时选择不同实现 |
| 编译时确定的多态 | CRTP模式 | 编译时类型安全，零运行时开销 |
| 需要测试替换的组件 | 依赖注入+可选单例 | 更好的可测试性 |

## 六、警告与陷阱

1. **全局状态过多**：单例过多会导致隐式依赖，难以测试和维护
2. **初始化顺序**：不同编译单元中单例的初始化顺序是不确定的
3. **析构顺序**：程序结束时单例析构顺序也不确定，可能导致问题
4. **线程安全**：需要考虑多线程环境下的初始化和访问安全性
5. **测试难度**：单例很难在测试中替换或模拟
