#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>
#include "Hazel/Scene/Component.h"

namespace Hazel {

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");
		m_Context->m_Registry.each([&](auto entityID) 
		{
				Entity entity{ entityID, m_Context.get() };
				DrawEntityNode(entity);

		});

		ImGui::End();
		ImGui::ShowDemoWindow();
	}
	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		if (entity.HasComponent<HAZEL::TagComponent>())
		{
			auto& tc = entity.GetComponent<HAZEL::TagComponent>();
			ImGui::Text("%s", tc.Tag.c_str());
		}
	}
}