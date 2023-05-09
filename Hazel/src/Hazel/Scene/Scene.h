#pragma once

#include "entt.hpp"
#include "Component.h"
namespace Hazel {
	
	class Entity;

	class Scene 
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = "");
		void OnUpdate(float ts);


		// TEMP
		entt::registry& Reg() { return m_Registry; }
	private:
		entt::registry m_Registry;

		friend class Entity;
	};
}