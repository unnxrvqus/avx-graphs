#ifndef ag_swapchain_h
#define ag_swapchain_h

#include <stdint.h>

#include <vulkan/vulkan.h>

typedef struct AgSwapchain {
    VkSwapchainKHR swapchain;
    VkPresentModeKHR presentMode;
    VkSurfaceFormatKHR format;
    VkImage* images;
    VkImageView *imageViews;
    uint32_t imageCount;
} AgSwapchain;

void ag_create_swapchain();
AgSwapchain* ag_get_swapchain();

#endif // ag_swapchain_h