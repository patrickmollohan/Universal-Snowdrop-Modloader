# Universal Snowdrop Modloader

![image](https://store.ubisoft.com/on/demandware.static/-/Sites-masterCatalog/default/dw23f30fd8/images/pdpbanner/645ba713a9ce0448bffa4c12-bg.jpg)

## DESCRIPTION

This is a DLL file that adds mod support and plugin loading functionality for Snowdrop engine games.

Currently, only Star Wars Outlaws and Avatar: Frontiers of Pandora are supported.

## INSTALLATION

To install, extract either version.dll or dinput8.dll and its corresponding config file into the root directory of the game (where "Outlaws.exe"/"AFOP.exe" is).

## MODDING

After installing Universal Snowdrop Modloader, you may copy any modified game files relative to the root directory of the game. Be sure to keep file names and folder structures the same! You can toggle mod support on or off via the "Settings" section of "version.ini".

You can use programs such as DTZxPorter's [Hunter](https://dtzxporter.com/tools/hunter) to extract the game files.

## PLUGIN LOADING

To add functionality to the game, you can put additional DLL or ASI files in the "plugins" folder in the root directory of the game, allowing you to execute arbitrary code. If the folder does not exist (it doesn't by default), create it. You can toggle plugin loading support on or off via the "Settings" section of "version.ini"/"dinput8.ini".

## OVERLAY MENU

Press INSERT in-game to open the overlay. It shows mod/plugin settings and, for any loaded plugin that opts in, an expandable menu entry - click the arrow next to a plugin's name to open its options.

The toggle key can be changed by adding a "ToggleKey" entry under "[Settings]" in the modloader's config file, e.g. "ToggleKey=F1". Accepts key names like "Insert", "Home", "F1"-"F12", "Delete", arrow keys, or a single letter/digit ("P", "9", ...). Defaults to "Insert" if not set or not recognised.

## CREDITS

TsudaKageyu - [MinHook](https://github.com/TsudaKageyu/minhook)
ocornut - [Dear ImGui](https://github.com/ocornut/imgui)
