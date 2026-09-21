#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdlib.h>

#include "vulkan/ag_instance.h"
#include "ag_log.h"
#include "ag_platform_headers.h"

#define AG_QUEUE_FAMILY_INVALID UINT32_MAX

VkApplicationInfo app_info = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pApplicationName = "AvxGraphs", //TODO: remove hard-coded name of framework
    .applicationVersion = VK_MAKE_VERSION(0, 1, 0), //TODO: remove hard-coded version of framework
    .pEngineName = "AvxGraphs",
    .engineVersion = VK_MAKE_VERSION(0, 1, 0), //TODO:: ??
    .apiVersion = VK_API_VERSION_1_4,
};

AgApplication gApplication;

uint32_t ag_find_queue_family(
    VkQueueFamilyProperties *queueFamilies, 
    uint32_t queueFamilyCount, 
    uint32_t requiredFlags
) {
    for(uint32_t i = 0; i < queueFamilyCount; i++) {

        if((queueFamilies[i].queueFlags & requiredFlags) == requiredFlags) {
            return i;
        }
    }
}

AgApplication* ag_get_app_instance() {
    return &gApplication;
}

AgApplication* ag_create_app_instance() {
    gApplication.surface = malloc(sizeof(AgSurface*));

    ag_log("Initializing Vulkan...");

    const char* instanceExtensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,

#ifdef AG_PLATFORM_WINDOWS
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#endif

    };

    VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
        .enabledExtensionCount = 2, //TODO: For now we pass it as-is, but should be controlled in future
        .ppEnabledExtensionNames = instanceExtensions
    };

    // Create instance of Vulkan App
    VkResult result = vkCreateInstance(
        &createInfo,
        NULL,
        &gApplication.instance
    );

    if(result != VK_SUCCESS) {
        printf("Something went wrong...\n");
        return NULL;
    }
    
    ag_log("Vulkan instance created");

    // Get count of avaiable GPUs
    uint32_t deviceCount = 0;
    result = vkEnumeratePhysicalDevices(
        gApplication.instance,
        &deviceCount,
        NULL
    );

    if(result != VK_SUCCESS || deviceCount == 0) {
        printf("Something went wrong...\n");\
        return NULL; //TODO: free memory for g_instance
    }

    ag_log("Avaiable GPUs: %d", deviceCount);

    VkPhysicalDevice devices[deviceCount];
    
    // Get array of avaiable GPUs
    result = vkEnumeratePhysicalDevices(
        gApplication.instance,
        &deviceCount,
        devices
    );
    
    gApplication.physical = devices[0];

    VkPhysicalDeviceProperties properties;
    // Get GPU properties
    vkGetPhysicalDeviceProperties(
        gApplication.physical,
        &properties
    );

    ag_log("Selected GPU: %s", properties.deviceName);

    // Get amount of avaiable queue families on that GPU
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
        gApplication.physical,
        &queueFamilyCount,
        NULL
    );
    
    // Fill array of queue family properties 
    VkQueueFamilyProperties *queueFamilies = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount); 
    vkGetPhysicalDeviceQueueFamilyProperties(
        gApplication.physical,
        &queueFamilyCount,
        queueFamilies
    );

    // Find family with graphics commands avaiable
    gApplication.graphicsFamilyIndex = AG_QUEUE_FAMILY_INVALID;
    gApplication.graphicsFamilyIndex = ag_find_queue_family(queueFamilies, queueFamilyCount, VK_QUEUE_GRAPHICS_BIT);

    if(gApplication.graphicsFamilyIndex == AG_QUEUE_FAMILY_INVALID) {
        printf("Something went wrong...\n");
        return NULL;
    }

    ag_log("Graphics queue family: %d", gApplication.graphicsFamilyIndex);

    // Describe queue creation info
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = gApplication.graphicsFamilyIndex,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };

    const char* deviceExtensions[] = { "VK_KHR_swapchain" };

    // Describe logical device creation info
    VkDeviceCreateInfo deviceInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueInfo,
        .enabledExtensionCount = 1,
        .ppEnabledExtensionNames = deviceExtensions
    };

    // Create logical device
    result = vkCreateDevice(
        gApplication.physical,
        &deviceInfo,
        NULL,
        &gApplication.logical
    );

    ag_log("Logical device created");

    if(result != VK_SUCCESS) {
        printf("Something went wrong...\n");
        return NULL;
    }

    // Finaly get queue with graphics commands avaiable
    vkGetDeviceQueue(
        gApplication.logical,
        gApplication.graphicsFamilyIndex,
        0,
        &gApplication.graphicsQueue
    );

    ag_log("Graphics queue acquired");

    ag_log("Vulkan initialized successfully");

    return &gApplication;
}