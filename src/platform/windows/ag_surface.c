#include <stdlib.h>
#include <stdio.h>

#include "vulkan/ag_surface.h"
#include "ag_platform_headers.h"
#include "vulkan/ag_instance.h"
#include "platform/ag_window.h"
#include "ag_log.h"


AgSurface _surface = {
    .width = 0,
    .height = 0,
    .window = NULL,
    .handle = 0,
};

AgSurface* agGetSurfaceInstance() {
    return &_surface;
}

void agCreateSurface() {
    
    _surface.window = agCreateWindow("AvxGraphs", 1920, 1080);

    AgApplication* app = agGetAppInstance();
    VkResult result;

    VkWin32SurfaceCreateInfoKHR surfaceInfo = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .hwnd = _surface.window->handle,
        .hinstance = (HINSTANCE) _surface.window->appInstance,
        .hwnd = (HWND) _surface.window->handle
    };

    // Create surface to which we can draw
    result = vkCreateWin32SurfaceKHR(
        app->instance,
        &surfaceInfo,
        NULL,
        &_surface.handle
    );

    if(result != VK_SUCCESS) {
        printf("Something went wrong\n");
        return;
    }

    // Check if current graphics queue supports present
    VkBool32 presentSupported;
    vkGetPhysicalDeviceSurfaceSupportKHR(
        app->physical_device,
        app->graphicsFamilyIndex,
        _surface.handle,
        &presentSupported
    );
    
    if(presentSupported == VK_FALSE) {
        // Present is not supported, shit...
        agLog("Current queue does not support presentation to surface");
        return;
    }

    app->presentFamilyIndex = app->graphicsFamilyIndex;
    agLog("Surface created successfully");

    app->surface = &_surface;
};