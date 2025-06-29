#pragma once
#include "Runtime/Graphics/Renderer/Camera.h"
#include "Runtime/Scene/Scene.h"
#include "Runtime/Graphics/RenderStruct.h"
namespace Hazel 
{
	class Culling 
	{
	public:
		static RenderNode* Cull(Camera* cam, Scene* scene);
	
	};
}