#pragma once

#include "hzpch.h"

#include "Runtime/Core/Core.h"
#include "Runtime/Core/Events/Event.h"
//#include "Hazel/Renderer/Renderer.h"

namespace Hazel {

	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		// struct��constructor����classֻ�г�ԱĬ��private��public������
		// sturuct Ĭ����public��
		WindowProps(const std::string& title = "Hazel Engine",
			unsigned int width = 1280,
			unsigned int height = 720)
			: Title(title), Width(width), Height(height)
		{
			//Application::Get().SetWindowWidth(width);
			//Application::Get().SetWindowHeight(height);
		}
	};

	// Interface representing a desktop system based Window
	class HAZEL_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		// ����һЩinterface�� ��Ҫ��ÿ��platformȥʵ��
		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual void SetBackGroundColor() = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		// Ҳ��Ҫ��ͬ��ƽ̨ȥʵ��implement
		static Window* Create(const WindowProps& props = WindowProps());
	};

}