#include "hzpch.h"
#include "ICommandListManager.h"

#ifdef RENDER_API_DIRECTX12
#include "Platform/D3D12/D3D12CommandListManager.h"
#endif

namespace Hazel {

    std::unique_ptr<ICommandListManager> ICommandListManager::s_Instance = nullptr;

    ICommandListManager& ICommandListManager::Get() {
        if (!s_Instance) {
#ifdef RENDER_API_DIRECTX12
            s_Instance = std::make_unique<D3D12CommandListManager>();
#else
            HZ_CORE_ASSERT(false, "No CommandListManager implementation available for current API");
#endif
            s_Instance->Initialize();
        }
        return *s_Instance;
    }

    void ICommandListManager::Shutdown() {
        s_Instance.reset();
    }

} // namespace Hazel 