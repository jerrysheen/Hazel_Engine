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

workspace "Hazel"
	architecture "x64"
	startproject "Hazelnut"
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
IncludeDir["GLFW"] = "Hazel/vendor/GLFW/include"
IncludeDir["Glad"] = "Hazel/vendor/Glad/include"
IncludeDir["ImGui"] = "Hazel/vendor/imgui"
IncludeDir["glm"] = "Hazel/vendor/glm"
IncludeDir["stb_image"] = "Hazel/vendor/stb_image"
IncludeDir["assimp"] = "Hazel/vendor/assimp/include"
IncludeDir["entt"] = "Hazel/vendor/entt/include"
IncludeDir["boost"] = "Hazel/vendor/boost"

group "Dependencies"
	-- This include file include the GLFW premake5.lua.
	include "Hazel/vendor/GLFW"
	include "Hazel/vendor/Glad"
	include "Hazel/vendor/imgui"
	include "Hazel/vendor/assimp"
	include "Hazel/vendor/boost"
group ""



project "Hazel"
	location (projectdir .. "/Hazel")
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir (projectdir .. "/bin/" .. outputdir .. "/%{prj.name}")
	objdir (projectdir .. "/bin-int/" .. outputdir .. "/%{prj.name}")
	-- defines { "RENDER_API_OPENGL", "TRACE" }
	defines { "RENDER_API_DIRECTX12", "TRACE" }
	pchheader "hzpch.h"
	pchsource "Hazel/src/hzpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.boost}"
	}
-- ����о�����������MSVS linker��������additional dependencyһ����
-- ��Ϊhazel��һ��dll lib�� ����include��һ��static lib�� ����sanbox include��hazelһ��
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
			"GLFW_INCLUDE_NONE"
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

project "Sandbox"
	location (projectdir .. "/Sandbox")
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
		"{COPYDIR} \"%{wks.location}../../../Sandbox/assets\" \"%{cfg.targetdir}/assets\""
	}

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Hazel/vendor/spdlog/include",
		"Hazel/src",
		"Hazel/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.boost}"
	}

	links
	{
		"Hazel"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"HZ_PLATFORM_WINDOWS"
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

project "Hazelnut"
	location (projectdir .. "/Hazelnut")
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
		"{COPYDIR} \"%{wks.location}../../../Hazelnut/assets\" \"%{cfg.targetdir}/assets\""
	}

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Hazel/vendor/spdlog/include",
		"Hazel/src",
		"Hazel/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.boost}"
	}

	links
	{
		"Hazel"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"HZ_PLATFORM_WINDOWS"
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