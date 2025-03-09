#include "hzpch.h"
#include "Hazel/Renderer/GraphicsPipeline.h"
#include "Hazel/Graphics/RenderAPI.h"

#include "Platform/D3D12/D3D12GraphicsPipeline.h"
#include "Platform/OpenGL/OpenGLGraphicsPipeline.h"

namespace Hazel {

    Ref<GraphicsPipeline> GraphicsPipeline::Create(const GraphicsPipelineDesc& desc)
    {
        //HZ_PROFILE_FUNCTION();

        switch (RenderAPI::GetAPI())
        {
            case RenderAPI::API::None:    
                HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported!"); 
                return nullptr;

            case RenderAPI::API::OpenGL:  
                return CreateRef<OpenGLGraphicsPipeline>(desc);

            case RenderAPI::API::DirectX12:   
                return CreateRef<D3D12GraphicsPipeline>(desc);

            // case RenderAPI::API::Vulkan:
            //     HZ_CORE_ASSERT(false, "Vulkan API is not implemented yet!");
            //     return nullptr;
        }

        HZ_CORE_ASSERT(false, "Unknown RenderAPI!");
        return nullptr;
    }

}
