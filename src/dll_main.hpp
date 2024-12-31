#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include <shlobj.h>
#include "disk_cache_enabler.hpp"
#include "minhook_handler.hpp"
#include "mod_loader.hpp"
#include "script_loader.hpp"
#include "utilities.hpp"
#include "version_wrapper.hpp"

enum GameID {
    GAME_UNSUPPORTED = 0,
    GAME_OUTLAWS = 1,
    GAME_AVATAR = 2
};

class DLLMain {
public:
    static std::string exeName;
    static GameID gameID;
};
