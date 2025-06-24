#include "hzpch.h"
#include "Hazel/Gfx/CustomRenderPass/OpaqueRenderPass.h"
#include "Hazel/RHI/Core/CommandList.h"

namespace Hazel
{
	void OpaquePass::OnCameraSetup() const
	{
	}

	void OpaquePass::Render(RenderNode* node, RenderingData* data) const
	{
		//CommandList command = CommandPool.GetCommand();
		//command.SetRenderTarget();
		//command.ClearRenderTarget();
		//// command.Draw(m_DrawCommand);
		//command.ChangeRenderTargetState();
		//command.Execute();
		//CommandPool.ReleaseCommand(command);
	}

}

