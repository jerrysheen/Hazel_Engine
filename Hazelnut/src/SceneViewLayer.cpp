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

        //ID3D12CommandAllocator* cmdAlloc = NULL;
        //hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAlloc));
        //assert(SUCCEEDED(hr));

        //ID3D12GraphicsCommandList* cmdList = NULL;
        //hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAlloc, NULL, IID_PPV_ARGS(&cmdList));
        //assert(SUCCEEDED(hr));
        Ref<CommandList> cmdList = CommandPool::getInstance()->GetCommand();
        cmdList->Reset();



        TextureBufferSpecification spec = { 1280, 720, TextureType::TEXTURE2D, TextureFormat::RGBA32, TextureRenderUsage::RENDER_TARGET, MultiSample::NONE};

        Ref<TextureBuffer> m_BackBuffer = TextureBuffer::Create(spec);
        //D3D12_RESOURCE_DESC colorBufferDesc = {};
        //colorBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        //colorBufferDesc.Width = 1280;       // 设置颜色缓冲区的宽度
        //colorBufferDesc.Height = 720;     // 设置颜色缓冲区的高度
        //colorBufferDesc.DepthOrArraySize = 1;
        //colorBufferDesc.MipLevels = 1;       // 不需要多级细化
        //colorBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 颜色格式
        //colorBufferDesc.SampleDesc.Count = 1;  // 不使用多重采样
        //colorBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        //colorBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET; // 声明这是一个渲染目标

        //// 为颜色缓冲区指定清除值
        //D3D12_CLEAR_VALUE clearValue = {};
        //clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        //clearValue.Color[0] = 1.0f;
        //clearValue.Color[1] = 1.0f;
        //clearValue.Color[2] = 1.0f;
        //clearValue.Color[3] = 1.0f;


        //device->CreateCommittedResource
        //(
        //    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        //    D3D12_HEAP_FLAG_NONE,
        //    &colorBufferDesc,
        //    D3D12_RESOURCE_STATE_RENDER_TARGET, // 初始状态为渲染目标
        //    &clearValue,
        //    IID_PPV_ARGS(&colorBuffer)
        //);

        //// 获取RTV描述符的句柄
        //Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap = renderAPIManager->GetRtvHeap();
        //UINT rtvDescriptorSize = renderAPIManager->GetRtvDescriptorSize();
        //rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), 0, rtvDescriptorSize;

        //// 创建RTV描述符
        //device->CreateRenderTargetView(colorBuffer.Get(), nullptr, rtvHandle);
        //cmdList->ClearRenderTargetView(rtvHandle, Colors::White, 0, nullptr);
        //UINT handle_increment = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        //int descriptor_index = renderAPIManager->GetRtvDescriptorCount(); // The descriptor table index to use (not normally a hard-coded constant, but in this case we'll assume we have slot 1 reserved for us)
        //my_texture_srv_cpu_handle = renderAPIManager->GetRtvHeap()->GetCPUDescriptorHandleForHeapStart();
        Ref<GfxDesc> renderTargetHandle = GfxViewManager::getInstance()->GetRtvHandle(m_BackBuffer);
        cmdList->ClearRenderTargetView(renderTargetHandle, Color::White);


        cmdList->ChangeResourceState(m_BackBuffer, TextureRenderUsage::RENDER_TARGET, TextureRenderUsage::RENDER_TEXTURE);


        // 创建 SRV（Shader Resource View）
        //D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        //srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        //srvDesc.Format = colorBuffer->GetDesc().Format; // 与 Render Target 的格式保持一致
        //srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        //srvDesc.Texture2D.MipLevels = 1;

        //srvHandle = renderAPIManager->GetCbvHeap()->GetCPUDescriptorHandleForHeapStart();
        //srvHandle.Offset(1, handle_increment);
        //device->CreateShaderResourceView(colorBuffer.Get(), &srvDesc, srvHandle);

        //my_texture_srv_gpu_handle = renderAPIManager->GetCbvHeap()->GetGPUDescriptorHandleForHeapStart();
        //my_texture_srv_gpu_handle.ptr += (handle_increment * 1);
        //ID3D12DescriptorHeap* descriptorHeaps[] = { renderAPIManager->GetCbvHeap().Get() };
        //cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

        auto gfxViewManager = GfxViewManager::getInstance();
        Ref<GfxDesc> renderTargetSrvDesc = gfxViewManager->GetSrvHandle(m_BackBuffer);
        my_texture_srv_gpu_handle = renderTargetSrvDesc->GetGPUDescHandle<D3D12_GPU_DESCRIPTOR_HANDLE>();
        cmdList->BindCbvHeap(gfxViewManager->GetCBVHeap());

        cmdList->Close();
        //CommandPool::ExecuteCommand(cmdList);

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

        //// 生成UUID
        //boost::uuids::uuid id = generator();

        //// 输出UUID
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
        // 最后将这个colorattachment作为backbuffer输出。
 
        // 
        // 
        // m_textureID = Camera.GetColorAttachment();
        // 
        // 参考unity的先写一版吧。
        // 每个camera持有一个renderer，renderer里面有renderfeature，一个renderfeature执行一次绘制。
        // 每个renderpass会声明自己对应的rendertarget，以及各自的渲染状态。
        // 最后我的相机的输出就是一个cameraColorAttachment， 但是在这个地方我还要去考虑这些rt的管理。
        // 我可能需要重构一下最底层那个代码。。就是imgui层。 把renderapi manager往上弄， 底层不需要这个东西。。
        // 这一层layer持有一个rt？ 所有的内容其实就是叠加在这个上面进行绘制的。

        // 所谓的sceneview到底应该干什么？这个窗口不应该承担任何渲染的逻辑，如果说sceneview底下有一个scene，然后里面有一些gameobject
        // 那么我应该就是往这个scene里面添加gameobject，我的绘制应该在这个地方拉起来吗？ 其实感觉也不应该。。 不过先这么写吧。
        // 后续的渲染逻辑肯定要更加细分，能想象到的就是这个地方，mtextureID的赋值应该是在postRender的地方，这个地方一切渲染的内容都已经绘制完了。
        // 这个地方的多线程， 想一下profiler里面，相当于是我主线程提交一个pass，多线程里面就立即执行这个内容，这个过程中我们如果先不考虑feedback的东西，
        // 那么GPU只需要按照command依次执行就好了，所以我们会看到主线程 -> 渲染线程 -> GPU 这样子的一个流程。

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