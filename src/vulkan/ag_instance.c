#include <vulkan/vulkan.h>
#include <stdio.h>

#include "vulkan/ag_app_info.h"

typedef enum agPlatformType {
    AG_WINDOWS,
    AG_LINUX
} agPlatformType;

VkInstance g_instance;

void ag_create_instance(agPlatformType platformType) {
    
    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info
    };

    VkResult result = vkCreateInstance(
        &create_info,
        NULL,
        &g_instance
    );

    if(result != VK_SUCCESS) {
        printf("Something went wrong...\n");
    }
    
}

int main(void) {
    ag_create_instance(AG_WINDOWS);

    printf("Everything is fine!\n");
    
    return 0;
}