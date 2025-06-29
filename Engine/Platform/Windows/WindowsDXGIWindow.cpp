#include "hzpch.h"
#include "WindowsDXGIWindow.h"
#include "examples/imgui_impl_win32.h"
#include "examples/imgui_impl_dx12.h"

namespace Hazel 
{
	WindowsDXGIWindow* WindowsDXGIWindow::s_Instance = nullptr;

	WindowsDXGIWindow::WindowsDXGIWindow(const WindowProps& props)
	{
		s_Instance = this;
		Init(props);

	}

	WindowsDXGIWindow::~WindowsDXGIWindow()
	{
		Shutdown();
		s_Instance = nullptr;
	}

	void WindowsDXGIWindow::OnUpdate()
	{
		MSG msg = {0};
 
	//mTimer.Reset();

	//while(msg.message != WM_QUIT)
	//{
		// If there are Window messages then process them.
		if(PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
		{
            TranslateMessage( &msg );
            DispatchMessage( &msg );
		}
		// Otherwise, do animation/game stuff.
		//else
  //      {	
		//	//mTimer.Tick();

		//	if( !mAppPaused )
		//	{
		//		//CalculateFrameStats();
		//		//Update(mTimer);	
		//		Draw();
		//	}
		//	else
		//	{
		//		Sleep(100);
		//	}
  //      }
    //}
		
	}


	static LRESULT CALLBACK
		MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// Forward hwnd on because we can get messages (e.g., WM_CREATE)
		// before CreateWindow returns, and thus before mhMainWnd is valid.
		//// ����ط�GetNativeWindow���ص���һ��void*��������Ҫcast��WindowsDXGIWindow*
		//Application& app = Application::Get();
		//WindowsDXGIWindow* nativeWindow = static_cast<WindowsDXGIWindow*>(app.GetWindow().GetNativeWindow());
		//return nativeWindow->MsgProc(hwnd, msg, wParam, lParam);

		return WindowsDXGIWindow::s_Instance->MsgProc(hwnd, msg, wParam, lParam);
	}

	void WindowsDXGIWindow::Init(const WindowProps& props)
	{
		mMainWndCaption = L"D3DApp";
		WNDCLASS wc;

		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = MainWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = mhAppInst;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.lpszMenuName = 0;
		wc.lpszClassName = L"MainWnd";

		if (!RegisterClass(&wc))
		{
			MessageBox(0, L"RegisterClass Failed.", 0, 0);
			//return false;
		}

		// Compute window rectangle dimensions based on requested client area dimensions.
		RECT R = { 0, 0, props.Width, props.Height };
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		int width = R.right - R.left;
		int height = R.bottom - R.top;

		mhMainWnd = CreateWindow(L"MainWnd", mMainWndCaption.c_str(),
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mhAppInst, 0);
		if (!mhMainWnd)
		{
			MessageBox(0, L"CreateWindow Failed.", 0, 0);
			//return false;
		}

		ShowWindow(mhMainWnd, SW_SHOW);
		UpdateWindow(mhMainWnd);

		//return true;
	}


	void WindowsDXGIWindow::Shutdown()
	{
	}

	void WindowsDXGIWindow::SetBackGroundColor()
	{
	}

	void WindowsDXGIWindow::SetVSync(bool enabled)
	{
	}


	bool WindowsDXGIWindow::IsVSync() const
	{
		return false;
	}

	//extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	LRESULT WindowsDXGIWindow::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		//HZ_CORE_INFO("WindowsDXGIWindow::MsgProc");
		//HZ_CORE_INFO("msg: {0}", msg);
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
			return true;
		return DefWindowProc(hwnd, msg, wParam, lParam);

		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			// 这里通常你不需要进行任何GDI绘制，因为你使用的是D3D12
			EndPaint(hwnd, &ps);
			return 0;
		}
		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				AppActiveEvent event(false);
				if(m_Data.EventCallback)m_Data.EventCallback(event);
			}
			else
			{
				AppActiveEvent event(true);
				if(m_Data.EventCallback)m_Data.EventCallback(event);
			}
			return 0;

			// WM_SIZE is sent when the user resizes the window.  
		case WM_SIZE: 
		{
			// Save the new client area dimensions.
			int width = LOWORD(lParam);
			int height = HIWORD(lParam);
			if (wParam == SIZE_MINIMIZED)
			{
				WindowResizeEvent event(width, height, true, false);
				m_Data.EventCallback(event);
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				WindowResizeEvent event(width, height, false, true);
				m_Data.EventCallback(event);
			}
			else if (wParam == SIZE_RESTORED)
			{

				//// Restoring from minimized state?
				//if (mMinimized)
				//{
				//	mAppPaused = false;
				//	mMinimized = false;
				//	OnResize();
				//}

				//// Restoring from maximized state?
				//else if (mMaximized)
				//{
				//	mAppPaused = false;
				//	mMaximized = false;
				//	OnResize();
				//}

			}
			return 0;
		}


			// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		case WM_ENTERSIZEMOVE:
			//mAppPaused = true;
			//mResizing = true;
			////mTimer.Stop();
			return 0;

			// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			// Here we reset everything based on the new window dimensions.
		case WM_EXITSIZEMOVE:
			//mAppPaused = false;
			//mResizing = false;
			////mTimer.Start();
			//OnResize();
			return 0;

			// WM_DESTROY is sent when the window is being destroyed.
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

			// The WM_MENUCHAR message is sent when a menu is active and the user presses 
			// a key that does not correspond to any mnemonic or accelerator key. 
		case WM_MENUCHAR:
			// Don't beep when we alt-enter.
			return MAKELRESULT(0, MNC_CLOSE);

			// Catch this message so to prevent the window from becoming too small.
		case WM_GETMINMAXINFO:
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
			return 0;

		case WM_LBUTTONDOWN:
			return 0;
		case WM_MBUTTONDOWN:
			return 0;
		case WM_RBUTTONDOWN: 
		{
			HZ_CORE_INFO("RBUTTTONDOWN event received..");

			MouseButtonPressedEvent event(0);
			m_Data.EventCallback(event);
			return 0;
		}
		case WM_LBUTTONUP:
			return 0;
		case WM_MBUTTONUP:
			return 0;
		case WM_RBUTTONUP:
			//OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_MOUSEMOVE:
			//OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_KEYUP:
			if (wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
			}
			else if ((int)wParam == VK_F2)
				//Set4xMsaaState(!m4xMsaaState);

			return 0;
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}


}