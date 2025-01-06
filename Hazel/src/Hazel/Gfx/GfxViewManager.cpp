#include "hzpch.h"
#include "GfxViewManager.h"

namespace Hazel 
{
    /// 初始化静态成员
    std::shared_ptr<GfxViewManager> GfxViewManager::instance = nullptr;
    std::mutex GfxViewManager::mutex;

    void GfxViewManager::Init()
    {
        // 针对第一种heap，我只需要创建DESCRIPTOR_TYPE_CBV， 但是它本质上会创建D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
        // 三种类型的desc都可以容纳在这个heap里面。
        m_SrvHeap = GfxDescHeap::Create(DescriptorType::DESCRIPTOR_TYPE_SRV);
        m_RtvHeap = GfxDescHeap::Create(DescriptorType::DESCRIPTOR_TYPE_RTV);
        m_DsvHeap = GfxDescHeap::Create(DescriptorType::DESCRIPTOR_TYPE_DSV);
        m_CbvHeap = GfxDescHeap::Create(DescriptorType::DESCRIPTOR_TYPE_CBV);
    }

    Ref<GfxDesc> GfxViewManager::GetRtvHandle(const Ref<TextureBuffer> textureBuffer)
    {
        auto Desc = m_RtvHeap->GetOrCreateDesc(textureBuffer, DescriptorType::DESCRIPTOR_TYPE_RTV);
        return Desc;
    }

    Ref<GfxDesc> GfxViewManager::GetSrvHandle(const Ref<TextureBuffer> textureBuffer)
    {
        auto Desc = m_SrvHeap->GetOrCreateDesc(textureBuffer, DescriptorType::DESCRIPTOR_TYPE_SRV);
        return Desc;
    }

    Ref<GfxDesc> GfxViewManager::GetDsvHandle(const Ref<TextureBuffer> textureBuffer)
    {
        auto Desc = m_DsvHeap->GetOrCreateDesc(textureBuffer, DescriptorType::DESCRIPTOR_TYPE_DSV);
        return Desc;
    }

    Ref<GfxDesc> GfxViewManager::GetCbvHandle(const Ref<ConstantBuffer> constantBuffer)
    {
        auto Desc = m_CbvHeap->GetOrCreateDesc(constantBuffer);
        return Desc;

    }

}
