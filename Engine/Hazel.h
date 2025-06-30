#pragma once

// for use hazel application

#include "Runtime/Core/Application.h"
#include "Runtime/Core/Layer/Layer.h"
#include "Runtime/Core/Log/Log.h"

#include "Runtime/Core/Time/Timestep.h"

#include "Runtime/Core/Input/Input.h"
#include "Runtime/Core/Input/KeyCode.h"
#include "Runtime/Core/Input/MouseButtonCode.h"
#include "Runtime/Graphics/Renderer/OrthographicCameraController.h"
#include "Runtime/Graphics/Renderer/PerspectiveCameraController.h"


#include "ImGui/ImGuiLayer.h"

// ---Renderer------------------------
#include "Runtime/Graphics/Renderer/Renderer3D.h"
#include "Runtime/Graphics/Renderer/RendererCommand.h"

#include "Runtime/Graphics/Renderer/Buffer.h"
#include "Runtime/Graphics/Renderer/Shader.h"
#include "Runtime/Graphics/Renderer/ShaderLibrary.h"
#include "Runtime/Graphics/Renderer/TextureBuffer.h"
#include "Runtime/Graphics/Renderer/VertexArray.h"
#include "Runtime/Graphics/Renderer/Texture.h"

#include "Runtime/Graphics/Mesh/Mesh.h"
//#include "Hazel/Model/Model.h"
#include "Runtime/Graphics/Material/Material.h"

#include "Runtime/Graphics/RHI/Interface/IPipelineStateManager.h"

#include "Runtime/Graphics/Renderer/OrthographicCamera.h"
#include "Runtime/Graphics/Renderer/RenderTargetEnum.h"
// -----------------------------------

// ---Utility------------------------
#include "Runtime/Core/Utility/Random.h"
// -----------------------------------

// ---Scene------------------------
#include "Runtime/Scene/Scene.h"
#include "Runtime/Scene/Component.h"
#include "Runtime/Scene/Entity.h"
// -----------------------------------