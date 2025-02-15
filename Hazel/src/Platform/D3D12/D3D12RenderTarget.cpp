#include "hzpch.h"
#include "D3D12RenderTarget.h"


namespace Hazel
{
	D3D12RenderTarget::D3D12RenderTarget(const RenderTargetDescriptor& spec)
	{
        D3D12RenderAPIManager* renderAPIManager = dynamic_cast<D3D12RenderAPIManager*>(RenderAPIManager::getInstance()->GetManager().get());
        Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();

        ID3D12Fence* fence = NULL;
        HRESULT hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
        assert(SUCCEEDED(hr));

        HANDLE event = CreateEvent(0, 0, 0, 0);
        assert(event != NULL);

        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.NodeMask = 1;

        ID3D12CommandQueue* cmdQueue = NULL;
        hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&cmdQueue));
        assert(SUCCEEDED(hr));

        ID3D12CommandAllocator* cmdAlloc = NULL;
        hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAlloc));
        assert(SUCCEEDED(hr));

        ID3D12GraphicsCommandList* cmdList = NULL;
        hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAlloc, NULL, IID_PPV_ARGS(&cmdList));
        assert(SUCCEEDED(hr));
        // 尝试直接在这个地方创建一个RT， 应该是只需要渲染到窗口上就行了。 
        // 以后所有的多线程渲染的内容， 应该都是基于这个窗口来进行的， 即最后都是渲染出一个framebuffer， 然后把colorattachement绑定在Imgui::Image上。
        // 我的所有渲染行为应该是基于相机的，所以相机会持有一个FrameBuffer，我最后应该是把相机的FrameBufffer结果注入到RT上。
        // 所以我这个地方先放一个相机，别的模型啥的后面再加入。
        // 相机后续的渲染逻辑可以参考Unity的。。 就是相机-> renderer -> renderingPipeline.
        // 应该先有一个framebuffer，相机是在这个framebuffer的逻辑之上的。
        //m_BackBuffer = Framebuffer::Create({ 1280, 720 });
                // delete model;
        D3D12_RESOURCE_DESC colorBufferDesc = {};
        colorBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        colorBufferDesc.Width = 1280;       // 设置颜色缓冲区的宽度
        colorBufferDesc.Height = 720;     // 设置颜色缓冲区的高度
        colorBufferDesc.DepthOrArraySize = 1;
        colorBufferDesc.MipLevels = 1;       // 不需要多级细化
        colorBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 颜色格式
        colorBufferDesc.SampleDesc.Count = 1;  // 不使用多重采样
        colorBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        colorBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET; // 声明这是一个渲染目标

        // 为颜色缓冲区指定清除值
        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        clearValue.Color[0] = 1.0f;
        clearValue.Color[1] = 1.0f;
        clearValue.Color[2] = 1.0f;
        clearValue.Color[3] = 1.0f;


        device->CreateCommittedResource
        (
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &colorBufferDesc,
            D3D12_RESOURCE_STATE_RENDER_TARGET, // 初始状态为渲染目标
            &clearValue,
            IID_PPV_ARGS(&bufferResrouce)
        );

        hr = cmdList->Close();
        assert(SUCCEEDED(hr));

        cmdQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&cmdList);
        hr = cmdQueue->Signal(fence, 1);
        assert(SUCCEEDED(hr));

        fence->SetEventOnCompletion(1, event);
        WaitForSingleObject(event, INFINITE);

        cmdList->Release();
        cmdAlloc->Release();
        cmdQueue->Release();
        CloseHandle(event);
        fence->Release();
	}

	D3D12RenderTarget::~D3D12RenderTarget()
	{
	}
}