#pragma once
#include "Hazel/Core/Core.h"
#include "Hazel/Gfx/GfxDesc.h"
#include "Hazel/Renderer/TextureBuffer.h"
#include "Hazel/Renderer/Buffer.h"

namespace Hazel {
    //// viewManager�����������descriptor�Լ�descriptorHeap��
    //// ����һ�㣬������uuid��Ѱ�ҵ�ĳһ����Դ��Ӧ��ĳ��������ʽ��
    //// �����������һ�����࣬�ҵ�RHI�㲻��Ҫ����heap��ԭ�������е�heap
    //// ���ʶ�����RrenderAPI�㣬ͨ����Դ��UUID���в��Һͷ��ʵġ�
    //// �ð���ôдҲ���ԣ� �ҿ�����ط�������һ�����൥�����о����heap�Լ�desc.
    //// ��ô���ϲ�������ù���Ϳ����ˣ��������������heap��Ȼ��ÿ��heap�ṩ���ҷ�ʽ�����ҷ�ʽӦ�þ���map<uuid, offset> �õ�һ������ĵ�ַ��
    //class GfxViewManager {
    //private:
    //    static Ref<GfxViewManager> instance;
    //    static std::mutex mutex;

    //    // ˽�й��캯������������
    //    GfxViewManager() {}
    //    ~GfxViewManager() { std::cout << "GfxDescManager Destroyed\n"; }

    //public:
    //    // ɾ���������캯���͸�ֵ������
    //    GfxViewManager(const GfxViewManager&) = delete;
    //    GfxViewManager& operator=(const GfxViewManager&) = delete;

    //    // getInstance ����
    //    static Ref<GfxViewManager> getInstance() {
    //        std::lock_guard<std::mutex> lock(mutex);
    //        if (!instance) {
    //            instance = Ref<GfxViewManager>(new GfxViewManager(), [](GfxViewManager* p) {
    //                delete p;
    //                });
    //        }
    //        return instance;
    //    }

    //    void Init();
    //    
    //    // �������߻�ȡTextureBuffer����������
    //    Ref<GfxDesc> GetRtvHandle(const Ref<TextureBuffer> textureBuffer);
    //    Ref<GfxDesc> GetSrvHandle(const Ref<TextureBuffer> textureBuffer);
    //    Ref<GfxDesc> GetDsvHandle(const Ref<TextureBuffer> textureBuffer);
    //    Ref<GfxDesc> GetCbvHandle(const Ref<ConstantBuffer> constantBuffer);

    //    inline Ref<GfxDescHeap> GetSrvHeap() { return m_SrvHeap; };

    //    // ���������ѭ���������Դ����imgui���뿪��
    //    inline Ref<GfxDescHeap> GetCbvHeap() { return m_CbvHeap; };
    //private:
    //    Ref<GfxDescHeap> m_RtvHeap;
    //    Ref<GfxDescHeap> m_SrvHeap;
    //    Ref<GfxDescHeap> m_DsvHeap;
    //    Ref<GfxDescHeap> m_CbvHeap;

    //};

}
