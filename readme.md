# Ultimate Star Wars Outlaws ModLoader

![image](https://store.ubisoft.com/on/demandware.static/-/Sites-masterCatalog/default/dw23f30fd8/images/pdpbanner/645ba713a9ce0448bffa4c12-bg.jpg)

## DESCRIPTION

This is a DLL file that adds mod support and script loading functionality. Additionally, it enables disk caching, resulting in better hard drive performance over time.

## INSTALLATION

To install, copy [version.dll](https://github.com/patrickmollohan/Ultimate-SWO-ModLoader/releases/latest/download/version.dll) into the root directory of the game (where "Outlaws.exe" is). This is usually C:\Program Files (x86)\Ubisoft\Ubisoft Game Launcher\games\Star Wars Outlaws

## MODDING

After installing Ultimate SWO ModLoader, you may copy any modified game files relative to the root directory of the game. Be sure to keep file names and folder structures the same!

You can use programs such as DTZxPorter's [Hunter](https://dtzxporter.com/tools/hunter) to extract the game files.

Note: texture replacement currently does not work, as file streaming is not yet supported.

## SCRIPT LOADING

To add functionality to the game, you can put additional DLL or ASI files in the "scripts" folder in the root directory of the game, allowing you to execute arbitrary code. If the folder does not exist (it doesn't by default), create it.

## DISK CACHE ENABLER

This DLL now also enables the use of Windows's file caching, which should result in less hard drive activity over time. This may allow the game to run on HDDs and should also result in slightly better SSD performance as well.

## CPU and I/O Priority

For additional speed, the DLL now also sets process and thread priority to high for Outlaws, reducing stuttering and lag spikes due to other processes in the background.

## CREDITS

TsudaKageyu - [MinHook](https://github.com/TsudaKageyu/minhook)

DTZxPorter - [avatar-modloader](https://github.com/dtzxporter/avatar-modloader)

Archost - [Disk Cache Enabler](https://www.nexusmods.com/skyrimspecialedition/mods/100975)
