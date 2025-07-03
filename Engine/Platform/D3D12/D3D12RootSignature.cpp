#include "hzpch.h"
#include "D3D12RootSignature.h"
#include "d3dx12.h"
#include <functional>
#include <algorithm>

namespace Hazel {

    // === D3D12RootSignatureDesc 实现 ===

    void D3D12RootSignatureDesc::PrintLayout() const
    {
        HZ_CORE_TRACE("Root Signature Layout:");
        HZ_CORE_TRACE("  Compute: {0}", compute ? "true" : "false");
        HZ_CORE_TRACE("  Shared UAVs: {0}", static_cast<uint32_t>(countUAVs));
        HZ_CORE_TRACE("  Built-in Constants: {0}", static_cast<uint32_t>(countBICs));
        
        const char* stageNames[] = { "VS", "HS", "DS", "GS", "PS", "CS" };
        const char* entryNames[] = { "SRV", "CBV", "SMP" };
        
        for (int stage = 0; stage < static_cast<int>(D3D12ShaderStage::Count); ++stage) {
            bool hasResources = false;
            for (int entry = 0; entry < kD3DRootSigEntryCount; ++entry) {
                if (count[entry][stage] > 0) {
                    hasResources = true;
                    break;
                }
            }
            
            if (hasResources) {
                HZ_CORE_TRACE("  {0}:", stageNames[stage]);
                for (int entry = 0; entry < kD3DRootSigEntryCount; ++entry) {
                    if (count[entry][stage] > 0) {
                        HZ_CORE_TRACE("    {0}: {1}", entryNames[entry], static_cast<uint32_t>(count[entry][stage]));
                    }
                }
            }
        }
    }

    // === D3D12RootSignature 实现 ===

    D3D12RootSignature::D3D12RootSignature(ID3D12Device* device, const D3D12RootSignatureDesc& rootSigDesc)
        : m_Desc(rootSigDesc), m_RootParameterCount(0)
    {
        HZ_CORE_ASSERT(device, "Device cannot be null");
        
        // 初始化索引数组
        memset(m_Indices, kInvalidIndex, sizeof(m_Indices));
        
        // 验证根签名描述
        bool hasValidResources = false;
        for (int stage = 0; stage < static_cast<int>(D3D12ShaderStage::Count); ++stage) {
            for (int entry = 0; entry < kD3DRootSigEntryCount; ++entry) {
                if (m_Desc.count[entry][stage] > 0) {
                    hasValidResources = true;
                    break;
                }
            }
            if (hasValidResources) break;
        }
        if (m_Desc.countUAVs > 0) {
            hasValidResources = true;
        }
        
        if (!hasValidResources) {
            HZ_CORE_WARN("Creating root signature with no resources");
        }
        
        // 创建根签名
        CreateRootSignature(device);
    }

    D3D12RootSignature::~D3D12RootSignature()
    {
        // ComPtr 会自动管理资源
    }

    std::unique_ptr<D3D12RootSignature> D3D12RootSignature::CreateFromShaderReflection(
        ID3D12Device* device,
        const std::vector<Ref<ShaderReflection>>& reflections)
    {
        D3D12RootSignatureDesc desc = AnalyzeShaderReflections(reflections);
        return std::make_unique<D3D12RootSignature>(device, desc);
    }

    uint8_t D3D12RootSignature::GetRootParameterIndex(D3DRootSigSlot slot, D3D12ShaderStage stage) const
    {
        if (slot >= kD3DRootSigSlotCount || stage >= D3D12ShaderStage::Count) {
            return kInvalidIndex;
        }
        
        return m_Indices[slot][static_cast<size_t>(stage)];
    }

    void D3D12RootSignature::CreateRootSignature(ID3D12Device* device)
    {
        // 预先计算需要的根参数数量，避免vector重新分配
        uint32_t totalRootParams = 0;
        for (int stage = 0; stage < static_cast<int>(D3D12ShaderStage::Count); ++stage) {
            for (int entry = 0; entry < kD3DRootSigEntryCount; ++entry) {
                if (m_Desc.count[entry][stage] > 0) {
                    totalRootParams++;
                }
            }
        }
        if (m_Desc.countUAVs > 0) {
            totalRootParams++;
        }
        
        // 预先分配足够的容量，避免重新分配
        std::vector<CD3DX12_ROOT_PARAMETER> rootParameters;
        std::vector<CD3DX12_DESCRIPTOR_RANGE> descriptorRanges;
        rootParameters.reserve(totalRootParams);
        descriptorRanges.reserve(totalRootParams);
        
        uint32_t rootParamIndex = 0;
        
        // 为每个着色器阶段创建根参数
        for (int stage = 0; stage < static_cast<int>(D3D12ShaderStage::Count); ++stage) {
            D3D12ShaderStage shaderStage = static_cast<D3D12ShaderStage>(stage);
            
            // 为当前阶段的每种资源类型创建描述符表
            for (int entry = 0; entry < kD3DRootSigEntryCount; ++entry) {
                uint8_t count = m_Desc.count[entry][stage];
                if (count == 0) continue;
                
                // 确定描述符范围类型
                D3D12_DESCRIPTOR_RANGE_TYPE rangeType;
                switch (static_cast<D3DRootSigEntry>(entry)) {
                    case kD3DRootSigEntrySRV:
                        rangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                        break;
                    case kD3DRootSigEntryCBV:
                        rangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
                        break;
                    case kD3DRootSigEntrySMP:
                        rangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
                        break;
                    default:
                        HZ_CORE_ERROR("Invalid root signature entry type: {0}", entry);
                        continue;
                }
                
                // 创建并初始化描述符范围
                CD3DX12_DESCRIPTOR_RANGE range;
                range.Init(rangeType, count, 0, 0);
                descriptorRanges.push_back(range);
                
                // 创建根参数（使用稳定的指针）
                CD3DX12_ROOT_PARAMETER rootParam;
                rootParam.InitAsDescriptorTable(1, &descriptorRanges[descriptorRanges.size() - 1], 
                    ConvertShaderStageToVisibility(shaderStage));
                rootParameters.push_back(rootParam);
                
                // 记录根参数索引
                D3DRootSigSlot slot = static_cast<D3DRootSigSlot>(entry);
                m_Indices[slot][stage] = static_cast<uint8_t>(rootParamIndex);
                rootParamIndex++;
                
                HZ_CORE_TRACE("Created root parameter {0} for stage {1}, entry {2}, count {3}", 
                    rootParamIndex - 1, stage, entry, count);
            }
        }
        
        // 为共享UAV创建根参数
        if (m_Desc.countUAVs > 0) {
            CD3DX12_DESCRIPTOR_RANGE range;
            range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, m_Desc.countUAVs, 0, 0);
            descriptorRanges.push_back(range);
            
            CD3DX12_ROOT_PARAMETER rootParam;
            rootParam.InitAsDescriptorTable(1, &descriptorRanges[descriptorRanges.size() - 1], 
                D3D12_SHADER_VISIBILITY_ALL);
            rootParameters.push_back(rootParam);
            
            // 记录共享UAV的索引
            for (int stage = 0; stage < static_cast<int>(D3D12ShaderStage::Count); ++stage) {
                m_Indices[kD3DRootSigSlotUAVShared][stage] = static_cast<uint8_t>(rootParamIndex);
            }
            rootParamIndex++;
            
            HZ_CORE_TRACE("Created shared UAV root parameter {0} with count {1}", 
                rootParamIndex - 1, static_cast<uint32_t>(m_Desc.countUAVs));
        }
        
        m_RootParameterCount = rootParamIndex;
        
        // 验证数据一致性
        if (rootParameters.size() != descriptorRanges.size()) {
            HZ_CORE_ERROR("Mismatch between root parameters and descriptor ranges: {0} vs {1}", 
                rootParameters.size(), descriptorRanges.size());
            return;
        }
        
        if (rootParameters.size() != totalRootParams) {
            HZ_CORE_ERROR("Unexpected root parameter count: expected {0}, got {1}", 
                totalRootParams, rootParameters.size());
            return;
        }
        
        // 验证描述符范围的有效性
        for (size_t i = 0; i < descriptorRanges.size(); ++i) {
            const auto& range = descriptorRanges[i];
            if (range.RangeType < D3D12_DESCRIPTOR_RANGE_TYPE_SRV || 
                range.RangeType > D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER) {
                HZ_CORE_ERROR("Invalid descriptor range type at index {0}: {1}", i, -1);
                return;
            }
            HZ_CORE_TRACE("Descriptor range {0}: Type={1}, Count={2}, BaseShaderRegister={3}, RegisterSpace={4}", 
                i, -1, range.NumDescriptors, range.BaseShaderRegister, range.RegisterSpace);
        }
        
        HZ_CORE_INFO("Creating root signature with {0} root parameters", rootParameters.size());
        
        // 创建根签名描述
        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init(
            static_cast<UINT>(rootParameters.size()),
            rootParameters.empty() ? nullptr : rootParameters.data(),
            0, nullptr,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
        );
        
        // 序列化根签名
        Microsoft::WRL::ComPtr<ID3DBlob> signature;
        Microsoft::WRL::ComPtr<ID3DBlob> error;
        
        HRESULT hr = D3D12SerializeRootSignature(
            &rootSignatureDesc,
            D3D_ROOT_SIGNATURE_VERSION_1_0,
            &signature,
            &error
        );
        
        if (FAILED(hr)) {
            std::string errorMsg = "Failed to serialize root signature";
            if (error) {
                errorMsg += ": ";
                errorMsg += static_cast<char*>(error->GetBufferPointer());
            }
            HZ_CORE_ERROR(errorMsg);
            return;
        }
        
        // 创建根签名对象
        hr = device->CreateRootSignature(
            0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&m_RootSignature)
        );
        
        if (FAILED(hr)) {
            HZ_CORE_ERROR("Failed to create root signature");
            return;
        }
        
        // 打印根签名布局（调试用）
        #ifdef HZ_DEBUG
        m_Desc.PrintLayout();
        #endif
    }

    D3D12RootSignatureDesc D3D12RootSignature::AnalyzeShaderReflections(
        const std::vector<Ref<ShaderReflection>>& reflections)
    {
        D3D12RootSignatureDesc desc;
        
        HZ_CORE_TRACE("Analyzing {0} shader reflections", reflections.size());
        
        for (const auto& reflection : reflections) {
            if (!reflection) {
                HZ_CORE_WARN("Null shader reflection encountered, skipping");
                continue;
            }
            
            // 获取所有可用的着色器阶段
            auto stages = reflection->GetAvailableStages();
            
            for (ShaderStage stage : stages) {
                D3D12ShaderStage d3d12Stage = ConvertShaderStage(stage);
                if (d3d12Stage >= D3D12ShaderStage::Count) continue;
                
                size_t stageIndex = static_cast<size_t>(d3d12Stage);
                
                // 获取该阶段的资源信息
                auto* stageInfo = reflection->GetStageResources(stage);
                if (!stageInfo) continue;
                
                // 统计各种资源类型的数量
                for (const auto& binding : stageInfo->resourceBindings) {
                    D3DRootSigEntry entry = ConvertResourceTypeToEntry(binding.Type);
                    if (entry < kD3DRootSigEntryCount) {
                        desc.count[entry][stageIndex]++;
                        HZ_CORE_TRACE("Found resource: {0} at stage {1}, entry {2}, bind point {3}", 
                            binding.Name, -1 , 0, binding.BindPoint);
                    } else if (entry == kD3DRootSigEntryUAVShared) {
                        desc.countUAVs++;
                        HZ_CORE_TRACE("Found shared UAV: {0} at bind point {1}", 
                            binding.Name, binding.BindPoint);
                    } else {
                        HZ_CORE_WARN("Unknown resource type for binding: {0}", binding.Name);
                    }
                }
                
                // 检查是否为计算着色器
                if (stage == ShaderStage::Compute) {
                    desc.compute = 1;
                }
            }
        }
        
        // 输出分析结果
        HZ_CORE_INFO("Root signature analysis complete:");
        bool hasAnyResources = false;
        for (int stage = 0; stage < static_cast<int>(D3D12ShaderStage::Count); ++stage) {
            for (int entry = 0; entry < kD3DRootSigEntryCount; ++entry) {
                if (desc.count[entry][stage] > 0) {
                    hasAnyResources = true;
                    const char* stageNames[] = { "VS", "HS", "DS", "GS", "PS", "CS" };
                    const char* entryNames[] = { "SRV", "CBV", "SMP" };
                    HZ_CORE_INFO("  {0}: {1} = {2}", 
                        stageNames[stage], entryNames[entry], static_cast<uint32_t>(desc.count[entry][stage]));
                }
            }
        }
        if (desc.countUAVs > 0) {
            hasAnyResources = true;
            HZ_CORE_INFO("  Shared UAVs = {0}", static_cast<uint32_t>(desc.countUAVs));
        }
        if (!hasAnyResources) {
            HZ_CORE_WARN("No resources found in shader reflections - root signature will be empty");
        }
        
        return desc;
    }

    D3D12ShaderStage D3D12RootSignature::ConvertShaderStage(ShaderStage stage)
    {
        switch (stage) {
            case ShaderStage::Vertex: return D3D12ShaderStage::Vertex;
            case ShaderStage::Hull: return D3D12ShaderStage::Hull;
            case ShaderStage::Domain: return D3D12ShaderStage::Domain;
            case ShaderStage::Geometry: return D3D12ShaderStage::Geometry;
            case ShaderStage::Pixel: return D3D12ShaderStage::Pixel;
            case ShaderStage::Compute: return D3D12ShaderStage::Compute;
            default: return D3D12ShaderStage::Count;
        }
    }

    D3DRootSigEntry D3D12RootSignature::ConvertResourceTypeToEntry(ResourceType type)
    {
        switch (type) {
            case ResourceType::ShaderResource: return kD3DRootSigEntrySRV;
            case ResourceType::ConstantBuffer: return kD3DRootSigEntryCBV;
            case ResourceType::Sampler: return kD3DRootSigEntrySMP;
            case ResourceType::UnorderedAccess: return kD3DRootSigEntryUAVShared;
            default: return kD3DRootSigEntryCount; // 无效类型
        }
    }

    D3D12_SHADER_VISIBILITY D3D12RootSignature::ConvertShaderStageToVisibility(D3D12ShaderStage stage)
    {
        switch (stage) {
            case D3D12ShaderStage::Vertex: return D3D12_SHADER_VISIBILITY_VERTEX;
            case D3D12ShaderStage::Hull: return D3D12_SHADER_VISIBILITY_HULL;
            case D3D12ShaderStage::Domain: return D3D12_SHADER_VISIBILITY_DOMAIN;
            case D3D12ShaderStage::Geometry: return D3D12_SHADER_VISIBILITY_GEOMETRY;
            case D3D12ShaderStage::Pixel: return D3D12_SHADER_VISIBILITY_PIXEL;
            case D3D12ShaderStage::Compute: return D3D12_SHADER_VISIBILITY_ALL;
            default: return D3D12_SHADER_VISIBILITY_ALL;
        }
    }

    // === D3D12RootSignatureManager 实现 ===

    D3D12RootSignatureManager::D3D12RootSignatureManager(ID3D12Device* device)
        : m_Device(device)
    {
        HZ_CORE_ASSERT(device, "Device cannot be null");
    }

    D3D12RootSignatureManager::~D3D12RootSignatureManager()
    {
        ClearCache();
    }

    std::shared_ptr<D3D12RootSignature> D3D12RootSignatureManager::GetOrCreateRootSignature(
        const D3D12RootSignatureDesc& desc)
    {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        
        size_t hash = HashRootSignatureDesc(desc);
        
        auto it = m_RootSignatureCache.find(hash);
        if (it != m_RootSignatureCache.end()) {
            return it->second;
        }
        
        // 创建新的根签名
        auto rootSignature = std::make_shared<D3D12RootSignature>(m_Device, desc);
        if (rootSignature->IsValid()) {
            m_RootSignatureCache[hash] = rootSignature;
        }
        
        return rootSignature;
    }

    std::shared_ptr<D3D12RootSignature> D3D12RootSignatureManager::GetOrCreateRootSignature(
        const std::vector<Ref<ShaderReflection>>& reflections)
    {
        D3D12RootSignatureDesc desc = D3D12RootSignature::AnalyzeShaderReflections(reflections);
        return GetOrCreateRootSignature(desc);
    }

    void D3D12RootSignatureManager::ClearCache()
    {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        m_RootSignatureCache.clear();
    }

    size_t D3D12RootSignatureManager::HashRootSignatureDesc(const D3D12RootSignatureDesc& desc) const
    {
        // 使用简单的哈希函数
        size_t hash = 0;
        const uint8_t* data = reinterpret_cast<const uint8_t*>(&desc);
        
        for (size_t i = 0; i < sizeof(D3D12RootSignatureDesc); ++i) {
            hash = hash * 31 + data[i];
        }
        
        return hash;
    }

} // namespace Hazel 