#ifndef SCENE_H
#define SCENE_H

#include "Init_Window.h"
#include "Init_Graphics.h"
#include "Init_IDevice.h"
#include "Graphics.h"
#include "Resources.h"
#include "Handler_Proc.h"
#include <thread>

void initComponents(wnd *window);

void testThread(resources *Data, hWndInfo *setWnd);

#endif