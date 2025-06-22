#include "hzpch.h"
#include "IPipelineStateManager.h"

#ifdef RENDER_API_DIRECTX12
#include "Platform/D3D12/D3D12PipelineStateManager.h"
#elif RENDER_API_OPENGL
// TODO: Add OpenGL implementation
#endif

namespace Hazel {

    static std::unique_ptr<IPipelineStateManager> s_Instance = nullptr;
    static std::mutex s_Mutex;

    IPipelineStateManager& IPipelineStateManager::Get() {
        std::lock_guard<std::mutex> lock(s_Mutex);
        
        if (!s_Instance) {
            // Create the appropriate implementation based on the current render API
#ifdef RENDER_API_DIRECTX12
            s_Instance = std::make_unique<D3D12PipelineStateManager>();
#elif RENDER_API_OPENGL
            // TODO: Create OpenGL implementation
            HZ_CORE_ASSERT(false, "OpenGL PipelineStateManager not implemented yet");
#else
            HZ_CORE_ASSERT(false, "Unknown render API");
#endif
        }
        
        return *s_Instance;
    }

    void IPipelineStateManager::Shutdown() {
        std::lock_guard<std::mutex> lock(s_Mutex);
        if (s_Instance) {
            // Clean up any cached pipelines before shutdown
            s_Instance->GarbageCollect();
            s_Instance.reset();
        }
    }

} // namespace Hazel 