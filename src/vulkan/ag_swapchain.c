#include <stdio.h>
#include <vulkan/vulkan.h>

#include "vulkan/ag_instance.h"
#include "vulkan/ag_surface.h"
#include "vulkan/ag_swapchain.h"
#include "ag_log.h"

AgSwapchain gSwapchain = {
    .imageCount = 0,
    .images = NULL,
    .swapchain = 0,
    .format = 0,
    .presentMode = 0
};

AgSwapchain* ag_get_swapchain() {
    return &gSwapchain;
}

VkResult ag_create_swapchain_image_views(
    AgApplication *application
) {
    for (uint32_t i = 0; i < gSwapchain.imageCount; i++) {

        VkImageViewCreateInfo info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,

            .image = gSwapchain.images[i],

            .viewType = VK_IMAGE_VIEW_TYPE_2D,

            .format = gSwapchain.format.format,

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
            application->logical,
            &info,
            NULL,
            &gSwapchain.imageViews[i]
        );

        if (result != VK_SUCCESS) {
            ag_log("Failed to create swapchain image view");
            return result;
        }
    }

    ag_log("Swapchain image views created");

    return VK_SUCCESS;
}


void ag_aquire_images() {

    VkResult result = vkGetSwapchainImagesKHR(
        ag_get_app_instance()->logical,
        gSwapchain.swapchain,
        &gSwapchain.imageCount,
        NULL
    );

    if (result != VK_SUCCESS) {
        ag_log(
            "Failed to get swapchain image count: %d",
            result
        );
        return;
    }

    ag_log(
        "Swapchain image count: %u",
        gSwapchain.imageCount
    );
    
    gSwapchain.images = malloc(sizeof(VkImage) * gSwapchain.imageCount);

    if (gSwapchain.images == NULL) {
        ag_log("Failed to allocate swapchain images");
        return;
    }

    result = vkGetSwapchainImagesKHR(
        ag_get_app_instance()->logical,
        gSwapchain.swapchain,
        &gSwapchain.imageCount,
        gSwapchain.images
    );

    if (result != VK_SUCCESS) {
        ag_log(
            "Failed to get swapchain images: %d",
            result
        );

        free(gSwapchain.images);
        return;
    }

    for (uint32_t i = 0; i < gSwapchain.imageCount; i++) {
    
        ag_log(
            "Swapchain image %u: %p",
            i,
            (void *)gSwapchain.images[i]
        );

        gSwapchain.imageViews = malloc(sizeof(VkImageView) * gSwapchain.imageCount);
    
    }

    result = ag_create_swapchain_image_views(
        ag_get_app_instance()
    );

}

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

    AgApplication *app = ag_get_app_instance();
    
    // Get amount of avaiable surface formats
    uint32_t surfaceFormatCount = UINT32_MAX;
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        app->physical,
        app->surface->surface,
        &surfaceFormatCount,
        NULL
    );

    // Get avaiable formats
    VkSurfaceFormatKHR surfaceFormats[surfaceFormatCount];
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(
        app->physical,
        app->surface->surface,
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
        app->physical,
        app->surface->surface,
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
        app->physical,
        app->surface->surface,
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
        app->physical,
        app->surface->surface,
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
        
        .surface = app->surface->surface,

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
        app->logical,
        &createInfo,
        NULL,
        &swapchain
    );

    if(result != VK_SUCCESS) {
        printf("Something went wrong...\n");
    }

    ag_log("Swapchain created");

    gSwapchain.swapchain = swapchain;

    gSwapchain.format = chosenFormat;
    gSwapchain.presentMode = chosenPresentMode;

    ag_aquire_images();

    app->swapchain = &gSwapchain;
}