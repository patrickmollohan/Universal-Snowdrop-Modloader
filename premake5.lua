workspace "Universal-Snowdrop-Modloader"
    architecture "x86_64"
    location "build"
    cppdialect "C++latest"
    exceptionhandling "SEH"

    configurations {
        "dinput8",
        "version"
    }

-- =========================================
-- Proxy configuration map
-- =========================================
local ProxyMap = {
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
-- Universal Snowdrop Modloader
-- =========================================
project "Universal-Snowdrop-Modloader"
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
    targetname "version"
    linkoptions { "/DEF:\"%{wks.location}/../src/wrappers/version.def\"" }

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

    filter { "configurations:dinput8" }
        removefiles { "include/wrappers/version.hpp", "src/wrappers/version.def", "src/wrappers/version.cpp" }
        
    filter { "configurations:version" }
        removefiles { "include/wrappers/dinput8.hpp", "src/wrappers/dinput8.def", "src/wrappers/dinput8.cpp" }

    filter {}
