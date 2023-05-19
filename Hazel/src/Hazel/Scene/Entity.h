#pragma once

#include "Scene.h"
#include "entt.hpp"
namespace Hazel 
{
	class Entity 
	{
	public: 
		//Entity() = default;
		Entity(entt::entity handle, Scene* scnee);
		Entity(const Entity& other) = default;
		template<typename T>
		bool HasComponent() 
		{
			// has already change to all_of
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			HZ_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		T& RemoveComponent()
		{
			HZ_CORE_ASSERT(!HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		operator bool() const { return m_EntityHandle != entt::null; }

	private:
		entt::entity m_EntityHandle{ entt::null};
		Scene* m_Scene = nullptr;
	};

}  