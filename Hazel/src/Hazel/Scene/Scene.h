#pragma once

#include "entt.hpp"
#include "Component.h"
namespace Hazel {
	
	class Scene 
	{
	public:
		Scene();
		~Scene();

		entt::entity CreateEntity();
		void OnUpdate(float ts);


		// TEMP
		entt::registry& Reg() { return m_Registry; }
	private:
		entt::registry m_Registry;
	};
}