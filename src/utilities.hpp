#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <string>

class Utilities {
public:
    class Files {
    public:
        static bool FileExists(LPCSTR file_path);
    };

    class MainModule {
    private:
        uintptr_t BaseAddress;
        uintptr_t EndAddress;
        std::string ModulePath;

    public:
        MainModule();
        ~MainModule();
        
        uintptr_t GetBaseAddress() const; // Gets the module base address
        uintptr_t GetCodeSize() const; // Gets the size of code reported by the PE header
        size_t Begin() const;
        size_t End() const;
        std::string GetModulePath();
    };

    class Processes {
    public:
        static uintptr_t FindPatternAddress(const unsigned char* pattern);
        static uintptr_t FindPatternAddressMask(const unsigned char* pattern, const unsigned char* mask);
    };
};