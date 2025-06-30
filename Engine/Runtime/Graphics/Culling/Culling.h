#pragma once
#include "Runtime/Graphics/Camera/Camera.h"
#include "Runtime/Scene/Scene.h"
#include "Runtime/Graphics/Renderer/RenderStruct.h"
namespace Hazel 
{
	class Culling 
	{
	public:
		static RenderNode* Cull(Camera* cam, Scene* scene);
	
	};
}