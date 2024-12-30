newoption {
    trigger     = "with-version",
    value       = "STRING",
    description = "Universal Snowdrop Modloader version",
    default     = "1.0.0",
}

workspace "Universal-Snowdrop-Modloader"
   configurations { "Release" }
   architecture "x86_64"
   location "build"
   cppdialect "C++latest"
   exceptionhandling ("SEH")

   defines {
      "rsc_CompanyName=\"Patrick Mollohan\"",
      "rsc_LegalCopyright=\"MIT License\"",
      "rsc_InternalName=\"%{prj.name}\"", "rsc_ProductName=\"%{prj.name}\"", "rsc_OriginalFilename=\"version.dll\"",
      "rsc_FileDescription=\"Universal Snowdrop Modloader\"",
      "rsc_UpdateUrl=\"https://github.com/patrickmollohan/Universal-Snowdrop-Modloader\""
   }

   local major = 1
   local minor = 0
   local build = 0
   local revision = 0

   if(_OPTIONS["with-version"]) then
     local t = {}
     for i in _OPTIONS["with-version"]:gmatch("([^.]+)") do
       t[#t + 1], _ = i:gsub("%D+", "")
     end
     while #t < 4 do t[#t + 1] = 0 end
     major = math.min(tonumber(t[1]), 255)
     minor = math.min(tonumber(t[2]), 255)
     build = math.min(tonumber(t[3]), 65535)
     revision = math.min(tonumber(t[4]), 65535)
   end

   defines {
      "rsc_FileVersion_MAJOR=" .. major,
      "rsc_FileVersion_MINOR=" .. minor,
      "rsc_FileVersion_BUILD=" .. build,
      "rsc_FileVersion_REVISION=" .. revision,
      "rsc_FileVersion=\"" .. major .. "." .. minor .. "." .. build .. "\"",
      "rsc_ProductVersion=\"" .. major .. "." .. minor .. "." .. build .. "\""
   }

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
      "src/dll_main.cpp", "src/dll_main.hpp",
      "src/minhook_handler.cpp", "src/minhook_handler.hpp",
      "src/mod_loader.cpp", "src/mod_loader.hpp",
	   "src/script_loader.cpp", "src/script_loader.hpp",
	   "src/settings.cpp", "src/settings.hpp",
      "src/utilities.cpp", "src/utilities.hpp",
	   "src/version.def",
	   "src/version_wrapper.cpp", "src/version_wrapper.hpp",
      "src/Versioninfo.rc"
   }

   characterset ("UNICODE")

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      buildoptions { "/Ox" }
      staticruntime "On"