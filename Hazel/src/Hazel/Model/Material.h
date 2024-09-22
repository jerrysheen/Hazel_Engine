#pragma once

#include "hzpch.h"
#include "../Renderer/Shader.h"
#include "Hazel/Renderer/Texture.h"

//#include "../Renderer/Renderer3D.h"

namespace Hazel 
{
	class Material 
	{
	public : 
		class Material() = default;
		static Ref<Material> Create();
	
	public :
		// 最后或许会变成一个keymap的格式， 我从一个脚本语言里面定义这个地方有哪些参数，然后这个地方自动上传上去。
		Ref<Shader> shader;
		//Ref<Texture2D> baseMap;
		//Ref<Texture2D> bumpMap;
		//Ref<Texture2D> glossnessMap;
		//Ref<Texture2D> specularMap;
		//Ref<Texture2D> aoMap;
		Ref<Texture2D> tex00;
		Ref<Texture2D> tex01;
		Ref<Texture2D> tex02;
		Ref<Texture2D> tex03;
		Ref<Texture2D> tex04;
		Ref<Texture2D> tex05;

		Ref<Texture3D> tex3D;

		Ref<glm::vec4> color;
		//Renderer3D::DRAW_TYPE drawType;

		Ref<glm::mat4> modelMatrix;
		Ref<glm::mat4> translate;
		Ref<glm::mat4> rotate;
		Ref<glm::mat4> scale;
	};
}