#include "hzpch.h"
#include "D3D12ConstantBuffer.h"
#include "Platform/D3D12/D3D12RenderAPIManager.h"
#include "Runtime/Core/Application.h"

namespace Hazel
{
	D3D12ConstantBuffer::D3D12ConstantBuffer(uint32_t elementSize)
		: mUploadBuffer(std::get<Microsoft::WRL::ComPtr<ID3D12Resource>>(m_BufferResource)), mMappedData(nullptr)
	{
        m_UUID = Unique::GetUUID();
		// 计算常量buffer对齐大小
		elementSize = d3dUtil::CalcConstantBufferByteSize(elementSize);
        m_BufferSize = elementSize;
        
        // 获取D3D12设备
        D3D12RenderAPIManager* renderAPIManager = dynamic_cast<D3D12RenderAPIManager*>(RenderAPIManager::getInstance()->GetManager().get());
        Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();
        
        // 创建上传缓冲区
        ThrowIfFailed(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(elementSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&mUploadBuffer)));

        // 映射缓冲区
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
        // 确保源数据指针和目标缓冲区都已正确映射
        if (srcData && mMappedData) {
            memcpy(mMappedData, srcData, length);
        }
        else {
            // 处理无效输入：无效指针
            std::cerr << "Invalid source or destination pointer." << std::endl;
        }
    }
}
