#pragma once

#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include "Runtime/Core/Core.h"
#include "Runtime/Graphics/RHI/Core/Buffer.h"

namespace Hazel {
	
	// 新增：Shader Stage 枚举
	enum class ShaderStage : uint32_t {
		Vertex = 0,
		Pixel = 1,
		Geometry = 2,
		Hull = 3,
		Domain = 4,
		Compute = 5,
		Count
	};
	
	// 辅助函数：将ShaderStage转换为字符串（用于调试）
	inline const char* ShaderStageToString(ShaderStage stage) {
		switch (stage) {
			case ShaderStage::Vertex: return "Vertex";
			case ShaderStage::Pixel: return "Pixel";
			case ShaderStage::Geometry: return "Geometry";
			case ShaderStage::Hull: return "Hull";
			case ShaderStage::Domain: return "Domain";
			case ShaderStage::Compute: return "Compute";
			default: return "Unknown";
		}
	}
	
	// ShaderParameter类，表示着色器中的常量参数
	struct ShaderParameter
	{
		// 这里的size 和 offset 是字节对齐的，CPU端如果需要填充数据，需要做一些转换。
		std::string Name;
		uint32_t Size;
		uint32_t Offset; 
		// 可以根据需要扩展更多属性，如类型、寄存器等
	};

	// 着色器寄存器块描述 - 表示一个寄存器(如b0)所对应的所有数据
	struct ShaderRegisterBlock
	{
		std::string Name;         // cbuffer的名称
		uint32_t BindPoint;       // 绑定点(比如b0中的0)
		uint32_t BindSpace;       // 绑定空间
		uint32_t Size;            // 缓冲区大小
		ShaderStage Stage;        // 新增：此寄存器块所属的Shader Stage
		std::vector<ShaderParameter> Parameters; // 缓冲区中包含的参数
	};

	// 资源类型枚举
	enum class ResourceType
	{
		ConstantBuffer,    // 常量缓冲区(b)
		ShaderResource,    // 着色器资源视图(t)
		UnorderedAccess,   // 无序访问视图(u)
		Sampler           // 采样器(s)
	};

	// 资源绑定描述
	struct ResourceBinding
	{
		std::string Name;
		ResourceType Type;
		uint32_t BindPoint;
		uint32_t BindSpace;
		ShaderStage Stage;        // 新增：资源所属的Shader Stage
		// 如果是ConstantBuffer类型，对应的寄存器块索引
		int RegisterBlockIndex = -1;
	};

	// 新增：Stage资源信息结构
	struct StageResourceInfo {
		ShaderStage stage;
		std::vector<ShaderRegisterBlock> registerBlocks;
		std::vector<ResourceBinding> resourceBindings;
		std::unordered_map<std::string, ShaderParameter> parameters;
		
		// 便捷查询接口
		const ShaderRegisterBlock* GetRegisterBlock(const std::string& name) const {
			for (const auto& block : registerBlocks) {
				if (block.Name == name) {
					return &block;
				}
			}
			return nullptr;
		}
		
		const ShaderRegisterBlock* GetRegisterBlock(uint32_t bindPoint, uint32_t space = 0) const {
			for (const auto& block : registerBlocks) {
				if (block.BindPoint == bindPoint && block.BindSpace == space) {
					return &block;
				}
			}
			return nullptr;
		}
		
		const ShaderParameter* GetParameter(const std::string& name) const {
			auto it = parameters.find(name);
			return (it != parameters.end()) ? &it->second : nullptr;
		}
		
		// 检查是否包含特定类型的资源
		bool HasResourceType(ResourceType type) const {
			for (const auto& binding : resourceBindings) {
				if (binding.Type == type) {
					return true;
				}
			}
			return false;
		}
		
		// 获取特定类型的资源数量
		size_t GetResourceCount(ResourceType type) const {
			size_t count = 0;
			for (const auto& binding : resourceBindings) {
				if (binding.Type == type) {
					count++;
				}
			}
			return count;
		}
	};

	// ShaderReflection类，用于分析着色器并提取元数据
	class ShaderReflection
	{
	public:
		virtual ~ShaderReflection() = default;
		
		// 新增：Stage-based 接口
		virtual std::vector<StageResourceInfo> ReflectStageResources() = 0;
		virtual StageResourceInfo* GetStageResources(ShaderStage stage) const = 0;
		virtual bool HasStage(ShaderStage stage) const = 0;
		virtual std::vector<ShaderStage> GetAvailableStages() const = 0;
		
		// InputLayout 接口（只属于顶点着色器）
		virtual BufferLayout ReflectVertexInputLayout() = 0;
		
		// 便利方法：收集所有阶段的资源（用于现有代码兼容）
		std::vector<ShaderRegisterBlock> GetAllRegisterBlocks() const {
			std::vector<ShaderRegisterBlock> allBlocks;
			auto stages = GetAvailableStages();
			
			for (auto stage : stages) {
				auto* stageRes = GetStageResources(stage);
				if (stageRes) {
					for (const auto& block : stageRes->registerBlocks) {
						// 避免重复添加相同绑定点的块
						bool exists = false;
						for (const auto& existing : allBlocks) {
							if (existing.BindPoint == block.BindPoint && 
								existing.BindSpace == block.BindSpace) {
								exists = true;
								break;
							}
						}
						if (!exists) {
							allBlocks.push_back(block);
						}
					}
				}
			}
			return allBlocks;
		}
		
		std::vector<ResourceBinding> GetAllResourceBindings() const {
			std::vector<ResourceBinding> allBindings;
			auto stages = GetAvailableStages();
			
			for (auto stage : stages) {
				auto* stageRes = GetStageResources(stage);
				if (stageRes) {
					for (const auto& binding : stageRes->resourceBindings) {
						// 避免重复添加相同的绑定
						bool exists = false;
						for (const auto& existing : allBindings) {
							if (existing.Name == binding.Name && 
								existing.BindPoint == binding.BindPoint &&
								existing.BindSpace == binding.BindSpace) {
								exists = true;
								break;
							}
						}
						if (!exists) {
							allBindings.push_back(binding);
						}
					}
				}
			}
			return allBindings;
		}
		
		// Stage-specific 查询接口
		virtual Ref<ShaderRegisterBlock> GetRegisterBlockByName(ShaderStage stage, const std::string& name) = 0;
		virtual Ref<ShaderRegisterBlock> GetRegisterBlockByBindPoint(ShaderStage stage, uint32_t bindPoint, uint32_t space = 0) = 0;
		virtual Ref<ShaderParameter> GetParameterByName(ShaderStage stage, const std::string& name) = 0;
	};

} // namespace Hazel 