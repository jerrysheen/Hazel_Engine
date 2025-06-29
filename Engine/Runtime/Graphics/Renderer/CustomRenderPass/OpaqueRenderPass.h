#pragma once
#include "Runtime/Graphics/RenderPass.h"

namespace Hazel 
{
	class OpaquePass : public RenderPass
	{
	public :
		virtual void Render(RenderNode* node, RenderingData* data) const override;
		virtual void OnCameraSetup() const override;

	}; 
}