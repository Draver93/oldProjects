#include "Init_IDevice.h"

devices::devices(std::vector<hWndInfo>* wInfo)
{
	pWI = wInfo;
}

devices::~devices()
{
}

HRESULT devices::сreateDevices(LPCSTR name)
{
	hWndInfo *pInfo = NULL;
	bool entrance = FALSE;
	for (unsigned int i = 0; i < pWI->size(); i++)
	{
		if (pWI->at(i).settingsWnd.lpszClassName == name) { pInfo = &pWI->at(i); entrance = TRUE; }
	}
	if (!entrance) { errorMsg(1, "Не удается создать Device. Device с данным именем класса не найдено!"); return E_FAIL;}
	if (pInfo->bDevices == TRUE) { errorMsg(1, "Не удается создать Device. Device с данным именем уже существует!"); return E_FAIL; }
	if (FAILED(DirectInput8Create(pInfo->settingsWnd.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pInput, NULL))) return E_FAIL;

	pInput->CreateDevice(GUID_SysKeyboard, &pInfo->pKeyboard, NULL);
	pInfo->pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	pInfo->pKeyboard->SetCooperativeLevel(pInfo->hWnd, DISCL_FOREGROUND);
	pInfo->pKeyboard->Acquire();

	pInput->CreateDevice(GUID_SysMouse, &pInfo->pMouse, NULL);
	pInfo->pMouse->SetDataFormat(&c_dfDIMouse);
	pInfo->pMouse->SetCooperativeLevel(pInfo->hWnd, DISCL_FOREGROUND);
	pInfo->pMouse->Acquire();

	pInfo->bDevices = TRUE;

	return S_OK;
}
HRESULT devices::recreateDevices(LPCSTR name)
{
	hWndInfo *pInfo = NULL;
	bool entrance = FALSE;
	for (unsigned int i = 0; i < pWI->size(); i++)
	{
		if (pWI->at(i).settingsWnd.lpszClassName == name) { pInfo = &pWI->at(i); entrance = TRUE; }
	}
	if (!entrance) { errorMsg(1, "Не удается обновить Device. Окно с данным именем класса не найдено!"); return E_FAIL; }
	if (pInfo->bDevices != TRUE) { errorMsg(1, "Не удается обновить Device. У окна нет обработчика Devices!"); return E_FAIL; }

	if (pInfo->pKeyboard != NULL)pInfo->pKeyboard->Release();
	if (pInfo->pMouse != NULL)pInfo->pKeyboard->Release();
	сreateDevices(pInfo->settingsWnd.lpszClassName);
	return S_OK;
}
HRESULT devices::deleteDevices(LPCSTR name)
{
	hWndInfo *pInfo = NULL;
	bool entrance = FALSE;
	for (unsigned int i = 0; i < pWI->size(); i++)
	{
		if (pWI->at(i).settingsWnd.lpszClassName == name) { pInfo = &pWI->at(i); entrance = TRUE; }
	}
	if (!entrance) { errorMsg(1, "Не удается удалить Device. Окно с данным именем класса не найдено!"); return E_FAIL; }
	if (pInfo->bDevices != TRUE) { errorMsg(1, "Не удается удалить Device. У окна нет обработчика Devices!"); return E_FAIL; }

	if (pInfo->pKeyboard != NULL)pInfo->pKeyboard->Release();
	if (pInfo->pMouse != NULL)pInfo->pMouse->Release();
	pInfo->bDevices = FALSE;
	return E_NOTIMPL;
}
