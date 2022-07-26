#pragma once

#include "hzpch.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "../Renderer/Shader.h"
#include "../Renderer/Renderer3D.h"
#include "Mesh.h"
#include "glm/gtc/matrix_transform.hpp"


namespace Hazel 
{
    class Model
    {
    public:
        Model(const std::string& path);
        void Draw(Shader& shader);
        Ref<glm::mat4> GetModelMatrix() { return std::make_shared<glm::mat4>((*translate) * (*rotate) * (*scale)); };
    public:
        // model data
        Ref<VertexArray> mesh;
        Ref<Shader> shader;
        Ref<Texture2D> baseMap;
        Ref<Texture2D> bumpMap;
        Ref<glm::vec4> color;
        Renderer3D::DRAW_TYPE drawType;
        
        Ref<glm::mat4> modelMatrix;
        Ref<glm::mat4> translate;
        Ref<glm::mat4> rotate;
        Ref<glm::mat4> scale;

    private:
        std::vector<float> vertexBuffer;
        std::vector<uint32_t> indexBuffer;
        void LoadModel(const std::string& path);
        void processNode(aiNode* node, const aiScene* scene);

        void processMesh(aiMesh* mesh, const aiScene* scene);

        std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
            std::string typeName);
    };
}