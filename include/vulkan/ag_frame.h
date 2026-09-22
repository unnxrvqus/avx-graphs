#ifndef ag_frame_h
#define ag_frame_h

#include "vulkan/ag_cmd_buffer.h"

typedef struct AgFrame AgFrame;

struct AgFrame {
    uint32_t index;

    void (*record)(const AgFrame *this, AgCommandBuffer *cmdBuffer);
    void (*submit)(const AgFrame *this, AgCommandBuffer *cmdBuffer);
    void (*present)(const AgFrame *this);
};

const AgFrame* agAcquireNextFrame();

#endif // ag_frame_h