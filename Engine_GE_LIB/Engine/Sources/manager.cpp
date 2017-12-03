#include "manager.h"



cManager manager;

cManager::cManager()
{
	handl = false;
	hWindow = NULL;
	hInstance = NULL;
	defaultParameters();
	setTimer(1050, false, true);

	if (pDevice != NULL)
		pDevice->Release();
	flagExit = false;
}
cManager::~cManager()
{
}

void cManager::defaultParameters()
{
	ZeroMemory(&gParameters, sizeof(gParameters));
	gParameters.Windowed = TRUE;
	gParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	gParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	gParameters.AutoDepthStencilFormat = D3DFMT_D16;
	gParameters.EnableAutoDepthStencil = TRUE;
	gParameters.BackBufferCount = 0;
	gParameters.FullScreen_RefreshRateInHz = 0;
	gParameters.BackBufferHeight = 0;
	gParameters.BackBufferWidth = 0;

}
void cManager::setCamera()
{

	D3DXMatrixLookAtLH(&camSettings.vMatrix, &D3DXVECTOR3(1.1f, 0, 0),
			&D3DXVECTOR3(0, 0, 0),
			&D3DXVECTOR3(0, 1, 0));

}

HRESULT cManager::createWindow(HINSTANCE hInstanceNew, WNDCLASSESEX_EX *windowParameters, D3DPRESENT_PARAMETERS *gp)
{
	if (windowParameters != NULL)
		wParameters = *windowParameters;
	if (gp != NULL)
		gParameters = *gp;
	if (hWindow != NULL) return E_FAIL;

	wParameters.lpfnWndProc = handler;
	wParameters.hInstance = hInstanceNew;
	hInstance = hInstanceNew;
	
	if (!RegisterClassEx(&(WNDCLASSEX)wParameters)) return E_FAIL;
	if (!(hWindow = CreateWindowEx(NULL, wParameters.lpszClassName, wParameters.lpszMenuName, wParameters.dwStile, wParameters.pos.x, wParameters.pos.y,
		wParameters.width, wParameters.height,
		NULL, NULL, hInstance, NULL))) return E_FAIL;
	ShowCursor(wParameters.cursorState);
	UpdateWindow(hWindow);

	////

	D3DDISPLAYMODE display;
	LPDIRECT3D9 device;
	D3DCAPS9 pCaps;
	if (!(device = Direct3DCreate9(D3D_SDK_VERSION)))  return E_FAIL;
	if (device->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &display)) return E_FAIL;
	gParameters.BackBufferFormat = display.Format;
	if (device->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWindow,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
		&gParameters, &pDevice)) return E_FAIL;
	
	pDevice->GetDeviceCaps(&pCaps);
	pCaps;

	//// Установка шаблона вершинного буфера
	D3DVERTEXELEMENT9 pDecl[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
		{ 0, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },

		D3DDECL_END()
	};
	pDevice->CreateVertexDeclaration(pDecl, &Declaration);
	pDevice->SetVertexDeclaration(Declaration);
	////

	D3DXCreateTexture(pDevice, manager.getGParameters().BackBufferWidth , manager.getGParameters().BackBufferHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R16F, D3DPOOL_DEFAULT, &manager.dTexture);
	D3DXCreateTexture(pDevice, manager.getGParameters().BackBufferWidth, manager.getGParameters().BackBufferHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &manager.cTexture);
	D3DXCreateTexture(pDevice, manager.getGParameters().BackBufferWidth, manager.getGParameters().BackBufferHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X1R5G5B5, D3DPOOL_DEFAULT, &manager.nTexture);
	D3DXCreateTexture(pDevice, manager.getGParameters().BackBufferWidth, manager.getGParameters().BackBufferHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X1R5G5B5, D3DPOOL_DEFAULT, &manager.pTexture);
	D3DXCreateTexture(pDevice, manager.getGParameters().BackBufferWidth, manager.getGParameters().BackBufferHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &manager.tmpData);
	pDevice->GetRenderTarget(0, &manager.oldbuff);

	rDisplay = new object();
	D3DXVECTOR2 vec = { (float)gParameters.BackBufferHeight, (float)gParameters.BackBufferWidth };
	rDisplay->mesh = createRndPanel(vec);
	rDisplay->active = true;
	rDisplay->nameModel = "DISPLAY";
	rDisplay->SettingsTransform.state = true;
	rDisplay->SettingsTransform.angleZ = 90;
	rDisplay->SettingsTransform.size = 5;
	CAMERA *cam = manager.getCamSetting();
	attr at;
	at.name = "MatrixWorld";
	at.setPtr(&cam->wMatrix);
	rDisplay->plData.cAttr.push_back(at);
	at.name = "MatrixProjection";
	at.setPtr(&cam->pMatrix);
	rDisplay->plData.cAttr.push_back(at);
	at.name = "MatrixView";
	at.setPtr(&cam->vMatrix);
	rDisplay->plData.cAttr.push_back(at);
	at.name = "InterStage";
	at.setPtr(&manager.cTexture);
	rDisplay->plData.cAttr.push_back(at);
	at.name = "txrData";
	at.setPtr(&manager.tmpData);
	rDisplay->plData.cAttr.push_back(at);
	at.name = "resolution";
	std::vector<float> screenSize = { (float)manager.getGParameters().BackBufferWidth , (float)manager.getGParameters().BackBufferHeight };
	at.setMas(&screenSize);
	rDisplay->plData.cAttr.push_back(at);
	sEffect *effect = in::loadEffect("Effects/Display.fx");
	resources.setObject(effect, OT_EFFECT);
	rDisplay->plData.pEffects = effect;
	////

	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pInput, NULL))) return E_FAIL;

	pInput->CreateDevice(GUID_SysKeyboard, &pKeyboard, NULL);
	pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	pKeyboard->SetCooperativeLevel(hWindow, DISCL_FOREGROUND);
	pKeyboard->Acquire();

	pInput->CreateDevice(GUID_SysMouse, &pMouse, NULL);
	pMouse->SetDataFormat(&c_dfDIMouse);
	pMouse->SetCooperativeLevel(hWindow, DISCL_FOREGROUND);
	pMouse->Acquire();

	////
	WINDOWINFO pwi;
	GetWindowInfo(manager.getHWnd(), &pwi);

	if (gParameters.Windowed)
	{
		int width = pwi.rcClient.right - pwi.rcClient.left;
		int height = pwi.rcClient.bottom - pwi.rcClient.top;
		D3DXMatrixPerspectiveFovLH(&camSettings.pMatrix, D3DX_PI / 4, (float)width / (float)height, 1.0f, 10000.0f);
	}
	else
	{
		//pp.BackBufferWidth = pwi.rcWindow.left;
		//pp.BackBufferHeight = pwi.rcWindow.top;
		D3DXMatrixPerspectiveFovLH(&camSettings.pMatrix, D3DX_PI / 4, (float)display.Width / (float)display.Height, 1.0f, 10000.0f);
	}
	setCamera();
	D3DXCreateFont(pDevice,
		20,
		0,
		FW_NORMAL,
		1,
		false,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		"Calibri",
		&pFont
	);
	return S_OK;
}
HRESULT cManager::destroyWindow()
{
	DestroyWindow(hWindow);
	if(!UnregisterClass(wParameters.lpszClassName, wParameters.hInstance)) return E_FAIL; 
	hWindow = NULL;

	if (pDevice != NULL)
	{
		pDevice->Release();
		pDevice = NULL;
	}
	if (pInput != NULL)
	{
		pInput->Release(); 
		pInput = NULL;
	}
	if (pKeyboard != NULL)
	{
		pKeyboard->Release();
		pKeyboard = NULL;
	}
	if (pMouse != NULL)
	{
		pMouse->Release();
		pMouse = NULL;
	}

	return S_OK;
}
HRESULT cManager::editWindow(WNDCLASSESEX_EX *wp, D3DPRESENT_PARAMETERS *gp)
{
	if (wp != NULL)
	{
		DestroyWindow(manager.getHWnd());

		if (!UnregisterClass(wp->lpszClassName, wp->hInstance))
		{
			return E_FAIL;
		}
		if (!RegisterClassEx((WNDCLASSEX*)wp))
		{
			return E_FAIL;
		}
		if (!(manager.hWindow = CreateWindowEx(NULL, wp->lpszClassName, wp->lpszMenuName, wp->dwStile, wp->pos.x, wp->pos.y,
			wp->width, wp->height,
			NULL, NULL, wp->hInstance, NULL)))
			return E_FAIL;

		ShowCursor(wp->cursorState);
		UpdateWindow(manager.hWindow);
		gp->hDeviceWindow = manager.hWindow;
	}
	gp->hDeviceWindow = manager.hWindow;

	HRESULT hr = NULL;
	LPD3DXFONT pFont = manager.getFont();
	hr = pFont->OnLostDevice();

	LPDIRECT3DDEVICE9 pDevice = manager.getDevice();
	D3DCAPS9 pCaps;
	pDevice->GetDeviceCaps(&pCaps);
	LPDIRECT3DSURFACE9 sf;
	for (UINT i = 0; i < pCaps.NumSimultaneousRTs; i++)
	{		
		pDevice->SetRenderTarget(i, NULL);
	}
	manager.oldbuff->Release();
	manager.pTexture->Release();
	manager.cTexture->Release();
	manager.nTexture->Release();
	manager.dTexture->Release();
	manager.tmpData->Release();
	std::vector<sEffect*> *effects = (std::vector<sEffect*>*)resources.getAll(OT_EFFECT);
	for (UINT i = 0; i < effects->size(); i++)
	{
		effects->at(i)->data->OnLostDevice();
	}
	if (gp == NULL)
		hr = pDevice->Reset(&gParameters);
	else
	{
		hr = pDevice->Reset(gp);
		if (hr != S_OK)
		{
			hr = pDevice->TestCooperativeLevel();
			if (D3DERR_DEVICELOST == hr)
			{
				manager.exitProgram();
			}
			if (D3DERR_DEVICENOTRESET == hr)
			{
				manager.exitProgram();
			}
			if (D3DERR_DRIVERINTERNALERROR == hr)
			{
				manager.exitProgram();
			}
		}
		gParameters = *gp;
	}
	pFont->OnResetDevice();
	for (UINT i = 0; i < effects->size(); i++)
	{
		effects->at(i)->data->OnLostDevice();
	}

	D3DXMatrixPerspectiveFovLH(&camSettings.pMatrix, D3DX_PI / 4, (float)gp->BackBufferWidth / (float)gp->BackBufferHeight, 1.0f, 10000.0f);
	pDevice->SetVertexDeclaration(Declaration);

	D3DXCreateTexture(pDevice, manager.getGParameters().BackBufferWidth, manager.getGParameters().BackBufferHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R16F, D3DPOOL_DEFAULT, &manager.dTexture);
	D3DXCreateTexture(pDevice, manager.getGParameters().BackBufferWidth, manager.getGParameters().BackBufferHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &manager.cTexture);
	D3DXCreateTexture(pDevice, manager.getGParameters().BackBufferWidth, manager.getGParameters().BackBufferHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X1R5G5B5, D3DPOOL_DEFAULT, &manager.nTexture);
	D3DXCreateTexture(pDevice, manager.getGParameters().BackBufferWidth, manager.getGParameters().BackBufferHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X1R5G5B5, D3DPOOL_DEFAULT, &manager.pTexture);
	D3DXCreateTexture(pDevice, manager.getGParameters().BackBufferWidth, manager.getGParameters().BackBufferHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &manager.tmpData);
	pDevice->GetRenderTarget(0, &manager.oldbuff);

	if (rDisplay->mesh != NULL)
		rDisplay->mesh->release();

	D3DXVECTOR2 vec = { (float)gParameters.BackBufferHeight, (float)gParameters.BackBufferWidth };
	rDisplay->mesh = createRndPanel(vec);

	return E_NOTIMPL;
}
void	cManager::close()
{
	flagExit = true;
}
void cManager::exitProgram()
{
	destroyWindow();
	PostQuitMessage(1);
}
HRESULT cManager::editFont(int size, char * name)
{
	D3DXCreateFont(pDevice,
		size,
		0,
		FW_NORMAL,
		1,
		false,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		name,
		&pFont
	);
	return S_OK;
}
LPD3DXFONT cManager::getFont()
{
	return pFont;
}

HWND cManager::getHWnd()
{
	return hWindow;
}
void cManager::setHWnd(HWND hw)
{
	hWindow = hw;
}
WNDCLASSESEX_EX cManager::getWParameters()
{
	return wParameters;
}
LPDIRECT3DDEVICE9 cManager::getDevice()
{
	return pDevice;
}
D3DPRESENT_PARAMETERS cManager::getGParameters()
{
	return gParameters;
}

HRESULT cManager::setRenderfunc(int(*newRenderFunc)())
{
	renderFunction = newRenderFunc;
	return E_NOTIMPL;
}

LRESULT CALLBACK handler(HWND hw, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_CLOSE:
		manager.close();
		break;
	default:
		return DefWindowProc(hw, msg, wp, lp);
	}
	return 0;
}
int		cManager::startHandler()
{
	while (msg.message != WM_QUIT)
	{
		ZeroMemory(&msg, sizeof(MSG));
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		manager.renderScene();

		if(pMouse != NULL)
			pMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse_data);
		if (pKeyboard != NULL)
			pKeyboard->GetDeviceState(sizeof(manager.keyboard_data), (LPVOID)&manager.keyboard_data);

		if (!timers.empty())
			for (UINT i = 0; i < timers.size(); i++)
				if(timers.at(i).changeLoop == false)
					if (timers.at(i).multipleLoop == false)
					{
						timers.erase(timers.begin() + i);
						i = -1;
					}


		if(flagExit)
			exitProgram();

	}
	return (msg.wParam);
}
HRESULT	cManager::renderScene()
{

	if (pDevice == nullptr)	return E_FAIL;
	if (getTimer(1050).getDeltaTime(UOT_MILI) > (1000.0f / 100.0f))	handl = true;
	manager.drArray.clear();
	

	LPDIRECT3DSURFACE9 sf;
	dTexture->GetSurfaceLevel(0, &sf);
	pDevice->SetRenderTarget(1,sf);
	sf->Release();
	cTexture->GetSurfaceLevel(0, &sf);
	pDevice->SetRenderTarget(2, sf);
	sf->Release();
	nTexture->GetSurfaceLevel(0, &sf);
	pDevice->SetRenderTarget(3, sf);
	sf->Release();
	dTexture->GetSurfaceLevel(0, &sf);
	pDevice->SetRenderTarget(4, sf);
	sf->Release();

	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	pDevice->SetRenderTarget(1, NULL);
	pDevice->SetRenderTarget(2, NULL);
	pDevice->SetRenderTarget(3, NULL);
	pDevice->SetRenderTarget(4, NULL);


	pDevice->BeginScene();

	if(renderFunction != NULL)
		renderFunction();

	pDevice->EndScene();
	pDevice->Present(NULL, NULL, NULL, NULL);

	if (getTimer(1050).getDeltaTime(UOT_MILI) > (1000.0f / 100.0f) && handl)
	{
		setTimer(1050, false, true);
		handl = false;
	}
	return S_OK;
}
void*	cManager::renderSceneToTxr(std::string name, int(*renderFunction)(), RECT *rc )
{
	texture *tmpTxr = new texture();
	tmpTxr->textureName = name;
	tmpTxr->data = NULL;
	

	if (pDevice == nullptr)
	{
		return NULL;
	}
	pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	LPDIRECT3DSURFACE9 pBackBufferOld = NULL;
	LPDIRECT3DSURFACE9 pBackBufferNew = NULL;
	LPDIRECT3DSURFACE9 dz = NULL, dzn = NULL;
	pDevice->GetRenderTarget(0, &pBackBufferOld);
	pDevice->GetDepthStencilSurface(&dz);


	//D3DSURFACE_DESC desc;
	//pBackBufferOld->GetDesc(&desc);
	if (rc == NULL)
	{	
		pDevice->CreateRenderTarget(manager.getGParameters().BackBufferWidth, manager.getGParameters().BackBufferHeight, D3DFMT_X8R8G8B8, 
																			manager.getGParameters().MultiSampleType, manager.getGParameters().MultiSampleQuality, false, &pBackBufferNew, NULL);
		D3DXCreateTexture(pDevice, manager.getGParameters().BackBufferWidth, manager.getGParameters().BackBufferHeight, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &tmpTxr->data);
	}
	else
	{
		pDevice->CreateDepthStencilSurface(rc->right - rc->left, rc->bottom - rc->top, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &dzn, NULL);
		pDevice->SetDepthStencilSurface(dzn);

		pDevice->CreateRenderTarget(rc->right - rc->left, rc->bottom - rc->top, D3DFMT_X8R8G8B8,
			manager.getGParameters().MultiSampleType, manager.getGParameters().MultiSampleQuality, false, &pBackBufferNew, NULL);
		D3DXCreateTexture(pDevice, rc->right - rc->left, rc->bottom - rc->top, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &tmpTxr->data);
	}
	pDevice->SetRenderTarget(0, pBackBufferNew);

	pDevice->BeginScene();
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
		
		if (renderFunction != NULL)
			renderFunction();
	pDevice->EndScene();
	LPDIRECT3DSURFACE9 pTexSurface = NULL;
	tmpTxr->data->GetSurfaceLevel(0, &pTexSurface);
	D3DXLoadSurfaceFromSurface(pTexSurface, NULL, NULL, pBackBufferNew, NULL, NULL, D3DX_DEFAULT, 0);
	if (pTexSurface != NULL) { pTexSurface->Release(); pTexSurface = NULL; }
	/*for (int i = 0; i < tmpTxr->data->GetLevelCount(); i++) //Не производительный метод
	{
		tmpTxr->data->GetSurfaceLevel(i, &pTexSurface);
		D3DXLoadSurfaceFromSurface(pTexSurface,NULL,NULL, pBackBufferNew,NULL,NULL,D3DX_DEFAULT, 0);

		//pDevice->StretchRect(pBackBufferNew, NULL, pTexSurface, NULL, D3DTEXF_ANISOTROPIC);
		if (pTexSurface != NULL) { pTexSurface->Release(); pTexSurface = NULL; }
	}*/
	if (pBackBufferNew != NULL) { pBackBufferNew->Release(); pBackBufferNew = NULL; }
	if (dzn != NULL) { dzn->Release(); dzn = NULL; }

	pDevice->SetDepthStencilSurface(dz);
	pDevice->SetRenderTarget(0, pBackBufferOld);

	if (pBackBufferOld != NULL) { pBackBufferOld->Release(); pBackBufferOld = NULL; }
	if (dz != NULL) { dz->Release(); dz = NULL; }

	return tmpTxr;
}

void cManager::setTimer(int id, bool ct, bool ml)
{
	if(!timers.empty())
		for (UINT i = 0; i < timers.size(); i++)
			if (timers.at(i).id == id)
			{
				if(!ct)
					timers.at(i).timePoint = std::chrono::high_resolution_clock::now();
				else
				{
					std::chrono::time_point<std::chrono::steady_clock> tmpPt;
					int tmpTm = std::chrono::duration_cast <std::chrono::microseconds>(tmpPt - timers.at(i).timePoint).count();
					if (tmpTm == 0)
						timers.at(i).timePoint = std::chrono::high_resolution_clock::now();
				}
				timers.at(i).changeLoop = true;
				return;
			}

	sTimer tmpTimer;
	tmpTimer.id = id;
	tmpTimer.constTime = ct;
	tmpTimer.multipleLoop = ml;
	tmpTimer.timePoint = std::chrono::high_resolution_clock::now();
	tmpTimer.changeLoop = true;
	timers.push_back(tmpTimer);
}
sTimer cManager::getTimer(int id)
{
	if (!timers.empty())
		for (UINT i = 0; i < timers.size(); i++)
			if (timers.at(i).id == id)
			{
				return timers.at(i);
			}

	return sTimer();
}
void cManager::updateCam(View flag)
{
	D3DXMatrixIdentity(&camSettings.vMatrix);
	if (flag == VIEW_3D)
	{
		if (gParameters.Windowed)
		{
			WINDOWINFO pwi;
			GetWindowInfo(manager.getHWnd(), &pwi);
			int width = pwi.rcClient.right - pwi.rcClient.left;
			int height = pwi.rcClient.bottom - pwi.rcClient.top;
			D3DXMatrixPerspectiveFovLH(&camSettings.pMatrix, D3DX_PI / 4, (float)width / (float)height, 1.0f, 10000.0f);
		}
		else
		{
			//pp.BackBufferWidth = pwi.rcWindow.left;
			//pp.BackBufferHeight = pwi.rcWindow.top;
			D3DXMatrixPerspectiveFovLH(&camSettings.pMatrix, D3DX_PI / 4, (float)getGParameters().BackBufferWidth / (float)getGParameters().BackBufferHeight, 1.0f, 10000.0f);
		}

		D3DXMatrixLookAtLH(&camSettings.vMatrix,	&D3DXVECTOR3(camSettings.posX, camSettings.posY, camSettings.posZ),
													&D3DXVECTOR3(camSettings.lookX, camSettings.lookY, camSettings.lookZ),
													&D3DXVECTOR3(0, 1, 0));
	}
	else
	{
		D3DXMatrixOrthoLH(&camSettings.pMatrix, (float)getGParameters().BackBufferWidth , (float)getGParameters().BackBufferHeight, 1.0f, 1000.0f);
		D3DXMatrixLookAtLH(&camSettings.vMatrix,	&D3DXVECTOR3(10.0f, 0.0f, 0.0f),
													&D3DXVECTOR3(0.0f, 0.0f, 0.0f),
													&D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	}
}
CAMERA* cManager::getCamSetting()
{
	return &camSettings;
}
