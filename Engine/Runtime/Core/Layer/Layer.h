#pragma once

#include "Runtime/Core/Core.h"
#include "Runtime/Core/Events/Event.h"
#include "Runtime/Core/Time/Timestep.h"
namespace Hazel {

	class HAZEL_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();
		
		// pushed to layer stack
		virtual void OnAttach() {}
		// pop...
		virtual void OnDetach() {}

		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}


		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

}