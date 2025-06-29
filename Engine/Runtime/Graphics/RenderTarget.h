#pragma once
#include "Runtime/Core/Core.h"
#include <Platform/D3D12/d3dx12.h>
#include "hzpch.h"

namespace Hazel {

	struct RenderTargetDescriptor
	{
		uint32_t Width, Height;
		uint32_t Samples = 1;
	};
	class RenderTarget
	{
	private:

	public:
		
		static Ref<RenderTarget> Create(const RenderTargetDescriptor& spec);

	};
}