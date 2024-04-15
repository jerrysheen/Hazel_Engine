#pragma once

#include "Hazel/Core/Window.h"
#include "Hazel/Renderer/GraphicsContext.h"

#include <WindowsX.h>

namespace Hazel {

	class WindowsDXGIWindow : public Window
	{
	public:
		WindowsDXGIWindow(const WindowProps& props);
		virtual ~WindowsDXGIWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetBackGroundColor()  override;
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		inline virtual void* GetNativeWindow() const { return m_Window; }
		LRESULT WindowsDXGIWindow::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static WindowsDXGIWindow* s_instance;
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
		
		void InitDirect3D();
	protected:
		HINSTANCE mhAppInst = nullptr;
		HWND      mhMainWnd = nullptr;
		std::wstring mMainWndCaption = L"D3DApp";
	private:
		HWND m_Window = nullptr;
		GraphicsContext* m_Context;

		// required by GLFW
		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}