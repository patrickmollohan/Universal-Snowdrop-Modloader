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
        buildoptions { "/Ox", "/fp:fast" }
        linktimeoptimization ("On")

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
        "lib/MinHook/include",
        "lib/ImGui",
        "lib/ImGui/backends"
    }

    files {
        "include/**.hpp",
        "src/**.def",
        "src/**.cpp",
        "src/**.rc",
        "lib/ImGui/imgui.cpp",
        "lib/ImGui/imgui_draw.cpp",
        "lib/ImGui/imgui_tables.cpp",
        "lib/ImGui/imgui_widgets.cpp",
        "lib/ImGui/backends/imgui_impl_win32.cpp",
        "lib/ImGui/backends/imgui_impl_dx12.cpp"
    }

    pchheader "pch.hpp"
    pchsource "src/pch.cpp"
    characterset "UNICODE"
    links { "MinHook", "d3d12", "dxgi" }

    filter { "files:lib/ImGui/**.cpp" }
        flags { "NoPCH" }
    filter {}

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
            linktimeoptimization ("On")
            buildoptions { "/Ox", "/fp:fast" }
    end

    filter { "configurations:dinput8" }
        removefiles { "include/wrappers/version.hpp", "src/wrappers/version.def", "src/wrappers/version.cpp" }
        
    filter { "configurations:version" }
        removefiles { "include/wrappers/dinput8.hpp", "src/wrappers/dinput8.def", "src/wrappers/dinput8.cpp" }

    filter {}
