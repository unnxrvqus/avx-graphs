#ifndef ag_frame_sync_h
#define ag_frame_sync_h

#include <vulkan/vulkan.h>

#define AG_MAX_FRAMES_IN_FLIGHT 2

typedef struct AgFrameSync {
    VkSemaphore imageAvaiable;
    VkSemaphore renderFinished;
    VkFence inFlight;
} AgFrameSync;

void agCreateFrameSync();

#endif // ag_frame_sync_h