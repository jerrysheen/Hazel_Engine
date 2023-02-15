#pragma once

#include "hzpch.h"
#include <glm/glm.hpp>
#include "Hazel/Model/Mesh.h"
#include "Hazel/Model/Material.h"

namespace HAZEL {
	
	struct TransformComponent 
	{
		glm::mat4 Transform{1.0};
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4 & transform)
			: Transform(transform) {}

		operator glm::mat4& () { return Transform; }
		operator const glm::mat4& () { return Transform; }
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
		Hazel::Mesh* mesh;
		MeshFilterComponent(const std::string& meshAddress)
		{
			mesh->Create();
			mesh->LoadMesh(meshAddress);
		}
	};

	struct MeshRendererComponent
	{
		std::vector<Hazel::Material> Materials;
	};
}