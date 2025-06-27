#pragma once

#include <string>
#include <vector>
#include "Hazel/Core/Core.h"
#include "Buffer.h"

namespace Hazel {
	
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
		// 如果是ConstantBuffer类型，对应的寄存器块索引
		int RegisterBlockIndex = -1;
	};

	// ShaderReflection类，用于分析着色器并提取元数据
	class ShaderReflection
	{
	public:
		virtual ~ShaderReflection() = default;
		
		// 反射顶点输入布局
		virtual BufferLayout ReflectVertexInputLayout() = 0;
		
		// 反射寄存器块
		virtual std::vector<ShaderRegisterBlock> ReflectRegisterBlocks() = 0;
		
		// 反射资源绑定
		virtual std::vector<ResourceBinding> ReflectResourceBindings() = 0;

		// 通过名称获取寄存器块
		virtual Ref<ShaderRegisterBlock> GetRegisterBlockByName(const std::string& name) = 0;
		
		// 通过绑定点获取寄存器块
		virtual Ref<ShaderRegisterBlock> GetRegisterBlockByBindPoint(uint32_t bindPoint, uint32_t space = 0) = 0;
		
		// 通过名称获取参数
		virtual Ref<ShaderParameter> GetParameterByName(const std::string& name) = 0;
	};

} // namespace Hazel 