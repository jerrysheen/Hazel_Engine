#pragma once
#include "hzpch.h"
#include "Runtime/Graphics/RenderAPI.h"
#include "Runtime/Graphics/Renderer/RenderStruct.h"
#include "Runtime/Graphics/Renderer/RenderPass.h"

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