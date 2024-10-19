#include "hzpch.h"
#include "GfxViewManager.h"

namespace Hazel 
{
    /// ��ʼ����̬��Ա
    std::shared_ptr<GfxViewManager> GfxViewManager::instance = nullptr;
    std::mutex GfxViewManager::mutex;

    void GfxViewManager::Init()
    {
        // ��Ե�һ��heap����ֻ��Ҫ����DESCRIPTOR_TYPE_CBV�� �����������ϻᴴ��D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
        // �������͵�desc���������������heap���档
        m_SrvHeap = GfxDescHeap::Create(DescriptorType::DESCRIPTOR_TYPE_CBV);
        m_RtvHeap = GfxDescHeap::Create(DescriptorType::DESCRIPTOR_TYPE_RTV);
        m_DsvHeap = GfxDescHeap::Create(DescriptorType::DESCRIPTOR_TYPE_DSV);
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

}
