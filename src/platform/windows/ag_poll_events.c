#include <windows.h>

#include "platform/ag_poll_events.h"

void agPollEvents() {
    MSG msg;

    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}