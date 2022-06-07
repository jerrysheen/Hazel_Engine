#pragma once

#include "Hazel.h"
#include "imgui/imgui.h"
#include "glm/gtc/type_ptr.hpp"

namespace Hazel 
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer(Window& window);
		virtual ~EditorLayer() = default;
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;

	private:
		glm::vec2 playerPosition = { 0.0f, 0.0f };
		float playerMoveSpeed = 1;
		Window& m_window;

	};
}

