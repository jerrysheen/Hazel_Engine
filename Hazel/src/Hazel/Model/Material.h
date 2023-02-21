#pragma once

#include "hzpch.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Renderer3D.h"

namespace Hazel 
{
	class Material 
	{
	public : 
		class Material() = default;
		static Ref<Material> Create();
	
	public :
		Ref<Shader> shader;
		Ref<Texture2D> baseMap;
		Ref<Texture2D> bumpMap;
		Ref<Texture2D> glossnessMap;
		Ref<Texture2D> specularMap;
		Ref<Texture2D> aoMap;

		Ref<glm::vec4> color;
		Renderer3D::DRAW_TYPE drawType;

		Ref<glm::mat4> modelMatrix;
		Ref<glm::mat4> translate;
		Ref<glm::mat4> rotate;
		Ref<glm::mat4> scale;
	};
}