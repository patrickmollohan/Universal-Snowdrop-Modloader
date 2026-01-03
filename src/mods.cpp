#include "pch.hpp"
#include "mods.hpp"

bool Mods::LoadMods() {
    if (!Settings::EnableMods) return false;
    return true;
}

bool Mods::UnloadMods() {
    if (!Settings::EnableMods) return false;
    return true;
}
