#pragma once
#include "Hazel.h"
#include "Hazel/Core/Core.h"
#include "Hazel/Scene/Scene.h"
#include "Hazel/Scene/Entity.h"

namespace Hazel {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);
		void OnImGuiRender();

		enum MenuItem
		{
			NULL_SELECTED,
			MESH_CREATE_NEW_CUBE,
			MESH_CREATE_NEW_SPHERE,
			SCENCE_CREATE_NEW_SCENE,
		};

	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
		void DrawLeftMouseMenu(const Ref<Scene>& context);
		Ref<Scene> m_Context;

		Entity m_SelectionContext;
		bool m_isMenuEnablede = false;
		MenuItem m_MenuItem = NULL_SELECTED;
	};


}