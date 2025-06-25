#pragma once
#include "Hazel/RHI/Interface/ICommandListAllocator.h"
#include "Hazel/Gfx/Color.h"
#include "Hazel/Renderer/TextureStruct.h"
#include "Hazel/Renderer/TextureBuffer.h"
#include <atomic>
#include <functional>

namespace Hazel 
{
	// 前向声明 - 避免循环依赖和减少编译时间
	class IGraphicsPipeline;

	// CommandList执行状态
	enum class ExecutionState {
		Idle,
		Recording,
		Closed,
		Executing,
		Completed,
		Error
	};

	class CommandList 
	{
	public:
		CommandList();
		virtual ~CommandList() = default;
		
		// 工厂方法
		static Ref<CommandList> Create(CommandListType type = CommandListType::Graphics);
		
		// 基本操作 - 添加PSO支持
		virtual void Reset() = 0;  // 保持无参数版本用于向后兼容
		virtual void Reset(Ref<IGraphicsPipeline> pipeline) = 0;  // 新增：支持PSO的版本
		virtual void Close() = 0;
		virtual void Execute() = 0;
		
		// 渲染管线操作 - 新增
		virtual void SetPipelineState(Ref<IGraphicsPipeline> pipeline) = 0;
		virtual Ref<IGraphicsPipeline> GetCurrentPipeline() const = 0;
		
		// 渲染操作
		virtual void ClearRenderTargetView(const Ref<TextureBuffer>& buffer, const glm::vec4& color) = 0;
		virtual void ChangeResourceState(const Ref<TextureBuffer>& texture, 
		                               const TextureRenderUsage& fromFormat, 
		                               const TextureRenderUsage& toFormat) = 0;
		
		// 状态管理
		ExecutionState GetState() const { return m_state.load(); }
		CommandListType GetType() const { return m_type; }
		uint64_t GetId() const { return m_id; }
		
		// 异步执行支持
		void ExecuteAsync(std::function<void()> callback = nullptr);
		bool IsCompleted() const { return m_state.load() == ExecutionState::Completed; }
		void WaitForCompletion();
		
		// 调试和性能分析
		void SetDebugName(const std::string& name) { m_debugName = name; }
		const std::string& GetDebugName() const { return m_debugName; }
		
		// 统计信息
		uint32_t GetCommandCount() const { return m_commandCount; }
		double GetLastExecutionTime() const { return m_lastExecutionTime; }
		
		// 获取原生句柄 - 简化版本，直接返回指针
		CommandListHandle GetNativeHandle() const { return m_nativeHandle; }
		void* GetNativeCommandList() const { return m_nativeHandle.commandList; }
		void* GetNativeAllocator() const { return m_nativeHandle.commandAllocator; }
		
		// 设置原生句柄（由管理器调用）
		void SetNativeHandle(const CommandListHandle& handle) { m_nativeHandle = handle; }

	protected:
		std::atomic<ExecutionState> m_state{ExecutionState::Idle};
		CommandListType m_type = CommandListType::Graphics;
		uint64_t m_id;
		std::string m_debugName;
		uint32_t m_commandCount = 0;
		double m_lastExecutionTime = 0.0;
		
		// 原生句柄
		CommandListHandle m_nativeHandle;
		
		// 当前绑定的管线状态 - 新增
		Ref<IGraphicsPipeline> m_currentPipeline;
		
		// 回调函数
		std::function<void()> m_completionCallback;
		
	private:
		static std::atomic<uint64_t> s_nextId;
		
		friend class ICommandListManager;
	};

} 