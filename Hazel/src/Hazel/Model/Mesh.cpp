#include "hzpch.h"
#include "Mesh.h"

namespace Hazel 
{

	Ref<Mesh> Mesh::Create()
	{
		return std::make_shared<Mesh>();
	}
	

	bool Mesh::SetupMesh()
	{
		return true;
	}


}