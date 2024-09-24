#pragma once

#include "Hazel/Gfx/RenderStruct.h"
namespace Hazel {

	class RenderPass
	{
	public:
		virtual void OnCameraSetup() const = 0 ;
		virtual void Render(RenderNode* node, RenderingData* data) const = 0;
	};
}