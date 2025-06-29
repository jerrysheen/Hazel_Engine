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
        CbvSrvUav,      // 组合堆
        Sampler,        // 采样器堆
        Rtv,            // 渲染目标堆
        Dsv,            // 深度模板堆
        ImGuiSrvUav     // ImGui专用SRV/UAV堆
    };

    // 抽象的视图描述结构
    struct ViewDescription {
        // 公共属性
        enum class ViewDimension {
            Texture2D,
            Texture3D,
            TextureCube,
            Buffer
        } dimension = ViewDimension::Texture2D;
        
        enum class TextureFormat {
            Unknown,
            RGBA8_UNORM,
            BGRA8_UNORM,
            RGBA16_FLOAT,
            RGBA32_FLOAT,
            D24_UNORM_S8_UINT,
            D32_FLOAT
        } format = TextureFormat::Unknown;
        
        // 纹理相关
        uint32_t mipLevels = 1;
        uint32_t arraySize = 1;
        uint32_t mostDetailedMip = 0;
        
        // 缓冲区相关
        uint64_t bufferOffset = 0;
        uint32_t bufferElementCount = 0;
        uint32_t bufferElementSize = 0;
        
        ViewDescription() = default;
        
        // 便捷构造函数
        static ViewDescription Texture2D(TextureFormat fmt, uint32_t mips = 1) {
            ViewDescription desc;
            desc.dimension = ViewDimension::Texture2D;
            desc.format = fmt;
            desc.mipLevels = mips;
            return desc;
        }
        
        static ViewDescription Buffer(uint32_t elementCount, uint32_t elementSize, uint64_t offset = 0) {
            ViewDescription desc;
            desc.dimension = ViewDimension::Buffer;
            desc.bufferElementCount = elementCount;
            desc.bufferElementSize = elementSize;
            desc.bufferOffset = offset;
            return desc;
        }
    };

    /*
     * 使用示例：
     * 
     * // 上层代码（抽象层）- 不需要知道D3D12细节
     * ViewDescription texDesc = ViewDescription::Texture2D(
     *     ViewDescription::TextureFormat::RGBA8_UNORM, 1);
     * 
     * DescriptorHandle srvHandle = heapManager.CreateView(
     *     DescriptorType::SRV, textureBuffer.get(), &texDesc);
     * 
     * // 或者使用默认参数
     * DescriptorHandle defaultHandle = heapManager.CreateView(
     *     DescriptorType::SRV, textureBuffer.get(), nullptr);
     */

} 