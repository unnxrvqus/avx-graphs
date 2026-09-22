#include "vulkan/ag_frame.h"
#include "vulkan/ag_instance.h"

void agTransitionImageLayout(
    AgCommandBuffer *cmdBuffer,
    VkImage image,
    VkImageLayout oldLayout,
    VkImageLayout newLayout
) {
    VkImageMemoryBarrier2 barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext = NULL,

        .oldLayout = oldLayout,
        .newLayout = newLayout,

        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .image = image,

        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    if (
        oldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR &&
        newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    ) {
        barrier.srcStageMask =
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

        barrier.srcAccessMask = 0;

        barrier.dstStageMask =
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

        barrier.dstAccessMask =
            VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    }
    else if (
        oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL &&
        newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    ) {
        barrier.srcStageMask =
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

        barrier.srcAccessMask =
            VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

        barrier.dstStageMask =
            VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;

        barrier.dstAccessMask = 0;
    }

    VkDependencyInfo dependencyInfo = {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .pNext = NULL,

        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &barrier
    };

    vkCmdPipelineBarrier2(
        cmdBuffer->handle,
        &dependencyInfo
    );
}

void _AgFrame_record(const AgFrame *this, AgCommandBuffer *cmdBuffer) {
    AgApplication *app = agGetAppInstance();

    cmdBuffer->begin(cmdBuffer);

    VkImage image = app->swapchain->images[this->index];

    agTransitionImageLayout(
        cmdBuffer,
        image,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    );

    agTransitionImageLayout(
        cmdBuffer,
        image,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    );

    cmdBuffer->end(cmdBuffer);
}

void _AgFrame_submit(const AgFrame *this, AgCommandBuffer *cmdBuffer) {
    AgApplication *app = agGetAppInstance();

    AgFrameSync sync = app->sync[app->currentFrame];

    vkResetFences(
        app->logical_device,
        1,
        &sync.inFlight
    );

    VkSemaphoreSubmitInfo waitSemaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = sync.imageAvaiable,
        .value = 0,
        .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .deviceIndex = 0
    };

    VkCommandBufferSubmitInfo commandBufferInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .commandBuffer = cmdBuffer->handle,
        .deviceMask = 0
    };

    VkSemaphoreSubmitInfo signalSemaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = sync.renderFinished,
        .value = 0,
        .stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        .deviceIndex = 0
    };

    VkSubmitInfo2 submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .pNext = NULL,
        .flags = 0,

        .waitSemaphoreInfoCount = 1,
        .pWaitSemaphoreInfos = &waitSemaphoreInfo,
        
        .commandBufferInfoCount = 1,
        .pCommandBufferInfos = &commandBufferInfo,
        
        .signalSemaphoreInfoCount = 1,
        .pSignalSemaphoreInfos = &signalSemaphoreInfo,
    };

    vkQueueSubmit2(
        app->graphicsQueue,
        1,
        &submitInfo,
        sync.inFlight
    );
}

void _AgFrame_present(const AgFrame *this) {
    AgApplication *app = agGetAppInstance();
    AgFrameSync sync = app->sync[app->currentFrame];

    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,

        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &sync.renderFinished,

        .swapchainCount = 1,
        .pSwapchains = &app->swapchain->handle,
        
        .pImageIndices = &this->index,

        .pResults = NULL
    };

    vkQueuePresentKHR(
        app->graphicsQueue,
        &presentInfo
    );
}

static AgFrame _frame = {
    .record = _AgFrame_record,
    .submit = _AgFrame_submit,
    .present = _AgFrame_present,
    .index = -1
};

const AgFrame* agAcquireNextFrame() {
    AgApplication *app = agGetAppInstance();

    AgFrameSync sync = app->sync[app->currentFrame];

    vkWaitForFences(
        app->logical_device,
        1,
        &sync.inFlight,
        VK_TRUE,
        UINT64_MAX
    );

    VkResult result = vkAcquireNextImageKHR(
        app->logical_device,
        app->swapchain->handle,
        UINT64_MAX,
        sync.imageAvaiable,
        VK_NULL_HANDLE,
        &_frame.index
    );

    if (result != VK_SUCCESS &&
        result != VK_SUBOPTIMAL_KHR) {
        return NULL;
    }

    return &_frame;
}