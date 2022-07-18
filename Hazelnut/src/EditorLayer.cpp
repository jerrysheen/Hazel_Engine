#include "EditorLayer.h"
#include "Platform/OpenGL/OpenGLShader.h"



namespace Hazel
{
	EditorLayer::EditorLayer(Window& window)
		:Layer("EditorLayer"),
        m_CameraController(45.0f, window.GetWidth(), window.GetHeight(), 0.1f, 100000.0f),
		m_window(window)
	{
        std::string abpath = std::filesystem::current_path().u8string();
        std::string curr = abpath.append(std::string("/assets/Resources/Models/RivetGun/source/Rivet_Gun.obj"));
        model = new Model(curr);
        m_Texture = Texture2D::Create("assets/Resources/Models/RivetGun/textures/initialShadingGroup_Diffuse.tga.png");
	}

	void EditorLayer::OnAttach()
	{

        FramebufferSpecification fbSpec;
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_FrameBuffer = Framebuffer::Create(fbSpec);

	}

	void EditorLayer::OnDetach()
	{
        delete model;
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
        {
            m_FrameBuffer->Bind();
            RendererCommand::SetClearColor({ 0.5f, 0.5f, 0.5f, 1 });
            RendererCommand::Clear();
        }

        //RendererCommand::SetClearColor({ 104.0 / 256.0, 97.0 / 256.0, 92.0 / 256.0, 1 });
        //RendererCommand::Clear();


        Renderer3D::BeginScene(m_CameraController.GetCamera());
        Renderer3D::DrawPrimitives();


        // draw mesh
        {

            model->baseMap = Texture2D::Create(1, 1);
            uint32_t whiteTextureData = 0xffffffff;
            
            model->baseMap->SetData(&m_Texture, sizeof(uint32_t));

            model->shader = Shader::Create("assets/shaders/Texture.glsl");
            model->color = std::make_shared<glm::vec4>(1.0, 1.0, 1.0, 1.0);
            model->scale = std::make_shared<glm::vec3>(1.0, 1.0, 1.0);
            model->drawType = Renderer3D::DRAW_TYPE::HZ_TRIANGLES;
            

            model->shader->Bind();
            model->shader->SetFloat4("u_Color", *model->color);
            model->shader->SetFloat("u_TilingFactor", 1.0f);
            model->baseMap->Bind(0);
            // transform 里面有 translate 和 scale了
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))
                * glm::scale(glm::mat4(1.0f), *model->scale);
            model->shader->SetMat4("u_Transform", transform);
            model->shader->SetInt("u_Texture", 0);
            model->shader->SetMat4("u_ViewProjection", m_CameraController.GetCamera().GetViewProjectionMatrix());
            model->shader->SetFloat3("u_CameraPos", m_CameraController.GetCamera().GetCamPos());
            model->mesh->Bind();
            RendererCommand::DrawIndexed(model->mesh);

        }

        Renderer3D::EndScene();


        m_CameraController.OnUpdate(ts);

        m_FrameBuffer->Unbind();
	}

    void EditorLayer::OnImGuiRender()
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
                    Application::Get().Close();
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
            Renderer3D::CreatePlane();
        }
        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("ViewPort");
        ImVec2 viewPortSize = ImGui::GetContentRegionAvail();
        //  HZ_CORE_INFO("Viewport size X: {0};  : {1}", viewPortSize.x, viewPortSize.y);
        if (m_viewPortPanelSize != *((glm::vec2*)&viewPortSize)) 
        {
            m_viewPortPanelSize = { viewPortSize.x, viewPortSize.y };
            m_FrameBuffer->Resize(m_viewPortPanelSize);
            RendererCommand::SetViewPort(0,0,m_viewPortPanelSize.x, m_viewPortPanelSize.y);
            m_CameraController.ResetAspectRatio(m_viewPortPanelSize.x, m_viewPortPanelSize.y);
        }
        uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
        ImGui::Image((void*)textureID, ImVec2(m_viewPortPanelSize.x, m_viewPortPanelSize.y), ImVec2(0, 1), ImVec2(1,0));

        ImGui::End();
        ImGui::PopStyleVar();

#pragma endregion



        ImGui::End();
    }

	void EditorLayer::OnEvent(Event& e)
	{
        m_CameraController.OnEvent(e);
	}

}