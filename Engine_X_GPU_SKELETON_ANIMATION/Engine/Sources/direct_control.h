#pragma once
#include "header.h"

#define KEYDOWN(name, key)(name[key]&0x80)

#define LEFT_BUTTON		0
#define RIGHT_BUTTON	1
#define MIDDLE_BUTTON	2

class direct_control
{
private:
	LPDIRECTINPUT8 p_input;
	LPDIRECTINPUTDEVICE8 p_keyboard;
	LPDIRECTINPUTDEVICE8 p_mouse;

	char keyboard_state[256];
	DIMOUSESTATE mouse_state;
public:
	direct_control();
	~direct_control();
	bool init_device(HINSTANCE h_inst);
	bool create_keyboard(HWND h_window);
	void keyboard_handler();
	bool create_mouse(HWND h_window);
	void mouse_handler();
	float get_control(char xyz);
	char* get_keyboard_state();
	DIMOUSESTATE get_mouse_state();
};

