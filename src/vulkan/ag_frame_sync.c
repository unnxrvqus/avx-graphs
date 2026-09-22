#include "vulkan/ag_frame_sync.h"
#include "vulkan/ag_instance.h"
#include "ag_log.h"

void agCreateFrameSync() {
    AgApplication *app = agGetAppInstance();

    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0
    };

    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    for (uint32_t i = 0; i < AG_MAX_FRAMES_IN_FLIGHT; ++i) {

        VkResult result;

        result = vkCreateSemaphore(
            app->logical_device,
            &semaphoreInfo,
            NULL,
            &app->sync[i].imageAvaiable
        );

        if (result != VK_SUCCESS) {
            // обработка ошибки
            abort();
        }

        result = vkCreateSemaphore(
            app->logical_device,
            &semaphoreInfo,
            NULL,
            &app->sync[i].renderFinished
        );

        if (result != VK_SUCCESS) {
            abort();
        }

        result = vkCreateFence(
            app->logical_device,
            &fenceInfo,
            NULL,
            &app->sync[i].inFlight
        );

        if (result != VK_SUCCESS) {
            abort();
        }
    }

    agLog("FrameSync created for %d iamges", AG_MAX_FRAMES_IN_FLIGHT);
}