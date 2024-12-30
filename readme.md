# Universal Snowdrop Modloader

![image](https://store.ubisoft.com/on/demandware.static/-/Sites-masterCatalog/default/dw23f30fd8/images/pdpbanner/645ba713a9ce0448bffa4c12-bg.jpg)

## DESCRIPTION

This is a DLL file that adds mod support and script loading functionality for Snowdrop engine games. Additionally, it enables disk caching, resulting in better hard drive performance over time, and allows for control of the CPU, I/O, and thread priorities.

Currently, Star Wars Outlaws and Avatar: Frontiers of Pandora are supported.

## INSTALLATION

To install, extract [version.zip](https://github.com/patrickmollohan/Universal-Snowdrop-Modloader/releases/latest/download/version.zip) into the root directory of the game (where "Outlaws.exe"/"AFOP.exe" is).

## MODDING

After installing Universal Snowdrop Modloader, you may copy any modified game files relative to the root directory of the game. Be sure to keep file names and folder structures the same! You can toggle mod support on or off via the "Settings" section of "version.ini".

You can use programs such as DTZxPorter's [Hunter](https://dtzxporter.com/tools/hunter) to extract the game files.

Note: file streaming is not yet supported, so custom assets that are streamed such as mipmaps and LOD meshes are ignored.

## SCRIPT LOADING

To add functionality to the game, you can put additional DLL or ASI files in the "scripts" folder in the root directory of the game, allowing you to execute arbitrary code. If the folder does not exist (it doesn't by default), create it. You can toggle script loading support on or off via the "Settings" section of "version.ini".

## DISK CACHE ENABLER

This DLL now also enables the use of Windows's file caching, which should result in less hard drive activity over time. This may allow the game to run on HDDs and should also result in slightly better SSD performance as well. This can be configured via the "DiskCacheEnabler" section of "version.ini".

## CPU, I/O, and Thread Priority

For additional speed, the DLL now allows you to set the CPU, I/O, and thread priorities to high, reducing stuttering and lag spikes due to other processes in the background. This can be configured via the "Priorities" section of "version.ini".

## CREDITS

TsudaKageyu - [MinHook](https://github.com/TsudaKageyu/minhook)

DTZxPorter - [avatar-modloader](https://github.com/dtzxporter/avatar-modloader)

Archost - [Disk Cache Enabler](https://www.nexusmods.com/skyrimspecialedition/mods/100975)
