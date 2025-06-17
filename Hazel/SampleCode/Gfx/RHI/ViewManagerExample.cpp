#include "hzpch.h"
//#include "Hazel/RHI/Interface/IGfxViewManager.h"s
//#include "Hazel/Renderer/TextureBuffer.h"
//
//// Usage example for IGfxViewManager singleton
//namespace Hazel {
//
//    void ExampleUsage() {
//        // 获取单例实例 - 会自动创建并注册适当的实现（比如D3D12GfxViewManager）
//        IGfxViewManager& viewManager = IGfxViewManager::Get();
//        
//        // 创建一些纹理
//        std::vector<Ref<TextureBuffer>> textures;
//        for (int i = 0; i < 5; ++i) {
//            TextureBufferSpecification spec = { 
//                256, 256, 
//                TextureType::TEXTURE2D, 
//                TextureFormat::RGBA32, 
//                TextureRenderUsage::RENDER_TEXTURE, 
//                MultiSample::NONE 
//            };
//            textures.push_back(TextureBuffer::Create(spec));
//        }
//        
//        // 分配连续描述符并创建SRV - 这就是你想要使用的代码
//        DescriptorAllocation textureViews = viewManager.CreateConsecutiveShaderResourceViews(textures);
//        
//        // 帧管理示例
//        viewManager.BeginFrame();
//        
//        // 使用帧级分配器分配临时描述符
//        auto& frameAllocator = viewManager.GetFrameAllocator(DescriptorHeapType::CbvSrvUav);
//        DescriptorAllocation tempAllocation = frameAllocator.Allocate(10);
//        
//        // 在帧结束时清理
//        viewManager.EndFrame();
//        
//        // 创建单个视图
//        DescriptorHandle rtvHandle = viewManager.CreateRenderTargetView(textures[0]);
//        DescriptorHandle srvHandle = viewManager.CreateShaderResourceView(textures[0]);
//        
//        // 从缓存获取视图
//        DescriptorHandle cachedView = viewManager.GetCachedView(textures[0]->GetUUID(), DescriptorType::SRV);
//        
//        // 垃圾回收
//        viewManager.GarbageCollect();
//    }
//
//} 