#include "Mesh.h"

namespace Hazel 
{
	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<int>& indices, const std::vector<Texture>& texture)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		this->setupMesh();
	}
	Ref<Mesh> Mesh::Create(const std::vector<Vertex>& vertices, const std::vector<int>& indices, const std::vector<Texture>& texture)
	{

		return Ref<Mesh>();
	}
}