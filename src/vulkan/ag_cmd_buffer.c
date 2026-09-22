#include <stdio.h>

#include "vulkan/ag_cmd_buffer.h"
#include "ag_log.h"

void _agCmdBuffer_begin(AgCommandBuffer *this) {

    VkResult result;

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = 0,
        .pInheritanceInfo = NULL
    };

    result = vkBeginCommandBuffer(
        this->handle,
        &beginInfo
    );

    if(result != VK_SUCCESS) {
        printf("Something went wrong...\n");
    }

    agLog("Started recording in command buffer with id: %d", this->id);
}

void _agCmdBuffer_end(AgCommandBuffer *this) {
    VkResult result = vkEndCommandBuffer(this->handle);

    if(result != VK_SUCCESS) {
        printf("Something went wrong...");
    }

    agLog("Finished recording in command buffer with id: %d", this->id);
}
