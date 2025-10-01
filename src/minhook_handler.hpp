#pragma once

class MinHookHandler {
public:
    static bool Initialise();
    static void EnableAllHooks();
    static void Shutdown();

private:
    static bool mhInitialised;
};