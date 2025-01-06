#include "hzpch.h"
#include "D3D12ConstantBuffer.h"
#include "Platform/D3D12/D3D12RenderAPIManager.h"
#include "Hazel/Core/Application.h"

namespace Hazel
{
	D3D12ConstantBuffer::D3D12ConstantBuffer(uint32_t elementSize)
		: mUploadBuffer(std::get<Microsoft::WRL::ComPtr<ID3D12Resource>>(m_BufferResource)), mMappedData(nullptr)
	{
        m_UUID = Unique::GetUUID();
		//在这里进行buffer初始化， 然后将内容换成Sharedpointer吧
		elementSize = d3dUtil::CalcConstantBufferByteSize(elementSize);
        m_BufferSize = elementSize;
        // todo:: 这个地方肯定需要修改， 不依赖application。。。
        D3D12RenderAPIManager* renderAPIManager = static_cast<D3D12RenderAPIManager*>(Application::Get().GetRenderAPIManager().get());
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



    D3D12ConstantBuffer::~D3D12ConstantBuffer()
    {
        if (mUploadBuffer != nullptr)
            mUploadBuffer->Unmap(0, nullptr);

        mMappedData = nullptr;
    }

    void D3D12ConstantBuffer::SetData(void* srcData, int length)
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
}
