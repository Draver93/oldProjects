#include "direct_control.h"

direct_control::direct_control()
{
}
direct_control::~direct_control()
{
}

float contrl_x = 0, contrl_y = 0, contrl_z = 0;

bool direct_control::init_device(HINSTANCE h_inst)
{
	if (FAILED(DirectInput8Create(h_inst, 
								DIRECTINPUT_VERSION, 
								IID_IDirectInput8, 
								(void**)&p_input, 
								NULL)))	return FALSE;
	return TRUE;
}
bool direct_control::create_keyboard(HWND h_window)
{
	p_input->CreateDevice(GUID_SysKeyboard, &p_keyboard, NULL);
	p_keyboard->SetDataFormat(&c_dfDIKeyboard);
	p_keyboard->SetCooperativeLevel(h_window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	p_keyboard->Acquire();

	return TRUE;
}
void direct_control::keyboard_handler()
{
	p_keyboard->GetDeviceState(sizeof(keyboard_state), (LPVOID)&keyboard_state);
	if (KEYDOWN(keyboard_state, DIK_ESCAPE))	PostQuitMessage(0);
}
bool direct_control::create_mouse(HWND h_window)
{
	p_input->CreateDevice(GUID_SysMouse, &p_mouse, NULL);
	p_mouse->SetDataFormat(&c_dfDIMouse);
	p_mouse->SetCooperativeLevel(h_window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	p_mouse->Acquire();

	return TRUE;
}
void direct_control::mouse_handler()
{
	p_mouse->GetDeviceState(sizeof(mouse_state), (LPVOID)&mouse_state);
}
char* direct_control::get_keyboard_state()
{
	return keyboard_state;
}
DIMOUSESTATE direct_control::get_mouse_state()
{
	return mouse_state;
}