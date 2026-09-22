#ifndef ag_cmd_buffer_h
#define ag_cmd_buffer_h

#include <vulkan/vulkan.h>

typedef enum AgCommandBufferTag {
    AG_CMD_BUF_TAG_GRAPHICS
} AgCommandBufferTag;

typedef struct AgCommandBuffer AgCommandBuffer;

struct AgCommandBuffer {
    AgCommandBufferTag tag;
    VkCommandBuffer handle;
    uint32_t id;

    void (*begin)(AgCommandBuffer *this);
    void (*end)(AgCommandBuffer *this);
};

void _agCmdBuffer_begin(AgCommandBuffer *this);
void _agCmdBuffer_end(AgCommandBuffer *this);

#endif // ag_cmd_buffer_h