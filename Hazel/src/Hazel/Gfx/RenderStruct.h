#pragma once
#include "Hazel/Material/Material.h"
//#include "Hazel/Model/Model.h"

namespace Hazel 
{
	struct RenderNode 
	{
		Material* m_mat;
		//Model* m_model;
	};

	struct RenderingData 
	{
		int lightCount;
		//Camera* m_Camera;
		//Light m_Light;
		//RenderingData() { m_Camera = nullptr; };
	};

}