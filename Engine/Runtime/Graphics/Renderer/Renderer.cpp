#include "hzpch.h"
#include "Renderer.h"

namespace Hazel {


	void Renderer::Init()
	{
	}

	void Renderer::OnCameraSetup()
	{
		for (auto& pass : m_RenderPasses) 
		{
			pass.OnCameraSetup();
		}
	}

	void Renderer::Render(RenderNode* node, RenderingData* data)
	{
		for (auto& pass : m_RenderPasses)
		{
			pass.Render(node, data);
		}
	}

}
