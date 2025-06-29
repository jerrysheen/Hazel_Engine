#pragma once

#include "PipelineTypes.h"
#include "Runtime/Core/Core.h"

namespace Hazel {

    class IGraphicsPipeline {
    public:
        virtual ~IGraphicsPipeline() = default;
        
        // 绑定管线状态（考虑OpenGL需要运行时设置状态）
        virtual void Bind() const = 0;
        
        // 获取管线描述
        virtual const GraphicsPipelineDesc& GetDescription() const = 0;
        
        // 获取管线状态句柄
        virtual PipelineStateHandle GetHandle() const = 0;
        
        // 平台特定的原生对象访问（可选）
        virtual void* GetNativeHandle() const { return nullptr; }
        
        // 验证管线状态是否有效
        virtual bool IsValid() const = 0;
    };

} // namespace Hazel 