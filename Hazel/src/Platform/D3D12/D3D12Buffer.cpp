#include "hzpch.h"
#include "D3D12Buffer.h"
#include "Platform/D3D12/D3D12RenderAPIManager.h"
#include "Hazel/Core/Application.h"
#include "Hazel/Gfx/CommandPool.h"

namespace Hazel
{
	D3D12Buffer::D3D12Buffer(uint32_t elementSize)
		: mUploadBuffer(std::get<Microsoft::WRL::ComPtr<ID3D12Resource>>(m_BufferResource)), mMappedData(nullptr)
	{
        m_UUID = Unique::GetUUID();
		//在这里进行buffer初始化， 然后将内容换成Sharedpointer吧
		elementSize = d3dUtil::CalcConstantBufferByteSize(elementSize);
        m_BufferSize = elementSize;
        // todo:: 这个地方肯定需要修改， 不依赖application。。。
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
        if (mUploadBuffer != nullptr)
            mUploadBuffer->Unmap(0, nullptr);

        mMappedData = nullptr;
    }

    void D3D12Buffer::SetData(void* srcData, int length)
    {
        // 确保源数据指针和目标缓存区已正确映射
        if (srcData && mMappedData) {
            memcpy(mMappedData, srcData, length);
        }
        else {
            // 处理错误情况，例如无效的指针
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
        Ref<CommandList> cmdList = CommandPool::getInstance()->GetCommand();

        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList = cmdList->getCommandList<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>>();
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator = cmdList->getCommandAllocator<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>();

        ThrowIfFailed(m_CommandAllocator->Reset());
        // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
        // Reusing the command list reuses memory.
        ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), NULL));


        const UINT vbByteSize = (UINT)size;
        VertexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(),
            m_CommandList.Get(), vertices, vbByteSize, VertexBufferUploader);
        // 此时可能为空

        CommandPool::getInstance()->RecycleCommand(cmdList);
        ID3D12CommandList* rawCommandList = m_CommandList.Get();
        commandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&rawCommandList);
		renderAPIManager->FlushCommandQueue();

    }

    D3D12VertexBuffer::~D3D12VertexBuffer()
    {
    }

    // bind unbind 可能要想一想怎么做
    void D3D12VertexBuffer::Bind() const
    {
    }

    void D3D12VertexBuffer::Unbind() const
    {
    }

    // todo: 拆分输入槽
    //typedef struct D3D12_INPUT_ELEMENT_DESC {
    //    LPCSTR                     SemanticName;          // 语义名称, 一般POSITION等是规定写死的。
    //    UINT                       SemanticIndex;         // 语义索引, Texcoord0, Texcoord1,语义名称就是0，1
    //    DXGI_FORMAT                Format;                // 数据格式, float3/ float4
    //    UINT                       InputSlot;             // 输入槽索引， 相当于可以实现拆分position，和其他normal tangent的功能，之后可以实现一下，现在先写到一起。
    //    UINT                       AlignedByteOffset;     // 对齐字节偏移量
    //    D3D12_INPUT_CLASSIFICATION InputSlotClass;        // 输入槽分类
    //    UINT                       InstanceDataStepRate;  // 实例数据步进率
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
        Ref<CommandList> cmdList = CommandPool::getInstance()->GetCommand();

        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList = cmdList->getCommandList<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>>();
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator = cmdList->getCommandAllocator<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>();

        ThrowIfFailed(m_CommandAllocator->Reset());
        // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
        // Reusing the command list reuses memory.
        ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), NULL));


        const UINT indexSize = (UINT)size;
        IndexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(),
            m_CommandList.Get(), indices, size, IndexBufferUploader);
        // 此时可能为空
        //VertexByteStride = m_Layout.GetStride();
        IndexBufferByteSize = size;

        CommandPool::getInstance()->RecycleCommand(cmdList);
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
