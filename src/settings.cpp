#include "pch.hpp"
#include "settings.hpp"

#include <algorithm>
#include <cctype>
#include <unordered_map>

bool Settings::EnableMods;
bool Settings::EnablePlugins;
bool Settings::EnableGUI;
UINT Settings::MenuToggleKey;
std::string Settings::MenuToggleKeyName;

void Settings::LoadSettings() {
    using SettingsParser = Utilities::SettingsParser;

    EnableMods = SettingsParser::GetBoolean("Settings", "EnableMods", true);
    EnablePlugins = SettingsParser::GetBoolean("Settings", "EnablePlugins", true);
    EnableGUI = SettingsParser::GetBoolean("Settings", "EnableGUI", true);

    std::string toggleKeyName = SettingsParser::GetString("Settings", "ToggleKey", "Insert");
    MenuToggleKey = ParseVirtualKeyName(toggleKeyName, VK_INSERT);
    MenuToggleKeyName = toggleKeyName.empty() ? "Insert" : toggleKeyName;
}

LPCSTR Settings::GetConfigPath() {
    thread_local static std::string path = (
        g_DllInfo.directory /
        (std::string{ g_DllInfo.stem } + ".ini")
        ).string();

    return path.c_str();
}

UINT Settings::ParseVirtualKeyName(const std::string& name, UINT defaultValue) {
    if (name.empty()) return defaultValue;

    std::string upper = name;
    std::transform(upper.begin(), upper.end(), upper.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });

    if (upper.size() == 1) {
        char c = upper[0];
        if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
            return static_cast<UINT>(c);
        }
    }

    static const std::unordered_map<std::string, UINT> namedKeys = {
        { "INSERT", VK_INSERT }, { "DELETE", VK_DELETE }, { "HOME", VK_HOME }, { "END", VK_END },
        { "PAGEUP", VK_PRIOR }, { "PAGEDOWN", VK_NEXT },
        { "UP", VK_UP }, { "DOWN", VK_DOWN }, { "LEFT", VK_LEFT }, { "RIGHT", VK_RIGHT },
        { "TAB", VK_TAB }, { "ESCAPE", VK_ESCAPE }, { "ESC", VK_ESCAPE },
        { "SPACE", VK_SPACE }, { "ENTER", VK_RETURN }, { "RETURN", VK_RETURN }, { "BACKSPACE", VK_BACK },
        { "PAUSE", VK_PAUSE }, { "CAPSLOCK", VK_CAPITAL }, { "SCROLLLOCK", VK_SCROLL }, { "NUMLOCK", VK_NUMLOCK },
        { "F1", VK_F1 }, { "F2", VK_F2 }, { "F3", VK_F3 }, { "F4", VK_F4 },
        { "F5", VK_F5 }, { "F6", VK_F6 }, { "F7", VK_F7 }, { "F8", VK_F8 },
        { "F9", VK_F9 }, { "F10", VK_F10 }, { "F11", VK_F11 }, { "F12", VK_F12 },
        { "NUMPAD0", VK_NUMPAD0 }, { "NUMPAD1", VK_NUMPAD1 }, { "NUMPAD2", VK_NUMPAD2 },
        { "NUMPAD3", VK_NUMPAD3 }, { "NUMPAD4", VK_NUMPAD4 }, { "NUMPAD5", VK_NUMPAD5 },
        { "NUMPAD6", VK_NUMPAD6 }, { "NUMPAD7", VK_NUMPAD7 }, { "NUMPAD8", VK_NUMPAD8 }, { "NUMPAD9", VK_NUMPAD9 },
    };

    auto it = namedKeys.find(upper);
    return (it != namedKeys.end()) ? it->second : defaultValue;
}
