#ifndef ag_instance_h
#define ag_instance_h

#include <vulkan/vulkan.h>

typedef struct AgDevice {
    VkInstance instance;
    VkPhysicalDevice physical;
    VkDevice logical;

    VkQueue graphicsQueue;
    uint32_t graphicsFamilyIndex;

    VkQueue presentQueue;
    uint32_t presentFamilyIndex;
} AgDevice;

void ag_create_instance();

uint32_t ag_find_queue_family(
    VkQueueFamilyProperties *queueFamilies, 
    uint32_t queueFamilyCount, 
    uint32_t requiredFlags
);

AgDevice* ag_get_device();

#endif // ag_instance_h