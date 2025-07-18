#pragma once

#include "hzpch.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Runtime/Graphics/Mesh/Mesh.h"
#include <Runtime/Graphics/Material/Material.h>
//#include <Hazel/Model/Model.h>
#include <Runtime/Graphics/Camera/Camera.h>


namespace Hazel {

	struct TransformComponent 
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.x), { 1, 0, 0 })
				* glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.y), { 0, 1, 0 })
				* glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.z), { 0, 0, 1 });

			return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{1.0f, 1.0f, 1.0f, 1.0f};
		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) {} 
	};

	//struct TransformComponent
	//{
	//	glm::vec3 position;
	//};


	struct MeshFilterComponent
	{
		Ref<Mesh> mesh;
		MeshFilterComponent(const std::string& meshAddress)
		{
			mesh = Mesh::Create();
			//mesh->Create();
			mesh->LoadMesh(meshAddress);
		}
	};

	struct MeshRendererComponent
	{
		Ref<Material> material;

		//MeshRendererComponent()
		//{
		//	material = Material::Create();
		//}
	};


	struct TagComponent
	{
		std::string Tag;
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	
	};

	struct CameraComponent 
	{
		Camera camera;
	};


}