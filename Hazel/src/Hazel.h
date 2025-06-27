#pragma once

// for use hazel application

#include "Hazel/Core/Application.h"
#include "Hazel/Core/Layer.h"
#include "Hazel/Core/Log.h"

#include "Hazel/Core/Timestep.h"

#include "Hazel/Core/Input.h"
#include "Hazel/Core/KeyCode.h"
#include "Hazel/Core/MouseButtonCode.h"
#include "Hazel/Renderer/OrthographicCameraController.h"
#include "Hazel/Renderer/PerspectiveCameraController.h"


#include "Hazel/ImGui/ImGuiLayer.h"

// ---Renderer------------------------
#include "Hazel/Renderer/Renderer3D.h"
#include "Hazel/Renderer/RendererCommand.h"

#include "Hazel/Renderer/Buffer.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/TextureBuffer.h"
#include "Hazel/Renderer/VertexArray.h"
#include "Hazel/Renderer/Texture.h"

#include "Hazel/Model/Mesh.h"
//#include "Hazel/Model/Model.h"
#include "Hazel/Material/Material.h"

#include "Hazel/RHI/Interface/IPipelineStateManager.h"

#include "Hazel/Renderer/OrthographicCamera.h"
#include "Hazel/Renderer/RenderTargetEnum.h"
// -----------------------------------

// ---Utility------------------------
#include "Hazel/Utility/Random.h"
// -----------------------------------

// ---Scene------------------------
#include "Hazel/Scene/Scene.h"
#include "Hazel/Scene/Component.h"
#include "Hazel/Scene/Entity.h"
// -----------------------------------