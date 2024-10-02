project "boost"           -- 项目名称
    kind "None"                   -- 项目类型为None，因为没有源代码编译
    language "C++"                -- 设置使用的编程语言

    files { "*.hpp", "**/*.hpp" }      -- 包含所有头文件，用于IDE项目管理，实际不编译

    includedirs { "*" }     -- 头文件包含目录

    -- 由于没有源文件和输出目标，通常不需要配置Debug或Release特定设置
