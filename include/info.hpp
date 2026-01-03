#pragma once

#include <winresrc.h>

#define DLL_VERSION_MAJOR 1
#define DLL_VERSION_MINOR 0
#define DLL_VERSION_BUILD 0
#define DLL_VERSION_REVISION 0

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define DLL_VERSION_STRING \
    STR(DLL_VERSION_MAJOR) "." STR(DLL_VERSION_MINOR) "." STR(DLL_VERSION_BUILD) "." STR(DLL_VERSION_REVISION)

#define DLL_COMMENTS "May the Force be with you, always!"
#define DLL_COMPANY_NAME "Patrick Mollohan"
#define DLL_FILE_DESCRIPTION "A universal framework for modloader creation using DLL proxying."
#define DLL_INTERNAL_NAME "Universal-Modloader-Core"
#define DLL_LEGAL_COPYRIGHT "MIT License"
#define DLL_ORIGINAL_FILENAME "Universal-Modloader-Core.dll"
#define DLL_PRODUCT_NAME "Universal Modloader Core"
#define DLL_PRODUCT_VERSION DLL_VERSION_STRING
#define DLL_UPDATE_URL "https://github.com/patrickmollohan/Universal-Modloader-Core"
