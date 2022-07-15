#pragma once

#include "hzpch.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "../Renderer/Shader.h"
#include "Mesh.h"

namespace Hazel 
{
    class Model
    {
    public:
        Model(const std::string& path);
        void Draw(Shader& shader);
    private:
        // model data
        std::vector<Mesh> meshes;
        std::string directory;

        void LoadModel(const std::string& path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
            std::string typeName);
    };
}