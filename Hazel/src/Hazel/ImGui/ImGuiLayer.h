#pragma once

#include "Hazel/Core/Layer.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"

namespace Hazel {

	class  HAZEL_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		// pushed to layer stack
		virtual void OnAttach();
		// pop...
		virtual void OnDetach();
		// updated, every frame
		virtual void OnImGuiRender();
		// event get sent to layer
		void Begin();
		void End();

		void SetBlockEvents(bool block) { m_BlockEvents = block; };
	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;
		void* m_RenderAPIManager;

		int NUM_FRAMES_IN_FLIGHT = 3;
	};

}
