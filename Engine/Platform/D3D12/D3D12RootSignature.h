#pragma once

#include "hzpch.h"
#include "Runtime/Graphics/Shader/ShaderReflection.h"
#include "D3D12Utils.h"

#include <d3d12.h>
#include <wrl.h>
#include <memory>
#include <vector>

namespace Hazel {

    // 着色器阶段枚举（匹配D3D12）
    enum class D3D12ShaderStage : uint8_t
    {
        Vertex = 0,
        Hull,
        Domain,
        Geometry,
        Pixel,
        Compute,
        Count
    };

    // 根签名条目类型
    enum D3DRootSigEntry
    {
        // 每个着色器阶段
        kD3DRootSigEntrySRV = 0,    // 着色器资源视图
        kD3DRootSigEntryCBV,        // 常量缓冲区视图
        kD3DRootSigEntrySMP,        // 采样器
        kD3DRootSigEntryCount,

        // 对所有着色器阶段可见
        kD3DRootSigEntryUAVShared = kD3DRootSigEntryCount,
    };

    // 根签名槽位
    enum D3DRootSigSlot
    {
        // 每个着色器阶段
        kD3DRootSigSlotSRV = 0,     // 包含SRV和CBV
        kD3DRootSigSlotCBV,
        kD3DRootSigSlotSMP,
        kD3DRootSigSlotCount,

        // 对所有着色器阶段可见
        kD3DRootSigSlotUAVShared = kD3DRootSigSlotCount,
    };

    // 根签名描述
    struct D3D12RootSignatureDesc
    {
        uint8_t count[kD3DRootSigEntryCount][static_cast<size_t>(D3D12ShaderStage::Count)]; // 每个着色器阶段的每种资源类型的数量
        uint8_t countUAVs : 7;      // UAV数量
        uint8_t compute : 1;        // 是否为计算着色器
        uint8_t countBICs;          // 内建常量数量

        D3D12RootSignatureDesc()
        {
            memset(this, 0, sizeof(D3D12RootSignatureDesc));
        }

        bool operator==(const D3D12RootSignatureDesc& other) const
        {
            return memcmp(this, &other, sizeof(D3D12RootSignatureDesc)) == 0;
        }

        bool operator!=(const D3D12RootSignatureDesc& other) const
        {
            return memcmp(this, &other, sizeof(D3D12RootSignatureDesc)) != 0;
        }

        void PrintLayout() const;
    };

    // 根签名类
    class D3D12RootSignature
    {
    public:
        enum
        {
            kInvalidIndex = 0xFF
        };

    public:
        D3D12RootSignature(ID3D12Device* device, const D3D12RootSignatureDesc& rootSigDesc);
        ~D3D12RootSignature();

        // 从着色器反射创建根签名
        static std::unique_ptr<D3D12RootSignature> CreateFromShaderReflection(
            ID3D12Device* device,
            const std::vector<Ref<ShaderReflection>>& reflections);

        // 获取根签名对象
        ID3D12RootSignature* GetRootSignature() const { return m_RootSignature.Get(); }

        // 获取描述
        const D3D12RootSignatureDesc& GetDesc() const { return m_Desc; }

        // 获取根参数索引
        uint8_t GetRootParameterIndex(D3DRootSigSlot slot, D3D12ShaderStage stage) const;

        // 检查是否有效
        bool IsValid() const { return m_RootSignature != nullptr; }

        // 获取根参数总数
        uint32_t GetRootParameterCount() const { return m_RootParameterCount; }

        // 从反射信息分析根签名描述
        static D3D12RootSignatureDesc AnalyzeShaderReflections(
            const std::vector<Ref<ShaderReflection>>& reflections);

    private:
        D3D12RootSignatureDesc m_Desc;
        Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
        uint8_t m_Indices[kD3DRootSigSlotCount][static_cast<size_t>(D3D12ShaderStage::Count)];
        uint32_t m_RootParameterCount;

        // 内部创建函数
        void CreateRootSignature(ID3D12Device* device);

        
        // 转换着色器阶段枚举
        static D3D12ShaderStage ConvertShaderStage(ShaderStage stage);
        
        // 转换资源类型到根签名条目
        static D3DRootSigEntry ConvertResourceTypeToEntry(ResourceType type);
        
        // 转换着色器阶段到可见性
        static D3D12_SHADER_VISIBILITY ConvertShaderStageToVisibility(D3D12ShaderStage stage);
    };

    // 根签名管理器（缓存重复的根签名）
    class D3D12RootSignatureManager
    {
    public:
        D3D12RootSignatureManager(ID3D12Device* device);
        ~D3D12RootSignatureManager();

        // 获取或创建根签名
        std::shared_ptr<D3D12RootSignature> GetOrCreateRootSignature(
            const D3D12RootSignatureDesc& desc);

        // 从着色器反射获取或创建根签名
        std::shared_ptr<D3D12RootSignature> GetOrCreateRootSignature(
            const std::vector<Ref<ShaderReflection>>& reflections);

        // 清理缓存
        void ClearCache();

    private:
        ID3D12Device* m_Device;
        std::unordered_map<size_t, std::shared_ptr<D3D12RootSignature>> m_RootSignatureCache;
        std::mutex m_CacheMutex;

        // 计算根签名描述的哈希值
        size_t HashRootSignatureDesc(const D3D12RootSignatureDesc& desc) const;
    };

} // namespace Hazel 