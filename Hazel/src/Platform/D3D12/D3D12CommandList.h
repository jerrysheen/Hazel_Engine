#pragma once
#include "Hazel/Gfx/CommandList.h"

namespace Hazel 
{
	class D3D12CommandList : public CommandList
	{
	public :
		D3D12CommandList();
		virtual ~D3D12CommandList();
	};
}