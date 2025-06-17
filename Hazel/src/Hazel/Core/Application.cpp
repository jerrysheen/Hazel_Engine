#include "hzpch.h"
#include "Application.h"

#include "Hazel/Core/Log.h"
#include "Glfw/glfw3.h"
#include "Hazel/Gfx/CommandPool.h"
#include "Hazel/Gfx/GfxViewManager.h"

namespace Hazel{

	// ���this����ͬһ�����������������������󶨵��Ǹ�������Ҳ����
	// EventFn/EventCallbackFn ���ˣ�EVentFn�������������������������һ��placeholder�Ǻ���
	// ��EventFn/EventCallbackFn����ʹ�õ� 
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
	
	Application* Application::s_Instance = nullptr;

	Application::Application(std::string title) : m_title(title)
	{
		HZ_CORE_ASSERT(!s_Instance, "Application already exists");
		s_Instance = this;
		WindowProps props;
		props.Title = title;
		m_Window = Scope<Window>(Window::Create(props));
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
		
		// ��ʼ��RenderAPI, ��ʵ������е����� 
		// ����ôд�ɣ�����ط�����Ƚϼ򵥣�����Ҫ�ڱ�ĵط��ܼ�ȡ��devices��
		//m_RenderAPIManager = Ref<RenderAPIManager>(RenderAPIManager::Create());
		RenderAPIManager::Register<D3D12RenderAPIManager>();
		RenderAPIManager::getInstance();
		CommandPool::getInstance()->Init();
		//GfxViewManager::getInstance()->Init();

		m_Window->SetBackGroundColor();
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}	
	
	void Application::Close()
	{
		m_Running = false;
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	// window ��event ��ȫ�����ˣ�ֻ��event�໥����
	void Application::OnEvent(Event& e)
	{
		
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(OnMouseButtonPressed));
		dispatcher.Dispatch<AppActiveEvent>(BIND_EVENT_FN(OnAppActiveStateChange));
		//HZ_CORE_INFO("{0}", e);

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{ 
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}
	
	void Application::Run()
	{

		while (true) 
		{
			m_Window->OnUpdate();
			//m_RenderAPIManager->OnUpdate();
			//float time = (float)glfwGetTime();		// platform gettime();
			//Timestep timestep = time - m_LastFrameTime;
			//m_LastFrameTime = time;

			if (!m_Minimized) 
			{
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(0.01f);
			}
			// ÿһ���������ImGui�㣬����Ⱦ��
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			//m_Window->OnUpdate();
		};
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) 
	{
		m_Running = false;
		return true;
	}

	bool Application::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		HZ_CORE_INFO("RBUTTTONDOWN event Catch!");
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (!e.GetHeight()|| !e.GetWidth()) 
		{
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;
		//Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		// todo: renderAPI��window resize
		// since we want all other layout know this event;
		return false;
	}

	bool Application::OnAppActiveStateChange(AppActiveEvent& e)
	{
		return true;
	}
}

