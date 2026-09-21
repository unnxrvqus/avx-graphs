#include <windows.h>
#include <stdio.h>

#include "platform/ag_window.h"

LRESULT CALLBACK ag_window_proc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
) {
    switch (message) {

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(
        hwnd,
        message,
        wParam,
        lParam
    );
}

AgWindow* ag_create_window(
    const char *title,
    uint32_t width,
    uint32_t height
) {
    const char *className = "AvxGraphsWindow";

    // Register window class
    WNDCLASS windowClass = {
        .lpfnWndProc = ag_window_proc,
        .hInstance = GetModuleHandle(NULL),
        .lpszClassName = className
    };

    if (!RegisterClass(&windowClass)) {
        printf("Failed to register window class: %lu\n", GetLastError());
        return NULL;
    }

    // Create window
    HWND hwnd = CreateWindowEx(
        0,
        className,
        title,
        WS_OVERLAPPEDWINDOW,

        CW_USEDEFAULT,
        CW_USEDEFAULT,

        width,
        height,

        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    if (hwnd == NULL) {
        printf("Failed to create window: %lu\n", GetLastError());
        return NULL;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    AgWindow *window = malloc(sizeof(AgWindow));

    window->handle = hwnd;
    window->height = height;
    window->width = width;
    window->appInstance = GetModuleHandle(NULL);

    window->title = malloc(sizeof(char) * (strlen(title) + 1));
    strncpy(window->title, title, strlen(title));
    window->title[strlen(title)] = '\0';

    return window;
}