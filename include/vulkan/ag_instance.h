#ifndef ag_instance_h
#define ag_instance_h

#include <vulkan/vulkan.h>
#include "vulkan/ag_surface.h"
#include "vulkan/ag_swapchain.h"
#include "vulkan/ag_cmd_pool.h"
#include "vulkan/ag_frame_sync.h"

typedef struct AgApplication {
    VkBool32 running;

    VkInstance instance;
    VkPhysicalDevice physical_device;
    VkDevice logical_device;

    VkQueue graphicsQueue;
    uint32_t graphicsFamilyIndex;

    VkQueue presentQueue;
    uint32_t presentFamilyIndex;

    AgSurface *surface;
    AgSwapchain *swapchain;

    AgCommandPool *commandPool;

    AgFrameSync sync[AG_MAX_FRAMES_IN_FLIGHT];
    uint32_t currentFrame;

} AgApplication;

AgApplication* agCreateApp();

uint32_t agFindQueueFamily(
    VkQueueFamilyProperties *queueFamilies, 
    uint32_t queueFamilyCount, 
    uint32_t requiredFlags
);

AgApplication* agGetAppInstance();

#endif // ag_instance_h