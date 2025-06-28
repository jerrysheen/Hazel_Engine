#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include <imgui_internal.h>
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
		ImGui::ShowDemoWindow();

		ImGui::Begin("Scene Hierarchy");
		m_Context->m_Registry.each([&](auto entityID) 
		{
			Entity entity{ entityID, m_Context.get() };
			DrawEntityNode(entity);

		});

		if (m_isMenuEnablede)
		{
			DrawLeftMouseMenu(m_Context);
		}


		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) 
		{
			m_SelectionContext = {};

		}
		else if (ImGui::IsMouseClicked(1)) 
		{
			m_isMenuEnablede = true;
		}
		//else if (ImGui::IsMouseClicked(0) && m_MenuItem == MenuItem::NULL_SELECTED)
		//{
		//	m_isMenuEnablede = false;
		//}


		

		ImGui::End();


		ImGui::Begin("Properties");
		
		if (m_SelectionContext) 
		{
			DrawComponents(m_SelectionContext);
		}
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tc = entity.GetComponent<TagComponent>().Tag;
			ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tc.c_str());
			if (ImGui::IsItemClicked())
			{
				m_SelectionContext = entity;
			}
			if (opened)
			{
				ImGui::Text("Some Child...");
				ImGui::TreePop();
			}
		
		}
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f) 
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();
		
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });

		float linHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { linHeight + 3.0f, linHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f});


		if (ImGui::Button("X",buttonSize))
			values.x = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();

	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>()) 
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		if (entity.HasComponent<TransformComponent>()) 
		{
			if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{
				auto& transform = entity.GetComponent<TransformComponent>();
				//ImGui::DragFloat3("Position", glm::value_ptr(transform), 0.1f);
				DrawVec3Control("Position", transform.Translation);
				// if we use radians
				//glm::vec3 rotation = glm::degrees(transform.Rotation);
				//DrawVec3Control("Rotation", rotation);
				//transform.Rotation = glm::radians(rotation);
				DrawVec3Control("Rotation", transform.Rotation);
				DrawVec3Control("Scale", transform.Scale, 1.0f);
				ImGui::TreePop();
			}
		}
	}
	void SceneHierarchyPanel::DrawLeftMouseMenu(const Ref<Scene>& context)
	{
		ImGui::OpenPopup("MyPopup");

		if (ImGui::BeginPopup("MyPopup")) {
			if (ImGui::BeginMenu("Create Mesh")) {
				if (ImGui::MenuItem("Create Cube")) {
					// ��ѡ��2�����ʱ�Ĵ����߼�
					m_MenuItem = MenuItem::MESH_CREATE_NEW_CUBE;
					m_isMenuEnablede = false;

				}
				else if (ImGui::MenuItem("Create Sphere"))
				{
					m_MenuItem = MenuItem::MESH_CREATE_NEW_SPHERE;
					m_isMenuEnablede = false;
				}
				// ��ѡ��1�����ʱ�Ĵ����߼�
				ImGui::EndMenu();
			}
			// ��Ӹ���ѡ��...
			ImGui::EndPopup();
		}
	}
}