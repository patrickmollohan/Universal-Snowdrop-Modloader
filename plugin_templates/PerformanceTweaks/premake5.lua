workspace "PerformanceTweaks"
    architecture "x86_64"
    location "build"
    cppdialect "C++latest"
    exceptionhandling "SEH"

    configurations { "release" }

-- =========================================
-- MinHook
-- =========================================
project "MinHook"
    kind "StaticLib"
    language "C"
    targetdir "bin/%{cfg.buildcfg}"

    includedirs { "../../lib/MinHook/include" }

    files {
        "../../lib/MinHook/src/**.c",
        "../../lib/MinHook/src/**.h",
        "../../lib/MinHook/include/**.h"
    }

    filter "configurations:*"
        optimize "On"
        staticruntime "On"
        linktimeoptimization ("On")

-- =========================================
-- Performance Tweaks
-- =========================================
project "PerformanceTweaks"
    kind "SharedLib"
    language "C++"
    targetextension ".dll"

    includedirs {
        "../../include",
        "../../lib/MinHook/include"
    }

    files { "PerformanceTweaks.cpp" }

    characterset "UNICODE"
    links { "MinHook" }

    filter "configurations:release"
        targetname "PerformanceTweaks"
        targetdir "bin/release"
        defines { "NDEBUG" }
        optimize "On"
        staticruntime "On"
        linktimeoptimization ("On")

    filter {}
