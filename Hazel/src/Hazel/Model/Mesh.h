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
        Mesh() 
        {
            meshData = VertexArray::Create();;
        };
        void Draw(Shader shader);

        static Ref<Mesh> Create();

        bool LoadMesh(const std::string& path);
        Ref<VertexArray> meshData;

    private:
        bool hasPosition = false;
        bool hasNormal = false;
        bool hastexCoord0 = false;
        bool hasTangent = false;
        bool hastexCoord1 = false;
        bool hasVertexColor = false;
        std::vector<float> vertexData;

        std::vector<uint16_t> indexData;
        //void LoadModel(const std::string& path);
        void processNode(aiNode* node, const aiScene* scene);

        void processMesh(aiMesh* mesh, const aiScene* scene);

    };
}
