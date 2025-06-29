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
        // ����ֱ��������ط�����һ��RT�� Ӧ����ֻ��Ҫ��Ⱦ�������Ͼ����ˡ� 
        // �Ժ����еĶ��߳���Ⱦ�����ݣ� Ӧ�ö��ǻ���������������еģ� ���������Ⱦ��һ��framebuffer�� Ȼ���colorattachement����Imgui::Image�ϡ�
        // �ҵ�������Ⱦ��ΪӦ���ǻ�������ģ�������������һ��FrameBuffer�������Ӧ���ǰ������FrameBufffer���ע�뵽RT�ϡ�
        // ����������ط��ȷ�һ����������ģ��ɶ�ĺ����ټ��롣
        // �����������Ⱦ�߼����Բο�Unity�ġ��� �������-> renderer -> renderingPipeline.
        // Ӧ������һ��framebuffer������������framebuffer���߼�֮�ϵġ�
        //m_BackBuffer = Framebuffer::Create({ 1280, 720 });
                // delete model;
        D3D12_RESOURCE_DESC colorBufferDesc = {};
        colorBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        colorBufferDesc.Width = 1280;       // ������ɫ�������Ŀ��
        colorBufferDesc.Height = 720;     // ������ɫ�������ĸ߶�
        colorBufferDesc.DepthOrArraySize = 1;
        colorBufferDesc.MipLevels = 1;       // ����Ҫ�༶ϸ��
        colorBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // ��ɫ��ʽ
        colorBufferDesc.SampleDesc.Count = 1;  // ��ʹ�ö��ز���
        colorBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        colorBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET; // ��������һ����ȾĿ��

        // Ϊ��ɫ������ָ�����ֵ
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
            D3D12_RESOURCE_STATE_RENDER_TARGET, // ��ʼ״̬Ϊ��ȾĿ��
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