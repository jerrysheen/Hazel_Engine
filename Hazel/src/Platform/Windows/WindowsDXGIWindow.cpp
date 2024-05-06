#include "hzpch.h"
#include "WindowsDXGIWindow.h"


namespace Hazel 
{
	WindowsDXGIWindow* WindowsDXGIWindow::s_instance = nullptr;

	WindowsDXGIWindow::WindowsDXGIWindow(const WindowProps& props)
	{
		s_instance = this;
		Init(props);

	}

	WindowsDXGIWindow::~WindowsDXGIWindow()
	{
		Shutdown();
		s_instance = nullptr;
	}

	void WindowsDXGIWindow::OnUpdate()
	{
		MSG msg = {0};
 
	//mTimer.Reset();

	while(msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if(PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
		{
            TranslateMessage( &msg );
            DispatchMessage( &msg );
		}
		// Otherwise, do animation/game stuff.
		else
        {	
			//mTimer.Tick();

			if( !mAppPaused )
			{
				//CalculateFrameStats();
				//Update(mTimer);	
				Draw();
			}
			else
			{
				Sleep(100);
			}
        }
    }
		
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

	LRESULT WindowsDXGIWindow::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		//HZ_CORE_INFO("WindowsDXGIWindow::MsgProc");
		//HZ_CORE_INFO("msg: {0}", msg);

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
				mAppPaused = true;
				//mTimer.Stop();
			}
			else
			{
				mAppPaused = false;
				//mTimer.Start();
			}
			return 0;

			// WM_SIZE is sent when the user resizes the window.  
		case WM_SIZE:
			// Save the new client area dimensions.
			mClientWidth = LOWORD(lParam);
			mClientHeight = HIWORD(lParam);
			if (md3dDevice)
			{
				if (wParam == SIZE_MINIMIZED)
				{
					mAppPaused = true;
					mMinimized = true;
					mMaximized = false;
				}
				else if (wParam == SIZE_MAXIMIZED)
				{
					mAppPaused = false;
					mMinimized = false;
					mMaximized = true;
					OnResize();
				}
				else if (wParam == SIZE_RESTORED)
				{

					// Restoring from minimized state?
					if (mMinimized)
					{
						mAppPaused = false;
						mMinimized = false;
						OnResize();
					}

					// Restoring from maximized state?
					else if (mMaximized)
					{
						mAppPaused = false;
						mMaximized = false;
						OnResize();
					}
					else if (mResizing)
					{
						// If user is dragging the resize bars, we do not resize 
						// the buffers here because as the user continuously 
						// drags the resize bars, a stream of WM_SIZE messages are
						// sent to the window, and it would be pointless (and slow)
						// to resize for each WM_SIZE message received from dragging
						// the resize bars.  So instead, we reset after the user is 
						// done resizing the window and releases the resize bars, which 
						// sends a WM_EXITSIZEMOVE message.
					}
					else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
					{
						OnResize();
					}
				}
			}
			return 0;

			// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		case WM_ENTERSIZEMOVE:
			mAppPaused = true;
			mResizing = true;
			//mTimer.Stop();
			return 0;

			// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			// Here we reset everything based on the new window dimensions.
		case WM_EXITSIZEMOVE:
			mAppPaused = false;
			mResizing = false;
			//mTimer.Start();
			OnResize();
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