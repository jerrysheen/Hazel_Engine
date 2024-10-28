#include "SceneViewLayer.h"
#include <Hazel/Gfx/RenderStruct.h>
#include <Hazel/Renderer/TextureBuffer.h>
#include <Hazel/Gfx/Culling.h>
#include "Hazel/Gfx/CommandPool.h"
#include "Hazel/Gfx/GfxViewManager.h"
#include "Hazel/Gfx/GfxDesc.h"
#include "Hazel/Gfx/GfxDesc.h"

namespace Hazel
{
    SceneViewLayer::SceneViewLayer(Window& window)
        :Layer("SceneViewLayer"),
        m_window(window)
    {

    }

    void SceneViewLayer::OnAttach()
    {


        D3D12RenderAPIManager* renderAPIManager = static_cast<D3D12RenderAPIManager*>(Application::Get().GetRenderAPIManager().get());
        Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();

        ID3D12Fence* fence = NULL;
        HRESULT hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
        assert(SUCCEEDED(hr));

        HANDLE event = CreateEvent(0, 0, 0, 0);
        assert(event != NULL);

        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.NodeMask = 1;

        ID3D12CommandQueue* cmdQueue = NULL;
        hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&cmdQueue));
        assert(SUCCEEDED(hr));

        Ref<CommandList> cmdList = CommandPool::getInstance()->GetCommand();
        cmdList->Reset();

        TextureBufferSpecification spec = { 1280, 720, TextureType::TEXTURE2D, TextureFormat::RGBA32, TextureRenderUsage::RENDER_TARGET, MultiSample::NONE};

        m_BackBuffer = TextureBuffer::Create(spec);
        Ref<GfxDesc> renderTargetHandle = GfxViewManager::getInstance()->GetRtvHandle(m_BackBuffer);
        cmdList->ClearRenderTargetView(renderTargetHandle, Color::Black);

        cmdList->ChangeResourceState(m_BackBuffer, TextureRenderUsage::RENDER_TARGET, TextureRenderUsage::RENDER_TEXTURE);
        auto gfxViewManager = GfxViewManager::getInstance();
        Ref<GfxDesc> renderTargetSrvDesc = gfxViewManager->GetSrvHandle(m_BackBuffer);
        my_texture_srv_gpu_handle = renderTargetSrvDesc->GetGPUDescHandle<D3D12_GPU_DESCRIPTOR_HANDLE>();
        cmdList->BindCbvHeap(gfxViewManager->GetCBVHeap());
        cmdList->Close();

        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList = cmdList->getCommandList<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>>();
        ID3D12CommandList* rawCommandList = m_CommandList.Get();

        cmdQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&rawCommandList);
        hr = cmdQueue->Signal(fence, 1);
        IM_ASSERT(SUCCEEDED(hr));

        fence->SetEventOnCompletion(1, event);
        WaitForSingleObject(event, INFINITE);

        cmdList->Release();
        cmdQueue->Release();
        CloseHandle(event);
        fence->Release();
    }

    void SceneViewLayer::OnDetach()
    {

    }

    void SceneViewLayer::OnUpdate(Timestep ts)
    {
        //boost::uuids::random_generator generator;

        //// ����UUID
        //boost::uuids::uuid id = generator();

        //// ���UUID
        //HZ_CORE_INFO("UUID: {0}", id);
        //cout << id << endl;
        // 
        //Culling result = Camera.Cull(Scene);
        //
        //Camera.Render(result);
        //defaultRenderer.AddRenderPass(opaquePass);
        //Camera.BindRenderer(defaultRenderer);
        //Camera* sceneCamera = new Camera(60, 1920, 1080, 0.1f, 1000.0f);
        //Scene* scene = new Scene();
        ////sceneCamera->BindRenderer(defaultRenderer);
        //RenderNode* node = Culling::Cull(sceneCamera, scene);
        //RenderingData* renderingData = new RenderingData();
        //sceneCamera->Render(node, renderingData);
        // 
        // ������colorattachment��Ϊbackbuffer�����
 
        // 
        // 
        // m_textureID = Camera.GetColorAttachment();
        // 
        // �ο�unity����дһ��ɡ�
        // ÿ��camera����һ��renderer��renderer������renderfeature��һ��renderfeatureִ��һ�λ��ơ�
        // ÿ��renderpass�������Լ���Ӧ��rendertarget���Լ����Ե���Ⱦ״̬��
        // ����ҵ�������������һ��cameraColorAttachment�� ����������ط��һ�Ҫȥ������Щrt�Ĺ���
        // �ҿ�����Ҫ�ع�һ����ײ��Ǹ����롣������imgui�㡣 ��renderapi manager����Ū�� �ײ㲻��Ҫ�����������
        // ��һ��layer����һ��rt�� ���е�������ʵ���ǵ��������������л��Ƶġ�

        // ��ν��sceneview����Ӧ�ø�ʲô��������ڲ�Ӧ�óе��κ���Ⱦ���߼������˵sceneview������һ��scene��Ȼ��������һЩgameobject
        // ��ô��Ӧ�þ��������scene�������gameobject���ҵĻ���Ӧ��������ط��������� ��ʵ�о�Ҳ��Ӧ�á��� ��������ôд�ɡ�
        // ��������Ⱦ�߼��϶�Ҫ����ϸ�֣������󵽵ľ�������ط���mtextureID�ĸ�ֵӦ������postRender�ĵط�������ط�һ����Ⱦ�����ݶ��Ѿ��������ˡ�
        // ����ط��Ķ��̣߳� ��һ��profiler���棬�൱���������߳��ύһ��pass�����߳����������ִ��������ݣ������������������Ȳ�����feedback�Ķ�����
        // ��ôGPUֻ��Ҫ����command����ִ�оͺ��ˣ��������ǻῴ�����߳� -> ��Ⱦ�߳� -> GPU �����ӵ�һ�����̡�

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
        // ����ط�Ӧ�ò���ÿһ֡resize�� resize֮���൱���������Ļ��
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

        // ����ط�Ӧ�ÿ�һ��Imgui�е�texture ����ô�󶨵ġ�
        //uint32_t textureID = 0;
        //switch (m_renderTargetEnum)
        //{
        //case RenderTargetEnum::OPAQUE_TEXTURE:
        //    textureID = m_FrameBuffer->GetColorAttachmentRendererID();
          ImGui::Image((void*)my_texture_srv_gpu_handle.ptr, ImVec2(1280, 720), ImVec2(0, 1), ImVec2(1, 0));
        //    break;
        //}


        ImGui::End();
        ImGui::PopStyleVar();

#pragma endregion



        ImGui::End();
    }

    void SceneViewLayer::OnEvent(Event& e)
    {
    }

}