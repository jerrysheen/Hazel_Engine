#include "hzpch.h"
#include "Scene.h"
#include "Entity.h"

#include <glm/glm.hpp>
namespace Hazel {


	static void OnTransformConstruct() 
	{
	}

	Scene::Scene() 
	{
		//struct MeshComponent 
		//{
		//	float value;
		//	MeshComponent() = default;
		//};
		//struct TransformComponent 
		//{
		//	glm::mat4 Transform;
		//	TransformComponent() = default;
		//	TransformComponent(const TransformComponent&) = default;
		//	TransformComponent(const glm::mat4& transform)
		//		: Transform(transform) {}

		//	operator glm::mat4& () { return Transform; }
		//	operator const glm::mat4& () { return Transform; }
		//};
		
		entt::entity entity = m_Registry.create();

		m_Registry.emplace<HAZEL::TransformComponent>(entity, glm::mat4(1.0f));

		//  entity.GetComponent<>();
		//if(m_Registry.has<TransformComponent>(entity))
		HAZEL::TransformComponent& transform = m_Registry.get<HAZEL::TransformComponent>(entity);

		// get all entity's that has mesh component
		auto view = m_Registry.view<HAZEL::TransformComponent>();
		for (auto entity : view) 
		{
			// can directly do your job inside view
			HAZEL::TransformComponent& transform = view.get<HAZEL::TransformComponent>(entity);
		}

		// Group two component <Mesh & Transform>
		// But don't know why group.get not working
		auto group = m_Registry.group<HAZEL::TransformComponent>(entt::get<HAZEL::SpriteRendererComponent>);
		for (auto entity : group) 
		{
			auto& [a, b] = m_Registry.get<HAZEL::TransformComponent, HAZEL::SpriteRendererComponent>(entity);
			 
			//Rrenderer::submit(mesh, transform);
		}

		// add a callback to Transform constructor. when every time an TransformComponent added
		m_Registry.on_construct<HAZEL::TransformComponent>().connect<&OnTransformConstruct>();
	}


	Scene::~Scene() 
	{
	}

	void Scene::OnUpdate(float ts) 
	{
		//HZ_CORE_INFO("{0} test test test");
	}	
	
	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(),this };
		entity.AddComponent<HAZEL::TransformComponent>();
		auto& tag = entity.AddComponent<HAZEL::TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}
}