#pragma once

#include "../Renderer/Shader.h"
#include "hzpch.h"
#include "assimp/Importer.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Hazel 
{
    class Model
    {
    public:
        /*  ��Ա����   */
        Model(char* path)
        {
            this->loadModel(path);
        }
        void Draw(Shader shader);
    private:
        /*  ģ������  */
        //std::vector<Mesh> meshes;
        std::string directory;

        /*  ˽�г�Ա����   */
        void loadModel(std::string path);
        void processNode(aiNode* node, const aiScene* scene);
        //Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        //vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
    };
}