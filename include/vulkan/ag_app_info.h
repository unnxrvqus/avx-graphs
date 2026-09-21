#include <vulkan/vulkan.h>

VkApplicationInfo app_info = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pApplicationName = "AvxGraphs", //TODO: remove hard-coded name of framework
    .applicationVersion = VK_MAKE_VERSION(0, 1, 0), //TODO: remove hard-coded version of framework
    .pEngineName = "AvxGraphs",
    .engineVersion = VK_MAKE_VERSION(0, 1, 0), //TODO:: ??
    .apiVersion = VK_API_VERSION_1_4,
};
