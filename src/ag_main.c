#include "vulkan/ag_instance.h"
#include "vulkan/ag_surface.h"
#include "vulkan/ag_swapchain.h"

#include <windows.h>

int main(void) {
    ag_create_instance();

    ag_create_surface();

    ag_create_swapchain();
    return 0;
}