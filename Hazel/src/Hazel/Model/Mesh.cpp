#include "hzpch.h"
#include "Mesh.h"



namespace Hazel 
{
	// todo: 导入的mesh相关通道肯定是要根据用户设置取调整的， 这个地方先写死了。。
	// Mesh需要做具体的接口吗？
	Ref<Mesh> Mesh::Create()
	{
		
		return std::make_shared<Mesh>();
	}
	

	bool Mesh::LoadMesh(const std::string& path)
	{
		//std::vector<float> vertexBuffer;
		//vertexBuffer.push_back(0.0);
		//std::vector<uint32_t> indexBuffer;
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			HZ_WARN("ERROR::ASSIMP:: {0}", import.GetErrorString());
			return false;
		}
		//directory = path.substr(0, path.find_last_of('/'));
		
		processNode(scene->mRootNode, scene);


		Ref<VertexBuffer> squareVB;
		float* p = &vertexData[0];
		//HZ_CORE_INFO("vertexBuffer size : {0}", vertexBuffer.size());
		squareVB.reset(VertexBuffer::Create(p, vertexData.size() * sizeof(float)));
		squareVB->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float3, "a_Normal" },
				{ ShaderDataType::Float3, "a_Tangent" },
				{ ShaderDataType::Float2, "a_TexCoord" }

			});
		meshData->AddVertexBuffer(squareVB);


		//HZ_CORE_INFO("indexBuffer size : {0}", indexBuffer.size());
		Ref<IndexBuffer> squareIB;
		uint32_t* indexP = &indexData[0];
		//squareIB.reset(IndexBuffer::Create(indexP, sizeof(indexBuffer) / sizeof(uint32_t)));
		squareIB.reset(IndexBuffer::Create(indexP, indexData.size()));
		meshData->SetIndexBuffer(squareIB);
		return true;
	}

	void Mesh::processNode(aiNode* node, const aiScene* scene)
	{
		// 处理节点所有的网格（如果有的话）
		//HZ_CORE_INFO("{0}, {1}", node->mNumMeshes, node->mNumChildren);
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
			processMesh(aimesh, scene);
		}
		// 接下来对它的子节点重复这一过程
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	void Mesh::processMesh(aiMesh* aiMesh, const aiScene* scene)
	{
		//VertexArray
		// 对于里面每一个节点， 都有Vertex, Normal, TexCoords
		// todo： 这个地方或许会有问题，导入的通道有点太单一了，不太对。
		// add vertexbuffer to mesh.
		HZ_CORE_INFO("vertices size :{0}", aiMesh->mNumVertices);
		for (unsigned int i = 0; i < aiMesh->mNumVertices; i++)
		{
			vertexData.push_back(aiMesh->mVertices[i].x);
			vertexData.push_back(aiMesh->mVertices[i].y);
			vertexData.push_back(aiMesh->mVertices[i].z);
			vertexData.push_back(aiMesh->mNormals[i].x);
			vertexData.push_back(aiMesh->mNormals[i].y);
			vertexData.push_back(aiMesh->mNormals[i].z);
			vertexData.push_back(aiMesh->mTangents[i].x);
			vertexData.push_back(aiMesh->mTangents[i].y);
			vertexData.push_back(aiMesh->mTangents[i].z);

			if (aiMesh->mTextureCoords[0]) // 网格是否有纹理坐标？
			{
				vertexData.push_back(aiMesh->mTextureCoords[0][i].x);
				vertexData.push_back(aiMesh->mTextureCoords[0][i].y);
			}
			else
			{
				vertexData.push_back(0.0f);
				vertexData.push_back(0.0f);
			}
		}


		//int step = indexBuffer.size()
		for (unsigned int i = 0; i < aiMesh->mNumFaces; i++)
		{
			aiFace face = aiMesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indexData.push_back(face.mIndices[j]);
			}
		}



	}


}