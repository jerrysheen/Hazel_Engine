 -- 根据不同的生成器设置项目目录
if _ACTION == "vs2022" then
    projectdir = "Project/Windows/Visual Studio 2022"
elseif _ACTION == "vs2019" then
    projectdir = "Project/Windows/Visual Studio 2019"
elseif _ACTION == "xcode4" then
    projectdir = "Project/macOS/Xcode"
elseif _ACTION == "gmake2" then
    if os.host() == "linux" then
        projectdir = "Project/Linux/Makefiles"
    elseif os.host() == "macosx" then
        projectdir = "Project/macOS/Makefiles"
    else
        projectdir = "Project/Unix/Makefiles"
    end
else
    -- 默认情况
    projectdir = "Project/Generated"
end

workspace "ShanEngine"
	architecture "x64"
	startproject "Editor"
	location (projectdir)

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "ThirdParty/Runtime/Graphics/GLFW/include"
IncludeDir["Glad"] = "ThirdParty/Runtime/Graphics/GLAD/include"
IncludeDir["ImGui"] = "ThirdParty/Editor/imgui"
IncludeDir["glm"] = "ThirdParty/Runtime/Core/glm"
IncludeDir["stb_image"] = "ThirdParty/Runtime/Asset/stb_image"
IncludeDir["assimp"] = "ThirdParty/Runtime/Asset/assimp/include"
IncludeDir["entt"] = "ThirdParty/Runtime/Core/entt/include"
IncludeDir["boost"] = "ThirdParty/Runtime/Core/boost"

group "Dependencies"
	-- This include file include the GLFW premake5.lua.
	include "ThirdParty/Runtime/Graphics/GLFW"
	include "ThirdParty/Runtime/Graphics/GLAD"
	include "ThirdParty/Editor/imgui"
	include "ThirdParty/Runtime/Asset/assimp"
	include "ThirdParty/Runtime/Core/boost"
group ""



project "EngineCore"
	location (projectdir .. "/EngineCore")
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir (projectdir .. "/bin/" .. outputdir .. "/%{prj.name}")
	objdir (projectdir .. "/bin-int/" .. outputdir .. "/%{prj.name}")
	-- defines { "RENDER_API_OPENGL", "TRACE" }
	defines { "RENDER_API_DIRECTX12", "TRACE" }
	pchheader "hzpch.h"
	pchsource "Engine/hzpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.cpp",
		"Engine/**.cpp",
		"ThirdParty/Runtime/Asset/stb_image/**.h",
		"ThirdParty/Runtime/Asset/stb_image/**.cpp",
		"ThirdParty/Runtime/Core/glm/glm/**.hpp",
		"ThirdParty/Runtime/Core/glm/glm/**.inl",
	}

	includedirs
	{
		"%{prj.name}/src",
		"Engine/",
		"ThirdParty/Runtime/Core/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.boost}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib",
		"assimp",
	}

  	filter "system:windows"
		systemversion "latest"

		defines
		{
			"HZ_PLATFORM_WINDOWS",
			"HZ_BUILD_DLL",
			"GLFW_INCLUDE_NONE",
			"RENDER_API_DIRECTX12",
			"NOMINMAX"
		}
		buildoptions { "/source-charset:utf-8", "/execution-charset:utf-8" }

	filter "configurations:Debug"
		defines "HZ_DEBUG"
		runtime "Debug"
		staticruntime "on"
		symbols "on"

	filter "configurations:Release"
		defines "HZ_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "HZ_DIST"
		runtime "Release"
		optimize "on"

project "Editor"
	location (projectdir .. "/Editor")
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir (projectdir .. "/bin/" .. outputdir .. "/%{prj.name}")
	objdir (projectdir .. "/bin-int/" .. outputdir .. "/%{prj.name}")
	
	-- 设置调试时的工作目录为可执行文件所在目录
	debugdir "%{cfg.targetdir}"
	
	-- 构建后复制资源文件到输出目录
	postbuildcommands {
		"{COPYDIR} \"%{wks.location}../../../Resource\" \"%{cfg.targetdir}/Resource\""
	}

	files
	{
		"Editor/**.h",
		"Editor/**.cpp",
		"Engine/**.h",
		"Engine/**.cpp",
		"ThirdParty/Runtime/Asset/stb_image/**.h",
		"ThirdParty/Runtime/Asset/stb_image/**.cpp",
	}

	includedirs
	{
		"ThirdParty/Runtime/Core/spdlog/include",
		"Engine/",
		"Editor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.boost}"
	}

	links
	{
		"EngineCore"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"HZ_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE",
			"RENDER_API_DIRECTX12",
			"NOMINMAX"
		}

	filter "configurations:Debug"
		defines "HZ_DEBUG"
		runtime "Debug"
		staticruntime "on"
		symbols "On"

	filter "configurations:Release"
		defines "HZ_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "HZ_DIST"
		runtime "Release"
		optimize "On"