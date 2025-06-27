#include "SceneViewLayer.h"
#include <Hazel/Gfx/RenderStruct.h>
#include <Hazel/Renderer/TextureBuffer.h>
#include <Hazel/Gfx/Culling.h>
#include "Platform/D3D12/D3D12Buffer.h"
#include "Platform/D3D12/D3D12Shader.h"
#include "Platform/D3D12/D3D12VertexArray.h"
#include "Hazel/Renderer/VertexArray.h"
#include "Hazel/Asset/MaterialLibrary.h"
#include "Hazel/RHI/Interface/IGfxViewManager.h"
#include "Hazel/RHI/Interface/ICommandListManager.h"
#include "Hazel/RHI/Core/CommandList.h"
#include "Hazel/RHI/Core/ScopedCommandList.h"
#include "Hazel/RHI/Interface/IPipelineStateManager.h"
#include "Hazel/RHI/Interface/PipelineTypes.h"


namespace Hazel
{

    SceneViewLayer::SceneViewLayer(Window& window)
        :Layer("SceneViewLayer"),
        m_window(window)
    {

    }

    void SceneViewLayer::OnAttach()
    {
        my_texture_srv_gpu_handle.ptr = -1;
        //�������Ż���
        D3D12RenderAPIManager* renderAPIManager = dynamic_cast<D3D12RenderAPIManager*>(RenderAPIManager::getInstance()->GetManager().get());
        Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();

        HRESULT hr = S_OK;
        assert(SUCCEEDED(hr));

		IGfxViewManager& gfxViewManager = IGfxViewManager::Get();

        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.NodeMask = 1;

        mCommandQueue = renderAPIManager->GetCommandQueue();
        //hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue));
        //assert(SUCCEEDED(hr));



        TextureBufferSpecification spec = { 800, 600, TextureType::TEXTURE2D, TextureFormat::RGBA32, TextureRenderUsage::RENDER_TARGET, MultiSample::NONE};

        m_BackBuffer = TextureBuffer::Create(spec);
        m_DepthBuffer = TextureBuffer::Create({ 800, 600, TextureType::TEXTURE2D, TextureFormat::DEPTH24STENCIL8, TextureRenderUsage::RENDER_TARGET, MultiSample::NONE });

        m_ColorShader = Shader::Create("assets/shaders/color.hlsl");

		MaterialLibrary& materialLibrary = MaterialLibrary::Get();
		material = materialLibrary.LoadMaterial("assets/Materials/TestMat.meta");
		//auto resourceBinding = material->GetShader()->GetReflection()->ReflectResourceBindings();
		//auto parameters = material->GetShader()->GetReflection()->ReflectRegisterBlocks();
        

        std::string abpath = std::filesystem::current_path().u8string();
        std::string cubeModelPath = abpath + std::string("/assets/Resources/Models/Cube/Cube.obj");
        mesh = Mesh::Create();
        mesh->LoadMesh(cubeModelPath);

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
        
        
        D3D12VertexArray* vertexArray = dynamic_cast<D3D12VertexArray*>
            (mesh->meshData.get());

        D3D12Shader* d3d12Shader = dynamic_cast<D3D12Shader*>(m_ColorShader.get());
		mInputLayout = d3d12Shader->GetD3D12InputLayout();
        mvsByteCode = d3d12Shader->GetVSByteCode();
        mpsByteCode = d3d12Shader->GetPSByteCode();
        psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size()};
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



		auto& pipelineStateManager = IPipelineStateManager::Get();

		GraphicsPipelineDesc desc = {};
        desc.SetBlendState(BlendStateDesc::Opaque())
            .SetDepthStencilState(DepthStencilStateDesc::Default())
            .SetPrimitiveTopology(PrimitiveTopology::TriangleList)
            .SetRasterizerState(RasterizerStateDesc::Default())
            .SetShader(m_ColorShader);
        auto pipelineState = pipelineStateManager.CreateGraphicsPipeline(desc);


        // No need to execute command list here - initialization commands are immediate
        // mCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&rawCommandList);
        // renderAPIManager->FlushCommandQueue();
        //FlushCommandQueue();

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


  //      // 直接使用mWorld中的数据，确保内存布局兼容
        material->Set<glm::mat4>("gWorldViewProj", *reinterpret_cast<glm::mat4*>(&mWorld));
        material->Set("baseColor", glm::vec4(1.0, 0.0, 0.0, 1.0));
        material->SyncToRawData();
        auto propertyBlock = material->GetPropertyBlock(0, 0);
		std::vector<float> rawData;
        if (propertyBlock != nullptr) 
        {
            rawData = propertyBlock->RawData;
        }
        UINT32 size = rawData.size()* sizeof(float);
        objectCB = ConstantBuffer::Create(size);
        objectCB->SetData(rawData.data(), size);
		gfxViewManager.CreateConstantBufferView(objectCB);
    }

    void SceneViewLayer::OnDetach()
    {

    }

    void SceneViewLayer::OnUpdate(Timestep ts)
    {
        D3D12RenderAPIManager* renderAPIManager = dynamic_cast<D3D12RenderAPIManager*>(RenderAPIManager::getInstance()->GetManager().get());
        Microsoft::WRL::ComPtr<ID3D12Device> device = renderAPIManager->GetD3DDevice();

        auto& manager = ICommandListManager::Get();
        manager.BeginFrame(getCurrentFrameId());
        currentFrameID++;

        ID3D12Fence* fence = NULL;
        HRESULT hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
        assert(SUCCEEDED(hr));


		ScopedCommandList cmdList(CommandListType::Graphics);
        Ref<CommandList> m_cmdList = cmdList.Get();

        // 添加空指针检查
        if (!m_cmdList) {
            HZ_CORE_ERROR("Failed to get CommandList from ScopedCommandList");
            return;
        }
        
        void* nativeCommandList = m_cmdList->GetNativeCommandList();
        void* nativeAllocator = m_cmdList->GetNativeAllocator();
        
        if (!nativeCommandList || !nativeAllocator) {
            HZ_CORE_ERROR("Failed to get native D3D12 objects");
            return;
        }
        
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList = static_cast<ID3D12GraphicsCommandList*>(nativeCommandList);
        ID3D12CommandAllocator* rawAllocator = static_cast<ID3D12CommandAllocator*>(nativeAllocator);
        ThrowIfFailed(rawAllocator->Reset());
        // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
        // Reusing the command list reuses memory.
        ThrowIfFailed(m_CommandList->Reset(rawAllocator, mPSO.Get()));


        m_CommandList->RSSetViewports(1, &mScreenViewport);
        m_CommandList->RSSetScissorRects(1, &mScissorRect);
        // ������Դ�����л���
        if (m_BackBuffer->GetTextureRenderUsage() == TextureRenderUsage::RENDER_TEXTURE)
        {
            cmdList->ChangeResourceState(m_BackBuffer, TextureRenderUsage::RENDER_TEXTURE, TextureRenderUsage::RENDER_TARGET);
        }

        IGfxViewManager& gfxViewManager = IGfxViewManager::Get();
        DescriptorAllocation renderTargetHandle = gfxViewManager.CreateRenderTargetView(m_BackBuffer);
        DescriptorAllocation deptgBufferHandle = gfxViewManager.CreateDepthStencilView(m_DepthBuffer);



        D3D12_CPU_DESCRIPTOR_HANDLE depthHandle = D3D12_CPU_DESCRIPTOR_HANDLE{ deptgBufferHandle.baseHandle.cpuHandle };
        D3D12_CPU_DESCRIPTOR_HANDLE rtDescHandle = D3D12_CPU_DESCRIPTOR_HANDLE{ renderTargetHandle.baseHandle.cpuHandle };

        cmdList->ClearRenderTargetView(m_BackBuffer, Color::White);
        m_CommandList->ClearDepthStencilView(depthHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
        m_CommandList->OMSetRenderTargets(1, &rtDescHandle, true, &depthHandle);


        //����ط��Ǽ���ʹ�õ���heap������ʹ�õ���CBV���ͼ������heap��
        auto d3dCbvHeap = static_cast<ID3D12DescriptorHeap*>(gfxViewManager.GetHeap(DescriptorHeapType::CbvSrvUav));
        ID3D12DescriptorHeap* descriptorHeaps[] = { d3dCbvHeap };
        m_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

        m_CommandList->SetGraphicsRootSignature(mRootSignature.Get());

        D3D12VertexBuffer* positionBuffer = nullptr;
        auto vertexBufferIter = mesh->meshData->GetVertexBuffers().find(VertexProperty::Position);
        if (vertexBufferIter != mesh->meshData->GetVertexBuffers().end()) {
            positionBuffer = dynamic_cast<D3D12VertexBuffer*>(vertexBufferIter->second.get());
        }

        D3D12VertexBuffer* normalBuffer = nullptr;
        auto normalBufferIter = mesh->meshData->GetVertexBuffers().find(VertexProperty::Normal);
        if (normalBufferIter != mesh->meshData->GetVertexBuffers().end()) {
            normalBuffer = dynamic_cast<D3D12VertexBuffer*>(normalBufferIter->second.get());
        }

        D3D12VertexBuffer* tangentBuffer = nullptr;
        auto tangentBufferIter = mesh->meshData->GetVertexBuffers().find(VertexProperty::Tangent);
        if (tangentBufferIter != mesh->meshData->GetVertexBuffers().end()) {
            tangentBuffer = dynamic_cast<D3D12VertexBuffer*>(tangentBufferIter->second.get());
        }

        D3D12VertexBuffer* texcoordBuffer = nullptr;
        auto texcoordBufferIter = mesh->meshData->GetVertexBuffers().find(VertexProperty::TexCoord0);
        if (texcoordBufferIter != mesh->meshData->GetVertexBuffers().end()) {
            texcoordBuffer = dynamic_cast<D3D12VertexBuffer*>(texcoordBufferIter->second.get());
        }

        D3D12VertexBuffer* texcoord1Buffer = nullptr;
        auto texcoord1BufferIter = mesh->meshData->GetVertexBuffers().find(VertexProperty::TexCoord1);
        if (texcoord1BufferIter != mesh->meshData->GetVertexBuffers().end()) {
            texcoord1Buffer = dynamic_cast<D3D12VertexBuffer*>(texcoord1BufferIter->second.get());
        }

        D3D12VertexBuffer* colorBuffer = nullptr;
        auto colorBufferIter = mesh->meshData->GetVertexBuffers().find(VertexProperty::VertexColor);
        if (colorBufferIter != mesh->meshData->GetVertexBuffers().end()) {
            colorBuffer = dynamic_cast<D3D12VertexBuffer*>(colorBufferIter->second.get());
        }





        D3D12IndexBuffer* indexBuffer = dynamic_cast<D3D12IndexBuffer*>(mesh->meshData->GetIndexBuffer().get());
        // Create vertex buffer views for each vertex property
        D3D12_VERTEX_BUFFER_VIEW positionVBV;
        if (positionBuffer) {
            positionVBV.BufferLocation = positionBuffer->VertexBufferGPU->GetGPUVirtualAddress();
            positionVBV.StrideInBytes = positionBuffer->GetStride();
            positionVBV.SizeInBytes = positionBuffer->GetCount();
        }

        D3D12_VERTEX_BUFFER_VIEW normalVBV;
        if (normalBuffer) {
            normalVBV.BufferLocation = normalBuffer->VertexBufferGPU->GetGPUVirtualAddress();
            normalVBV.StrideInBytes = normalBuffer->GetStride();
            normalVBV.SizeInBytes = normalBuffer->GetCount();
        }

        D3D12_VERTEX_BUFFER_VIEW tangentVBV;
        if (tangentBuffer) {
            tangentVBV.BufferLocation = tangentBuffer->VertexBufferGPU->GetGPUVirtualAddress();
            tangentVBV.StrideInBytes = tangentBuffer->GetStride();
            tangentVBV.SizeInBytes = tangentBuffer->GetCount();
        }

        D3D12_VERTEX_BUFFER_VIEW texcoordVBV;
        if (texcoordBuffer) {
            texcoordVBV.BufferLocation = texcoordBuffer->VertexBufferGPU->GetGPUVirtualAddress();
            texcoordVBV.StrideInBytes = texcoordBuffer->GetStride();
            texcoordVBV.SizeInBytes = texcoordBuffer->GetCount();
        }

        D3D12_VERTEX_BUFFER_VIEW texcoord1VBV;
        if (texcoord1Buffer) {
            texcoord1VBV.BufferLocation = texcoord1Buffer->VertexBufferGPU->GetGPUVirtualAddress();
            texcoord1VBV.StrideInBytes = texcoord1Buffer->GetStride();
            texcoord1VBV.SizeInBytes = texcoord1Buffer->GetCount();
        }

        D3D12_VERTEX_BUFFER_VIEW colorVBV;
        if (colorBuffer) {
            colorVBV.BufferLocation = colorBuffer->VertexBufferGPU->GetGPUVirtualAddress();
            colorVBV.StrideInBytes = colorBuffer->GetStride();
            colorVBV.SizeInBytes = colorBuffer->GetCount();
        }


        D3D12_INDEX_BUFFER_VIEW ibv;
        ibv.BufferLocation = indexBuffer->IndexBufferGPU->GetGPUVirtualAddress();
        ibv.Format = indexBuffer->GetIndexFormat();
        ibv.SizeInBytes = indexBuffer->GetIndexBufferSize();
        // Create an array of vertex buffer views
        D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[6];
        UINT numViews = 0;

        if (positionBuffer) {
            vertexBufferViews[numViews++] = positionVBV;
        }
        if (normalBuffer) {
            vertexBufferViews[numViews++] = normalVBV;
        }
        if (tangentBuffer) {
            vertexBufferViews[numViews++] = tangentVBV;
        }
        if (texcoordBuffer) {
            vertexBufferViews[numViews++] = texcoordVBV;
        }
        if (texcoord1Buffer) {
            vertexBufferViews[numViews++] = texcoord1VBV;
        }
        if (colorBuffer) {
            vertexBufferViews[numViews++] = colorVBV;
        }

        // Set all vertex buffer views at once
        m_CommandList->IASetVertexBuffers(0, numViews, vertexBufferViews);
        m_CommandList->IASetIndexBuffer(&ibv);
        m_CommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        auto Allocation = gfxViewManager.CreateConstantBufferView(objectCB);
        m_CommandList->SetGraphicsRootDescriptorTable(0, d3dCbvHeap->GetGPUDescriptorHandleForHeapStart());

        m_CommandList->DrawInstanced(
            indexBuffer->GetIndexBufferSize(),
            1, 0, 0);

        cmdList->ChangeResourceState(m_BackBuffer, TextureRenderUsage::RENDER_TARGET, TextureRenderUsage::RENDER_TEXTURE);

        if (my_texture_srv_gpu_handle.ptr == -1) 
        {
            DescriptorAllocation rtAllocation = gfxViewManager.CreateImGuiSRV(m_BackBuffer);
            D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = D3D12_GPU_DESCRIPTOR_HANDLE{ rtAllocation.baseHandle.gpuHandle };
            my_texture_srv_gpu_handle = gpuHandle;
        }
        cmdList->Close();
        
        ID3D12CommandList* rawCommandList = m_CommandList.Get();
        mCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&rawCommandList);
        
		renderAPIManager->FlushCommandQueue();
		manager.EndFrame();
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
        // ����طӦ�ò���ÿһ֡resize�� resize֮���൱���������Ļ��
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




}