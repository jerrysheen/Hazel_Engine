#pragma once

#include "Hazel/Core/Core.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"

#include "Window.h"
#include "Hazel/Core/LayerStack.h"

#include "Hazel/ImGui/ImGuiLayer.h"

#include "Hazel/Core/Timestep.h"


namespace Hazel 
{

	class HAZEL_API Application
	{
	public:
		Application(std::string title);
		virtual ~Application();

		void Run();
	
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void Close();

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
		std::string m_title;
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;


		
	};		 

	// To be defined in Client
	Application* CreateApplication();
}

