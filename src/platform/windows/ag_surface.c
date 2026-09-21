#include <stdlib.h>
#include <stdio.h>

#include "vulkan/ag_surface.h"
#include "ag_platform_headers.h"
#include "vulkan/ag_instance.h"
#include "platform/ag_window.h"
#include "ag_log.h"


AgSurface surface = {
    .width = 0,
    .height = 0,
    .window = NULL,
    .surface = 0
};

AgSurface* ag_get_surface() {
    return &surface;
}

void ag_create_surface() {
    surface.window = ag_create_window("AvxGraphs", 1920, 1080);
    AgDevice* device = ag_get_device();
    VkResult result;

    VkWin32SurfaceCreateInfoKHR surfaceInfo = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .hwnd = surface.window->handle,
        .hinstance = (HINSTANCE) surface.window->appInstance,
        .hwnd = (HWND) surface.window->handle
    };

    // Create surface to which we can draw
    result = vkCreateWin32SurfaceKHR(
        device->instance,
        &surfaceInfo,
        NULL,
        &surface.surface
    );

    if(result != VK_SUCCESS) {
        printf("Something went wrong\n");
        return;
    }

    // Check if current graphics queue supports present
    VkBool32 presentSupported;
    vkGetPhysicalDeviceSurfaceSupportKHR(
        device->physical,
        device->graphicsFamilyIndex,
        surface.surface,
        &presentSupported
    );
    
    if(presentSupported == VK_FALSE) {
        // Present is not supported, shit...
        ag_log("Current queue does not support presentation to surface");
        return;
    }

    device->presentFamilyIndex = device->graphicsFamilyIndex;
    ag_log("Surface created successfully");
};