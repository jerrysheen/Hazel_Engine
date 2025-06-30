#pragma once

#include "Runtime/Graphics/Renderer/RenderStruct.h"
namespace Hazel {

	class RenderPass
	{
	public:
		virtual void OnCameraSetup() const = 0 ;
		virtual void Render(RenderNode* node, RenderingData* data) const = 0;
	};
}