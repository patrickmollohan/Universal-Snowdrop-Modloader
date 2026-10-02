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

### Plugin config files

API-based plugins store their settings in their own INI file next to the plugin, named after it (e.g. "plugins/PerformanceTweaks.ini" for "PerformanceTweaks.dll"), under a "[Settings]" section. The file is created the first time the plugin saves a setting. Nothing plugin-specific is written to the modloader's config file other than the "[Plugins]" enable/disable list above.

If two plugins share a name and differ only by extension (e.g. "Foo.dll" and "Foo.asi"), they can't share "Foo.ini", so each uses its full file name instead: "Foo.dll.ini" and "Foo.asi.ini". When this first happens and a "Foo.ini" already exists, it is renamed to belong to the plugin that was added first (judged by file creation time). If one of the pair is later removed, the remaining plugin keeps using its "Foo.<ext>.ini" file.

## OVERLAY MENU

Press INSERT in-game to open the overlay. It shows mod/plugin settings and, for any loaded plugin that opts in, an expandable menu entry - click the arrow next to a plugin's name to open its options.

The toggle key can be changed by adding a "ToggleKey" entry under "[Settings]" in the modloader's config file, e.g. "ToggleKey=F1". Accepts key names like "Insert", "Home", "F1"-"F12", "Delete", arrow keys, or a single letter/digit ("P", "9", ...). Defaults to "Insert" if not set or not recognised.

## BUILDING

Run "premake5 vs2022" in the repository root and open the generated solution in the "build" folder. It contains the modloader and the example plugins (currently PerformanceTweaks, under "plugin_templates"), for both the "version" and "dinput8" configurations. Each configuration outputs to "bin/<configuration>/" with plugins in "bin/<configuration>/plugins/", so the folder can be copied straight into the game's root directory.

To add another plugin to the solution, put its sources in "plugin_templates/<Name>/" and add a "PluginProject("<Name>")" line at the bottom of "premake5.lua".

## CREDITS

TsudaKageyu - [MinHook](https://github.com/TsudaKageyu/minhook)

ocornut - [Dear ImGui](https://github.com/ocornut/imgui)

mackron - [miniaudio](https://github.com/mackron/miniaudio)
