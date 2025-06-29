#pragma once
#include "hzpch.h"
#include "Runtime/Graphics/RenderAPI.h"
#include "Runtime/Graphics/RenderStruct.h"
#include "Runtime/Graphics/RenderPass.h"

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