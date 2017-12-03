#ifndef INIT_IDEVICE_H
#define INIT_IDEVICE_H

#include <windows.h>
#include <vector>
#include <dinput.h>

#include "Init_Window.h"
#include "pErrors.h"

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")



class devices
{
private:
	LPDIRECTINPUT8 pInput;
	std::vector<hWndInfo> *pWI;
public:
	devices(std::vector<hWndInfo> *wInfo);
	~devices();
	HRESULT ñreateDevices(LPCSTR name);
	HRESULT recreateDevices(LPCSTR name);
	HRESULT deleteDevices(LPCSTR name);
};

#endif