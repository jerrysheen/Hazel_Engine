#include "hzpch.h"
#include "platform/OpenGL/OpenGLRendererAPI.h"

#include "RendererCommand.h"

namespace Hazel {
	
	RendererAPI* RendererCommand::s_RendererAPI = new OpenGLRendererAPI;
}