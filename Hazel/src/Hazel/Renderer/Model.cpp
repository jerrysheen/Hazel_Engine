#include "hzpch.h"
#include "Model.h"


namespace Hazel 
{
	Model::Model(char* path) 
	{
		LoadModel(path);
	}

	void Model::LoadModel(std::string path)
	{
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			HZ_WARN("ERROR::ASSIMP:: {0}", import.GetErrorString());
			return;
		}
		directory = path.substr(0, path.find_last_of('/'));
	}
}