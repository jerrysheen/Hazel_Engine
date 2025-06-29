#include "hzpch.h"
#include "Runtime/Graphics/RHI/Core/CommandList.h"
#include "Runtime/Graphics/RenderAPI.h"

#ifdef RENDER_API_DIRECTX12
#include "Platform/D3D12/D3D12CommandList.h"
#endif

namespace Hazel
{
	std::atomic<uint64_t> CommandList::s_nextId{1};

	CommandList::CommandList() 
		: m_id(s_nextId.fetch_add(1)) {
	}

	Ref<CommandList> CommandList::Create(CommandListType type) {
		switch(RenderAPI::GetAPI()) {
			case RenderAPI::API::None: 
				HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported");
				return nullptr;
			case RenderAPI::API::DirectX12: 
				return CreateRef<D3D12CommandList>(type);
		}
		HZ_CORE_ASSERT(false, "Unknown API...");
		return nullptr;
	}

	void CommandList::ExecuteAsync(std::function<void()> callback) {
		m_completionCallback = callback;
		m_state = ExecutionState::Executing;
		Execute();
		
		// 简单的同步实现，真正的异步需要更复杂的线程管理
		m_state = ExecutionState::Completed;
		if (m_completionCallback) {
			m_completionCallback();
		}
	}

	void CommandList::WaitForCompletion() {
		while (m_state.load() != ExecutionState::Completed && 
			   m_state.load() != ExecutionState::Error) {
			std::this_thread::sleep_for(std::chrono::microseconds(10));
		}
	}

} // namespace Hazel 