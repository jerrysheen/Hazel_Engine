#include "hzpch.h"
#include "Mesh.h"



namespace Hazel 
{
	// todo: ?????mesh??????????????????????????????? ????????д???????
	// Mesh???????????????????? mesh??????? + layout?????
	Ref<Mesh> Mesh::Create()
	{
		return std::make_shared<Mesh>();
	}
	

	bool Mesh::LoadMesh(const std::string& path)
	{
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			HZ_WARN("ERROR::ASSIMP:: {0}", import.GetErrorString());
			return false;
		}
		
		processNode(scene->mRootNode, scene);

		// todo: accroding to meta file, fill vertex array
		std::string metaFilePath = path.substr(0, path.find_last_of('.')) + ".meta";
		FillVertexArray(metaFilePath);
		return true;
	}

	void Mesh::processNode(aiNode* node, const aiScene* scene)
	{
		// ??????????е?????????е????
		//HZ_CORE_INFO("{0}, {1}", node->mNumMeshes, node->mNumChildren);
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
			processMesh(aimesh, scene);
		}
		// ??????????????????????????
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	void Mesh::processMesh(aiMesh* aiMesh, const aiScene* scene)
	{
		//VertexArray
		// ??????????????? ????Vertex, Normal, TexCoords
		// todo?? ???????????????????????????е??????????????
		// ??????????????????? ??????????????????????????????? needposition??neednormal?????????
		// ??????????????????????????vertexdata array?? ???????????????Щ?????
		// ???????????????У? ??д?????????????????position normal??color???????
		HZ_CORE_INFO("vertices size :{0}", aiMesh->mNumVertices);
		for (unsigned int i = 0; i < aiMesh->mNumVertices; i++)
		{
			if (aiMesh->mVertices)
			{
				positionData.push_back(aiMesh->mVertices[i].x);
				positionData.push_back(aiMesh->mVertices[i].y);
				positionData.push_back(aiMesh->mVertices[i].z);
			}
			else
			{
				positionData.push_back(0);
				positionData.push_back(0);
				positionData.push_back(0);
			}

			if (aiMesh->mNormals) 
			{
				normalData.push_back(aiMesh->mNormals[i].x);
				normalData.push_back(aiMesh->mNormals[i].y);
				normalData.push_back(aiMesh->mNormals[i].z);
			}
			else
			{
				normalData.push_back(0);
				normalData.push_back(0);
				normalData.push_back(0);
			}

			if (aiMesh->mTangents)
			{
				tangentData.push_back(aiMesh->mTangents[i].x);
				tangentData.push_back(aiMesh->mTangents[i].y);
				tangentData.push_back(aiMesh->mTangents[i].z);
			}
			else
			{
				tangentData.push_back(0);
				tangentData.push_back(0);
				tangentData.push_back(0);
			}


			if (aiMesh->mTextureCoords[0]) // ?????????????????
			{
				texCoord0Data.push_back(aiMesh->mTextureCoords[0][i].x);
				texCoord0Data.push_back(aiMesh->mTextureCoords[0][i].y);
			}
			else
			{
				texCoord0Data.push_back(0);
				texCoord0Data.push_back(0);
			}

			if (aiMesh->mTextureCoords[1]) // ?????????????????
			{
				texCoord1Data.push_back(aiMesh->mTextureCoords[1][i].x);
				texCoord1Data.push_back(aiMesh->mTextureCoords[1][i].y);
			}
			else
			{
				texCoord1Data.push_back(0);
				texCoord1Data.push_back(0);
			}

			if (aiMesh->mColors[0])
			{
				vertexColorData.push_back(aiMesh->mColors[0][i].r);
				vertexColorData.push_back(aiMesh->mColors[0][i].g);
				vertexColorData.push_back(aiMesh->mColors[0][i].b);
				vertexColorData.push_back(aiMesh->mColors[0][i].a);
			}
			else
			{
				vertexColorData.push_back(0);
				vertexColorData.push_back(0);
				vertexColorData.push_back(0);
				vertexColorData.push_back(0);
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

	//Summary: Fill vertex array according to meta file
	void Mesh::FillVertexArray(const std::string& metaFilePath)
	{
		// todo load meta file config. It will help us decide which data we need to load.


		// add default data position, normal, tangent, texcoord0, texcoord1, color
		// why we need to seperate all data to different slot?
		// because later we will add data according to shader input layout.

		if(needPosition)
		{
			Ref<VertexBuffer> vertexBuffer;
			float* p = &positionData[0];
			vertexBuffer = (VertexBuffer::Create(p, positionData.size() * sizeof(float), 3 * sizeof(float)));
			meshData->AddVertexBuffer(VertexProperty::Position, vertexBuffer);
		}

		if(needNormal)
		{
			Ref<VertexBuffer> normalBuffer;
			float* p = &normalData[0];
			normalBuffer = (VertexBuffer::Create(p, normalData.size() * sizeof(float), 3 * sizeof(float)));
			//meshData->AddVertexBuffer(vertexBuffer);
			meshData->AddVertexBuffer(VertexProperty::Normal, normalBuffer);
		}

		if(needTangent)
		{
			Ref<VertexBuffer> tangentBuffer;
			float* p = &tangentData[0];
			tangentBuffer = (VertexBuffer::Create(p, tangentData.size() * sizeof(float), 3 * sizeof(float)));
			meshData->AddVertexBuffer(VertexProperty::Tangent, tangentBuffer);
		}

		if(needTexCoord0)
		{
			Ref<VertexBuffer> texCoord0Buffer;
			float* p = &texCoord0Data[0];
			texCoord0Buffer = (VertexBuffer::Create(p, texCoord0Data.size() * sizeof(float), 2 * sizeof(float)));
			meshData->AddVertexBuffer(VertexProperty::TexCoord0, texCoord0Buffer);
		}

		if(needTexCoord1)
		{
			Ref<VertexBuffer> texCoord1Buffer;
			float* p = &texCoord1Data[0];
			texCoord1Buffer = (VertexBuffer::Create(p, texCoord1Data.size() * sizeof(float), 2 * sizeof(float)));
			meshData->AddVertexBuffer(VertexProperty::TexCoord1, texCoord1Buffer);
		}

		if(needVertexColor)
		{
			Ref<VertexBuffer> vertexColorBuffer;
			float* p = &vertexColorData[0];
			vertexColorBuffer = (VertexBuffer::Create(p, vertexColorData.size() * sizeof(float), 4 * sizeof(float)));
			meshData->AddVertexBuffer(VertexProperty::VertexColor, vertexColorBuffer);
		}

		//HZ_CORE_INFO("indexBuffer size : {0}", indexBuffer.size());
		Ref<IndexBuffer> indexBuffer;
		uint16_t* indexP = &indexData[0];
		//squareIB.reset(IndexBuffer::Create(indexP, sizeof(indexBuffer) / sizeof(uint32_t)));
		indexBuffer = (IndexBuffer::Create(indexP, indexData.size()));
		meshData->SetIndexBuffer(indexBuffer);
	}


}