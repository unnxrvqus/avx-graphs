#ifndef ag_surface_h
#define ag_surface_h

#include <vulkan/vulkan.h>
#include <stdint.h>

#include "platform/ag_window.h"

typedef struct AgSurfaceData AgSurfaceData;

typedef struct AgSurface {
    VkSurfaceKHR surface;
    AgWindow* window;
    uint32_t width;
    uint32_t height;
} AgSurface;

void ag_create_surface();
AgSurface* ag_get_surface();

#endif // ag_surface_h