#include <stdio.h>
#include <vulkan/vulkan.h>

#include "vulkan/ag_instance.h"
#include "vulkan/ag_surface.h"
#include "vulkan/ag_swapchain.h"
#include "ag_log.h"

VkSurfaceFormatKHR ag_choose_surface_format(
    VkSurfaceFormatKHR *formats,
    uint32_t count,
    uint32_t format,
    uint32_t colorSpace
) {
    for (uint32_t i = 0; i < count; i++) {
        if (
            formats[i].format == format
            && formats[i].colorSpace == colorSpace
        ) {
            return formats[i];
        }
    }

    return formats[0];
}

VkPresentModeKHR ag_choose_present_mode(
    VkPresentModeKHR *modes,
    uint32_t count
) {
    for (uint32_t i = 0; i < count; i++) {

        if (modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return VK_PRESENT_MODE_MAILBOX_KHR;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

void ag_create_swapchain() {
    VkResult result;

    AgDevice *device = ag_get_device();
    AgSurface *surface = ag_get_surface();
    
    // Get amount of avaiable surface formats
    uint32_t surfaceFormatCount = UINT32_MAX;
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        device->physical,
        surface->surface,
        &surfaceFormatCount,
        NULL
    );

    // Get avaiable formats
    VkSurfaceFormatKHR surfaceFormats[surfaceFormatCount];
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(
        device->physical,
        surface->surface,
        &surfaceFormatCount,
        surfaceFormats
    );

    if(result != VK_SUCCESS) {
        printf("Failed to get surface avaible formats: %d\n", result);
        return;
    }

    // Choose surface format we want 
    VkSurfaceFormatKHR chosenFormat = ag_choose_surface_format(
        surfaceFormats, 
        surfaceFormatCount, 
        VK_FORMAT_B8G8R8A8_SRGB,
        VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
    );

    // Get amount of avaiable present modes
    uint32_t presentModeCount = 0;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(
        device->physical,
        surface->surface,
        &presentModeCount,
        NULL
    );

    if(result != VK_SUCCESS) {
        printf("Failed to get surface present modes: %d\n", result);
        return;
    }

    // Get avaiable present modes
    VkPresentModeKHR presentModes[presentModeCount];
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device->physical,
        surface->surface,
        &presentModeCount,
        presentModes
    );
    
    // Chose present mode: MAILBOX if avaiable else FIFO
    VkPresentModeKHR chosenPresentMode = ag_choose_present_mode(
        presentModes, 
        presentModeCount
    );

    // Get current surface capabilities
    VkSurfaceCapabilitiesKHR capabilities;
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        device->physical,
        surface->surface,
        &capabilities
    );

    if (result != VK_SUCCESS) {
        printf("Failed to get surface capabilities: %d", result);
        return;
    }

    ag_log(
        "Selected surface format: %d",
        chosenFormat.format
    );

    ag_log(
        "Selected color space: %d",
        chosenFormat.colorSpace
    );

    ag_log(
        "Selected present mode: %s",
        chosenPresentMode == VK_PRESENT_MODE_MAILBOX_KHR ? "MAILBOX" : "FIFO"
    );

    // Create swapchain:
    VkSwapchainCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        
        .surface = surface->surface,

        .minImageCount = capabilities.minImageCount,

        .imageFormat = chosenFormat.format,
        .imageColorSpace = chosenFormat.colorSpace,

        .imageExtent = capabilities.currentExtent,

        .imageArrayLayers = 1,

        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, //TODO: what is it?

        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE, //TODO: what is it?

        .preTransform = capabilities.currentTransform,

        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, //TODO: what is it?

        .presentMode = chosenPresentMode,

        .clipped = VK_TRUE, //TODO: what is it?

        .oldSwapchain = VK_NULL_HANDLE
    };

    VkSwapchainKHR swapchain;

    result = vkCreateSwapchainKHR(
        device->logical,
        &createInfo,
        NULL,
        &swapchain
    );

    if(result != VK_SUCCESS) {
        printf("Something went wrong...\n");
    }

    ag_log("Swapchain created");
}