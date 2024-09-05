#pragma once

#include "Hazel.h"
#include "imgui/imgui.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Panels/SceneHierarchyPanel.h"

namespace Hazel
{
	class SceneViewLayer : public Layer
	{
	public:
		SceneViewLayer(Window& window);
		virtual ~SceneViewLayer() = default;
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;

	private:
		Window& m_window;
	};
}

