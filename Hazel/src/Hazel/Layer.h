#pragma once

#include "Hazel/Core.h"
#include "Hazel/Events/Event.h"

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
		// updated, every frame
		virtual void OnUpdate() {}
		// event get sent to layer
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

}