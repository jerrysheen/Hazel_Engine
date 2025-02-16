#include "EditorLayer.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include "Platform/OpenGL/OpenGLTexture2D.h"
#include "glm/gtc/matrix_transform.hpp"

namespace Hazel
{
	EditorLayer::EditorLayer(Window& window)
		:Layer("EditorLayer"),
        m_CameraController(45.0f, window.GetWidth(), window.GetHeight(), 0.1f, 100000.0f),
		m_window(window), m_GunObj(entt::null, nullptr), m_Plane(entt::null, nullptr), m_SkyBox(entt::null, nullptr), m_Sphere(entt::null, nullptr)
        , m_Cube(entt::null, nullptr)
	{
        std::string abpath = std::filesystem::current_path().u8string();
        std::string gunModelPath = abpath + std::string("/assets/Resources/Models/RivetGun/source/Rivet_Gun.obj");
        std::string planeModelPath = abpath + std::string("/assets/Resources/Models/Plane/Plane.obj");
        std::string cubeModelPath = abpath + std::string("/assets/Resources/Models/Cube/Cube.obj");
        std::string sphereModelPath = abpath + std::string("/assets/Resources/Models/Sphere/Sphere.obj");
        //std::string curr = abpath.append(std::string("/assets/Resources/Models/OldHelmet/source/helmet.obj"));
        //model = new Model(modelPath);
        //model->shader = Shader::Create("assets/shaders/Standard.glsl");

        m_IrradianceMap = Texture3D::Create(32, 32);



        m_ActiveScene = CreateRef <Scene>();
        //m_GunObj = m_ActiveScene->CreateEntity();
        ////Entity m_GameObject = m_ActiveScene->CreateEntity();
        //m_GunObj.HasComponent<TransformComponent>();
        //m_GunObj.AddComponent<MeshFilterComponent>(gunModelPath);
        //m_GunObj.AddComponent<MeshRendererComponent>();
        
        m_Plane = m_ActiveScene->CreateEntity("myPlane");
        m_Plane.HasComponent<TransformComponent>();
        m_Plane.AddComponent<MeshFilterComponent>(planeModelPath);
        m_Plane.AddComponent<MeshRendererComponent>();

        m_Sphere = m_ActiveScene->CreateEntity("mySphere");
        m_Sphere.HasComponent<TransformComponent>();
        m_Sphere.AddComponent<MeshFilterComponent>(sphereModelPath);
        m_Sphere.AddComponent<MeshRendererComponent>();

        

        m_MainLightShader = Shader::Create("assets/shaders/Shadow.glsl");
        m_PBRTextureShader = Shader::Create("assets/shaders/Standard.glsl");
        m_UnLitShader = Shader::Create("assets/shaders/Ground.glsl");
        m_SkyboxShader = Shader::Create("assets/shaders/Skybox.glsl");
        m_PBRShader = Shader::Create("assets/shaders/PBRShader.glsl");
        equirectangularToCubemapShader = Shader::Create("assets/shaders/EquirectangularToCubemap.glsl");

        std::vector<std::string> faces;
        faces.push_back(std::string("assets/Resources/Skybox/right.dds"));
        faces.push_back(std::string("assets/Resources/Skybox/left.dds"));
        faces.push_back(std::string("assets/Resources/Skybox/top.dds"));
        faces.push_back(std::string("assets/Resources/Skybox/bottom.dds"));
        faces.push_back(std::string("assets/Resources/Skybox/back.dds"));
        faces.push_back(std::string("assets/Resources/Skybox/front.dds"));

        m_SkyBox = m_ActiveScene->CreateEntity("mySkyBox");
        m_SkyBox.HasComponent<TransformComponent>();
        m_SkyBox.AddComponent<MeshFilterComponent>(cubeModelPath);
        m_SkyBox.AddComponent<MeshRendererComponent>();
        m_SkyBox.GetComponent<MeshRendererComponent>().material->tex3D = Texture3D::Create(faces, true, false);

        m_Cube = m_ActiveScene->CreateEntity("myCube");
        m_Cube.HasComponent<TransformComponent>();
        m_Cube.AddComponent<MeshFilterComponent>(cubeModelPath);
        m_Cube.AddComponent<MeshRendererComponent>();
        //m_Cube.GetComponent<MeshRendererComponent>().material->tex3D = Texture3D::Create(faces, true, false);
        //MeshRendererComponent& meshRenderer = m_Plane.GetComponent<MeshRendererComponent>();
	}

	void EditorLayer::OnAttach()
	{
//
//        m_fbSpec.width = 960;
//        m_fbSpec.height = 540;
//        m_FrameBuffer = TextureBuffer::Create(m_fbSpec);
//
//        m_shadowMapSpec.width = 1024;
//        m_shadowMapSpec.height = 1024;
//        m_ShadowMap = Texture2D::Create(m_shadowMapSpec.width, m_shadowMapSpec.height, GL_DEPTH24_STENCIL8);
//        //m_FrameBuffer->RebindColorAttachment(m_ShadowMap->GetRendererID(), m_shadowMapSpec);
//        m_OpaqueTexture = Texture2D::Create(m_fbSpec.width, m_fbSpec.height, GL_RGBA8);
//        m_DepthTexture = Texture2D::Create(m_fbSpec.width, m_fbSpec.height, GL_DEPTH24_STENCIL8);
//        m_FrameBuffer->RebindColorAttachment(m_OpaqueTexture->GetRendererID(), m_fbSpec);
//        //m_ShadowMapRenderTarget = Framebuffer::Create(m_shadowMapSpec);
//
//
//        HZ_CORE_INFO("EditorLayer On attach!");
//
//        MeshRendererComponent meshRenderer;
//        //meshRenderer = m_GunObj.GetComponent<MeshRendererComponent>();
//        //meshRenderer.material->tex00 = Texture2D::Create("assets/Resources/Models/RivetGun/textures_compressed/diffuse.dds", true, true);
//        //meshRenderer.material->tex01 = Texture2D::Create("assets/Resources/Models/RivetGun/textures_compressed/normal.dds", true, true);
//        //meshRenderer.material->tex02 = Texture2D::Create("assets/Resources/Models/RivetGun/textures_compressed/ao.dds", true, true);
//        //meshRenderer.material->tex03 = Texture2D::Create("assets/Resources/Models/RivetGun/textures_compressed/glossiness.dds", true, true);
//        //meshRenderer.material->tex04 = Texture2D::Create("assets/Resources/Models/RivetGun/textures_compressed/specular.dds", true, true);
//
//
//        meshRenderer = m_SkyBox.GetComponent<MeshRendererComponent>();
//
//#pragma region Diffuse Irradiance
//        // onAttach的时候，把irrandianceMap给绑定好
//        glm::mat4 captureViews[] =
//        {
//            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
//            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
//            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
//            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
//            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
//            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
//        };
//        RendererCommand::SetViewPort(0, 0, 32, 32);
//        glm::mat4 enviromentCamProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
//        m_FrameBuffer->Bind();
//        equirectangularToCubemapShader->Bind();
//        m_SkyBox.GetComponent<MeshRendererComponent>().material->tex3D->Bind(0);
//        for (unsigned int i = 0; i < 6; ++i)
//        {
//            equirectangularToCubemapShader->SetMat4("u_ViewMatrix", captureViews[i]);
//            equirectangularToCubemapShader->SetMat4("u_ProjectionMatrix", enviromentCamProjection);
//            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_IrradianceMap->GetRendererID(), 0);
//            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
//            RendererCommand::Clear();
//            HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");
//            MeshFilterComponent& meshFilter = m_SkyBox.GetComponent<MeshFilterComponent>();
//            meshFilter.mesh->meshData->Bind();
//            RendererCommand::DrawIndexed(meshFilter.mesh->meshData);
//        }
//        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
//        RendererCommand::SetViewPort(0, 0, m_fbSpec.width, m_fbSpec.height);
//        // 重新绑定一次后，渲染的图片不会被清掉
//        m_FrameBuffer->RebindColorAndDepthAttachment(m_OpaqueTexture->GetRendererID(), m_DepthTexture->GetRendererID(), m_fbSpec);
//        // 
//#pragma region Diffuse Irradiance
//	
//        // imgui layout settings:
//        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
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
                //m_ShadowMapRenderTarget->Bind();
                
                RendererCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
                RendererCommand::Clear();
            }


            RendererCommand::SetViewPort(0, 0, m_shadowMapSpec.width, m_shadowMapSpec.height);
            m_viewPortPanelSize = { m_shadowMapSpec.width, m_shadowMapSpec.height };
            Renderer3D::BeginScene(m_CameraController.GetCamera());

            GLfloat near_plane =0.1f, far_plane = 57.5f;
            glm::mat4 lightProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, near_plane, far_plane);
            glm::mat4 lightView = glm::lookAt(glm::vec3(m_LightPos[0], m_LightPos[1], m_LightPos[2]), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 lightSpaceMatrix = lightProjection * lightView;



#pragma endregion




        {
            m_FrameBuffer->Bind();
            RendererCommand::SetViewPort(0, 0, m_fbSpec.width, m_fbSpec.height);
            RendererCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
            RendererCommand::Clear();
        }

        Renderer3D::BeginScene(m_CameraController.GetCamera());

        // update Scene
        m_ActiveScene->OnUpdate(ts);


        m_FrameBuffer->RebindColorAndDepthAttachment(m_OpaqueTexture->GetRendererID(), m_DepthTexture->GetRendererID(), m_fbSpec);
        //glObjectLabel(GL_FRAMEBUFFER, m_FrameBuffer->GetRendererID(), -1, "FrameBuffer");
        //{
        //    m_FrameBuffer->Bind();
        //    //RendererCommand::SetViewPort(0, 0, m_fbSpec.Width, m_fbSpec.Height);
        //    // m_viewPortPanelSize = { m_fbSpec.Width, m_fbSpec.Height };
        //    //RendererCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
        //    RendererCommand::Clear();
        //}

        //// draw Gun
        //{
        //        // can directly do your job inside view
        //        MeshRendererComponent& meshRenderer = m_GunObj.GetComponent<MeshRendererComponent>();
        //        meshRenderer.material->shader = m_PBRTextureShader;

        //        meshRenderer.material->shader->Bind();
        //        meshRenderer.material->shader->SetFloat4("u_Color", glm::vec4(1.0, 1.0, 1.0, 1.0));
        //        meshRenderer.material->shader->SetFloat("u_TilingFactor", 1.0f);
        //        
        //        // bind Texture
        //        
        //        meshRenderer.material->tex00->Bind(0);
        //        meshRenderer.material->shader->SetInt("u_DiffuseMap", 0);
        //        
        //        meshRenderer.material->tex01->Bind(1);
        //        meshRenderer.material->shader->SetInt("u_NormalMap", 1);

        //        meshRenderer.material->tex02->Bind(2);
        //        meshRenderer.material->shader->SetInt("u_AoMap", 2);
        //        
        //        meshRenderer.material->tex03->Bind(3);
        //        meshRenderer.material->shader->SetInt("u_GlossnessMap", 3);
        //        
        //        meshRenderer.material->tex04->Bind(4);
        //        meshRenderer.material->shader->SetInt("u_SpecularMap", 4);

        //        glBindTextureUnit(5, m_ShadowMapRenderTarget->GetDepthAttachmentRendererID());
        //        meshRenderer.material->shader->SetInt("u_ShadowMap", 5);

        //        meshRenderer.material->shader->SetMat4("u_ModelMatrix", *(std::make_shared<glm::mat4>(glm::mat4(1.0f))));
        //        meshRenderer.material->shader->SetMat4("u_ViewProjection", m_CameraController.GetCamera().GetViewProjectionMatrix());
        //        meshRenderer.material->shader->SetFloat3("u_CameraPos", m_CameraController.GetCamera().GetCamPos());
        //        meshRenderer.material->shader->SetMat4("u_LightSpaceViewProjection", lightSpaceMatrix);

                MeshFilterComponent& meshFilter = m_GunObj.GetComponent<MeshFilterComponent>();
                meshFilter.mesh->meshData->Bind();
                RendererCommand::DrawIndexed(meshFilter.mesh->meshData);
        //}

        // draw plane

        {
                // can directly do your job inside view
                MeshRendererComponent& meshRenderer = m_Plane.GetComponent<MeshRendererComponent>();
                TransformComponent& transform = m_Plane.GetComponent<TransformComponent>();
                meshRenderer.material->shader = m_UnLitShader;

                meshRenderer.material->shader->Bind();
                meshRenderer.material->shader->SetFloat4("u_Color", glm::vec4(0.5, 0.5, 0.5, 1.0));
                meshRenderer.material->shader->SetFloat("u_TilingFactor", 1.0f);


                meshRenderer.material->shader->SetMat4("u_ModelMatrix", transform.GetTransform());
                meshRenderer.material->shader->SetMat4("u_ViewProjection", m_CameraController.GetCamera().GetViewProjectionMatrix());
                meshRenderer.material->shader->SetFloat3("u_CameraPos", m_CameraController.GetCamera().GetCamPos());

                glBindTextureUnit(0, m_ShadowMap->GetRendererID());
                meshRenderer.material->shader->SetInt("u_ShadowMap", 0);

                //meshRenderer.material->shader->SetMat4("u_LightSpaceViewProjection", lightSpaceMatrix);
                MeshFilterComponent& meshFilter = m_Plane.GetComponent<MeshFilterComponent>();
                meshFilter.mesh->meshData->Bind();
                RendererCommand::DrawIndexed(meshFilter.mesh->meshData);
        }
        

        //// draw Cube

        //{
        //        // can directly do your job inside view
        //        MeshRendererComponent& meshRenderer = m_Cube.GetComponent<MeshRendererComponent>();
        //        meshRenderer.material->shader = m_UnLitShader;

        //        meshRenderer.material->shader->Bind();
        //        meshRenderer.material->shader->SetFloat4("u_Color", glm::vec4(0.5, 0.5, 0.5, 1.0));
        //        meshRenderer.material->shader->SetFloat("u_TilingFactor", 1.0f);

        //        glm::mat4x4 identity = glm::mat4x4(1.0f);
        //        glm::mat4x4 translate = glm::translate(identity, glm::vec3(0.0, 0.0, 0.0));
        //        meshRenderer.material->shader->SetMat4("u_ModelMatrix", translate);
        //        meshRenderer.material->shader->SetMat4("u_ViewProjection", m_CameraController.GetCamera().GetViewProjectionMatrix());
        //        meshRenderer.material->shader->SetFloat3("u_CameraPos", m_CameraController.GetCamera().GetCamPos());

        //        //lBindTextureUnit(0, m_ShadowMapRenderTarget->GetDepthAttachmentRendererID());
        //        //meshRenderer.material->shader->SetInt("u_ShadowMap", 0);

        //        meshRenderer.material->shader->SetMat4("u_LightSpaceViewProjection", lightSpaceMatrix);
        //        MeshFilterComponent& meshFilter = m_Cube.GetComponent<MeshFilterComponent>();
        //        meshFilter.mesh->meshData->Bind();
        //        RendererCommand::DrawIndexed(meshFilter.mesh->meshData);
        //}


        // draw m_Sphere

        {
            MeshRendererComponent& meshRenderer = m_Sphere.GetComponent<MeshRendererComponent>();
            TransformComponent& transform = m_Sphere.GetComponent<TransformComponent>();
            meshRenderer.material->shader = m_PBRShader;

            meshRenderer.material->shader->Bind();
            meshRenderer.material->shader->SetFloat4("u_Color", glm::vec4(m_DiffuseColor[0], m_DiffuseColor[1], m_DiffuseColor[2], m_DiffuseColor[3]));
            meshRenderer.material->shader->SetFloat("u_TilingFactor", 1.0f);
            meshRenderer.material->shader->SetFloat("u_Metallic", m_Metallic);
            meshRenderer.material->shader->SetFloat("u_Roughness", m_Roughness);


            //meshRenderer.material->shader->SetMat4("u_ModelMatrix", *(std::make_shared<glm::mat4>(glm::mat4(1.0f))));
            //glm::mat4x4 identity = glm::mat4x4(1.0f);
            //glm::mat4x4 translate = glm::translate(identity, glm::vec3(2.0, 1.0, 2.0));
            meshRenderer.material->shader->SetMat4("u_ModelMatrix", transform.GetTransform());
            meshRenderer.material->shader->SetMat3("u_WorldToModelMatrix", glm::transpose(glm::inverse(glm::mat3(transform.GetTransform()))));
            meshRenderer.material->shader->SetMat4("u_ViewProjection", m_CameraController.GetCamera().GetViewProjectionMatrix());
            meshRenderer.material->shader->SetFloat3("u_CameraPos", m_CameraController.GetCamera().GetCamPos());


            glBindTextureUnit(0, m_ShadowMap->GetRendererID());
            meshRenderer.material->shader->SetInt("u_ShadowMap", 0);

            meshRenderer.material->shader->SetMat4("u_LightSpaceViewProjection", lightSpaceMatrix);
            MeshFilterComponent& meshFilter = m_Sphere.GetComponent<MeshFilterComponent>();
            meshFilter.mesh->meshData->Bind();
            RendererCommand::DrawIndexed(meshFilter.mesh->meshData);
        }


        // draw skybox
        {

            glDepthFunc(GL_LEQUAL);
            // can directly do your job inside view
            MeshRendererComponent& meshRenderer = m_SkyBox.GetComponent<MeshRendererComponent>();
            meshRenderer.material->shader = m_SkyboxShader;

            meshRenderer.material->shader->Bind();
            meshRenderer.material->shader->SetFloat4("u_Color", glm::vec4(1.0, 1.0, 1.0, 1.0));
            meshRenderer.material->shader->SetFloat("u_TilingFactor", 1.0f);


            meshRenderer.material->shader->SetMat4("u_ModelMatrix", *(std::make_shared<glm::mat4>(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)))));

            glm::mat4 view = glm::mat4(glm::mat3(m_CameraController.GetCamera().GetViewMatrix()));
            glm::mat4 projection = m_CameraController.GetCamera().GetProjectionMatrix();
            meshRenderer.material->shader->SetMat4("u_ViewProjection", projection * view);
            meshRenderer.material->shader->SetFloat3("u_CameraPos", m_CameraController.GetCamera().GetCamPos());

            //meshRenderer.material->tex3D->Bind(0);
            // 测试生成的irradiance map
            glBindTextureUnit(0, m_IrradianceMap->GetRendererID());
            meshRenderer.material->shader->SetInt("u_SkyboxTexture", 0);

            meshRenderer.material->shader->SetMat4("u_LightSpaceViewProjection", lightSpaceMatrix);
            MeshFilterComponent& meshFilter = m_SkyBox.GetComponent<MeshFilterComponent>();
            meshFilter.mesh->meshData->Bind();
            RendererCommand::DrawIndexed(meshFilter.mesh->meshData);
            glDepthFunc(GL_LESS);
        }
            //// Lighting config

            //// transform 里面有 translate 和 scale了
            //model->shader->SetMat4("u_ModelMatrix", *model->GetModelMatrix());
            //model->shader->SetMat4("u_ViewProjection", m_CameraController.GetCamera().GetViewProjectionMatrix());
            //model->shader->SetFloat3("u_CameraPos", m_CameraController.GetCamera().GetCamPos());
            //model->mesh->Bind();
            

            //auto view_Filter = m_ActiveScene->Reg().view<MeshFilterComponent>();
            //for (auto entity : view_Filter)
            //{
            //    // can directly do your job inside view
            //    MeshFilterComponent& meshFilter = view_Filter.get<MeshFilterComponent>(entity);
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
                if (ImGui::MenuItem("OpaqueTexture"))
                {
                    m_renderTargetEnum = RenderTargetEnum::OPAQUE_TEXTURE;
                }

                if (ImGui::MenuItem("ShadowMap"))
                {
                    m_renderTargetEnum = RenderTargetEnum::SHADOWMAP;
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

#pragma region  Settings panel
        m_SceneHierarchyPanel.OnImGuiRender();

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
        ImGui::ColorEdit4("Diffuse", m_DiffuseColor);
        ImGui::SliderFloat("Metallic", &m_Metallic, 0.0, 1.0);
        ImGui::SliderFloat("Roughness", &m_Roughness, 0.0, 1.0);
        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("ViewPort");
        m_ViewPortFocused = ImGui::IsWindowFocused();
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
        switch (m_renderTargetEnum) 
        {
            case RenderTargetEnum::OPAQUE_TEXTURE:
                textureID = m_FrameBuffer->GetColorAttachmentRendererID();
                ImGui::Image((void*)textureID, ImVec2(m_FrameBuffer->GetSpecification().width, m_FrameBuffer->GetSpecification().height), ImVec2(0, 1), ImVec2(1, 0));
			    break;
			case RenderTargetEnum::SHADOWMAP:
                glBindTextureUnit(0, m_ShadowMap->GetRendererID());
                textureID = m_ShadowMap->GetRendererID();
                ImGui::Image((void*)textureID, ImVec2(m_ShadowMap->GetWidth(), m_ShadowMap->GetHeight()), ImVec2(0, 1), ImVec2(1, 0));
				break;

        
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