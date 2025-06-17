//#include "Hazel/RHI/Interface/IGfxViewManager.h"
//#include "Hazel/RHI/Interface/IDescritorAllocator.h"
#include "hzpch.h"
//namespace Hazel {
//
//void UnifiedDescriptorUsageExample() {
//    // 获取视图管理器
//    IGfxViewManager& viewManager = IGfxViewManager::Get();
//    
//    // === 单个描述符分配使用 ===
//    
//    // 1. 创建单个纹理视图 - 返回 DescriptorAllocation (count=1)
//    Ref<TextureBuffer> texture = nullptr; // 假设已创建
//    auto srvAllocation = viewManager.CreateShaderResourceView(texture);
//    
//    // 可以直接当作 DescriptorHandle 使用（隐式转换）
//    DescriptorHandle srvHandle = srvAllocation;
//    
//    // 或者显式获取句柄
//    DescriptorHandle explicitHandle = srvAllocation.GetHandle(0);
//    
//    // 判断是否为单个描述符
//    if (srvAllocation.IsSingle()) {
//        // 单个描述符的处理逻辑
//        HZ_CORE_INFO("Single descriptor allocated");
//    }
//    
//    // === 批量描述符分配使用 ===
//    
//    // 2. 批量分配描述符
//    auto batchAllocation = viewManager.AllocateDescriptors(10, DescriptorHeapType::CbvSrvUav);
//    
//    // 遍历所有分配的描述符
//    for (uint32_t i = 0; i < batchAllocation.count; ++i) {
//        DescriptorHandle handle = batchAllocation.GetHandle(i);
//        // 在每个位置创建视图...
//        // viewManager.CreateShaderResourceView(textures[i], batchAllocation.Slice(i, 1));
//    }
//    
//    // === 连续批量视图创建 ===
//    
//    // 3. 一次创建多个连续的SRV
//    std::vector<Ref<TextureBuffer>> textures; // 假设已填充
//    auto consecutiveSRVs = viewManager.CreateConsecutiveShaderResourceViews(textures);
//    
//    // 获取第3个纹理的SRV
//    DescriptorHandle thirdTextureSRV = consecutiveSRVs.GetHandle(2);
//    
//    // === 分片操作 ===
//    
//    // 4. 从大的分配中切片出小的部分
//    auto slice = batchAllocation.Slice(2, 3); // 从索引2开始，取3个描述符
//    
//    // === 统一的资源释放 ===
//    
//    // 5. 释放描述符（假设分配器支持）
//    // allocator.Free(srvAllocation);     // 释放单个
//    // allocator.Free(batchAllocation);   // 释放批量
//    // allocator.Free(consecutiveSRVs);   // 释放连续视图
//}
//
//// 展示向后兼容性
//void BackwardCompatibilityExample() {
//    IGfxViewManager& viewManager = IGfxViewManager::Get();
//    
//    // 旧代码可能期望 DescriptorHandle
//    auto allocation = viewManager.CreateShaderResourceView(nullptr);
//    
//    // 这些都能正常工作：
//    DescriptorHandle handle1 = allocation;              // 隐式转换
//    DescriptorHandle handle2 = allocation.GetHandle();  // 显式获取
//    
//    // 新代码可以利用统一接口
//    if (allocation.count > 1) {
//        // 处理多个描述符的情况
//        for (uint32_t i = 0; i < allocation.count; ++i) {
//            DescriptorHandle individualHandle = allocation.GetHandle(i);
//            // 处理每个描述符...
//        }
//    }
//}
//
//// 性能优化示例
//void PerformanceOptimizedExample() {
//    IGfxViewManager& viewManager = IGfxViewManager::Get();
//    
//    // 批量分配比多次单独分配更高效
//    uint32_t textureCount = 100;
//    
//    // ❌ 低效：多次分配
//    // std::vector<DescriptorAllocation> individualAllocations;
//    // for (uint32_t i = 0; i < textureCount; ++i) {
//    //     auto allocation = viewManager.AllocateDescriptors(1, DescriptorHeapType::CbvSrvUav);
//    //     individualAllocations.push_back(allocation);
//    // }
//    
//    // ✅ 高效：批量分配
//    auto batchAllocation = viewManager.AllocateDescriptors(textureCount, DescriptorHeapType::CbvSrvUav);
//    
//    // 在批量分配的连续位置创建视图
//    for (uint32_t i = 0; i < textureCount; ++i) {
//        // 获取第i个位置的分配（单个描述符）
//        auto singleSlice = batchAllocation.Slice(i, 1);
//        // viewManager.CreateShaderResourceView(textures[i], singleSlice);
//    }
//}
//
//} // namespace Hazel 