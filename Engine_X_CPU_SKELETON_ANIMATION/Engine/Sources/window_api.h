#pragma once
#include "header.h"

class window_api
{
private:
	HWND h_window;
	MSG msg;
	WNDCLASSEX window_class;
public:
	window_api();
	~window_api();
	int setup_window();
	int handler_proc();
	HWND get_hwnd();
};

