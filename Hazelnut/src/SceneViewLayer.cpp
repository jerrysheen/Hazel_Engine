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
    struct D3DVertex
    {
        XMFLOAT3 Pos;
        XMFLOAT4 Color;
    };

    SceneViewLayer::SceneViewLayer(Window& window)
        :Layer("SceneViewLayer"),
        m_window(window)
    {

    }

    void SceneViewLayer::OnAttach()
    {

        //后续会优化。
        D3D12RenderAPIManager* renderAPIManager = dynamic_cast<D3D12RenderAPIManager*>(RenderAPIManager::getInstance()->GetManager().get());
        Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();

        HRESULT hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
        assert(SUCCEEDED(hr));


        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.NodeMask = 1;

        
        hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue));
        assert(SUCCEEDED(hr));



        TextureBufferSpecification spec = { 800, 600, TextureType::TEXTURE2D, TextureFormat::RGBA32, TextureRenderUsage::RENDER_TARGET, MultiSample::NONE};

        m_BackBuffer = TextureBuffer::Create(spec);
        m_DepthBuffer = TextureBuffer::Create({ 800, 600, TextureType::TEXTURE2D, TextureFormat::DEPTH24STENCIL8, TextureRenderUsage::RENDER_TARGET, MultiSample::NONE });

        Ref<CommandList> cmdList = CommandPool::getInstance()->GetCommand();
        cmdList->Reset();


        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList = cmdList->getCommandList<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>>();
        ID3D12CommandList* rawCommandList = m_CommandList.Get();
        //先硬写写死， 估计后面还要再考虑多平台的问题，也可能用一个编译器直接转过去。
        m_PbrShader = Shader::Create("assets/shaders/color.hlsl");
        mvsByteCode = d3dUtil::CompileShader(L"assets/shaders/color.hlsl", nullptr, "VS", "vs_5_0");
        mpsByteCode = d3dUtil::CompileShader(L"assets/shaders/color.hlsl", nullptr, "PS", "ps_5_0");
        // build shader layoutinput
        mInputLayout =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        // mesh gen:

        std::array<D3DVertex, 8> vertices =
        {
            D3DVertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
            D3DVertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) }),
            D3DVertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) }),
            D3DVertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),
            D3DVertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) }),
            D3DVertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }),
            D3DVertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) }),
            D3DVertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) })
        };

        std::array<std::uint16_t, 36> indices =
        {
            // front face
            0, 1, 2,
            0, 2, 3,

            // back face
            4, 6, 5,
            4, 7, 6,

            // left face
            4, 5, 1,
            4, 1, 0,

            // right face
            3, 2, 6,
            3, 6, 7,

            // top face
            1, 5, 6,
            1, 6, 2,

            // bottom face
            4, 0, 3,
            4, 3, 7
        };

        const UINT vbByteSize = (UINT)vertices.size() * sizeof(D3DVertex);
        const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

        mBoxGeo = std::make_unique<MeshGeometry>();
        mBoxGeo->Name = "boxGeo";

        //ThrowIfFailed(D3DCreateBlob(vbByteSize, &mBoxGeo->VertexBufferCPU));
        //CopyMemory(mBoxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

        //ThrowIfFailed(D3DCreateBlob(ibByteSize, &mBoxGeo->IndexBufferCPU));
        //CopyMemory(mBoxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

       mBoxGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(),
            m_CommandList.Get(), vertices.data(), vbByteSize, mBoxGeo->VertexBufferUploader);

        mBoxGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(),
            m_CommandList.Get(), indices.data(), ibByteSize, mBoxGeo->IndexBufferUploader);

        mBoxGeo->VertexByteStride = sizeof(D3DVertex);
        mBoxGeo->VertexBufferByteSize = vbByteSize;
        mBoxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
        mBoxGeo->IndexBufferByteSize = ibByteSize;

        SubmeshGeometry submesh;
        submesh.IndexCount = (UINT)indices.size();
        submesh.StartIndexLocation = 0;
        submesh.BaseVertexLocation = 0;

        mBoxGeo->DrawArgs["box"] = submesh;


        // build root signature
        
        // Shader programs typically require resources as input (constant buffers,
    // textures, samplers).  The root signature defines the resources the shader
    // programs expect.  If we think of the shader programs as a function, and
    // the input resources as function parameters, then the root signature can be
    // thought of as defining the function signature.  

    // Root parameter can be a table, root descriptor or root constants.
        CD3DX12_ROOT_PARAMETER slotRootParameter[1];

        // Create a single descriptor table of CBVs.
        CD3DX12_DESCRIPTOR_RANGE cbvTable;
        cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
        slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

        // A root signature is an array of root parameters.
        CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
        ComPtr<ID3DBlob> serializedRootSig = nullptr;
        ComPtr<ID3DBlob> errorBlob = nullptr;
        hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
            serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

        if (errorBlob != nullptr)
        {
            ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        }
        ThrowIfFailed(hr);

        ThrowIfFailed(device->CreateRootSignature(
            0,
            serializedRootSig->GetBufferPointer(),
            serializedRootSig->GetBufferSize(),
            IID_PPV_ARGS(&mRootSignature)));

        // build pso

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
        ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
        psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
        psoDesc.pRootSignature = mRootSignature.Get();
        psoDesc.VS =
        {
            reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()),
            mvsByteCode->GetBufferSize()
        };
        psoDesc.PS =
        {
            reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()),
            mpsByteCode->GetBufferSize()
        };
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;
        psoDesc.SampleDesc.Quality = 0;
        psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)));




        // Execute the initialization commands.
        //cmdList->Close();
        CommandPool::getInstance()->RecycleCommand(cmdList);

        mCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&rawCommandList);
        FlushCommandQueue();

        mScreenViewport.TopLeftX = 0;
        mScreenViewport.TopLeftY = 0;
        mScreenViewport.Width = static_cast<float>(800);
        mScreenViewport.Height = static_cast<float>(600);
        mScreenViewport.MinDepth = 0.0f;
        mScreenViewport.MaxDepth = 1.0f;

        mScissorRect = { 0, 0, 800, 600 };
      
        // Convert Spherical to Cartesian coordinates.
        float mTheta = 1.5f * XM_PI;
        float mPhi = XM_PIDIV4;
        float mRadius = 5;
        float x = mRadius * sinf(mPhi) * cosf(mTheta);
        float z = mRadius * sinf(mPhi) * sinf(mTheta);
        float y = mRadius * cosf(mPhi);

        // Build the view matrix.
        XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
        XMVECTOR target = XMVectorZero();
        XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

        XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
        XMStoreFloat4x4(&mView, view);

        XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, (800.0f/600.0f), 1.0f, 1000.0f);
        XMStoreFloat4x4(&mProj, P);

        XMMATRIX world = XMLoadFloat4x4(&mWorld);
        XMMATRIX proj = XMLoadFloat4x4(&mProj);
        XMMATRIX worldViewProj = world * view * proj;
        XMStoreFloat4x4(&mWorld, XMMatrixTranspose(worldViewProj));
        uint32_t size = sizeof(ObjectConstants);
        objectCB = ConstantBuffer::Create(size);
        objectCB->SetData(&mWorld, size);
        GfxViewManager::getInstance()->GetCbvHandle(objectCB);

        //开始加载一张贴图进来：
    }

    void SceneViewLayer::OnDetach()
    {

    }

    void SceneViewLayer::OnUpdate(Timestep ts)
    {

        
        ////boost::uuids::random_generator generator;

        ////// 生成UUID
        ////boost::uuids::uuid id = generator();

        ////// 输出UUID
        ////HZ_CORE_INFO("UUID: {0}", id);
        ////cout << id << endl;
        //// 
        ////Culling result = Camera.Cull(Scene);
        ////
        ////Camera.Render(result);
        ////defaultRenderer.AddRenderPass(opaquePass);
        ////Camera.BindRenderer(defaultRenderer);
        ////Camera* sceneCamera = new Camera(60, 1920, 1080, 0.1f, 1000.0f);
        ////Scene* scene = new Scene();
        //////sceneCamera->BindRenderer(defaultRenderer);
        ////RenderNode* node = Culling::Cull(sceneCamera, scene);
        ////RenderingData* renderingData = new RenderingData();
        ////sceneCamera->Render(node, renderingData);
        //// 
        //// 最后将这个colorattachment作为backbuffer输出。
 
        //// 
        //// 
        //// m_textureID = Camera.GetColorAttachment();
        //// 
        //// 参考unity的先写一版吧。
        //// 每个camera持有一个renderer，renderer里面有renderfeature，一个renderfeature执行一次绘制。
        //// 每个renderpass会声明自己对应的rendertarget，以及各自的渲染状态。
        //// 最后我的相机的输出就是一个cameraColorAttachment， 但是在这个地方我还要去考虑这些rt的管理。
        //// 我可能需要重构一下最底层那个代码。。就是imgui层。 把renderapi manager往上弄， 底层不需要这个东西。。
        //// 这一层layer持有一个rt？ 所有的内容其实就是叠加在这个上面进行绘制的。

        //// 所谓的sceneview到底应该干什么？这个窗口不应该承担任何渲染的逻辑，如果说sceneview底下有一个scene，然后里面有一些gameobject
        //// 那么我应该就是往这个scene里面添加gameobject，我的绘制应该在这个地方拉起来吗？ 其实感觉也不应该。。 不过先这么写吧。
        //// 后续的渲染逻辑肯定要更加细分，能想象到的就是这个地方，mtextureID的赋值应该是在postRender的地方，这个地方一切渲染的内容都已经绘制完了。
        //// 这个地方的多线程， 想一下profiler里面，相当于是我主线程提交一个pass，多线程里面就立即执行这个内容，这个过程中我们如果先不考虑feedback的东西，
        //// 那么GPU只需要按照command依次执行就好了，所以我们会看到主线程 -> 渲染线程 -> GPU 这样子的一个流程。

        //// 在这个地方尝试起一个渲染命令

        D3D12RenderAPIManager* renderAPIManager = dynamic_cast<D3D12RenderAPIManager*>(RenderAPIManager::getInstance()->GetManager().get());
        Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();

        ID3D12Fence* fence = NULL;
        HRESULT hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
        assert(SUCCEEDED(hr));


        Ref<CommandList> cmdList = CommandPool::getInstance()->GetCommand();

        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList = cmdList->getCommandList<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>>();
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator = cmdList->getCommandAllocator<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>();
        
        ThrowIfFailed(m_CommandAllocator->Reset());
        // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
        // Reusing the command list reuses memory.
        ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), mPSO.Get()));

        m_CommandList->RSSetViewports(1, &mScreenViewport);
        m_CommandList->RSSetScissorRects(1, &mScissorRect);
        // 进行资源类型切换：
        if (m_BackBuffer->GetTextureRenderUsage() == TextureRenderUsage::RENDER_TEXTURE) 
        {
            cmdList->ChangeResourceState(m_BackBuffer, TextureRenderUsage::RENDER_TEXTURE, TextureRenderUsage::RENDER_TARGET);
        }

        Ref<GfxDesc> renderTargetHandle = GfxViewManager::getInstance()->GetRtvHandle(m_BackBuffer);
        Ref<GfxDesc> deptgBufferHandle = GfxViewManager::getInstance()->GetDsvHandle(m_DepthBuffer);



        auto depthHandle = deptgBufferHandle->GetCPUDescHandle<D3D12_CPU_DESCRIPTOR_HANDLE>();
        auto rtDescHandle = renderTargetHandle->GetCPUDescHandle<D3D12_CPU_DESCRIPTOR_HANDLE>();

        cmdList->ClearRenderTargetView(renderTargetHandle, Color::White);
        m_CommandList->ClearDepthStencilView(depthHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
        m_CommandList->OMSetRenderTargets(1, &rtDescHandle, true, &depthHandle);
        
        
        //这个地方是激活使用到的heap，比如使用到了CBV，就激活这个heap。
        auto gfxViewManager = GfxViewManager::getInstance();
        auto d3dCbvHeap = gfxViewManager->GetCbvHeap()->getHeap<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>>();
        ID3D12DescriptorHeap* descriptorHeaps[] = { d3dCbvHeap.Get() };
        m_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

        m_CommandList->SetGraphicsRootSignature(mRootSignature.Get());

        m_CommandList->IASetVertexBuffers(0, 1, &mBoxGeo->VertexBufferView());
        m_CommandList->IASetIndexBuffer(&mBoxGeo->IndexBufferView());
        m_CommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


        m_CommandList->SetGraphicsRootDescriptorTable(0, d3dCbvHeap->GetGPUDescriptorHandleForHeapStart());

        m_CommandList->DrawIndexedInstanced(
            mBoxGeo->DrawArgs["box"].IndexCount,
            1, 0, 0, 0);


        cmdList->ChangeResourceState(m_BackBuffer, TextureRenderUsage::RENDER_TARGET, TextureRenderUsage::RENDER_TEXTURE);

        Ref<GfxDesc> renderTargetSrvDesc = gfxViewManager->GetSrvHandle(m_BackBuffer);
        my_texture_srv_gpu_handle = renderTargetSrvDesc->GetGPUDescHandle<D3D12_GPU_DESCRIPTOR_HANDLE>();





        //m_CommandList->OMSetRenderTargets(1, &renderTargetHandle->GetCPUDescHandle<D3D12_CPU_DESCRIPTOR_HANDLE>(), true, nullptr);

        CommandPool::getInstance()->RecycleCommand(cmdList);
        
        ID3D12CommandList* rawCommandList = m_CommandList.Get();
        mCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&rawCommandList);
        
        FlushCommandQueue();

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
          ImGui::Image((void*)my_texture_srv_gpu_handle.ptr, ImVec2(800, 600), ImVec2(0, 1), ImVec2(1, 0));
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


    // 暂时写这里，后续要变成event事件。
    void SceneViewLayer::FlushCommandQueue()
    {
            // Advance the fence value to mark commands up to this fence point.
            mCurrentFence++;

            // Add an instruction to the command queue to set a new fence point.  Because we 
            // are on the GPU timeline, the new fence point won't be set until the GPU finishes
            // processing all the commands prior to this Signal().
            ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mCurrentFence));

            // Wait until the GPU has completed commands up to this fence point.
            if (mFence->GetCompletedValue() < mCurrentFence)
            {
                HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

                // Fire event when GPU hits current fence.  
                ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFence, eventHandle));

                // Wait until the GPU hits current fence event is fired.
                WaitForSingleObject(eventHandle, INFINITE);
                CloseHandle(eventHandle);
            }
    }

}