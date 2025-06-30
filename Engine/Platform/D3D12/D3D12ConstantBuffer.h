#pragma once
#include "hzpch.h"
#include "Runtime/Graphics/RHI/Core/Buffer.h"

namespace Hazel 
{
	class D3D12ConstantBuffer : public ConstantBuffer
	{
	public:
		D3D12ConstantBuffer(uint32_t elementSize);
		virtual ~D3D12ConstantBuffer();
		virtual void SetData(void* srcData, int length) override;
		virtual void* GetNativeResource() const override { return mUploadBuffer.Get(); }
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource>& mUploadBuffer;
		BYTE* mMappedData = nullptr;
	};

}