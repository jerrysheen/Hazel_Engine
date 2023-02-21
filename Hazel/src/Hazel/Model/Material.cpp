#include "hzpch.h"
#include "Material.h"

namespace Hazel 
{
	Ref<Material> Material::Create()
	{
		return std::make_shared<Material>();
	}

}