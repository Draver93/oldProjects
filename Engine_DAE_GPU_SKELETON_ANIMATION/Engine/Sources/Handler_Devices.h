#ifndef HANDLER_DEVICE_H 
#define HANDLER_DEVICE_H

#include <windows.h>
#include <vector>
#include "Init_Window.h"


enum { LEFT_BUTTON, RIGHT_BUTTON, MIDDLE_BUTTON };

#define KEYDOWN(name, key)(name[key]&0x80)

HRESULT startHandlerDevice(wnd *windows);

#endif