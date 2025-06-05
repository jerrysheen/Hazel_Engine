#pragma once

namespace Hazel {

    enum class DescriptorType {
        CBV,        // 常量缓冲区视图
        SRV,        // 着色器资源视图
        UAV,        // 无序访问视图
        Sampler,    // 采样器
        RTV,        // 渲染目标视图
        DSV         // 深度模板视图
    };

    enum class DescriptorHeapType {
        CbvSrvUav,  // 组合堆
        Sampler,    // 采样器堆
        Rtv,        // 渲染目标堆
        Dsv         // 深度模板堆
    };

} 