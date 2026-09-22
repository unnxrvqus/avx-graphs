cmake_minimum_required(VERSION 3.20)

project(AvxGraph C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)

add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)

add_executable(AvxGraphs
    src/vulkan/ag_instance.c
    src/vulkan/ag_swapchain.c
    src/vulkan/ag_cmd_pool.c
    src/vulkan/ag_cmd_buffer.c
    src/vulkan/ag_frame.c
    src/vulkan/ag_frame_sync.c
    src/ag_log.c
    src/ag_main.c
    src/platform/windows/ag_surface.c
    src/platform/windows/ag_window.c
    src/platform/windows/ag_poll_events.c
)

target_include_directories(AvxGraphs PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(AvxGraphs
    Vulkan::Vulkan
)