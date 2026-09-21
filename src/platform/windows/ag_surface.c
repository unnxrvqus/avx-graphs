#include <stdlib.h>
#include <stdio.h>

#include "vulkan/ag_surface.h"
#include "ag_platform_headers.h"
#include "vulkan/ag_instance.h"
#include "platform/ag_window.h"
#include "ag_log.h"


AgSurface gSurface = {
    .width = 0,
    .height = 0,
    .window = NULL,
    .surface = 0,
};

AgSurface* ag_get_surface() {
    return &gSurface;
}

void ag_create_surface() {
    
    gSurface.window = ag_create_window("AvxGraphs", 1920, 1080);

    AgApplication* app = ag_get_app_instance();
    VkResult result;

    VkWin32SurfaceCreateInfoKHR surfaceInfo = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .hwnd = gSurface.window->handle,
        .hinstance = (HINSTANCE) gSurface.window->appInstance,
        .hwnd = (HWND) gSurface.window->handle
    };

    // Create surface to which we can draw
    result = vkCreateWin32SurfaceKHR(
        app->instance,
        &surfaceInfo,
        NULL,
        &gSurface.surface
    );

    if(result != VK_SUCCESS) {
        printf("Something went wrong\n");
        return;
    }

    // Check if current graphics queue supports present
    VkBool32 presentSupported;
    vkGetPhysicalDeviceSurfaceSupportKHR(
        app->physical,
        app->graphicsFamilyIndex,
        gSurface.surface,
        &presentSupported
    );
    
    if(presentSupported == VK_FALSE) {
        // Present is not supported, shit...
        ag_log("Current queue does not support presentation to surface");
        return;
    }

    app->presentFamilyIndex = app->graphicsFamilyIndex;
    ag_log("Surface created successfully");

    app->surface = &gSurface;
};