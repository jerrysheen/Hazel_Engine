#include "hzpch.h"
#include "Model.h"


namespace Hazel 
{
	Model::Model(const std::string&  path) 
	{
		mesh = VertexArray::Create();
		LoadModel(path);
	}

	void Model::LoadModel(const std::string& path)
	{
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			HZ_WARN("ERROR::ASSIMP:: {0}", import.GetErrorString());
			return;
		}
		//directory = path.substr(0, path.find_last_of('/'));

		processNode(scene->mRootNode, scene);


		Ref<VertexBuffer> squareVB;
		float* p = &vertexBuffer[0];
		HZ_CORE_INFO("{0}", vertexBuffer.size());
		squareVB.reset(VertexBuffer::Create(p, vertexBuffer.size()));
		squareVB->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float3, "a_Normal" },
				{ ShaderDataType::Float2, "a_TexCoord" }

			});
		mesh->AddVertexBuffer(squareVB);



		Ref<IndexBuffer> squareIB;
		uint32_t* indexP = &indexBuffer[0];
		//squareIB.reset(IndexBuffer::Create(indexP, sizeof(indexBuffer) / sizeof(uint32_t)));
		squareIB.reset(IndexBuffer::Create(indexP, indexBuffer.size()));
		mesh->SetIndexBuffer(squareIB);
	}

	void Model::processNode(aiNode* node, const aiScene* scene)
	{
		// �����ڵ����е���������еĻ���
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
			processMesh(aimesh, scene);
		}
		// �������������ӽڵ��ظ���һ����
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	void Model::processMesh(aiMesh* aiMesh, const aiScene* scene)
	{
		//VertexArray
		// ��������ÿһ���ڵ㣬 ����Vertex, Normal, TexCoords
		// add vertexbuffer to mesh.

		
		for (unsigned int i = 0; i < aiMesh->mNumVertices; i++)
		{
			vertexBuffer.push_back(aiMesh->mVertices[i].x);
			vertexBuffer.push_back(aiMesh->mVertices[i].y);
			vertexBuffer.push_back(aiMesh->mVertices[i].z);
			vertexBuffer.push_back(aiMesh->mNormals[i].x);
			vertexBuffer.push_back(aiMesh->mNormals[i].y);
			vertexBuffer.push_back(aiMesh->mNormals[i].z);
			if (aiMesh->mTextureCoords[0]) // �����Ƿ����������ꣿ
			{
				vertexBuffer.push_back(aiMesh->mTextureCoords[0][i].x);
				vertexBuffer.push_back(aiMesh->mTextureCoords[0][i].y);
			}
			else 
			{
				vertexBuffer.push_back(0.0f);
				vertexBuffer.push_back(0.0f);
			}
		}


		//int step = indexBuffer.size()
		for (unsigned int i = 0; i < aiMesh->mNumFaces; i++)
		{
			aiFace face = aiMesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indexBuffer.push_back(face.mIndices[j]);
			}
		}


	
	}
}