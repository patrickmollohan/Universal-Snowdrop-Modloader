#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <atomic>
#include <filesystem>
#include <memory>
#include <minhook.h>
#include <mutex>
#include <span>
#include <string>
#include <unknwn.h>
#include <vector>
#include <windows.h>

#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))
