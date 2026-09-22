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

AgApplication _Application;

uint32_t agFindQueueFamily(
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

AgApplication* agGetAppInstance() {
    return &_Application;
}

AgApplication* agCreateApp() {
    _Application.surface = malloc(sizeof(AgSurface*));

    agLog("Initializing Vulkan...");

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
        &_Application.instance
    );

    if(result != VK_SUCCESS) {
        printf("Something went wrong...\n");
        return NULL;
    }
    
    agLog("Vulkan instance created");

    // Get count of avaiable GPUs
    uint32_t deviceCount = 0;
    result = vkEnumeratePhysicalDevices(
        _Application.instance,
        &deviceCount,
        NULL
    );

    if(result != VK_SUCCESS || deviceCount == 0) {
        printf("Something went wrong...\n");\
        return NULL; //TODO: free memory for g_instance
    }

    agLog("Avaiable GPUs: %d", deviceCount);

    VkPhysicalDevice devices[deviceCount];
    
    // Get array of avaiable GPUs
    result = vkEnumeratePhysicalDevices(
        _Application.instance,
        &deviceCount,
        devices
    );
    
    _Application.physical_device = devices[0];

    VkPhysicalDeviceProperties properties;
    // Get GPU properties
    vkGetPhysicalDeviceProperties(
        _Application.physical_device,
        &properties
    );

    agLog("Selected GPU: %s", properties.deviceName);

    // Get amount of avaiable queue families on that GPU
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
        _Application.physical_device,
        &queueFamilyCount,
        NULL
    );
    
    // Fill array of queue family properties 
    VkQueueFamilyProperties *queueFamilies = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount); 
    vkGetPhysicalDeviceQueueFamilyProperties(
        _Application.physical_device,
        &queueFamilyCount,
        queueFamilies
    );

    // Find family with graphics commands avaiable
    _Application.graphicsFamilyIndex = AG_QUEUE_FAMILY_INVALID;
    _Application.graphicsFamilyIndex = agFindQueueFamily(queueFamilies, queueFamilyCount, VK_QUEUE_GRAPHICS_BIT);

    if(_Application.graphicsFamilyIndex == AG_QUEUE_FAMILY_INVALID) {
        printf("Something went wrong...\n");
        return NULL;
    }

    agLog("Graphics queue family: %d", _Application.graphicsFamilyIndex);

    // Describe queue creation info
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = _Application.graphicsFamilyIndex,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };

    const char* deviceExtensions[] = { "VK_KHR_swapchain" };

    VkPhysicalDeviceSynchronization2Features synchronization2Features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
        .synchronization2 = VK_TRUE
    };

    // Describe logical device creation info
    VkDeviceCreateInfo deviceInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueInfo,
        .enabledExtensionCount = 1,
        .ppEnabledExtensionNames = deviceExtensions,
        .pNext = &synchronization2Features,
    };

    // Create logical device
    result = vkCreateDevice(
        _Application.physical_device,
        &deviceInfo,
        NULL,
        &_Application.logical_device
    );

    agLog("Logical device created");

    if(result != VK_SUCCESS) {
        printf("Something went wrong...\n");
        return NULL;
    }

    // Finaly get queue with graphics commands avaiable
    vkGetDeviceQueue(
        _Application.logical_device,
        _Application.graphicsFamilyIndex,
        0,
        &_Application.graphicsQueue
    );

    agLog("Graphics queue acquired");

    agLog("Vulkan initialized successfully");

    _Application.running = VK_TRUE;

    return &_Application;
}