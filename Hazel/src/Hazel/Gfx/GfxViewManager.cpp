#include "hzpch.h"
#include "GfxViewManager.h"

namespace Hazel 
{
    /// 初始化静态成员
    std::shared_ptr<GfxViewManager> GfxViewManager::instance = nullptr;
    std::mutex GfxViewManager::mutex;

    void GfxViewManager::Init()
    {
        m_SrvHeap = GfxDescHeap::Create(DescriptorType::DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        m_RtvHeap = GfxDescHeap::Create(DescriptorType::DESCRIPTOR_TYPE_RTV);
        m_DsvHeap = GfxDescHeap::Create(DescriptorType::DESCRIPTOR_TYPE_DSV);
    }

    Ref<GfxDesc> GfxViewManager::GetRtvHandle(const TextureBuffer& textureBuffer)
    {
        auto Desc = m_RtvHeap->GetOrCreateRtvDesc(const TextureBuffer & textureBuffer);
        return Ref<GfxDesc>();
    }

    Ref<GfxDesc> GfxViewManager::GetSrvHandle(const TextureBuffer& textureBuffer)
    {
        return Ref<GfxDesc>();
    }

    Ref<GfxDesc> GfxViewManager::GetDsvHandle(const TextureBuffer& textureBuffer)
    {
        return Ref<GfxDesc>();
    }

}
