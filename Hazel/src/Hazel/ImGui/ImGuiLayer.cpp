#include "hzpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#ifdef RENDER_API_OPENGL
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "examples/imgui_impl_opengl3.cpp"
#include "examples/imgui_impl_glfw.cpp" 
#elif RENDER_API_DIRECTX12
#include "examples/imgui_impl_win32.cpp"
#include "examples/imgui_impl_dx12.cpp"
#endif
#include "imgui_internal.h"

#include "Hazel/Core/Application.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "Hazel/RHI/Interface/IGfxViewManager.h"
#include "Hazel/RHI/Interface/DescriptorTypes.h"


namespace Hazel {


	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
#ifdef RENDER_API_OPENGL
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
#elif RENDER_API_DIRECTX12

		//		// Create application window
		////ImGui_ImplWin32_EnableDpiAwareness();
		//		WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
		//		::RegisterClassExW(&wc);
		//		HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX12 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);
		//
		//		// Initialize Direct3D
		//		if (!CreateDeviceD3D(hwnd))
		//		{
		//			CleanupDeviceD3D();
		//			::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		//			return 1;
		//		}
		//
		//		// Show the window
		//		::ShowWindow(hwnd, SW_SHOWDEFAULT);
		//		::UpdateWindow(hwnd);

				// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// Setup Platform/Renderer backends
		//ImGui_ImplWin32_Init(hwnd);
		//ImGui_ImplDX12_Init(g_pd3dDevice, NUM_FRAMES_IN_FLIGHT,
		//	DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
		//	g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
		//	g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
		ImGui_ImplWin32_Init(Application::Get().GetWindow().GetNativeWindow());
		renderAPIManager = dynamic_cast<D3D12RenderAPIManager*>(RenderAPIManager::getInstance()->GetManager().get());
		IGfxViewManager& gfxViewManager = IGfxViewManager::Get();
		ID3D12DescriptorHeap* srvHeap = static_cast<ID3D12DescriptorHeap*>(gfxViewManager.GetHeap(DescriptorHeapType::CbvSrvUav));
		ImGui_ImplDX12_Init(renderAPIManager->GetD3DDevice().Get(), NUM_FRAMES_IN_FLIGHT,
			renderAPIManager->GetBackBufferFormat(), srvHeap,
			srvHeap->GetCPUDescriptorHandleForHeapStart(),
			srvHeap->GetGPUDescriptorHandleForHeapStart());
#endif

	}


	void ImGuiLayer::OnDetach()
	{
#ifdef RENDER_API_OPENGL
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
#elif RENDER_API_DIRECTX12
		// Cleanup
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
#endif

	}

	void ImGuiLayer::Begin()
	{
#ifdef RENDER_API_OPENGL
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
#elif RENDER_API_DIRECTX12
		// Start the Dear ImGui frame
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();



#endif
	}

	void ImGuiLayer::End()
	{
#ifdef RENDER_API_OPENGL
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
#elif RENDER_API_DIRECTX12
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());
		// Rendering

		ImGui::Render();


		// utf-8�� 
		// �������Ϊ���������UI�㣬ÿһ��EditorLayer���棬�Ұ����ݶ��ӵ�Imgui���棬Ȼ��������ط�ͳһ���ղ㼶��һ����Ⱦ��
		// EditorLayer�������Լ�����Ⱦ���ݣ��ҿ��Բ��ܣ������������϶���һ��RenderTexture����ʽ��
		// Ҳ����˵����������ط���Ҫ�ӹ� swapbuffer�Ĳ�����
		
		// 2024-06-02 ������ط��ȳ��Խ��룬 Ȼ���ٰѹ����滻һ�¡�

		renderAPIManager->ResetCommandList();


		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList = renderAPIManager->GetCmdList();

		// Indicate a state transition on the resource usage.
		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderAPIManager->GetCurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
		// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
		mCommandList->RSSetViewports(1, renderAPIManager->GetCurrentViewPort());
		mCommandList->RSSetScissorRects(1, renderAPIManager->GetCurrentScissorRect());

		// Clear the back buffer and depth buffer.
		// �о������ backbufferview�Ļ�ȡ�������ɻ�
		mCommandList->ClearRenderTargetView(renderAPIManager->CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);

		// Specify the buffers we are going to render to.
		//mCommandList->OMSetRenderTargets(1, &renderAPIManager->CurrentBackBufferView(), true, &renderAPIManager->DepthStencilView());
		mCommandList->OMSetRenderTargets(1, &renderAPIManager->CurrentBackBufferView(), false, nullptr);

		//ID3D12DescriptorHeap* descriptorHeaps[] = { renderAPIManager->GetCbvHeap().Get()};
		IGfxViewManager& gfxViewManager = IGfxViewManager::Get();
		ID3D12DescriptorHeap* cbxHeap = static_cast<ID3D12DescriptorHeap*>(gfxViewManager.GetHeap(DescriptorHeapType::CbvSrvUav));

		ID3D12DescriptorHeap* descriptorHeaps[] = { cbxHeap };
		mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mCommandList.Get());
		// Indicate a state transition on the resource usage.
		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderAPIManager->GetCurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		// Done recording commands.
		ThrowIfFailed(mCommandList->Close());

		// Add the command list to the queue for execution.
		ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
		renderAPIManager->GetCommandQueue()->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// swap the back and front buffers
		ThrowIfFailed(renderAPIManager->GetSwapChain()->Present(1, 0));

		// Wait until frame commands are complete.  This waiting is inefficient and is
		// done for simplicity.  Later we will show how to organize our rendering code
		// so we do not have to wait per frame.
		//renderAPIManager->FlushCommandQueue();
		renderAPIManager->IncreaseLastSignaledValue();
		renderAPIManager->SignalFence();
		renderAPIManager->SyncCurrentFenceValueToFrameContext();
#endif


	}

	void ImGuiLayer::OnImGuiRender()
	{
	}




}