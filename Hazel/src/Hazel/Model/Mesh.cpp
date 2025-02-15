#include "hzpch.h"
#include "Mesh.h"



namespace Hazel 
{
	// todo: �����mesh���ͨ���϶���Ҫ�����û�����ȡ�����ģ� ����ط���д���ˡ���
	// Mesh��Ҫ������Ľӿ���
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
		// ����ڵ����е���������еĻ���
		//HZ_CORE_INFO("{0}, {1}", node->mNumMeshes, node->mNumChildren);
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

	void Mesh::processMesh(aiMesh* aiMesh, const aiScene* scene)
	{
		//VertexArray
		// ��������ÿһ���ڵ㣬 ����Vertex, Normal, TexCoords
		// todo�� ����ط�����������⣬�����ͨ���е�̫��һ�ˣ���̫�ԡ�
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

			if (aiMesh->mTextureCoords[0]) // �����Ƿ����������ꣿ
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