# Universal Snowdrop Modloader

![image](https://store.ubisoft.com/on/demandware.static/-/Sites-masterCatalog/default/dw23f30fd8/images/pdpbanner/645ba713a9ce0448bffa4c12-bg.jpg)

## DESCRIPTION

This is a DLL file that adds mod support and plugin loading functionality for Snowdrop engine games.

Currently, only Star Wars Outlaws and Avatar: Frontiers of Pandora are supported.

## INSTALLATION

To install, extract [version.zip](https://github.com/patrickmollohan/Universal-Snowdrop-Modloader/releases/latest/download/version.zip) into the root directory of the game (where "Outlaws.exe"/"AFOP.exe" is).

## MODDING

After installing Universal Snowdrop Modloader, you may copy any modified game files relative to the root directory of the game. Be sure to keep file names and folder structures the same! You can toggle mod support on or off via the "Settings" section of "version.ini".

You can use programs such as DTZxPorter's [Hunter](https://dtzxporter.com/tools/hunter) to extract the game files.

## PLUGIN LOADING

To add functionality to the game, you can put additional DLL or ASI files in the "plugins" folder in the root directory of the game, allowing you to execute arbitrary code. If the folder does not exist (it doesn't by default), create it. You can toggle plugin loading support on or off via the "Settings" section of "version.ini".

## CREDITS

TsudaKageyu - [MinHook](https://github.com/TsudaKageyu/minhook)
