#include "hzpch.h"
#include "Application.h"
#include <GLFW/glfw3.h>
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Log.h"

namespace Hazel{

	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{

		while (m_Running) {
			glClearColor(1, 1, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
			HZ_CORE_INFO(m_Window->GetHeight());
		};
	}
}

