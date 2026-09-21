#ifndef ag_window_h
#define ag_window_h

#include <stdint.h>

typedef struct AgWindow {
    void* handle;
    void* appInstance;
    uint32_t width;
    uint32_t height;
    char* title;
    void* (*event_handler)(void* userdata);
} AgWindow;

AgWindow* ag_create_window(const char* title, uint32_t w, uint32_t h);

#endif // ag_window_h