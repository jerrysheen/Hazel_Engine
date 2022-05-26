#pragma once
#include "hzpch.h"
#include "glm/glm.hpp"
#include "Texture.h"
#include "Shader.h"
namespace Hazel 
{
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };
    class Mesh
    {
    public:
        std::vector<Vertex> vertices;
        std::vector<int> indices;
        //std::vector<Texture> textures;
        Mesh(const std::vector<Vertex>& vertices, const std::vector<int>& indices, const std::vector<Texture>& texture);
        void Draw(Shader shader);

        static Ref<Mesh> Create(const std::vector<Vertex>& vertices, const std::vector<int>& indices, const std::vector<Texture>& texture);

    private:
        int VAO, VBO, EBO;
        void setupMesh();
    };
}
