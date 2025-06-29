#include "hzpch.h"
#include "D3D12RenderAPIManager.h"
#include "Platform/Windows/WindowsDXGIWindow.h"

namespace Hazel
{

	D3D12RenderAPIManager::D3D12RenderAPIManager()
	{
		InitDirect3D();
		
		// 初始化当前CommandList为主CommandList
		mCurrentCommandList = mCommandList;

		// Do the initial resize code.
		//OnResize();
	}

	D3D12RenderAPIManager::~D3D12RenderAPIManager()
	{
	}

	ID3D12Resource* D3D12RenderAPIManager::GetCurrentBackBuffer()const
	{
		return g_mainRenderTargetResource[mCurrBackBufferIndex];
	}

	ID3D12CommandAllocator* D3D12RenderAPIManager::GetCurrentCommandAllocator() const
	{
		return g_frameContext[mCurrBackBufferIndex].CommandAllocator.Get();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE D3D12RenderAPIManager::CurrentBackBufferView()const
	{
		return g_mainRenderTargetDescriptor[mCurrBackBufferIndex];
		/*return CD3DX12_CPU_DESCRIPTOR_HANDLE(
			mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
			mCurrBackBufferIndex,
			mRtvDescriptorSize);*/
	}

	D3D12_CPU_DESCRIPTOR_HANDLE D3D12RenderAPIManager::DepthStencilView()const
	{
		return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
	}

	D3D12RenderAPIManager::FrameContext* D3D12RenderAPIManager::WaitForNextFrameResources()
	{
		UINT nextFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
		mCurrBackBufferIndex = nextFrameIndex;

		HANDLE waitableObjects[] = { g_hSwapChainWaitableObject, nullptr };
		DWORD numWaitableObjects = 1;

		FrameContext* frameCtx = &g_frameContext[nextFrameIndex % NUM_BACK_BUFFERS];
		UINT64 fenceValue = frameCtx->FenceValue;
		if (fenceValue != 0) // means no fence was signaled
		{
			frameCtx->FenceValue = 0;
			mFence->SetEventOnCompletion(fenceValue, g_fenceEvent);
			waitableObjects[1] = g_fenceEvent;
			numWaitableObjects = 2;
		}

		WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

		return frameCtx;
	}

	void D3D12RenderAPIManager::OnResize()
	{
		assert(md3dDevice);
		assert(mSwapChain);
		assert(g_frameContext[0].CommandAllocator);

		// Flush before changing any resources.
		FlushCommandQueue();

		ThrowIfFailed(mCommandList->Reset(GetCurrentCommandAllocator(), nullptr));

		// Release the previous resources we will be recreating.
		for (int i = 0; i < SwapChainBufferCount; ++i)
			mSwapChainBuffer[i].Reset();
		mDepthStencilBuffer.Reset();

		// Resize the swap chain.
		ThrowIfFailed(mSwapChain->ResizeBuffers(
			SwapChainBufferCount,
			mClientWidth, mClientHeight,
			mBackBufferFormat,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

		mCurrBackBufferIndex = 0;

		CD3DX12_CPU_DESCRIPTOR_HANDLE backbufferHeapHandle(mBackBufferHeap->GetCPUDescriptorHandleForHeapStart());
		//mRtvDescriptorCount = 0;
		for (UINT i = 0; i < SwapChainBufferCount; i++)
		{
			ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])));
			md3dDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, backbufferHeapHandle);
			backbufferHeapHandle.Offset(1, mRtvDescriptorSize);
			//mRtvDescriptorCount++;
		}

		// Create the depth/stencil buffer and view.
		D3D12_RESOURCE_DESC depthStencilDesc;
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDesc.Alignment = 0;
		depthStencilDesc.Width = mClientWidth;
		depthStencilDesc.Height = mClientHeight;
		depthStencilDesc.DepthOrArraySize = 1;
		depthStencilDesc.MipLevels = 1;

		// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
		// the depth buffer.  Therefore, because we need to create two views to the same resource:
		//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
		//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
		// we need to create the depth buffer resource with a typeless format.  
		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

		depthStencilDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear;
		optClear.Format = mDepthStencilFormat;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;
		ThrowIfFailed(md3dDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optClear,
			IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));

		// Create descriptor to mip level 0 of entire resource using the format of the resource.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHeapHandle(mDsvHeap->GetCPUDescriptorHandleForHeapStart());
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = mDepthStencilFormat;
		dsvDesc.Texture2D.MipSlice = 0;
		md3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, dsvHeapHandle);

		// Transition the resource from its initial state to be used as a depth buffer.
		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

		// Execute the resize commands.
		ThrowIfFailed(mCommandList->Close());
		ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// Wait until resize is complete.
		FlushCommandQueue();

		// Update the viewport transform to cover the client area.
		mScreenViewport.TopLeftX = 0;
		mScreenViewport.TopLeftY = 0;
		mScreenViewport.Width = static_cast<float>(mClientWidth);
		mScreenViewport.Height = static_cast<float>(mClientHeight);
		mScreenViewport.MinDepth = 0.0f;
		mScreenViewport.MaxDepth = 1.0f;

		mScissorRect = { 0, 0, mClientWidth, mClientHeight };
	}

	void D3D12RenderAPIManager::FlushCommandQueue()
	{
		// Advance the fence value to mark commands up to this fence point.
		mCurrentFence++;

		// Add an instruction to the command queue to set a new fence point.  Because we 
		// are on the GPU timeline, the new fence point won't be set until the GPU finishes
		// processing all the commands prior to this Signal().
		ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mCurrentFence));

		// Wait until the GPU has completed commands up to this fence point.
		if (mFence->GetCompletedValue() < mCurrentFence)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

			// Fire event when GPU hits current fence.  
			ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFence, eventHandle));

			// Wait until the GPU hits current fence event is fired.
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	//void D3D12RenderAPIManager::OnUpdate()
	//{
	//	//Draw();
	//}

	// reset时， current buffer + 1 ˡ。。。
	void D3D12RenderAPIManager::ResetCommandList()
	{
		FrameContext* frameCtx = WaitForNextFrameResources();
		frameCtx->CommandAllocator->Reset();

		// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
		// Reusing the command list reuses memory.
		ThrowIfFailed(mCommandList->Reset(frameCtx->CommandAllocator.Get(), nullptr));
	}

	void  D3D12RenderAPIManager::InitDirect3D()
	{
#if defined(DEBUG) || defined(_DEBUG) 
		// Enable the D3D12 debug layer.
		{
			ComPtr<ID3D12Debug> debugController;
			ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
			debugController->EnableDebugLayer();
		}
#endif

	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory)));

	// Try to create hardware device.
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,             // default adapter
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&md3dDevice));

	// Fallback to WARP device.
	if (FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(mdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&md3dDevice)));
	}

	ThrowIfFailed(md3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&mFence)));

	g_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	ThrowIfFailed(g_fenceEvent == nullptr);

	mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = mBackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(md3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));

	m4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(m4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

#ifdef _DEBUG
	LogAdapters();
#endif

	CreateCommandObjects();
	CreateSwapChain();
	CreateHeaps();
	//CreateCvbDescriptorHeaps();
	CreateRenderTarget();
	//return true;
	}

	void D3D12RenderAPIManager::CreateRenderTarget()
	{
		for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
		{
			ID3D12Resource* pBackBuffer = nullptr;
			mSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
			md3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, g_mainRenderTargetDescriptor[i]);
			g_mainRenderTargetResource[i] = pBackBuffer;
		}
	}


	void D3D12RenderAPIManager::LogAdapters()
	{
		UINT i = 0;
		IDXGIAdapter* adapter = nullptr;
		std::vector<IDXGIAdapter*> adapterList;
		while (mdxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC desc;
			adapter->GetDesc(&desc);

			std::wstring text = L"***Adapter: ";
			text += desc.Description;
			text += L"\n";

			OutputDebugString(text.c_str());

			adapterList.push_back(adapter);

			++i;
		}

		for (size_t i = 0; i < adapterList.size(); ++i)
		{
			LogAdapterOutputs(adapterList[i]);
			ReleaseCom(adapterList[i]);
		}
	}

	void D3D12RenderAPIManager::LogAdapterOutputs(IDXGIAdapter* adapter)
	{
		UINT i = 0;
		IDXGIOutput* output = nullptr;
		while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_OUTPUT_DESC desc;
			output->GetDesc(&desc);

			std::wstring text = L"***Output: ";
			text += desc.DeviceName;
			text += L"\n";
			OutputDebugString(text.c_str());

			LogOutputDisplayModes(output, mBackBufferFormat);

			ReleaseCom(output);

			++i;
		}
	}

	void D3D12RenderAPIManager::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
	{
		UINT count = 0;
		UINT flags = 0;

		// Call with nullptr to get list count.
		output->GetDisplayModeList(format, flags, &count, nullptr);

		std::vector<DXGI_MODE_DESC> modeList(count);
		output->GetDisplayModeList(format, flags, &count, &modeList[0]);

		for (auto& x : modeList)
		{
			UINT n = x.RefreshRate.Numerator;
			UINT d = x.RefreshRate.Denominator;
			std::wstring text =
				L"Width = " + std::to_wstring(x.Width) + L" " +
				L"Height = " + std::to_wstring(x.Height) + L" " +
				L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
				L"\n";

			::OutputDebugString(text.c_str());
		}
	}

	void D3D12RenderAPIManager::CreateCommandObjects()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		ThrowIfFailed(md3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));

		for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
		{
			ThrowIfFailed(md3dDevice->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(g_frameContext[i].CommandAllocator.GetAddressOf())));
		}


		ThrowIfFailed(md3dDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			g_frameContext[0].CommandAllocator.Get(), // Associated command allocator
			nullptr,                   // Initial PipelineStateObject
			IID_PPV_ARGS(mCommandList.GetAddressOf())));

		// Start off in a closed state.  This is because the first time we refer 
		// to the command list we will Reset it, and it needs to be closed before
		// calling Reset.
		mCommandList->Close();
	}

	void D3D12RenderAPIManager::CreateSwapChain()
	{
		// 初始化imgui模式去涉及的swapchain1sawapchain4.
		// Setup swap chain
		DXGI_SWAP_CHAIN_DESC1 sd;
		{
			ZeroMemory(&sd, sizeof(sd));
			sd.BufferCount = SwapChainBufferCount;
			sd.Width = 0;
			sd.Height = 0;
			sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
			sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			sd.Scaling = DXGI_SCALING_STRETCH;
			sd.Stereo = FALSE;
		}

		IDXGISwapChain1* swapChain1 = nullptr;
		//assert(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory)) != S_OK);
		assert(mdxgiFactory->CreateSwapChainForHwnd(mCommandQueue.Get(), WindowsDXGIWindow::s_Instance->GetDXGIWindowInstance(), &sd, nullptr, nullptr, &swapChain1) == S_OK);
		assert(swapChain1->QueryInterface(IID_PPV_ARGS(&mSwapChain)) == S_OK);
		swapChain1->Release();
		mSwapChain->SetMaximumFrameLatency(SwapChainBufferCount);
		g_hSwapChainWaitableObject = mSwapChain->GetFrameLatencyWaitableObject();
	}

	void D3D12RenderAPIManager::CreateHeaps()
	{
		// 存储Render Target和深度缓冲的描述符堆
		// 默认十个 todo 记录所有的backbuffer
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
		rtvHeapDesc.NumDescriptors = 10;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = 0;
		ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
			&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));
		mRtvHeap->SetName(L"RTV Heap");


		// 为backbuffer和swapchain分配rt描述符堆，backbuffer的描述符是固定的
		// Imgui模式，不需要关注RenderTargetDescriptor，注意这里是实现的一部分
		D3D12_DESCRIPTOR_HEAP_DESC backBufferHeapDesc;
		backBufferHeapDesc.NumDescriptors = SwapChainBufferCount;
		backBufferHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		backBufferHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		backBufferHeapDesc.NodeMask = 0;
		ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
			&backBufferHeapDesc, IID_PPV_ARGS(mBackBufferHeap.GetAddressOf())));
		mBackBufferHeap->SetName(L"BackBuffer Heap");

		SIZE_T rtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = mBackBufferHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
		{
			g_mainRenderTargetDescriptor[i] = rtvHandle;
			rtvHandle.ptr += rtvDescriptorSize;
			//mRtvDescriptorCount++;
		}

		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHeapDesc.NodeMask = 0;
		ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
			&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
		mDsvHeap->SetName(L"DSV Heap");



		// mSrvHeap是srv  srv用于shader访问的资源和view的heap
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // 仅GPU访问
		ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(mSrvHeap.GetAddressOf())));
		mSrvHeap->SetName(L"SRV Heap");

	}
		
	void D3D12RenderAPIManager::CreateCvbDescriptorHeaps()
	{
		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
		cbvHeapDesc.NumDescriptors = 10;
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		cbvHeapDesc.NodeMask = 0;
		ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(mCbvHeap.GetAddressOf())));
		mCbvHeap->SetName(L"mCbvHeap");
	}

	// === 新增的CommandList管理功能 ===
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> D3D12RenderAPIManager::GetCurrentCommandList()
	{
		// 如果没有设置当前CommandList，返回主CommandList
		return mCurrentCommandList ? mCurrentCommandList : mCommandList;
	}

	void D3D12RenderAPIManager::SetCurrentCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList)
	{
		// 设置当前活跃的CommandList
		mCurrentCommandList = cmdList;
		
		// 如果传入nullptr，则回退到主CommandList
		if (!mCurrentCommandList) {
			mCurrentCommandList = mCommandList;
		}
	}

}