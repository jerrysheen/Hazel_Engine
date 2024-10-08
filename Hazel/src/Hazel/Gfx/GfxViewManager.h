#pragma once
#include "Hazel/Core/Core.h"
#include "Hazel/Gfx/GfxDesc.h"
#include "Hazel/Renderer/TextureBuffer.h"

namespace Hazel {
    // viewManager用来管理各种descriptor以及descriptorHeap。
    // 在这一层，可以用uuid来寻找到某一个资源对应的某种描述方式。
    // 这个不用做成一个子类，我的RHI层不需要访问heap，原则上所有的heap
    // 访问都是在RrenderAPI层，通过资源的UUID进行查找和访问的。
    // 好吧这么写也可以， 我看这个地方依旧是一个父类单例持有具体的heap以及desc.
    // 那么我上层就是做好管理就可以了，创建几个具体的heap，然后每个heap提供查找方式，查找方式应该就是map<uuid, offset> 得到一个具体的地址。
    class GfxViewManager {
    private:
        static Ref<GfxViewManager> instance;
        static std::mutex mutex;

        // 私有构造函数和析构函数
        GfxViewManager() {}
        ~GfxViewManager() { std::cout << "GfxDescManager Destroyed\n"; }

    public:
        // 删除拷贝构造函数和赋值操作符
        GfxViewManager(const GfxViewManager&) = delete;
        GfxViewManager& operator=(const GfxViewManager&) = delete;

        // getInstance 方法
        static Ref<GfxViewManager> getInstance() {
            std::lock_guard<std::mutex> lock(mutex);
            if (!instance) {
                instance = Ref<GfxViewManager>(new GfxViewManager(), [](GfxViewManager* p) {
                    delete p;
                    });
            }
            return instance;
        }

        void Init();
        
        // 创建或者获取TextureBuffer的描述符。
        Ref<GfxDesc> GetRtvHandle(const TextureBuffer& textureBuffer);
        Ref<GfxDesc> GetSrvHandle(const TextureBuffer& textureBuffer);
        Ref<GfxDesc> GetDsvHandle(const TextureBuffer& textureBuffer);

    private:
        Ref<GfxDescHeap> m_RtvHeap;
        Ref<GfxDescHeap> m_SrvHeap;
        Ref<GfxDescHeap> m_DsvHeap;

    };

}
