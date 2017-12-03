#pragma once
#include "header.h"


class direct_api
{
private:
	LPDIRECT3D9 p_direct3d;
	LPDIRECT3DDEVICE9 p_direct3d_device;
	D3DDISPLAYMODE display;
	D3DPRESENT_PARAMETERS direct3d_parametr;
public:
	direct_api();
	~direct_api();
	HRESULT create_direct(HWND h_window, bool fullscrean);
	void render_scene();
	IDirect3DDevice9* get_device();
};

