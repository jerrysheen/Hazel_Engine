Singleton都重构成CRTP单例模式：
您问了一个很好的问题，关于模板类中静态成员的初始化。让我来详细解释：
模板类中的静态成员初始化
当您看到这段代码：
template<typename T>
T* Singleton<T>::s_Instance = nullptr;
这段代码并不是在初始化模板本身，而是为每一个模板特化分别初始化其静态成员。这是C++关于模板静态成员的特性：
模板的每个特化都有独立的静态成员：每个不同的T类型都会生成一个独立的Singleton<T>类，每个类都有自己的静态成员s_Instance
需要在全局范围内显式初始化：模板类的静态成员必须在全局范围内显式初始化，而不能只在类定义内初始化
对于您的例子
如果您有：
class RenderSystem : public Singleton<RenderSystem> {...};
class RenderAPIManager : public Singleton<RenderAPIManager> {...};
那么：
编译器会为Singleton<RenderSystem>生成一个类，这个类有一个静态成员变量s_Instance，类型为RenderSystem*
同样，会为Singleton<RenderAPIManager>生成另一个类，这个类有一个静态成员变量s_Instance，类型为RenderAPIManager*
那个初始化语句会为每一个特化的s_Instance分别初始化为nullptr
是如何工作的？
当程序开始执行时：
Singleton<RenderSystem>::s_Instance被初始化为nullptr
Singleton<RenderAPIManager>::s_Instance被初始化为nullptr
当首次调用RenderSystem::Get()时，它会使用Singleton<RenderSystem>::s_Instance
当首次调用RenderAPIManager::Get()时，它会使用Singleton<RenderAPIManager>::s_Instance
更具体的内存布局示意
内存中:
- Singleton<RenderSystem>::s_Instance      → nullptr (直到创建RenderSystem实例)
- Singleton<RenderAPIManager>::s_Instance  → nullptr (直到创建RenderAPIManager实例)
每个模板特化都有自己独立的静态变量存储位置，互不干扰。这正是我们想要的单例行为 - 每种类型有自己唯一的实例。
简单总结
是的，每个继承自Singleton<T>的类都会有自己的静态成员s_Instance被初始化为nullptr
这些初始化是相互独立的，一个类的实例不会影响另一个类
这种方式可以为每种类型提供类型安全的单例访问，且不需要在每个类中重复编写单例逻辑
这就是CRTP单例模式的强大之处 - 它可以将单例逻辑封装在基类模板中，而让每个子类自动获得类型安全的单例功能。