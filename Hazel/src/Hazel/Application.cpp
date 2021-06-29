#include "hzpch.h"
#include "Application.h"
#include <GLFW/glfw3.h>
#include "Hazel/Log.h"

namespace Hazel{

	// 这个this就是同一个函数下面和现在这个函数绑定的那个东西，也就是
	// EventFn/EventCallbackFn 绑定了，EVentFn就是我现在这个函数。并且有一个placeholder是后续
	// 在EventFn/EventCallbackFn中来使用的 
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
	}

	Application::~Application()
	{
	}

	// window 和event 完全分离了，只有event相互连接
	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		HZ_CORE_INFO("{0}", e);
	}
	
	void Application::Run()
	{

		while (m_Running) {
			glClearColor(1, 1, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
		};
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) 
	{
		m_Running = false;
		return true;
	}
}

