#include "hzpch.h"
#include "Mesh.h"



namespace Hazel 
{
	// todo: 导入的mesh相关通道肯定是要根据用户设置取调整的， 这个地方先写死了。。
	// Mesh需要做具体的接口吗？不需要， mesh只是数据 + layout而已。
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
		const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			HZ_WARN("ERROR::ASSIMP:: {0}", import.GetErrorString());
			return false;
		}
		//directory = path.substr(0, path.find_last_of('/'));
		
		processNode(scene->mRootNode, scene);

		Ref<VertexBuffer> vertexBuffer;
		float* p = &vertexData[0];
		vertexBuffer = (VertexBuffer::Create(p, vertexData.size() * sizeof(float)));
		// layout应该设置在VertexArray中。
		vertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "POSITION" },
				{ ShaderDataType::Float3, "NORMAL" },
				{ ShaderDataType::Float3, "TANGENT" },
				{ ShaderDataType::Float2, "TEXCOORD" },
				{ ShaderDataType::Float2, "TEXCOORD" },
				{ ShaderDataType::Float4, "COLOR" }

			});
		meshData->AddVertexBuffer(vertexBuffer);


		//HZ_CORE_INFO("indexBuffer size : {0}", indexBuffer.size());
		Ref<IndexBuffer> indexBuffer;
		uint16_t* indexP = &indexData[0];
		//squareIB.reset(IndexBuffer::Create(indexP, sizeof(indexBuffer) / sizeof(uint32_t)));
		indexBuffer = (IndexBuffer::Create(indexP, indexData.size()));
		meshData->SetIndexBuffer(indexBuffer);
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
		// 这个地方不应该是这样， 而是有一个默认的需要哪几个通道的参数， needposition，neednormal之类的东西。
		// 但是最后的结果需要结合在一起变成一个vertexdata array， 只是记录下当前用了哪些通道。
		// 这个地方为了方便运行， 先写成全量加载，加载全部position normal，color等数据。
		HZ_CORE_INFO("vertices size :{0}", aiMesh->mNumVertices);
		for (unsigned int i = 0; i < aiMesh->mNumVertices; i++)
		{
			if (aiMesh->mVertices)
			{
				hasPosition = true;
				vertexData.push_back(aiMesh->mVertices[i].x);
				vertexData.push_back(aiMesh->mVertices[i].y);
				vertexData.push_back(aiMesh->mVertices[i].z);
			}
			else
			{
				hasPosition = false;
				vertexData.push_back(0);
				vertexData.push_back(0);
				vertexData.push_back(0);
			}

			if (aiMesh->mNormals) 
			{
				hasNormal = true;
				vertexData.push_back(aiMesh->mNormals[i].x);
				vertexData.push_back(aiMesh->mNormals[i].y);
				vertexData.push_back(aiMesh->mNormals[i].z);
			}
			else
			{
				hasNormal = false;
				vertexData.push_back(0);
				vertexData.push_back(0);
				vertexData.push_back(0);
			}

			if (aiMesh->mTangents)
			{
				hasTangent = true;
				vertexData.push_back(aiMesh->mTangents[i].x);
				vertexData.push_back(aiMesh->mTangents[i].y);
				vertexData.push_back(aiMesh->mTangents[i].z);
			}
			else
			{
				hasTangent = false;
				vertexData.push_back(0);
				vertexData.push_back(0);
				vertexData.push_back(0);
			}


			if (aiMesh->mTextureCoords[0]) // 网格是否有纹理坐标？
			{
				hastexCoord0 = true;
				vertexData.push_back(aiMesh->mTextureCoords[0][i].x);
				vertexData.push_back(aiMesh->mTextureCoords[0][i].y);
			}
			else
			{
				hastexCoord0 = false;
				vertexData.push_back(0);
				vertexData.push_back(0);
			}

			if (aiMesh->mTextureCoords[1]) // 网格是否有纹理坐标？
			{
				hastexCoord1 = true;
				vertexData.push_back(aiMesh->mTextureCoords[1][i].x);
				vertexData.push_back(aiMesh->mTextureCoords[1][i].y);
			}
			else
			{
				hastexCoord1 = false;
				vertexData.push_back(0);
				vertexData.push_back(0);
			}

			if (aiMesh->mColors[0])
			{
				hasVertexColor = true;
				vertexData.push_back(aiMesh->mColors[0][i].r);
				vertexData.push_back(aiMesh->mColors[0][i].g);
				vertexData.push_back(aiMesh->mColors[0][i].b);
				vertexData.push_back(aiMesh->mColors[0][i].a);
			}
			else
			{
				hasVertexColor = false;
				vertexData.push_back(0);
				vertexData.push_back(0);
				vertexData.push_back(0);
				vertexData.push_back(0);
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