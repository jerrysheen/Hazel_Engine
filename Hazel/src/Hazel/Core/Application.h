#pragma once

#include "Hazel/Core/Core.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"

#include "Window.h"
#include "Hazel/Graphics/RenderAPIManager.h"
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
		inline void SetApplicationRunning(const bool& run) { m_Running = run; }
		inline void SetApplicationMinimized(const bool& isMinimized) { m_Minimized = isMinimized; }
		inline void SetApplicationMaximized(const bool& isMaximized) { m_Maximized = isMaximized; }
		inline int GetWindowWidth() { return m_WindowWidth; }
		inline void SetWindowWidth(const int& windowWidth) { m_WindowWidth = windowWidth; }
		inline int GetWindowHeight() { return m_WindowHeight; }
		inline void SetWindowHeight(const int& windowHeight) { m_WindowHeight = windowHeight; }
		inline Ref<RenderAPIManager> GetRenderAPIManager() { return m_RenderAPIManager; }

		inline Window& GetWindow() { return *m_Window; }
		std::string m_title;
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnAppActiveStateChange(AppActiveEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
	private:
		Scope<Window> m_Window;
		Ref<RenderAPIManager> m_RenderAPIManager;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		bool m_Maximized = true;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
		int m_WindowWidth = 0;
		int m_WindowHeight = 0;
	private:
		static Application* s_Instance;


		
	};		 

	// To be defined in Client
	Application* CreateApplication();
}

