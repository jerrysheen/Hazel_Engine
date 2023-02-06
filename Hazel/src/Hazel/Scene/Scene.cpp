#include "hzpch.h"
#include "Scene.h"

#include <glm/glm.hpp>
namespace Hazel {


	static void OnTransformConstruct() 
	{
	}

	Scene::Scene() 
	{
		struct MeshComponent {};
		struct TransformComponent 
		{
			glm::mat4 Transform;
			TransformComponent() = default;
			TransformComponent(const TransformComponent&) = default;
			TransformComponent(const glm::mat4& transform)
				: Transform(transform) {}

			operator glm::mat4& () { return Transform; }
			operator const glm::mat4& () { return Transform; }
		};
		
		entt::entity entity = m_Registry.create();

		m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));

		//  entity.GetComponent<>();
		//if(m_Registry.has<TransformComponent>(entity))
		TransformComponent& transform = m_Registry.get<TransformComponent>(entity);

		// get all entity's that has mesh component
		auto view = m_Registry.view<TransformComponent>();
		for (auto entity : view) 
		{
			// can directly do your job inside view
			TransformComponent& transform = view.get<TransformComponent>(entity);
		}

		// Group two component <Mesh & Transform>
		// But don't know why group.get not working
		auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
		for (auto entity : group) 
		{
			auto&[a, b] = m_Registry.get<TransformComponent, MeshComponent>(entity);

			//Rrenderer::submit(mesh, transform);
		}

		// add a callback to Transform constructor. when every time an TransformComponent added
		m_Registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>();
	}


	Scene::~Scene() 
	{
	}
}