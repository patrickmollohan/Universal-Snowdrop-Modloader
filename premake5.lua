newoption {
    trigger     = "with-version",
    value       = "STRING",
    description = "Current USWOML version",
    default     = "7.0.0",
}

workspace "Ultimate-SWO-ModLoader"
   configurations { "Release", "Debug" }
   architecture "x86_64"
   location "build"
   cppdialect "C++latest"
   exceptionhandling ("SEH")
   
   defines { "rsc_CompanyName=\"Patrick Mollohan\"" }
   defines { "rsc_LegalCopyright=\"MIT License\""}
   defines { "rsc_InternalName=\"%{prj.name}\"", "rsc_ProductName=\"%{prj.name}\"", "rsc_OriginalFilename=\"%{prj.name}.dll\"" }
   defines { "rsc_FileDescription=\"Ultimate Star Wars Outlaws ModLoader\"" }
   defines { "rsc_UpdateUrl=\"https://github.com/patrickmollohan/Ultimate-SWO-ModLoader\"" }

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
   defines { "rsc_FileVersion_MAJOR=" .. major }
   defines { "rsc_FileVersion_MINOR=" .. minor }
   defines { "rsc_FileVersion_BUILD=" .. build }
   defines { "rsc_FileVersion_REVISION=" .. revision }
   defines { "rsc_FileVersion=\"" .. major .. "." .. minor .. "." .. build .. "\"" }
   defines { "rsc_ProductVersion=\"" .. major .. "." .. minor .. "." .. build .. "\"" }

   defines { "X64" }
     
project "Ultimate-SWO-ModLoader"
   kind "SharedLib"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   targetname "dinput8"
   targetextension ".dll"
   
   includedirs { "src" }
   includedirs { "external" }

   includedirs { "external/injector/minhook/include" }
   files { "external/injector/minhook/include/*.h", "external/injector/minhook/src/**.h", "external/injector/minhook/src/**.c" }
   includedirs { "external/injector/utility" }
   files { "external/injector/utility/FunctionHookMinHook.hpp", "external/injector/utility/FunctionHookMinHook.cpp" }

   files { "src/asi_loader.hpp", "src/asi_loader.cpp" }
   files { "src/dllmain.cpp" }
   files { "src/dinput8.def" }
   files { "src/resources/Versioninfo.rc" }
   
   characterset ("UNICODE")
   
   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      staticruntime "On"