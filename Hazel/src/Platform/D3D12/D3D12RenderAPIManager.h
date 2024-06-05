#pragma once
#include "Hazel/Graphics/RenderAPIManager.h"

#include "Platform/D3D12/d3dUtil.h"
#include "Platform/D3D12/d3dx12.h"

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

#include <WindowsX.h>
using Microsoft::WRL::ComPtr;
using namespace std;
using namespace DirectX;
namespace Hazel {

	class D3D12RenderAPIManager : public RenderAPIManager
	{
	public:
		D3D12RenderAPIManager();
		virtual ~D3D12RenderAPIManager();

		static D3D12RenderAPIManager* D3D12RenderAPIManager::s_instance;
		virtual void OnUpdate() override;

		inline Microsoft::WRL::ComPtr<ID3D12Device> GetD3DDevice() { return md3dDevice; }
		inline DXGI_FORMAT GetBackBufferFormat() { return mBackBufferFormat; }
		inline Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetCbvHeap() { return mCbvHeap; }
		inline Microsoft::WRL::ComPtr<ID3D12CommandAllocator> GetCmdListAlloc() { return mDirectCmdListAlloc; }
		inline Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCmdList() { return mCommandList; }
		inline  D3D12_VIEWPORT* GetCurrentViewPort() { return &mScreenViewport; }
		inline  D3D12_RECT* GetCurrentScissorRect() { return &mScissorRect; }
		inline  Microsoft::WRL::ComPtr<IDXGISwapChain> GetSwapChain() { return mSwapChain; }
		inline  Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetCommandQueue() { return mCommandQueue; }
		inline  void UpdateBackBufferIndex() { mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;; }
		void ReInitCommandList();
		ID3D12Resource* GetCurrentBackBuffer()const;
		void FlushCommandQueue();

		D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;
	private:
		void InitDirect3D();

		void CreateCommandObjects();
		void CreateSwapChain();
		void CreateRtvAndDsvDescriptorHeaps();
		void CreateCvbDescriptorHeaps();
		void Draw();

		void OnResize();

		void LogAdapters();
		void LogAdapterOutputs(IDXGIAdapter* adapter);
		void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
	protected:


		bool      mAppPaused = false;  // is the application paused?
		bool      mMinimized = false;  // is the application minimized?
		bool      mMaximized = false;  // is the application maximized?
		bool      mResizing = false;   // are the resize bars being dragged?
		bool      mFullscreenState = false;// fullscreen enabled

		// Set true to use 4X MSAA (?.1.8).  The default is false.
		bool      m4xMsaaState = false;    // 4X MSAA enabled
		UINT      m4xMsaaQuality = 0;      // quality level of 4X MSAA

		// Used to keep track of the “delta-time?and game time (?.4).

		Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
		Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;

		Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
		UINT64 mCurrentFence = 0;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

		static const int SwapChainBufferCount = 3;
		int mCurrBackBuffer = 0;
		Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
		Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCbvHeap;

		D3D12_VIEWPORT mScreenViewport;
		D3D12_RECT mScissorRect;

		UINT mRtvDescriptorSize = 0;
		UINT mDsvDescriptorSize = 0;
		UINT mCbvSrvUavDescriptorSize = 0;


		D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
		DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		int mClientWidth = 800;
		int mClientHeight = 600;
	};
}