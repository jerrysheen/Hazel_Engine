#pragma once

/// <summary>
/// ���������ȾAPI�ĳ����࣬��Opengl�У������ʱ�����ǲ�����Ҫ����
/// ��ȾAPI����������ִ�API����һ���Լ��ĳ�ʼ�����Լ����Ӿ�ϸ�Ĺ�������
/// ���Ǵ�������ʹ����Դ������Ҫ��API����н������ʴ���Ҫ���API��
/// ���磬��ʼ����ʱ��D3D12��Ҫ��ʼ��Device, CommandListAlloc, CommandList,
/// ����������Դʱ������Ҫͨ��Device�����д�����
/// </summary>
namespace Hazel {

	// ���api��ֻ���ڶ�Ӧ��renderapi�㱻���ã���������ط�����ʹ��variant�ķ�ʽȥȡֵ��
    // ֻ�������ܷ��ʹ��캯��
	class HAZEL_API RenderAPIManager
	{
    protected:
        static Ref<RenderAPIManager> instance;
        static std::mutex mutex;
        static std::function<Ref<RenderAPIManager>()> createFunction;

        // ˽�й��캯������������
        RenderAPIManager() = default;
        virtual ~RenderAPIManager() = default;

    public:
        // ɾ���������캯���͸�ֵ������
        RenderAPIManager(const RenderAPIManager&) = delete;
        RenderAPIManager& operator=(const RenderAPIManager&) = delete;

        // ע���������Ĵ�������
        template <typename T>
        static void Register() 
        {
            createFunction = [] { return CreateRef<T>(); };
        }

        // getInstance ����
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