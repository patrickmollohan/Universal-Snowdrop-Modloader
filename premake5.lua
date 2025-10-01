workspace "Universal-Snowdrop-Modloader"
   configurations { "Release" }
   architecture "x86_64"
   location "build"
   cppdialect "C++latest"
   exceptionhandling ("SEH")

project "Universal-Snowdrop-Modloader"
   kind "SharedLib"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   targetname "version"
   targetextension ".dll"

   includedirs {
      "lib",
       "lib/MinHook/include",
      "src"
   }

   files {
      "lib/MinHook/include/*.h", "lib/MinHook/src/**.c", "lib/MinHook/src/**.h",
      "src/disk_cache_enabler.cpp", "src/disk_cache_enabler.hpp",
      "src/dll_info.rc",
      "src/dll_main.cpp", "src/dll_main.hpp",
      "src/minhook_handler.cpp", "src/minhook_handler.hpp",
      "src/mod_loader.cpp", "src/mod_loader.hpp",
      "src/pch.cpp", "src/pch.hpp",
      "src/script_loader.cpp", "src/script_loader.hpp",
      "src/settings.cpp", "src/settings.hpp",
      "src/utilities.cpp", "src/utilities.hpp",
      "src/version.def",
      "src/version_wrapper.cpp", "src/version_wrapper.hpp"
   }

   characterset ("UNICODE")

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      buildoptions { "/Ox" }
      staticruntime "On"