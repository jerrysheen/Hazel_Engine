#include "hzpch.h"
#include "WindowsGLFWWindow.h"
#include "Runtime/Core/Log.h"
#include "Runtime/Core/Events/ApplicationEvent.h"
#include "Runtime/Core/Events/MouseEvent.h"
#include "Runtime/Core/Events/Event.h"
#include "Runtime/Core/Events/KeyEvent.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include <glad/glad.h>

namespace Hazel {

	static bool s_GLFWInitialized = false;
	static void GLFWErrorCallback(int error, const char* description) 
	{
		HZ_CORE_ERROR("GLFW Error ({0}) : {1}", error, description);
	}

	WindowsGLFWWindow::WindowsGLFWWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsGLFWWindow::~WindowsGLFWWindow()
	{
		Shutdown();
	}

	void WindowsGLFWWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		

		HZ_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (!s_GLFWInitialized)
		{
			// TODO: glfwTerminate on system shutdown
			int success = glfwInit();
			HZ_CORE_ASSERT(success, "Could not intialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}
		
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

		m_Window = glfwCreateWindow(mode->width, mode->height, "My Title", nullptr, NULL);

		//m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		
		m_Context = new OpenGLContext(m_Window);
		m_Context->Init();
		
		// ָ�����ڵ�WindowProps��m_Window�������
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);
		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;
				WindowResizeEvent event(width, height, false, false);
				data.EventCallback(event);
				
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action) 
				{
					case GLFW_PRESS:
					{
						KeyPressedEvent event(key, 0);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasedEvent event(key);
						data.EventCallback(event);
						break;
					}
					case GLFW_REPEAT: 
					{
						KeyPressedEvent event(key, 1);
						data.EventCallback(event);
						break;
					}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.EventCallback(event);
			});


		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				switch (action)
				{
					case GLFW_PRESS:
					{
						MouseButtonPressedEvent event(button);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						MouseButtonReleasedEvent event(button);
						data.EventCallback(event);
						break;
					}

				}
			});
		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			//HZ_CORE_TRACE("{0}, {1}", xoffset, yoffset);
			MouseScrolledEvent event(xoffset, yoffset);
			data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
			});
	}

	void WindowsGLFWWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

	void WindowsGLFWWindow::OnUpdate()
	{
		glfwPollEvents();
		m_Context->SwapBuffers();
	}



	void WindowsGLFWWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool WindowsGLFWWindow::IsVSync() const
	{
		return m_Data.VSync;
	}
		
	void WindowsGLFWWindow::SetBackGroundColor()
	{
		// �л�һ��Swapbuffer, ��Ȼֻ����һ�Σ��ᵼ����ɫ��˸��
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		m_Context->SwapBuffers();
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

};