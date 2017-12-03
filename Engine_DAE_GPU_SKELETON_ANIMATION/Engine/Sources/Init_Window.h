#ifndef INIT_WINDOW_H
#define INIT_WINDOW_H

#include <windows.h>
#include <vector>
#include <d3d9.h>
#include <d3dx9mesh.h>
#include <d3dx9core.h>
#include <dinput.h>

#include <thread>

#include "pErrors.h"

struct WNDCLASSESEX_EX : WNDCLASSEX 
{
	int height, width;
	bool cursorState;
	WNDCLASSESEX_EX()
	{
		//hInstance = e_hInst;
		cbClsExtra = 0;
		cbSize = sizeof(WNDCLASSEX);
		cbWndExtra = 0;
		hbrBackground = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
		hCursor = NULL;
		hIcon = NULL;
		hIconSm = NULL;
		lpszClassName = "None";
		lpszMenuName = "None";
		style = CS_NOCLOSE;

		height = 800;
		width = 600;
		cursorState = TRUE;
	}
	~WNDCLASSESEX_EX()
	{

	}
};

enum View { VIEW_2D, VIEW_3D, DEFAULT_VIEW };
enum MethodRenders { RENDER_REF, RENDER_HAL, DEFAULT_RENDER };
enum AxisHeight { NORMAL_X, NORMAL_Y, NORMAL_Z, DEFAULT_AXIS };
enum CamControl { CC_MANUAL, CC_STATIC, CC_PATH, CC_DEFAULT };

struct settingCam
{
	float posX, posY, posZ;
	float lookX, lookY, lookZ;
	float radX, radY;
	float angleX, angleY;
	float sensetive ,speed;
	D3DXMATRIX wMatrix, pMatrix, vMatrix;


	POINT cPosNew, cPosOld;
	DWORD timeNew, timeOld;

	View			view;
	MethodRenders	methodRenders;
	AxisHeight		axisHeight;
	CamControl		control;
	settingCam()
	{
		posX	= 0, posY	= 0, posZ	= 0;
		lookX	= 0, lookY	= 0, lookZ	= 0;
		radX	= 0, radY	= 0;
		angleX	= 0, angleY = 0;

		cPosNew.x	= 0, cPosOld.x	= 0;
		cPosNew.y	= 0, cPosOld.y	= 0;
		timeNew		= 0, timeOld	= 0;
		sensetive	= 0.3f;
		speed = 0.01f;
		view = DEFAULT_VIEW;
		methodRenders = DEFAULT_RENDER;
		axisHeight = DEFAULT_AXIS;
		control = CC_DEFAULT;
	}
	~settingCam()
	{
	}
};

struct hWndInfo
{
	HWND hWnd;
	WNDCLASSESEX_EX settingsWnd;
	bool bWindows;

	LPDIRECT3DDEVICE9 pDevice;
	D3DPRESENT_PARAMETERS d3dParam;
	settingCam sCamera;
	LPD3DXFONT pFont;
	bool bGraphics;

	LPDIRECTINPUTDEVICE8 pKeyboard;
	LPDIRECTINPUTDEVICE8 pMouse;
	bool bDevices;

	std::vector<std::thread*> *threads;

	hWndInfo()
	{
		hWnd = NULL;
		bWindows = false;

		pDevice = NULL;
		ZeroMemory(&d3dParam, sizeof(d3dParam));
		pFont = NULL;
		bGraphics = false;

		pKeyboard = NULL;
		pMouse = NULL;
		bDevices = false;
		threads = new std::vector<std::thread*>();
	}
	~hWndInfo()
	{
	}
};

class wnd
{
private:
	HINSTANCE hInst;
	std::vector<hWndInfo> pWnds;
	hWndInfo tmp_settingsWnd;
public:
	wnd(HINSTANCE hInst);
	~wnd();
	HRESULT createWnd(LPSTR className);
	HRESULT recreateWnd(LPSTR className);
	HRESULT deleteWnd(LPSTR className);
	hWndInfo*  getSettingsWnd(LPSTR className);
	std::vector<hWndInfo>* getAllSetWnd();
};


LRESULT CALLBACK winProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp);

#endif // !INIT_WINDOW_H
