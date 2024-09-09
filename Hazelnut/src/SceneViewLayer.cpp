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
        // 尝试直接在这个地方创建一个RT， 应该是只需要渲染到窗口上就行了。 
        // 以后所有的多线程渲染的内容， 应该都是基于这个窗口来进行的， 即最后都是渲染出一个framebuffer， 然后把colorattachement绑定在Imgui::Image上。
        // 我的所有渲染行为应该是基于相机的，所以相机会持有一个FrameBuffer，我最后应该是把相机的FrameBufffer结果注入到RT上。
        // 所以我这个地方先放一个相机，别的模型啥的后面再加入。
        // 相机后续的渲染逻辑可以参考Unity的。。 就是相机-> renderer -> renderingPipeline.
        // 应该先有一个framebuffer，相机是在这个framebuffer的逻辑之上的。
        //m_BackBuffer = Framebuffer::Create({ 1280, 720 });
                // delete model;
        D3D12_RESOURCE_DESC colorBufferDesc = {};
        colorBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        colorBufferDesc.Width = 1280;       // 设置颜色缓冲区的宽度
        colorBufferDesc.Height = 720;     // 设置颜色缓冲区的高度
        colorBufferDesc.DepthOrArraySize = 1;
        colorBufferDesc.MipLevels = 1;       // 不需要多级细化
        colorBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 颜色格式
        colorBufferDesc.SampleDesc.Count = 1;  // 不使用多重采样
        colorBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        colorBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET; // 声明这是一个渲染目标

        // 为颜色缓冲区指定清除值
        //D3D12_CLEAR_VALUE clearValue = {};
        //clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        //clearValue.Color[0] = 1.0f; 
        //clearValue.Color[1] = 1.0f;
        //clearValue.Color[2] = 1.0f;
        //clearValue.Color[3] = 1.0f;

        D3D12RenderAPIManager* renderAPIManager = static_cast<D3D12RenderAPIManager*>(Application::Get().GetRenderAPIManager().get());
        Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();
        device->CreateCommittedResource
        (
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &colorBufferDesc,
            D3D12_RESOURCE_STATE_RENDER_TARGET, // 初始状态为渲染目标
            nullptr,
            IID_PPV_ARGS(&colorBuffer)
        );

        // 获取RTV描述符的句柄
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap = renderAPIManager->GetRtvHeap();
        UINT rtvDescriptorSize = renderAPIManager->GetRtvDescriptorSize();
        rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), 0, rtvDescriptorSize;

        // 创建RTV描述符
        device->CreateRenderTargetView(colorBuffer.Get(), nullptr, rtvHandle);

        UINT handle_increment = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        int descriptor_index = renderAPIManager->GetRtvDescriptorCount(); // The descriptor table index to use (not normally a hard-coded constant, but in this case we'll assume we have slot 1 reserved for us)
        my_texture_srv_cpu_handle = renderAPIManager->GetRtvHeap()->GetCPUDescriptorHandleForHeapStart();
        //my_texture_srv_cpu_handle.ptr += (handle_increment * descriptor_index);
        my_texture_srv_gpu_handle = renderAPIManager->GetRtvHeap()->GetGPUDescriptorHandleForHeapStart();
        //my_texture_srv_gpu_handle.ptr += (handle_increment * descriptor_index);

        CD3DX12_RESOURCE_BARRIER barrierToSRV = CD3DX12_RESOURCE_BARRIER::Transition(
            colorBuffer.Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET, // 渲染结束时是 Render Target 状态
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE // 切换到纹理资源状态
        );
        renderAPIManager->GetCmdList()->ResourceBarrier(1, &barrierToSRV);


        // 创建 SRV（Shader Resource View）
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = colorBuffer->GetDesc().Format; // 与 Render Target 的格式保持一致
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;

        srvHandle = renderAPIManager->GetSrvHeap()->GetCPUDescriptorHandleForHeapStart();
        device->CreateShaderResourceView(colorBuffer.Get(), &srvDesc, srvHandle);

        my_texture_srv_gpu_handle = renderAPIManager->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart();
        ID3D12DescriptorHeap* descriptorHeaps[] = { renderAPIManager->GetCbvHeap().Get() };
        renderAPIManager->GetCmdList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    }

    void SceneViewLayer::OnDetach()
    {

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

        // 这个地方应该看一下Imgui中的texture 是怎么绑定的。
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