#ifndef MANAGER_H
#define MANAGER_H

#include <d3d9.h>
#include <d3dx9.h>

#include <dinput.h>
#include <thread>
#include <vector>
#include <Windows.h>
#include "resources.h"
#include "processing.h"
#include "input.h"
#include <ctime>

#define KEYDOWN(name, key)(name[key]&0x80)
enum View { VIEW_2D, VIEW_3D};
enum flagUOT { UOT_MILI, UOT_NANO, UOT_MICRO};

struct WNDCLASSESEX_EX : WNDCLASSEX
{
	POINT pos;
	int height, width;
	bool cursorState;
	DWORD dwStile;
	WNDCLASSESEX_EX()
	{
		pos.x = 10;
		pos.y = 10;
		cbClsExtra = 0;
		cbSize = sizeof(WNDCLASSEX);
		cbWndExtra = 0;
		hbrBackground = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
		hCursor = LoadCursor(NULL, IDC_ARROW);
		hIcon = NULL;
		hIconSm = NULL;
		lpszClassName = "None";
		lpszMenuName = "None";
		//style = CS_NOCLOSE;
		dwStile = WS_VISIBLE;
		height = 600;
		width = 800;
		cursorState = TRUE;
	}
	~WNDCLASSESEX_EX()
	{

	}
};
struct CAMERA
{
	D3DXMATRIX pMatrix, vMatrix, wMatrix;
	float posX, posY, posZ;
	float lookX, lookY, lookZ;
	float radX, radY;
	float angleX, angleY;
	float sensetive, speed;
	POINT cPosNew, cPosOld;
	DWORD timeNew, timeOld;
	CAMERA()
	{
		D3DXMatrixIdentity(&pMatrix);
		D3DXMatrixIdentity(&vMatrix);
		D3DXMatrixIdentity(&wMatrix);

		posX = 0, posY = 0, posZ = 0;
		lookX = 0, lookY = 0, lookZ = 0;
		radX = 0, radY = 0;
		angleX = 0, angleY = 0;

		cPosNew.x = 0, cPosOld.x = 0;
		cPosNew.y = 0, cPosOld.y = 0;
		timeNew = 0, timeOld = 0;

		sensetive = 0.3f;
		speed = 0.3f;
	}
	~CAMERA()
	{

	}
};
struct sTimer
{
	int id;
	bool multipleLoop;
	bool changeLoop;
	bool constTime;
	std::chrono::time_point<std::chrono::steady_clock> timePoint;

	sTimer()
	{
		multipleLoop = false;
		changeLoop = false;
		constTime = false;
		//deltatime = 0;
		id = -1;
	}
	~sTimer()
	{
	}
	double getDeltaTime(flagUOT flag)
	{
		if (id == -1) return 0;
		auto time = std::chrono::high_resolution_clock::now();
		switch (flag)
		{
		case UOT_MILI:
			return (double)std::chrono::duration_cast <std::chrono::milliseconds>(time - timePoint).count();
			break;
		case UOT_NANO:
			return (double)std::chrono::duration_cast <std::chrono::nanoseconds>(time - timePoint).count();
			break;
		case UOT_MICRO:
			return (double)std::chrono::duration_cast <std::chrono::microseconds>(time - timePoint).count();
			break;
		}
		return 0;
	}
};

LRESULT CALLBACK handler(HWND hw, UINT msg, WPARAM wp, LPARAM lp);

class cManager
{
private:
	bool flagExit;
	HWND hWindow;
	HINSTANCE hInstance;
	MSG msg;
	LPDIRECT3DDEVICE9 pDevice;
	LPDIRECT3DSTATEBLOCK9 stBlock;

	LPDIRECTINPUT8 pInput;
	LPDIRECTINPUTDEVICE8 pKeyboard;
	LPDIRECTINPUTDEVICE8 pMouse;

	LPD3DXFONT pFont;

	WNDCLASSESEX_EX wParameters;
	D3DPRESENT_PARAMETERS gParameters;
	CAMERA camSettings;
	std::vector<sTimer> timers;

	PDIRECT3DVERTEXDECLARATION9 Declaration;

	int(*renderFunction)();
	void defaultParameters();
	void setCamera();

public:
	DIMOUSESTATE mouse_data;
	char keyboard_data[256];
	bool handl; // Для эвентов
	cManager();
	~cManager();

	////
	LPDIRECT3DSURFACE9 oldbuff;

	LPDIRECT3DTEXTURE9 dTexture;
	LPDIRECT3DTEXTURE9 cTexture;
	LPDIRECT3DTEXTURE9 nTexture;
	LPDIRECT3DTEXTURE9 pTexture;
	LPDIRECT3DTEXTURE9 tmpData;

	std::vector<sDFElement> drArray;

	object* rDisplay;
	///


	HRESULT createWindow(HINSTANCE hInstance, WNDCLASSESEX_EX *windowParameters, D3DPRESENT_PARAMETERS *graphicParameters);
	HRESULT editWindow(WNDCLASSESEX_EX *windowParameters = NULL, D3DPRESENT_PARAMETERS *graphicParameters = NULL);
	HRESULT destroyWindow();
	HRESULT editFont(int size, char * name);

	HWND getHWnd();
	void setHWnd(HWND hw);
	LPD3DXFONT getFont();
	LPDIRECT3DDEVICE9 getDevice();
	WNDCLASSESEX_EX getWParameters();
	D3DPRESENT_PARAMETERS getGParameters();
	MSG*	getMessage() { return &msg; }
	LPDIRECT3DSTATEBLOCK9	getStates() { return stBlock; }
	void					setStates(LPDIRECT3DSTATEBLOCK9 block) { stBlock = block; }

	CAMERA* getCamSetting();

	HRESULT setRenderfunc(int(*renderFunction)());
	void* renderSceneToTxr(std::string name, int(*renderFunction)(), RECT *rc = NULL);

	void setTimer(int id, bool ct = false, bool ml = false);
	sTimer getTimer(int id);

	void	close();
	void	exitProgram();
	int		startHandler();
	HRESULT	renderScene();
	void	updateCam(View flag);
};

extern cManager manager;

#endif // !MANAGER_H
