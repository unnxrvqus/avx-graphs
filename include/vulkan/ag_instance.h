#ifndef ag_instance_h
#define ag_instance_h

#include <vulkan/vulkan.h>
#include "vulkan/ag_surface.h"
#include "vulkan/ag_swapchain.h"

typedef struct AgApplication {
    VkInstance instance;
    VkPhysicalDevice physical;
    VkDevice logical;

    VkQueue graphicsQueue;
    uint32_t graphicsFamilyIndex;

    VkQueue presentQueue;
    uint32_t presentFamilyIndex;

    AgSurface *surface;
    AgSwapchain *swapchain;

    VkCommandPool commandPool;
} AgApplication;

AgApplication* ag_create_app_instance();

uint32_t ag_find_queue_family(
    VkQueueFamilyProperties *queueFamilies, 
    uint32_t queueFamilyCount, 
    uint32_t requiredFlags
);

AgApplication* ag_get_app_instance();

#endif // ag_instance_h