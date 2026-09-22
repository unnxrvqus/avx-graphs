#include <stdio.h>
#include <stdlib.h>

#include "vulkan/ag_instance.h"
#include "vulkan/ag_cmd_pool.h"
#include "vulkan/ag_instance.h"
#include "vulkan/ag_cmd_buffer.h"
#include "ag_log.h"

AgCommandPool _cmdPool = {
    .commandBufferCapacity = 0,
    .commandBufferCount = 0,
    .handle = 0,
    .commandBuffers = NULL
};

void _agAddCommandBufferToPool(AgCommandBuffer *buffer) {

    if (_cmdPool.commandBufferCount < _cmdPool.commandBufferCapacity) {
        _cmdPool.commandBuffers[_cmdPool.commandBufferCount++] = buffer;
    }

}

void agCommandPoolAllocateBuffer(AgCommandBufferTag tag) {
     
    VkCommandBufferAllocateInfo info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = _cmdPool.handle,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };

    VkCommandBuffer _buffer;

    if (vkAllocateCommandBuffers(
            agGetAppInstance()->logical_device,
            &info,
            &_buffer
        ) != VK_SUCCESS)
    {
        return;
    }

    AgCommandBuffer *buffer = malloc(sizeof(AgCommandBuffer));

    buffer->tag = tag;
    buffer->handle = _buffer;
    buffer->id = _cmdPool.commandBufferCount;
    buffer->begin = _agCmdBuffer_begin;
    buffer->end = _agCmdBuffer_end;
    
    _agAddCommandBufferToPool(buffer);

    agLog("Allocated command buffer to pool: %p with tag: %s", 
        _cmdPool,
        tag == AG_CMD_BUF_TAG_GRAPHICS ? "AG_CMD_BUF_TAG_GRAPHICS" : "Unknow"
    );
}

void agCreateCommandPool() {
    AgApplication *app = agGetAppInstance();

    VkCommandPoolCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .queueFamilyIndex = app->graphicsFamilyIndex,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
    };

    VkResult result = vkCreateCommandPool(
        app->logical_device,
        &createInfo,
        NULL,
        &_cmdPool.handle
    );

    if(result != VK_SUCCESS) {
        printf("Something went wrong...\n");
    }

    _cmdPool.commandBufferCapacity = 16;
    _cmdPool.commandBufferCount = 0;
    _cmdPool.commandBuffers = calloc(
        _cmdPool.commandBufferCapacity, 
        sizeof(AgCommandBuffer)
    );

    if(_cmdPool.commandBuffers == NULL) {
        printf("Something went wrong...");
    }

    agLog("Command pool created");

    app->commandPool = &_cmdPool;
}

void agDestroyCommandPool() {
    AgApplication *app = agGetAppInstance();
    AgCommandPool *commandPool = app->commandPool;
    vkDestroyCommandPool(app->logical_device, commandPool->handle, NULL); //TODO: we need to control user access after free
}