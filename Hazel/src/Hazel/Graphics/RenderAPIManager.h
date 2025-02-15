#pragma once

/// <summary>
/// 这个类是渲染API的抽象类，在Opengl中，大多数时候我们并不需要管理
/// 渲染API本身，但是现代API都有一套自己的初始化，以及更加精细的管理流程
/// 我们创建或者使用资源，都需要和API层进行交互，故此需要这个API。
/// 比如，初始化的时候D3D12需要初始化Device, CommandListAlloc, CommandList,
/// 后续创建资源时，又需要通过Device来进行创建。
/// </summary>
namespace Hazel {

	// 这个api，只会在对应的renderapi层被调用，所以这个地方依旧使用variant的方式去取值。
    // 只有子类能访问构造函数
	class HAZEL_API RenderAPIManager
	{
    protected:
        static Ref<RenderAPIManager> instance;
        static std::mutex mutex;
        static std::function<Ref<RenderAPIManager>()> createFunction;

        // 私有构造函数和析构函数
        RenderAPIManager() = default;
        virtual ~RenderAPIManager() = default;

    public:
        // 删除拷贝构造函数和赋值操作符
        RenderAPIManager(const RenderAPIManager&) = delete;
        RenderAPIManager& operator=(const RenderAPIManager&) = delete;

        // 注册具体子类的创建函数
        template <typename T>
        static void Register() 
        {
            createFunction = [] { return CreateRef<T>(); };
        }

        // getInstance 方法
        static Ref<RenderAPIManager> getInstance() {
            std::lock_guard<std::mutex> lock(mutex);
            if (!instance) {
                if (!createFunction) {
                    throw std::runtime_error("No render API manager registered.");
                }
                instance = createFunction();
            }
            return instance;
        }
        inline Ref<RenderAPIManager> GetManager() { return instance; };
        //void Init();
	};
}