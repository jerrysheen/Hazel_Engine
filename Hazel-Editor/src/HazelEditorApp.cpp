#include "Hazel.h"
#include "Hazel/Core/EntryPoint.h"
#include "imgui/imgui.h"
#include "Hazel/Events/Event.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Platform/OpenGL/OpenGLShader.h"
#include "EditorLayer.h"

namespace Hazel 
{
	class HazelEditor : public Application
	{
	public:
		HazelEditor()
		{
			// ExapleLayer helps us to see the effect imediately.
			// PushLayer(new ExampleLayer());
			//PushLayer(new Sandbox2D(GetWindow()));
			PushLayer(new EditorLayer(GetWindow()));

		}
		~HazelEditor() {
		}
	};

	Application* CreateApplication()
	{
		return new HazelEditor();
	}
}


