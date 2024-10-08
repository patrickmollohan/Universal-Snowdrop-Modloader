newoption {
    trigger     = "with-version",
    value       = "STRING",
    description = "Ultimate Star Wars Outlaws ModLoader version",
    default     = "1.0.0",
}

workspace "Ultimate-SWO-ModLoader"
   configurations { "Release", "Debug" }
   architecture "x86_64"
   location "build"
   cppdialect "C++latest"
   exceptionhandling ("SEH")
   
   defines {
      "rsc_CompanyName=\"Patrick Mollohan\"",
      "rsc_LegalCopyright=\"MIT License\"",
      "rsc_InternalName=\"%{prj.name}\"", "rsc_ProductName=\"%{prj.name}\"", "rsc_OriginalFilename=\"dinput8.dll\"",
      "rsc_FileDescription=\"Ultimate Star Wars Outlaws ModLoader\"",
      "rsc_UpdateUrl=\"https://github.com/patrickmollohan/Ultimate-SWO-ModLoader\""
   }

   local major = 1
   local minor = 0
   local build = 3
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
     
project "Ultimate-SWO-ModLoader"
   kind "SharedLib"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   targetname "dinput8"
   targetextension ".dll"

   includedirs {
      "lib",
      "lib/injector/minhook/include",
      "lib/injector/utility",
      "src"
   }

   files {
      "lib/injector/minhook/include/*.h", "lib/injector/minhook/src/**.h", "lib/injector/minhook/src/**.c",
      "lib/injector/utility/FunctionHookMinHook.hpp", "lib/injector/utility/FunctionHookMinHook.cpp",
      "src/asi_loader.hpp", "src/asi_loader.cpp",
      "src/dinput8.def",
      "src/disk_cache_enabler.hpp", "src/disk_cache_enabler.cpp",
      "src/dllmain.hpp", "src/dllmain.cpp",
      "src/minhook_handler.hpp", "src/minhook_handler.cpp",
      "src/mod_loader.hpp", "src/mod_loader.cpp",
      "src/utilities.hpp", "src/utilities.cpp",
      "src/Versioninfo.rc"
   }

   characterset ("UNICODE")
   
   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      staticruntime "On"