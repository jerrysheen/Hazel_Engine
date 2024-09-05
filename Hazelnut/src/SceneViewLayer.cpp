#include "SceneViewLayer.h"


namespace Hazel
{
    SceneViewLayer::SceneViewLayer(Window& window)
        :Layer("SceneViewLayer"),
        m_window(window)
    {
    }

    void SceneViewLayer::OnAttach()
    {

    }

    void SceneViewLayer::OnDetach()
    {
        // delete model;
    }

    void SceneViewLayer::OnUpdate(Timestep ts)
    {


    }

    void SceneViewLayer::OnImGuiRender()
    {
        static bool p_open = true;
        static bool opt_fullscreen = true;
        static bool opt_renderResult = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->GetWorkPos());
            ImGui::SetNextWindowSize(viewport->GetWorkSize());
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;


        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &p_open, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);

                if (ImGui::MenuItem("Close"))
                {
                    Application::Get().Close();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Debug"))
            {
                //if (ImGui::MenuItem("ShowImguiDemoWindow") && ImGui::IsWindowHovered()) 
                //{
                //    ImGui::ShowDemoWindow();
                //}
                ImGui::EndMenu();
            }



            ImGui::EndMenuBar();
        }

        //m_SceneHierarchyPanel.OnImGuiRender();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("ViewPort");
        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        style.WindowRounding = 0.25;
        // 这个地方应该不能每一帧resize， resize之后相当于清空了屏幕。
        //ImVec2 viewPortSize = ImGui::GetContentRegionAvail();
        ////HZ_CORE_INFO("Viewport size X: {0};  : {1}", viewPortSize.x, viewPortSize.y);
        //if (m_viewPortPanelSize != *((glm::vec2*)&viewPortSize)) 
        //{
        //    m_viewPortPanelSize = { viewPortSize.x, viewPortSize.y };
        //    m_FrameBuffer->Resize(m_viewPortPanelSize);
        //    RendererCommand::SetViewPort(0,0,m_viewPortPanelSize.x, m_viewPortPanelSize.y);
        //    m_CameraController.ResetAspectRatio(m_viewPortPanelSize.x, m_viewPortPanelSize.y);
        //}
        uint32_t textureID = 0;

        ImGui::End();
        ImGui::PopStyleVar();

#pragma endregion



        ImGui::End();
    }

    void SceneViewLayer::OnEvent(Event& e)
    {
    }

}