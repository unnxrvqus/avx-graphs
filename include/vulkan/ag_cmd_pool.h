#ifndef ag_cmd_pool_h
#define ag_cmd_pool_h

#include <vulkan/vulkan.h>
#include "vulkan/ag_cmd_buffer.h"

typedef struct AgCommandPool {
    VkCommandPool handle;

    AgCommandBuffer **commandBuffers;
    uint32_t commandBufferCount;
    uint32_t commandBufferCapacity;

} AgCommandPool;

void agCreateCommandPool();

void agCommandPoolAllocateBuffer(AgCommandBufferTag tag);

#endif // ag_cmd_pool_h