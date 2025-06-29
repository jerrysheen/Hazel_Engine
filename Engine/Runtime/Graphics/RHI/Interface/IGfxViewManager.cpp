#include "hzpch.h"
#include "IGfxViewManager.h"

#ifdef RENDER_API_DIRECTX12
#include "Platform/D3D12/D3D12GfxViewManager.h"
#elif RENDER_API_OPENGL
// TODO: Add OpenGL implementation
#endif

namespace Hazel {

    static std::unique_ptr<IGfxViewManager> s_Instance = nullptr;
    static std::mutex s_Mutex;

    IGfxViewManager& IGfxViewManager::Get() {
        std::lock_guard<std::mutex> lock(s_Mutex);
        
        if (!s_Instance) {
            // Create the appropriate implementation based on the current render API
#ifdef RENDER_API_DIRECTX12
            s_Instance = std::make_unique<D3D12GfxViewManager>();
#elif RENDER_API_OPENGL
            // TODO: Create OpenGL implementation
            HZ_CORE_ASSERT(false, "OpenGL GfxViewManager not implemented yet");
#else
            HZ_CORE_ASSERT(false, "Unknown render API");
#endif
            
            // Initialize the instance
            s_Instance->Initialize();
        }
        
        return *s_Instance;
    }

} 