#pragma once

#include <glm/glm.hpp>

#include "Runtime/Graphics/RHI/Core/VertexArray.h"

namespace Hazel {
	
	class RenderAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1, DirectX12 = 2
		};
	public:
		inline static API GetAPI() { return s_API; };

	private:
		static API s_API;
	};
}