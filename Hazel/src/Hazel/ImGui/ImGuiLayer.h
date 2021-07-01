#pragma once

#include "Hazel/Layer.h"


namespace Hazel {

	class  HAZEL_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		// pushed to layer stack
		void OnAttach();
		// pop...
		void OnDetach();
		// updated, every frame
		void OnUpdate();
		// event get sent to layer
		void OnEvent(Event& event);

	private:
		float m_Time = 0.0f;
	};

}
