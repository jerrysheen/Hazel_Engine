#pragma once

#include "hzpch.h"
#include "glm/glm.hpp"
#include "../Renderer/Texture.h"
#include "../Renderer/Shader.h"
#include "../Renderer/VertexArray.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

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
        Mesh() = default;
        void Draw(Shader shader);

        static Ref<Mesh> Create();
        //Ref<VertexArray> VertexArray;
        Ref<Texture2D> Texture;

        bool LoadMesh(const std::string& path);
        Ref<VertexArray> meshData;

    private:
        std::vector<float> vertexBuffer;
        std::vector<uint32_t> indexBuffer;
        //void LoadModel(const std::string& path);
        void processNode(aiNode* node, const aiScene* scene);

        void processMesh(aiMesh* mesh, const aiScene* scene);

    };
}
