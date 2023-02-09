#pragma once
#include <glm/glm.hpp>
#include <Hazel/Model/Mesh.h>
#include <Hazel/Model/Material.h>
namespace Hazel 
{
	struct TransformComponent 
	{
		glm::vec3 position;
	};


	struct MeshFilterComponent
	{
		Mesh* mesh;
	};

	struct MeshRenderer 
	{
		std::vector<Material*> Materials;
	};


}
