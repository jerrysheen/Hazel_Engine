#include "hzpch.h"
#include "Runtime/Graphics/Renderer/CustomRenderPass/OpaqueRenderPass.h"
#include "Runtime/Graphics/RHI/Core/CommandList.h"

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

