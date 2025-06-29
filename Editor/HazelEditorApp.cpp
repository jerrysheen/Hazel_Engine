#include "Hazel.h"
#include "Runtime/Core/EntryPoint.h"
#include "imgui.h"
#include "Runtime/Core/Events/Event.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Platform/OpenGL/OpenGLShader.h"
#include "SceneViewLayer.h"

namespace Hazel 
{
	class Hazelnut : public Application
	{
	public:
		Hazelnut() : Application("Editor")
		{
			// ExapleLayer helps us to see the effect imediately.
			// PushLayer(new ExampleLayer());
			//PushLayer(new EditorLayer(GetWindow()));
			PushLayer(new SceneViewLayer(GetWindow()));

		}
		~Hazelnut() {
		}
	};

	Application* CreateApplication()
	{
		return new Hazelnut();
	}
}


