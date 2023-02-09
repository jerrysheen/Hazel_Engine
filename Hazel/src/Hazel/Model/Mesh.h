#pragma once

#include "hzpch.h"
#include "glm/glm.hpp"
#include "../Renderer/Texture.h"
#include "../Renderer/Shader.h"
#include "../Renderer/VertexArray.h"

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
        
        //std::vector<Texture> textures;
        //Mesh(const std::vector<Vertex>& vertices, const std::vector<int>& indices, const std::vector<Texture>& texture);
        void Draw(Shader shader);

        static Ref<Mesh> Create();
        Ref<VertexArray> VertexArray;
        Ref<Texture2D> Texture;

        bool SetupMesh();

    private:
        int VAO, VBO, EBO;
    };
}
