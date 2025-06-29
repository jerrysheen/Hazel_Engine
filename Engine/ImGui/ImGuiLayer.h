#pragma once

#include "Runtime/Core/Layer.h"

#include "Runtime/Core/Events/ApplicationEvent.h"
#include "Runtime/Core/Events/KeyEvent.h"
#include "Runtime/Core/Events/MouseEvent.h"

#ifdef RENDER_API_OPENGL

#elif RENDER_API_DIRECTX12

#include "platform/D3D12/D3D12RenderAPIManager.h"
#endif

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

#ifdef RENDER_API_OPENGL
#elif RENDER_API_DIRECTX12
		D3D12RenderAPIManager* renderAPIManager;
#endif
	};

}
