#include "hzpch.h"
#include "D3D12Buffer.h"
#include "Platform/D3D12/D3D12RenderAPIManager.h"
#include "Hazel/Core/Application.h"
#include "Hazel/RHI/Interface/IGfxViewManager.h"

#include "Hazel/RHI/Core/ScopedCommandList.h"
#include "Hazel/RHI/Core/CommandList.h"
namespace Hazel
{
	D3D12Buffer::D3D12Buffer(uint32_t elementSize)
		: mUploadBuffer(std::get<Microsoft::WRL::ComPtr<ID3D12Resource>>(m_BufferResource)), mMappedData(nullptr)
	{
        m_UUID = Unique::GetUUID();
		//���������buffer��ʼ���� Ȼ�����ݻ���Sharedpointer��
		elementSize = d3dUtil::CalcConstantBufferByteSize(elementSize);
        m_BufferSize = elementSize;
        // todo:: ����ط��϶���Ҫ�޸ģ� ������application������
        D3D12RenderAPIManager* renderAPIManager = dynamic_cast<D3D12RenderAPIManager*>(RenderAPIManager::getInstance()->GetManager().get());
        Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();
        
        
        ThrowIfFailed(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(elementSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&mUploadBuffer)));

        ThrowIfFailed(mUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData)));
	}



    D3D12Buffer::~D3D12Buffer()
    {
        // 通知ViewManager该资源即将销毁
        auto& viewManager = IGfxViewManager::Get();
        viewManager.OnResourceDestroyed(m_UUID);
        
        if (mUploadBuffer != nullptr)
            mUploadBuffer->Unmap(0, nullptr);

        mMappedData = nullptr;
    }

    void D3D12Buffer::SetData(void* srcData, int length)
    {
        // ȷ��Դ����ָ���Ŀ�껺��������ȷӳ��
        if (srcData && mMappedData) {
            memcpy(mMappedData, srcData, length);
        }
        else {
            // �������������������Ч��ָ��
            std::cerr << "Invalid source or destination pointer." << std::endl;
        }
    }

    
    D3D12VertexBuffer::D3D12VertexBuffer(float* vertices, uint32_t size, uint32_t stride)
    {
        m_BufferSize = size;
		m_BufferStride = stride;
        D3D12RenderAPIManager* renderAPIManager = dynamic_cast<D3D12RenderAPIManager*>(RenderAPIManager::getInstance()->GetManager().get());
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = renderAPIManager->GetCommandQueue();
        Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();
        
        
        ScopedCommandList cmd(CommandListType::Graphics);
        cmd->Reset();
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList =
            static_cast<ID3D12GraphicsCommandList*>(cmd.GetNativeCommandList());
        const UINT vbByteSize = (UINT)size;
        VertexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(),
            m_CommandList.Get(), vertices, vbByteSize, VertexBufferUploader);
        // ��ʱ����Ϊ��

        cmd->Close();
        ID3D12CommandList* rawCommandList = m_CommandList.Get();
        commandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&rawCommandList);
		renderAPIManager->FlushCommandQueue();

    }

    D3D12VertexBuffer::~D3D12VertexBuffer()
    {
    }

    // bind unbind ����Ҫ��һ����ô��
    void D3D12VertexBuffer::Bind() const
    {
    }

    void D3D12VertexBuffer::Unbind() const
    {
    }

    // todo: ��������
    //typedef struct D3D12_INPUT_ELEMENT_DESC {
    //    LPCSTR                     SemanticName;          // ��������, һ��POSITION���ǹ涨д���ġ�
    //    UINT                       SemanticIndex;         // ��������, Texcoord0, Texcoord1,�������ƾ���0��1
    //    DXGI_FORMAT                Format;                // ���ݸ�ʽ, float3/ float4
    //    UINT                       InputSlot;             // ����������� �൱�ڿ���ʵ�ֲ��position��������normal tangent�Ĺ��ܣ�֮�����ʵ��һ�£�������д��һ��
    //    UINT                       AlignedByteOffset;     // �����ֽ�ƫ����
    //    D3D12_INPUT_CLASSIFICATION InputSlotClass;        // ����۷���
    //    UINT                       InstanceDataStepRate;  // ʵ�����ݲ�����
    //} D3D12_INPUT_ELEMENT_DESC;
    //void D3D12VertexBuffer::SetLayout(const BufferLayout& layout)
    //{
    //    //m_Layout = layout;
    //    //VertexByteStride = layout.GetStride();
    //    //// Data about the buffers.
    //    //for (int i = 0; i < m_Layout.GetCount(); i++)
    //    //{
    //    //    auto& element = m_Layout.GetElements()[i];
    //    //    m_D3DInputLayout.push_back(D3D12_INPUT_ELEMENT_DESC{ element.Name.c_str(), 0, GetLayOutFormat(element.Type), 0, element.Offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA ,0 });
    //    //}
    //}


    D3D12IndexBuffer::D3D12IndexBuffer(uint16_t* indices, uint32_t size)
    {
        D3D12RenderAPIManager* renderAPIManager = dynamic_cast<D3D12RenderAPIManager*>(RenderAPIManager::getInstance()->GetManager().get());
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = renderAPIManager->GetCommandQueue();
        Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();

        ScopedCommandList cmd(CommandListType::Graphics);
        cmd->Reset();
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList =
            static_cast<ID3D12GraphicsCommandList*>(cmd.GetNativeCommandList());


        const UINT indexSize = (UINT)size;
        IndexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(),
            m_CommandList.Get(), indices, size, IndexBufferUploader);
        // ��ʱ����Ϊ��
        //VertexByteStride = m_Layout.GetStride();
        IndexBufferByteSize = size;

		cmd->Close();
        ID3D12CommandList* rawCommandList = m_CommandList.Get();
        commandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&rawCommandList);
        renderAPIManager->FlushCommandQueue();
    }

    D3D12IndexBuffer::~D3D12IndexBuffer()
    {
    }
    void D3D12IndexBuffer::Bind() const
    {
    }
    void D3D12IndexBuffer::Unbind() const
    {
    }
}
