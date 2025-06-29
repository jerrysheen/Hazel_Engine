#pragma once

#include "Hazel/Core/Window.h"
#include "Hazel/Renderer/GraphicsContext.h"
#include "Hazel/Core/Application.h"


namespace Hazel {

	class WindowsDXGIWindow : public Window
	{
	public:
		WindowsDXGIWindow(const WindowProps& props);
		virtual ~WindowsDXGIWindow();

		void OnUpdate() override;

		inline static WindowsDXGIWindow& Get() { return *s_Instance; }
		inline HWND GetDXGIWindowInstance() const { return mhMainWnd; }
		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetBackGroundColor()  override;
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		inline virtual void* GetNativeWindow() const { return mhMainWnd; }
		LRESULT WindowsDXGIWindow::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static WindowsDXGIWindow* s_Instance;
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
		HINSTANCE mhAppInst = nullptr; // application instance handle
		HWND      mhMainWnd = nullptr; // main window handle
		// Derived class should set these in derived constructor to customize starting values.
		std::wstring mMainWndCaption = L"d3d App";
	private:
		//HWND m_Window = nullptr;
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