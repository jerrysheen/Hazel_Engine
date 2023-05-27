#include "EditorLayer.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include "Platform/OpenGL/OpenGLTexture2D.h"
#include "glm/gtc/matrix_transform.hpp"

namespace Hazel
{
	EditorLayer::EditorLayer(Window& window)
		:Layer("EditorLayer"),
        m_CameraController(45.0f, window.GetWidth(), window.GetHeight(), 0.1f, 100000.0f),
		m_window(window), m_GunObj(entt::null, nullptr), m_Plane(entt::null, nullptr), m_SkyBox(entt::null, nullptr)
	{
        std::string abpath = std::filesystem::current_path().u8string();
        std::string gunModelPath = abpath + std::string("/assets/Resources/Models/RivetGun/source/Rivet_Gun.obj");
        std::string planeModelPath = abpath + std::string("/assets/Resources/Models/Plane/Plane.obj");
        std::string cubeModelPath = abpath + std::string("/assets/Resources/Models/Cube/Cube.obj");
        //std::string curr = abpath.append(std::string("/assets/Resources/Models/OldHelmet/source/helmet.obj"));
        //model = new Model(modelPath);
        //model->shader = Shader::Create("assets/shaders/Standard.glsl");





        m_ActiveScene = CreateRef <Scene>();
        m_GunObj = m_ActiveScene->CreateEntity();
        //Entity m_GameObject = m_ActiveScene->CreateEntity();
        m_GunObj.HasComponent<HAZEL::TransformComponent>();
        m_GunObj.AddComponent<HAZEL::MeshFilterComponent>(gunModelPath);
        m_GunObj.AddComponent<HAZEL::MeshRendererComponent>();
        
        m_Plane = m_ActiveScene->CreateEntity();
        m_Plane.HasComponent<HAZEL::TransformComponent>();
        m_Plane.AddComponent<HAZEL::MeshFilterComponent>(planeModelPath);
        m_Plane.AddComponent<HAZEL::MeshRendererComponent>();

        m_SkyBox = m_ActiveScene->CreateEntity();
        m_SkyBox.HasComponent<HAZEL::TransformComponent>();
        m_SkyBox.AddComponent<HAZEL::MeshFilterComponent>(planeModelPath);
        m_SkyBox.AddComponent<HAZEL::MeshRendererComponent>();
        //HAZEL::MeshRendererComponent& meshRenderer = m_Plane.GetComponent<HAZEL::MeshRendererComponent>();
        

        m_MainLightShader = Shader::Create("assets/shaders/Shadow.glsl");
        m_PBRshader = Shader::Create("assets/shaders/Standard.glsl");
        m_UnLit = Shader::Create("assets/shaders/Ground.glsl");
       
	}

	void EditorLayer::OnAttach()
	{

        m_fbSpec.Width = 1024;
        m_fbSpec.Height = 1024;
        m_FrameBuffer = Framebuffer::Create(m_fbSpec);

        m_shadowMapSpec.Width = 1024;
        m_shadowMapSpec.Height = 1024;
        m_ShadowMapRenderTarget = Framebuffer::Create(m_shadowMapSpec);

        //model->baseMap = Texture2D::Create("assets/Resources/Models/RivetGun/textures/initialShadingGroup_Diffuse.tga.png");
        //model->bumpMap = Texture2D::Create("assets/Resources/Models/RivetGun/textures/initialShadingGroup_Normal.tga.png");
        //model->aoMap = Texture2D::Create("assets/Resources/Models/RivetGun/textures/ internal_ground_ao_texture.jpeg");
        //model->glossnessMap = Texture2D::Create("assets/Resources/Models/RivetGun/textures/initialShadingGroup_Glossiness.tga.png");
        //model->specularMap = Texture2D::Create("assets/Resources/Models/RivetGun/textures/initialShadingGroup_Specular.tga.png");
       
        //model->color = std::make_shared<glm::vec4>(1.0, 1.0, 1.0, 1.0);
        //
        //model->SetPosition(glm::vec3(0.5f, 0.5f, 0.0f));
        //model->SetRotation(glm::vec3(0.0f, 0.0f, 90.0f));
        //model->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
        //model->translate = std::make_shared<glm::mat4>(glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.0f)));
        //model->rotate = std::make_shared<glm::mat4>(glm::rotate(glm::mat4(1.0f), 90.0f, glm::vec3(0.0f, 0.0f, 1.0f)));
        //model->scale = std::make_shared<glm::mat4>(glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 1.0f)));
        HZ_CORE_INFO("EditorLayer On attach!");
        //auto view = m_ActiveScene->Reg().view<HAZEL::MeshRendererComponent>();
        //for (auto entity : view)
        //{
        //    // can directly do your job inside view
        //    HAZEL::MeshRendererComponent& meshRenderer = view.get<HAZEL::MeshRendererComponent>(entity);
        HAZEL::MeshRendererComponent meshRenderer = m_GunObj.GetComponent<HAZEL::MeshRendererComponent>();
///*            meshRenderer.material->baseMap = Texture2D::Create("assets/Resources/Models/RivetGun/textures/initialShadingGroup_Diffuse.tga.png");
//            meshRenderer.material->bumpMap = Texture2D::Create("assets/Resources/Models/RivetGun/textures/initialShadingGroup_Normal.tga.png");
//            meshRenderer.material->aoMap = Texture2D::Create("assets/Resources/Models/RivetGun/textures/internal_ground_ao_texture.jpeg");
//            meshRenderer.material-> glossnessMap = Texture2D::Create("assets/Resources/Models/RivetGun/textures/initialShadingGroup_Glossiness.tga.png");
//            meshRenderer.material->specularMap = Texture2D::Create("assets/Resources/Models/RivetGun/textures/initialShadingGroup_Specular.tga.png");   */         
            meshRenderer.material->baseMap = Texture2D::Create("assets/Resources/Models/RivetGun/textures_compressed/diffuse.dds", true);
            meshRenderer.material->bumpMap = Texture2D::Create("assets/Resources/Models/RivetGun/textures_compressed/normal.dds", true);
            meshRenderer.material->aoMap = Texture2D::Create("assets/Resources/Models/RivetGun/textures_compressed/ao.dds", true);
            meshRenderer.material-> glossnessMap = Texture2D::Create("assets/Resources/Models/RivetGun/textures_compressed/glossiness.dds", true);
            meshRenderer.material->specularMap = Texture2D::Create("assets/Resources/Models/RivetGun/textures_compressed/specular.dds", true);
            //model->color = std::make_shared<glm::vec4>(1.0, 1.0, 1.0, 1.0);
            //
        //}

        //for (auto entity : view)
        //{
        //    // can directly do your job inside view
        //    HAZEL::MeshRendererComponent& meshRenderer = view.get<HAZEL::MeshRendererComponent>(entity);
        //    meshRenderer.material->shader =
        //}
	}

	void EditorLayer::OnDetach()
	{
       // delete model;
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
#pragma region RENDER SHADOWMAP
        // render shadowmap
        {
            {
                m_ShadowMapRenderTarget->Bind();
                
                RendererCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
                RendererCommand::Clear();
            }


            RendererCommand::SetViewPort(0, 0, m_shadowMapSpec.Width, m_shadowMapSpec.Height);
            m_viewPortPanelSize = { m_shadowMapSpec.Width, m_shadowMapSpec.Height };
            Renderer3D::BeginScene(m_CameraController.GetCamera());

            GLfloat near_plane =0.1f, far_plane = 57.5f;
            glm::mat4 lightProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, near_plane, far_plane);
            glm::mat4 lightView = glm::lookAt(glm::vec3(m_LightPos[0], m_LightPos[1], m_LightPos[2]), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 lightSpaceMatrix = lightProjection * lightView;

            {
                // can directly do your job inside view
                HAZEL::MeshRendererComponent& meshRenderer = m_GunObj.GetComponent<HAZEL::MeshRendererComponent>();
                meshRenderer.material->shader = m_MainLightShader;

                meshRenderer.material->shader->Bind();
                meshRenderer.material->shader->SetFloat4("u_Color", glm::vec4(1.0, 1.0, 1.0, 1.0));
                meshRenderer.material->shader->SetFloat("u_TilingFactor", 1.0f);


                meshRenderer.material->shader->SetMat4("u_ModelMatrix", *(std::make_shared<glm::mat4>(glm::mat4(1.0f))));
                meshRenderer.material->shader->SetMat4("u_ViewProjection", lightSpaceMatrix);
                meshRenderer.material->shader->SetFloat3("u_CameraPos", m_CameraController.GetCamera().GetCamPos());

            }

            // draw plane
            auto view_Filter = m_ActiveScene->Reg().view<HAZEL::MeshFilterComponent>();
            for (auto entity : view_Filter)
            {
                // can directly do your job inside view
                HAZEL::MeshFilterComponent& meshFilter = view_Filter.get<HAZEL::MeshFilterComponent>(entity);
                meshFilter.mesh->meshData->Bind();
                RendererCommand::DrawIndexed(meshFilter.mesh->meshData);
            }

#pragma endregion




        {
            m_FrameBuffer->Bind();
            RendererCommand::SetViewPort(0, 0, m_fbSpec.Width, m_fbSpec.Height);
            // m_viewPortPanelSize = { m_fbSpec.Width, m_fbSpec.Height };
            RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
            RendererCommand::Clear();
        }

        //RendererCommand::SetClearColor({ 104.0 / 256.0, 97.0 / 256.0, 92.0 / 256.0, 1 });
        //RendererCommand::Clear();


        Renderer3D::BeginScene(m_CameraController.GetCamera());
        //Renderer3D::DrawPrimitives();

        // update Scene
        m_ActiveScene->OnUpdate(ts);

        // draw Gun
        {
                // can directly do your job inside view
                HAZEL::MeshRendererComponent& meshRenderer = m_GunObj.GetComponent<HAZEL::MeshRendererComponent>();
                meshRenderer.material->shader = m_PBRshader;

                meshRenderer.material->shader->Bind();
                meshRenderer.material->shader->SetFloat4("u_Color", glm::vec4(1.0, 1.0, 1.0, 1.0));
                meshRenderer.material->shader->SetFloat("u_TilingFactor", 1.0f);
                
                // bind Texture
                
                meshRenderer.material->baseMap->Bind(0);
                meshRenderer.material->shader->SetInt("u_DiffuseMap", 0);
                
                meshRenderer.material->bumpMap->Bind(1);
                meshRenderer.material->shader->SetInt("u_NormalMap", 1);

                meshRenderer.material->aoMap->Bind(2);
                meshRenderer.material->shader->SetInt("u_AoMap", 2);
                
                meshRenderer.material->glossnessMap->Bind(3);
                meshRenderer.material->shader->SetInt("u_GlossnessMap", 3);
                
                meshRenderer.material->specularMap->Bind(4);
                meshRenderer.material->shader->SetInt("u_SpecularMap", 4);

                glBindTextureUnit(5, m_ShadowMapRenderTarget->GetDepthAttachmentRendererID());
                meshRenderer.material->shader->SetInt("u_ShadowMap", 5);

                meshRenderer.material->shader->SetMat4("u_ModelMatrix", *(std::make_shared<glm::mat4>(glm::mat4(1.0f))));
                meshRenderer.material->shader->SetMat4("u_ViewProjection", m_CameraController.GetCamera().GetViewProjectionMatrix());
                meshRenderer.material->shader->SetFloat3("u_CameraPos", m_CameraController.GetCamera().GetCamPos());
                meshRenderer.material->shader->SetMat4("u_LightSpaceViewProjection", lightSpaceMatrix);

                HAZEL::MeshFilterComponent& meshFilter = m_GunObj.GetComponent<HAZEL::MeshFilterComponent>();
                meshFilter.mesh->meshData->Bind();
                RendererCommand::DrawIndexed(meshFilter.mesh->meshData);
        }

        // draw plane

        {
                // can directly do your job inside view
                HAZEL::MeshRendererComponent& meshRenderer = m_Plane.GetComponent<HAZEL::MeshRendererComponent>();
                meshRenderer.material->shader = m_UnLit;

                meshRenderer.material->shader->Bind();
                meshRenderer.material->shader->SetFloat4("u_Color", glm::vec4(1.0, 1.0, 1.0, 1.0));
                meshRenderer.material->shader->SetFloat("u_TilingFactor", 1.0f);


                meshRenderer.material->shader->SetMat4("u_ModelMatrix", *(std::make_shared<glm::mat4>(glm::mat4(1.0f))));
                meshRenderer.material->shader->SetMat4("u_ViewProjection", m_CameraController.GetCamera().GetViewProjectionMatrix());
                meshRenderer.material->shader->SetFloat3("u_CameraPos", m_CameraController.GetCamera().GetCamPos());

                glBindTextureUnit(0, m_ShadowMapRenderTarget->GetDepthAttachmentRendererID());
                meshRenderer.material->shader->SetInt("u_ShadowMap", 0);

                meshRenderer.material->shader->SetMat4("u_LightSpaceViewProjection", lightSpaceMatrix);
                HAZEL::MeshFilterComponent& meshFilter = m_Plane.GetComponent<HAZEL::MeshFilterComponent>();
                meshFilter.mesh->meshData->Bind();
                RendererCommand::DrawIndexed(meshFilter.mesh->meshData);
        }


            //// Lighting config

            //// transform 里面有 translate 和 scale了
            //model->shader->SetMat4("u_ModelMatrix", *model->GetModelMatrix());
            //model->shader->SetMat4("u_ViewProjection", m_CameraController.GetCamera().GetViewProjectionMatrix());
            //model->shader->SetFloat3("u_CameraPos", m_CameraController.GetCamera().GetCamPos());
            //model->mesh->Bind();
            

            //auto view_Filter = m_ActiveScene->Reg().view<HAZEL::MeshFilterComponent>();
            //for (auto entity : view_Filter)
            //{
            //    // can directly do your job inside view
            //    HAZEL::MeshFilterComponent& meshFilter = view_Filter.get<HAZEL::MeshFilterComponent>(entity);
            //    meshFilter.mesh->meshData->Bind();
            //    RendererCommand::DrawIndexed(meshFilter.mesh->meshData);
            //}

        }

        Renderer3D::EndScene();


        if(m_ViewPortFocused)m_CameraController.OnUpdate(ts);

        m_FrameBuffer->Unbind();


        
	}

    void EditorLayer::OnImGuiRender()
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

            if (ImGui::BeginMenu("RenderTarget"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                ImGui::MenuItem("Result", NULL, &opt_renderResult);

                if (ImGui::MenuItem("ShadowMap"))
                {
                    opt_renderResult = false; 
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

        float position[3] = {0.0, 0.0, 0.0};
        ImGui::SliderFloat3("position", position, INT_MIN, INT_MAX);
        //model->SetPosition(*((glm::vec3*)&position));
        
        ImGui::Text("Light Pos");
        ImGui::SliderFloat3("LightPos", m_LightPos, -20, 20);
        
        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("ViewPort");
        m_ViewPortFocused = ImGui::IsWindowFocused();
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
        if (opt_renderResult)
        {
            uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
            ImGui::Image((void*)textureID, ImVec2(m_FrameBuffer->GetSpecification().Width , m_FrameBuffer->GetSpecification().Height), ImVec2(0, 1), ImVec2(1,0));
        }
        else 
        {
            uint32_t textureID = m_ShadowMapRenderTarget->GetDepthAttachmentRendererID();
            ImGui::Image((void*)textureID, ImVec2(m_ShadowMapRenderTarget->GetSpecification().Width, m_ShadowMapRenderTarget->GetSpecification().Height), ImVec2(0, 1), ImVec2(1, 0));
        }

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