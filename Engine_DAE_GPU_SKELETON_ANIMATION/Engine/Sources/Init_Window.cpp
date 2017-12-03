#include "Init_Window.h"


wnd::wnd(HINSTANCE e_hInst)
{
	hInst = e_hInst;
}

wnd::~wnd()
{
}

HRESULT wnd::createWnd(LPSTR className)
{
	bool addOrNot = true;
	for (unsigned int i = 0; i < pWnds.size(); i++)
	{
		if (pWnds.at(i).settingsWnd.lpszClassName == className)
		{
			if(pWnds.at(i).bWindows == true) { errorMsg(1, "Окно уже создано!"); return E_FAIL; }
			if (!RegisterClassEx(&(WNDCLASSEX)pWnds.at(i).settingsWnd)) { errorMsg(1, "Ошибка регистрации окна"); return E_FAIL; }

			pWnds.at(i).hWnd = NULL;
			if (!(pWnds.at(i).hWnd = CreateWindowEx(NULL, pWnds.at(i).settingsWnd.lpszClassName,
				pWnds.at(i).settingsWnd.lpszMenuName, WS_VISIBLE, 10, 10,
				pWnds.at(i).settingsWnd.height, pWnds.at(i).settingsWnd.width, NULL, NULL, hInst, NULL))) {errorMsg(1, "Ошибка создания окна"); return E_FAIL; }


			ShowWindow(pWnds.at(i).hWnd, WS_VISIBLE);
			ShowCursor(pWnds.at(i).settingsWnd.cursorState);
			UpdateWindow(pWnds.at(i).hWnd);
			addOrNot = false;
			pWnds.at(i).bWindows = true;
			break; 
		}
	}
	if (addOrNot)
	{
		tmp_settingsWnd.settingsWnd.lpfnWndProc = winProc;
		tmp_settingsWnd.settingsWnd.lpszClassName = className;
		tmp_settingsWnd.settingsWnd.hInstance = hInst;

		if (!RegisterClassEx(&(WNDCLASSEX)tmp_settingsWnd.settingsWnd)) { errorMsg(1, "Ошибка регистрации окна"); return E_FAIL; }

		tmp_settingsWnd.hWnd = NULL;
		if (!(tmp_settingsWnd.hWnd = CreateWindowEx(NULL, tmp_settingsWnd.settingsWnd.lpszClassName,
			tmp_settingsWnd.settingsWnd.lpszMenuName, WS_VISIBLE, 10, 10,
			tmp_settingsWnd.settingsWnd.height, tmp_settingsWnd.settingsWnd.width, NULL, NULL, hInst, NULL))) { errorMsg(1, "Ошибка создания окна"); return E_FAIL; }


		ShowWindow(tmp_settingsWnd.hWnd, WS_VISIBLE);
		ShowCursor(tmp_settingsWnd.settingsWnd.cursorState);
		UpdateWindow(tmp_settingsWnd.hWnd);

		tmp_settingsWnd.bWindows = true;
		pWnds.push_back(tmp_settingsWnd);
		
	}

	return S_OK;
}
HRESULT wnd::recreateWnd(LPSTR className)
{
	bool entrance = false;
	hWndInfo tmpWinfo;	
	for (unsigned int i = 0; i < pWnds.size(); i++)
	{
		if (pWnds.at(i).settingsWnd.lpszClassName == className)
		{
			tmpWinfo = pWnds.at(i);
			entrance = true;
		}
	}
	deleteWnd(className);

	tmpWinfo.bWindows = false;

	if (!RegisterClassEx(&(WNDCLASSEX)tmpWinfo.settingsWnd)) { errorMsg(1, "Ошибка регистрации окна"); return E_FAIL; }

	if (!(tmpWinfo.hWnd = CreateWindowEx(NULL, tmpWinfo.settingsWnd.lpszClassName,
		tmpWinfo.settingsWnd.lpszMenuName, WS_VISIBLE, 10, 10,
		tmpWinfo.settingsWnd.height, tmpWinfo.settingsWnd.width, NULL, NULL, hInst, NULL))) { errorMsg(1, "Ошибка создания окна"); return E_FAIL;}

	ShowWindow(tmpWinfo.hWnd, WS_VISIBLE);
	ShowCursor(tmpWinfo.settingsWnd.cursorState);
	UpdateWindow(tmpWinfo.hWnd);
	tmpWinfo.bWindows = true;
	tmpWinfo.bGraphics = false;
	tmpWinfo.pDevice = NULL;
	ZeroMemory(&tmpWinfo.d3dParam, sizeof(tmpWinfo.d3dParam));

	pWnds.push_back(tmpWinfo);
	if (!entrance) errorMsg(1, "Введено неправильное имя класса для update или окно не существует");
	return S_OK;
}
HRESULT wnd::deleteWnd(LPSTR className)
{
	bool entrance = false;
	for (unsigned int i = 0; i < pWnds.size(); i++)
	{
		if (pWnds.at(i).settingsWnd.lpszClassName == className)
		{
			DestroyWindow(pWnds.at(i).hWnd);
			if (!UnregisterClass(className, pWnds.at(i).settingsWnd.hInstance)){errorMsg(1, "Не удалось UnregisterClass"); return E_FAIL;}
			pWnds.erase(pWnds.begin() + i);
			entrance = true;
		}
	}
	if (!entrance) errorMsg(1, "Введено неправильное имя класса для удаления");
	return S_OK;
}
hWndInfo* wnd::getSettingsWnd(LPSTR className)
{
	for (unsigned int i = 0; i < pWnds.size(); i++)
	{
		if (pWnds.at(i).settingsWnd.lpszClassName == className)
		{
			return &pWnds.at(i);
		}
	}
	return NULL;
}
std::vector<hWndInfo>* wnd::getAllSetWnd()
{
	return &pWnds;
}
LRESULT CALLBACK winProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
		case WM_DESTROY: 
		{
		} 
		break;
		default:
		{
		}
		break;
	}
	return DefWindowProc(hw, msg, wp, lp);
}


