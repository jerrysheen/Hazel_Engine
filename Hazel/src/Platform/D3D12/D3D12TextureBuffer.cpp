#include "hzpch.h"
#include "Platform/D3D12/D3D12TextureBuffer.h"
#include "Hazel/Core/Application.h"
#include "Hazel/Gfx/GfxViewManager.h"
#include "glm/gtc/type_ptr.hpp"

namespace Hazel 
{

	D3D12TextureBuffer::D3D12TextureBuffer(const TextureBufferSpecification& spec)
		: m_Spec(spec)
	{
        m_UUID = Unique::GetUUID();
        //temp code 
        switch (spec.format)
        {
            case TextureFormat::RGBA32:
                CreateRenderTargetBufferResource();
			    break;
            case TextureFormat::DEPTH24STENCIL8:
				CreateDepthStencilBufferResource();
            break;
            default:
				HZ_CORE_ASSERT(false, "Texture format not supported");
            break;
        }
	}

	void D3D12TextureBuffer::CreateRenderTargetBufferResource() 
	{
        D3D12_RESOURCE_DESC bufferDesc = {};
        bufferDesc.Dimension = GetResourceDimension();
        bufferDesc.Width = m_Spec.width;       // ������ɫ�������Ŀ��
        bufferDesc.Height = m_Spec.height;     // ������ɫ�������ĸ߶�
        bufferDesc.DepthOrArraySize = 1;
        bufferDesc.MipLevels = GetMipMapLevel();       // ����Ҫ�༶ϸ��
        bufferDesc.Format = GetTextureFormat(); // ��ɫ��ʽ
        bufferDesc.SampleDesc.Count = GetMSAASamplerCount();  
        bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        //bufferDesc.Flags = GetTextureFormat() == DXGI_FORMAT_R8G8B8A8_UNORM ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // ����  ����һ����ȾĿ��
        bufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET; // ����  ����һ����ȾĿ��

        // Ϊ��ɫ������ָ�����ֵ
        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = GetTextureFormat();;
        clearValue.Color[0] = 1.0f;
        clearValue.Color[1] = 1.0f;
        clearValue.Color[2] = 1.0f;
        clearValue.Color[3] = 1.0f;


        // ����ط��Ȳ����� device�������ˡ�����
        // ԭ��������ط��Ҳ�Ӧ����ô��devices...
        D3D12RenderAPIManager* renderAPIManager = dynamic_cast<D3D12RenderAPIManager*>(RenderAPIManager::getInstance()->GetManager().get());
        Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();
        device->CreateCommittedResource
        (
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_RENDER_TARGET, // ��ʼ״̬Ϊ��ȾĿ��
            &clearValue,
            IID_PPV_ARGS(&m_BufferResourceLocal)
        );
        m_BufferResource = m_BufferResourceLocal;

	}

    void D3D12TextureBuffer::CreateDepthStencilBufferResource()
    {
        D3D12_RESOURCE_DESC bufferDesc = {};
        bufferDesc.Dimension = GetResourceDimension();
        bufferDesc.Width = m_Spec.width;       // ������ɫ�������Ŀ��
        bufferDesc.Height = m_Spec.height;     // ������ɫ�������ĸ߶�
        bufferDesc.DepthOrArraySize = 1;
        bufferDesc.MipLevels = GetMipMapLevel();       // ����Ҫ�༶ϸ��
        bufferDesc.Format = GetTextureFormat(); // ��ɫ��ʽ
        bufferDesc.SampleDesc.Count = GetMSAASamplerCount();
        bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        //bufferDesc.Flags = GetTextureFormat() == DXGI_FORMAT_R8G8B8A8_UNORM ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // ����  ����һ����ȾĿ��
        bufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // ����  ����һ����ȾĿ��

        // Ϊ��ɫ������ָ�����ֵ
        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = GetTextureFormat();;
        clearValue.Color[0] = 1.0f;
        clearValue.Color[1] = 1.0f;
        clearValue.Color[2] = 1.0f;
        clearValue.Color[3] = 1.0f;


        // ����ط��Ȳ����� device�������ˡ�����
        // ԭ��������ط��Ҳ�Ӧ����ô��devices...
        D3D12RenderAPIManager* renderAPIManager = dynamic_cast<D3D12RenderAPIManager*>(RenderAPIManager::getInstance()->GetManager().get());
        Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();
        device->CreateCommittedResource
        (
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE, // ��ʼ״̬Ϊ��ȾĿ��
            &clearValue,
            IID_PPV_ARGS(&m_BufferResourceLocal)
        );
        m_BufferResource = m_BufferResourceLocal;

    }


    D3D12_RESOURCE_DIMENSION D3D12TextureBuffer::GetResourceDimension()
    {
        switch (m_Spec.textureType) 
        {
        case TextureType::TEXTURE2D:
			return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        case TextureType::TEXTURECUBE:
            return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
        case TextureType::TEXTURE2DARRAY:
            return D3D12_RESOURCE_DIMENSION_UNKNOWN;
        }
        return D3D12_RESOURCE_DIMENSION_UNKNOWN;
    }

    DXGI_FORMAT D3D12TextureBuffer::GetTextureFormat()
    {
        switch (m_Spec.format)
        {
        case TextureFormat::RGBA32:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case TextureFormat::DEPTH24STENCIL8:
            return DXGI_FORMAT_D24_UNORM_S8_UINT;
        }
        return DXGI_FORMAT_UNKNOWN;
    }

    int D3D12TextureBuffer::GetMSAASamplerCount()
    {
        switch (m_Spec.multiSample)
        {
            case MultiSample::MSAA16X:
				return 16;
            case MultiSample::MSAA8X:
                return 8;
            case MultiSample::MSAA4X:
                return 4;
            case MultiSample::MSAA2X:
                return 2;
        }
        return 1;
    }

    // ����ʵ�֡�
    int D3D12TextureBuffer::GetMipMapLevel()
    {
        return 1;
    }

	D3D12TextureBuffer::~D3D12TextureBuffer()
	{
	}
}