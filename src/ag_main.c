#include "vulkan/ag_instance.h"
#include "vulkan/ag_surface.h"
#include "vulkan/ag_swapchain.h"
#include "vulkan/ag_cmd_pool.h"
#include "vulkan/ag_cmd_buffer.h"
#include "vulkan/ag_frame.h"
#include "vulkan/ag_frame_sync.h"
#include "platform/ag_poll_events.h"

int main(void) {
    AgApplication *app = agCreateApp();

    agCreateSurface();

    agCreateSwapchain(); // Also creates Images and Image Views

    agCreateCommandPool();

    agCreateFrameSync();

    agCommandPoolAllocateBuffer(AG_CMD_BUF_TAG_GRAPHICS);

    agCommandPoolAllocateBuffer(AG_CMD_BUF_TAG_GRAPHICS);

    while(app->running) {
        AgCommandBuffer *cmdBuffer = app->commandPool->commandBuffers[app->currentFrame];

        agPollEvents();

        const AgFrame *frame = agAcquireNextFrame();
        
        if (!frame) continue;

        frame->record(frame, cmdBuffer);

        frame->submit(frame, cmdBuffer);

        frame->present(frame);
        
        app->currentFrame = (app->currentFrame + 1) % AG_MAX_FRAMES_IN_FLIGHT;
    }

    return 0;
}