workspace "Universal-Modloader-Core"
    architecture "x86_64"
    location "build"
    cppdialect "C++latest"
    exceptionhandling "SEH"

    configurations {
        "d3d11",
        "dinput8",
        "version"
    }

-- =========================================
-- Proxy configuration map
-- =========================================
local ProxyMap = {
    ["d3d11"] = { name="d3d11",  def="src/wrappers/d3d11.def",  define="D3D11" },
    ["dinput8"] = { name="dinput8",  def="src/wrappers/dinput8.def",  define="DINPUT8" },
    ["version"] = { name="version", def="src/wrappers/version.def", define="VERSION" }
}

-- =========================================
-- MinHook
-- =========================================
project "MinHook"
    kind "StaticLib"
    language "C"
    targetdir "bin/%{cfg.buildcfg}"

    includedirs { "lib/MinHook/include" }

    files {
        "lib/MinHook/src/**.c",
        "lib/MinHook/src/**.h",
        "lib/MinHook/include/**.h"
    }

    filter "configurations:*"
        optimize "On"
        staticruntime "On"
        flags { "linktimeoptimization" }
        buildoptions { "/Ox", "/fp:fast" }

-- =========================================
-- Universal Modloader Core
-- =========================================
project "Universal-Modloader-Core"
    kind "SharedLib"
    language "C++"
    targetextension ".dll"

    includedirs {
        "include",
        "include/wrappers",
        "lib/MinHook/include"
    }

    files {
        "include/**.hpp",
        "src/**.def",
        "src/**.cpp",
        "src/**.rc"
    }

    pchheader "pch.hpp"
    pchsource "src/pch.cpp"
    characterset "UNICODE"
    links { "MinHook" }

    -- defaults
    targetname "d3d11"
    linkoptions { "/DEF:\"%{wks.location}/../src/wrappers/d3d11.def\"" }

    for cfg, data in pairs(ProxyMap) do
        filter { "configurations:" .. cfg }

            targetname (data.name)
            targetdir ("bin/" .. cfg)
            linkoptions { "/DEF:\"%{wks.location}/../" .. data.def .. "\"" }

            defines { data.define, "NDEBUG" }
            optimize "On"
            staticruntime "On"
            flags { "linktimeoptimization" }
            buildoptions { "/Ox", "/fp:fast" }
    end
    
    filter { "configurations:d3d11" }
        removefiles { "include/wrappers/dinput8.hpp", "src/wrappers/dinput8.def", "src/wrappers/dinput8.cpp" }
        removefiles { "include/wrappers/version.hpp", "src/wrappers/version.def", "src/wrappers/version.cpp" }
        
    filter { "configurations:dinput8" }
        removefiles { "include/wrappers/d3d11.hpp", "src/wrappers/d3d11.def", "src/wrappers/d3d11.cpp" }
        removefiles { "include/wrappers/version.hpp", "src/wrappers/version.def", "src/wrappers/version.cpp" }
        
    filter { "configurations:version" }
        removefiles { "include/wrappers/d3d11.hpp", "src/wrappers/d3d11.def", "src/wrappers/d3d11.cpp" }
        removefiles { "include/wrappers/dinput8.hpp", "src/wrappers/dinput8.def", "src/wrappers/dinput8.cpp" }

    filter {}
