workspace "Universal-Snowdrop-Modloader"
    architecture "x86_64"
    location "build"
    cppdialect "C++latest"
    exceptionhandling "SEH"
    startproject "Universal-Snowdrop-Modloader"

    configurations {
        "dinput8",
        "version"
    }

-- =========================================
-- Proxy configuration map
-- =========================================
local ProxyMap = {
    ["dinput8"] = { name="dinput8",  def="src/proxies/dinput8.def",  define="DINPUT8" },
    ["version"] = { name="version", def="src/proxies/version.def", define="VERSION" }
}

-- =========================================
-- Shared third-party sources
-- =========================================
local MinHookFiles = {
    "lib/MinHook/src/**.c",
    "lib/MinHook/src/**.h",
    "lib/MinHook/include/**.h"
}

-- =========================================
-- Universal Snowdrop Modloader
-- =========================================
project "Universal-Snowdrop-Modloader"
    kind "SharedLib"
    language "C++"
    targetextension ".dll"

    includedirs {
        "include",
        "include/proxies",
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
    files(MinHookFiles)

    pchheader "pch.hpp"
    pchsource "src/pch.cpp"
    characterset "UNICODE"
    links { "d3d12", "dxgi" }

    filter { "files:lib/ImGui/**.cpp" }
        enablepch "Off"
    filter { "files:lib/MinHook/**.c" }
        enablepch "Off"
    filter {}

    -- defaults
    targetname "version"
    linkoptions { "/DEF:\"%{wks.location}/../src/proxies/version.def\"" }

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
        removefiles { "include/proxies/version.hpp", "src/proxies/version.def", "src/proxies/version.cpp" }
        
    filter { "configurations:version" }
        removefiles { "include/proxies/dinput8.hpp", "src/proxies/dinput8.def", "src/proxies/dinput8.cpp" }

    filter {}

-- =========================================
-- Plugins
-- =========================================
group "Plugins"

local function PluginProject(name)
    project(name)
        kind "SharedLib"
        language "C++"
        targetextension ".dll"
        targetname(name)
        targetdir "bin/%{cfg.buildcfg}/plugins"

        includedirs {
            "include",
            "lib/MinHook/include"
        }

        files { "plugin_templates/" .. name .. "/**.cpp" }
        files(MinHookFiles)

        characterset "UNICODE"
        defines { "NDEBUG" }
        optimize "On"
        staticruntime "On"
        linktimeoptimization "On"
        buildoptions { "/Ox", "/fp:fast" }

        filter { "files:lib/MinHook/**.c" }
            enablepch "Off"
        filter {}
end

PluginProject("PerformanceTweaks")