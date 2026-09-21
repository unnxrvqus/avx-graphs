#ifndef ag_platform_headers_h
#define ag_platform_headers_h

#include <vulkan/vulkan.h>

#ifdef __WIN32
#define AG_PLATFORM_WINDOWS
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#endif // PLATFORM DETECTION

#endif // ag_platform_headers_h