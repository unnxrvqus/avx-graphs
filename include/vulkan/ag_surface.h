#ifndef ag_surface_h
#define ag_surface_h

#include <vulkan/vulkan.h>
#include <stdint.h>

#include "platform/ag_window.h"

typedef struct AgSurfaceData AgSurfaceData;

typedef struct AgSurface {
    VkSurfaceKHR handle;
    AgWindow* window;
    uint32_t width;
    uint32_t height;
} AgSurface;

void agCreateSurface();
AgSurface* agGetSurfaceInstance();

#endif // ag_surface_h