#pragma once
#include "Runtime/Graphics/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Hazel {

	class OpenGLContext : public GraphicsContext {
	public :
		OpenGLContext(GLFWwindow* windowHandle);
		void Init() override;
		void SwapBuffers() override;
		
	private:
		GLFWwindow* m_WindowHandle;

	};
}