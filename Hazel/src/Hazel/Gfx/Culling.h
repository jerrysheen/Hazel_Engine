#pragma once
#include "Hazel/Renderer/Camera.h"
#include "Hazel/Scene/Scene.h"
#include "Hazel/Gfx/RenderStruct.h"
namespace Hazel 
{
	class Culling 
	{
	public:
		static RenderNode* Cull(Camera* cam, Scene* scene);
	
	};
}