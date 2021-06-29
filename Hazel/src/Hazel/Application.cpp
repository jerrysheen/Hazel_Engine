#include "hzpch.h"
#include "Application.h"
#include <GLFW/glfw3.h>
#include "Hazel/Log.h"

namespace Hazel{

	// ���this����ͬһ�����������������������󶨵��Ǹ�������Ҳ����
	// EventFn/EventCallbackFn ���ˣ�EVentFn�������������������������һ��placeholder�Ǻ���
	// ��EventFn/EventCallbackFn����ʹ�õ� 
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
	}

	Application::~Application()
	{
	}

	// window ��event ��ȫ�����ˣ�ֻ��event�໥����
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

