#include "hzpch.h"
#include "Mesh.h"



namespace Hazel 
{
	// todo: �����mesh���ͨ���϶���Ҫ�����û�����ȡ�����ģ� ����ط���д���ˡ���
	// Mesh��Ҫ������Ľӿ��𣿲���Ҫ�� meshֻ������ + layout���ѡ�
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
		// 目前stride 写死， 后续需要根据layout来计算
		bufferStride = 17 * sizeof(float);
		vertexBuffer = (VertexBuffer::Create(p, vertexData.size() * sizeof(float), bufferStride));
		meshData->SetLayout({
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
		// �����ڵ����е���������еĻ���
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
		// todo�� ����ط������������⣬�����ͨ���е�̫��һ�ˣ���̫�ԡ�
		// ����ط���Ӧ���������� ������һ��Ĭ�ϵ���Ҫ�ļ���ͨ���Ĳ����� needposition��neednormal֮��Ķ�����
		// �������Ľ����Ҫ�����һ����һ��vertexdata array�� ֻ�Ǽ�¼�µ�ǰ������Щͨ����
		// ����ط�Ϊ�˷������У� ��д��ȫ�����أ�����ȫ��position normal��color�����ݡ�
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


			if (aiMesh->mTextureCoords[0]) // �����Ƿ����������ꣿ
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

			if (aiMesh->mTextureCoords[1]) // �����Ƿ����������ꣿ
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