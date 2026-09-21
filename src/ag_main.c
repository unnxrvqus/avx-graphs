#include "vulkan/ag_instance.h"
#include "vulkan/ag_surface.h"
#include "vulkan/ag_swapchain.h"

#include <windows.h>

int main(void) {
    AgApplication *app = ag_create_app_instance();
    
    ag_create_surface();

    ag_create_swapchain();

    return 0;
}