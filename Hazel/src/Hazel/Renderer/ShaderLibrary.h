#pragma once

#include <string>
#include <unordered_map>
#include "Hazel/Core/Core.h"

namespace Hazel {

	// 前向声明
	class Shader;

	class ShaderLibrary
	{
	public:
		void Add(const Ref<Shader>& shader);
		void Add(const std::string& name, const Ref<Shader>& shader);
		Ref<Shader> Load(const std::string& filepath); // assets/Texture.glsl
		Ref<Shader> Load(const std::string& name, const std::string& filepath);
		
		Ref<Shader> Get(const std::string& name);
		bool Exists(const std::string& name) const;
		
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};

} // namespace Hazel 