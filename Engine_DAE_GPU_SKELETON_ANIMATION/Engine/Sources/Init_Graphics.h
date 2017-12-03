#ifndef INIT_GRAPHICS_H
#define INIT_GRAPHICS_H

#include <d3d9.h>
#include <d3dx9mesh.h>
#include <d3dx9core.h>

#include "Init_Window.h"
#include "Graphics.h"
#include "pErrors.h"
#include "Resources.h"

#pragma comment (lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

class graphics
{
private:
	LPDIRECT3D9 pDirect;
	D3DDISPLAYMODE pDispaly;
	std::vector<hWndInfo>* tmpWndInfo;
public:
	graphics(std::vector<hWndInfo>* WndInfo);
	~graphics();
	HRESULT createGraphicWnd(LPSTR ClassName);
	HRESULT recreateGraphicWnd(LPSTR ClassName);
	HRESULT deleteGraphicWnd(LPSTR ClassName);
};

void renderScene(wnd *windows, resources *Data);

#endif