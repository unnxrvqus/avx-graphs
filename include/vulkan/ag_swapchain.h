#ifndef ag_swapchain_h
#define ag_swapchain_h

#include <stdint.h>

#include <vulkan/vulkan.h>

typedef struct AgSwapchain {
    VkSwapchainKHR handle;
    VkPresentModeKHR presentMode;
    VkSurfaceFormatKHR format;
    VkImage* images;
    VkImageView *imageViews;
    uint32_t imageCount;
} AgSwapchain;

void agCreateSwapchain();
AgSwapchain* agGetSwapchainIsntance();

#endif // ag_swapchain_h