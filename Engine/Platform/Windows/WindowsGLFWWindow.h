#pragma once

#include "Runtime/Core/Window/Window.h"
#include "Runtime/Graphics/RHI/Core/GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace Hazel {

	class WindowsGLFWWindow : public Window
	{
	public:
		WindowsGLFWWindow(const WindowProps& props);
		virtual ~WindowsGLFWWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetBackGroundColor()  override;
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		inline virtual void* GetNativeWindow() const { return m_Window; }
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;
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