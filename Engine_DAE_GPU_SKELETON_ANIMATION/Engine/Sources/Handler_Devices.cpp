#include "Handler_Devices.h"

HRESULT startHandlerDevice(wnd *windows)
{
	std::vector<hWndInfo>* hWI = windows->getAllSetWnd();
	if (hWI->empty()) return E_FAIL;
	for (unsigned int i = 0; i < hWI->size(); i++)
	{
		if (hWI->at(i).bDevices == TRUE)
		{
			char keyboardState[256];
			DIMOUSESTATE mouseState;
			hWI->at(i).pKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);
			hWI->at(i).pMouse->GetDeviceState(sizeof(mouseState), (LPVOID)&mouseState);

			if (hWI->at(i).settingsWnd.lpszClassName == "Main" && KEYDOWN(keyboardState, DIK_ESCAPE))
			{
				windows->deleteWnd("Main");
				PostQuitMessage(1);
				if (hWI->empty()) return E_FAIL;
				break;
			}
			if (hWI->at(i).settingsWnd.lpszClassName == "First" && KEYDOWN(keyboardState, DIK_ESCAPE))
			{
				windows->deleteWnd("First");
				if (hWI->empty()) return E_FAIL;
				break;
			}

		}
	}
	return S_OK;
}
