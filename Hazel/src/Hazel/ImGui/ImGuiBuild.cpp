#include "hzpch.h"

#ifdef RENDER_API_OPENGL
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "examples/imgui_impl_opengl3.cpp"
#include "examples/imgui_impl_glfw.cpp" 
#elif RENDER_API_DIRECTX12
#include "imgui.h"
#include "examples/imgui_impl_win32.h"
#include "examples/imgui_impl_dx12.h"
#include "imgui_internal.h"
#endif