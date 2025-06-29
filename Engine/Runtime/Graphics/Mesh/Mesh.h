#pragma once

#include "hzpch.h"
#include "glm/glm.hpp"
#include "Runtime/Graphics/Renderer/Texture.h"
#include "Runtime/Graphics/Renderer/Shader.h"
#include "Runtime/Graphics/Renderer/VertexArray.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

namespace Hazel 
{

    class Mesh
    {
    public:
        Mesh() 
        {
            meshData = VertexArray::Create();;
        };

        static Ref<Mesh> Create();
        bool LoadMesh(const std::string& path);
        Ref<VertexArray> meshData;
    private:
        bool needPosition = true;
        bool needNormal = true;
        bool needTangent = true;
        bool needTexCoord0 = true;
        bool needTexCoord1 = true;
        bool needVertexColor = true;
        std::vector<float> positionData;
        std::vector<float> normalData;
        std::vector<float> texCoord0Data;
        std::vector<float> texCoord1Data;
        std::vector<float> tangentData;
        std::vector<float> vertexColorData;

		uint32_t bufferStride = 0;
        void FillVertexArray(const std::string& metaFilePath);
        std::vector<uint16_t> indexData;
        void processNode(aiNode* node, const aiScene* scene);
        void processMesh(aiMesh* mesh, const aiScene* scene);

    };
}
