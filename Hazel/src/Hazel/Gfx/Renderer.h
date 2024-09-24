#pragma once
#include "hzpch.h"
#include "Hazel/Graphics/RenderAPI.h"
#include "Hazel/Gfx/RenderStruct.h"
#include "Hazel/Gfx/RenderPass.h"

namespace Hazel {
	
	class Renderer 
	{
	public:
		static void Init();
		void OnCameraSetup();
		void Render(RenderNode* node, RenderingData* data);
		
		std::vector<RenderPass> m_RenderPasses;
	};
}