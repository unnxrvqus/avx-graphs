#include <stdio.h>
#include <vulkan/vulkan.h>

#include "vulkan/ag_instance.h"
#include "vulkan/ag_surface.h"
#include "vulkan/ag_swapchain.h"
#include "ag_log.h"

AgSwapchain _swapchain = {
    .imageCount = 0,
    .images = NULL,
    .handle = 0,
    .format = 0,
    .presentMode = 0
};

AgSwapchain* agGetSwapchainIsntance() {
    return &_swapchain;
}

VkResult agCreateSwapchainImageViews(
    AgApplication *application
) {
    for (uint32_t i = 0; i < _swapchain.imageCount; i++) {

        VkImageViewCreateInfo info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,

            .image = _swapchain.images[i],

            .viewType = VK_IMAGE_VIEW_TYPE_2D,

            .format = _swapchain.format.format,

            .components = {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY
            },

            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,

                .baseMipLevel = 0,
                .levelCount = 1,

                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        VkResult result = vkCreateImageView(
            application->logical_device,
            &info,
            NULL,
            &_swapchain.imageViews[i]
        );

        if (result != VK_SUCCESS) {
            agLog("Failed to create swapchain image view");
            return result;
        }
    }

    agLog("Swapchain image views created");

    return VK_SUCCESS;
}


void agAquireSwapchainImages() {

    VkResult result = vkGetSwapchainImagesKHR(
        agGetAppInstance()->logical_device,
        _swapchain.handle,
        &_swapchain.imageCount,
        NULL
    );

    if (result != VK_SUCCESS) {
        agLog(
            "Failed to get swapchain image count: %d",
            result
        );
        return;
    }

    agLog(
        "Swapchain image count: %u",
        _swapchain.imageCount
    );
    
    _swapchain.images = malloc(sizeof(VkImage) * _swapchain.imageCount);

    if (_swapchain.images == NULL) {
        agLog("Failed to allocate swapchain images");
        return;
    }

    result = vkGetSwapchainImagesKHR(
        agGetAppInstance()->logical_device,
        _swapchain.handle,
        &_swapchain.imageCount,
        _swapchain.images
    );

    if (result != VK_SUCCESS) {
        agLog(
            "Failed to get swapchain images: %d",
            result
        );

        free(_swapchain.images);
        return;
    }

    for (uint32_t i = 0; i < _swapchain.imageCount; i++) {
    
        agLog(
            "Swapchain image %u: %p",
            i,
            (void *)_swapchain.images[i]
        );

        _swapchain.imageViews = malloc(sizeof(VkImageView) * _swapchain.imageCount);
    
    }

    result = agCreateSwapchainImageViews(
        agGetAppInstance()
    );

}

VkSurfaceFormatKHR agChooseSurfaceFormat(
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

VkPresentModeKHR agChooseSurfacePresentMode(
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

void agCreateSwapchain() {
    VkResult result;

    AgApplication *app = agGetAppInstance();
    
    // Get amount of avaiable surface formats
    uint32_t surfaceFormatCount = UINT32_MAX;
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        app->physical_device,
        app->surface->handle,
        &surfaceFormatCount,
        NULL
    );

    // Get avaiable formats
    VkSurfaceFormatKHR surfaceFormats[surfaceFormatCount];
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(
        app->physical_device,
        app->surface->handle,
        &surfaceFormatCount,
        surfaceFormats
    );

    if(result != VK_SUCCESS) {
        printf("Failed to get surface avaible formats: %d\n", result);
        return;
    }

    // Choose surface format we want 
    VkSurfaceFormatKHR chosenFormat = agChooseSurfaceFormat(
        surfaceFormats, 
        surfaceFormatCount, 
        VK_FORMAT_B8G8R8A8_SRGB,
        VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
    );

    // Get amount of avaiable present modes
    uint32_t presentModeCount = 0;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(
        app->physical_device,
        app->surface->handle,
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
        app->physical_device,
        app->surface->handle,
        &presentModeCount,
        presentModes
    );
    
    // Chose present mode: MAILBOX if avaiable else FIFO
    VkPresentModeKHR chosenPresentMode = agChooseSurfacePresentMode(
        presentModes, 
        presentModeCount
    );

    // Get current surface capabilities
    VkSurfaceCapabilitiesKHR capabilities;
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        app->physical_device,
        app->surface->handle,
        &capabilities
    );

    if (result != VK_SUCCESS) {
        printf("Failed to get surface capabilities: %d", result);
        return;
    }

    agLog(
        "Selected surface format: %d",
        chosenFormat.format
    );

    agLog(
        "Selected color space: %d",
        chosenFormat.colorSpace
    );

    agLog(
        "Selected present mode: %s",
        chosenPresentMode == VK_PRESENT_MODE_MAILBOX_KHR ? "MAILBOX" : "FIFO"
    );

    // Create swapchain:
    VkSwapchainCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        
        .surface = app->surface->handle,

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
        app->logical_device,
        &createInfo,
        NULL,
        &swapchain
    );

    if(result != VK_SUCCESS) {
        printf("Something went wrong...\n");
    }

    agLog("Swapchain created");

    _swapchain.handle = swapchain;

    _swapchain.format = chosenFormat;
    _swapchain.presentMode = chosenPresentMode;

    agAquireSwapchainImages();

    app->swapchain = &_swapchain;
}