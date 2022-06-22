#include "SceneLayer.h"
#include "Platform/OpenGL/OpenGLShader.h"



SceneLayer::SceneLayer(Hazel::Window& window)
	:Layer("3Dlayer"),
	m_CameraController(45.0f, window.GetWidth(), window.GetHeight(), 0.1f, 100000.0f),
	m_window(window)
{
	HZ_INFO("{0}, {1}", window.GetWidth(), window.GetHeight());
}

void SceneLayer::OnAttach()
{
	//m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
    
    Hazel::FramebufferSpecification fbSpec;
    fbSpec.Width = 1280;
    fbSpec.Height = 720;
    m_FrameBuffer = Hazel::Framebuffer::Create(fbSpec);
}

void SceneLayer::OnDetach()
{

}

void SceneLayer::OnUpdate(Hazel::Timestep ts)
{
    {
        m_FrameBuffer->Bind();
        Hazel::RendererCommand::SetClearColor({0.5f, 0.5f, 0.5f, 1});
        Hazel::RendererCommand::Clear();
    }

	Hazel::RendererCommand::SetClearColor({ 104.0/256.0, 97.0 / 256.0, 92.0 / 256.0, 1 });
	Hazel::RendererCommand::Clear();

	Hazel::Renderer3D::BeginScene(m_CameraController.GetCamera());
	Hazel::Renderer3D::DrawPrimitives();
	
	Hazel::Renderer3D::EndScene();

	m_CameraController.OnUpdate(ts);

    m_FrameBuffer->Unbind();
}

void SceneLayer::OnImGuiRender()
{
    static bool p_open = true;
    static bool opt_fullscreen = true;
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
                Hazel::Application::Get().Close();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

#pragma region  Settings panel
    ImGui::Begin("Settings");
    ImGui::Text("Renderer2d Stats");
    //ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
    if (ImGui::Button("Add a new Plane"))
    {
        Hazel::Renderer3D::CreatePlane();
    }
    ImGui::End();

    ImGui::Begin("ViewPort");
    uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
    ImGui::Image((void*)textureID, ImVec2(1280.0f, 720.0f));

    ImGui::End();
#pragma endregion



    ImGui::End();
}

void SceneLayer::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}
