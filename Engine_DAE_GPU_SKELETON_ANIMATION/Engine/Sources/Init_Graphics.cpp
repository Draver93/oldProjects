#include "Init_Graphics.h"

graphics::graphics(std::vector<hWndInfo>* WndInfo)
{
	tmpWndInfo = WndInfo;
	pDirect = NULL;
}
graphics::~graphics()
{
}

HRESULT graphics::createGraphicWnd(LPSTR ClassName)
{
	hWndInfo *pHWI = NULL;
	bool entrance = false;
	
	for (unsigned int i = 0; i < tmpWndInfo->size(); i++)
	{
		if (tmpWndInfo->at(i).settingsWnd.lpszClassName == ClassName) { pHWI = &tmpWndInfo->at(i); entrance = true; }
	}
	if (!entrance) {errorMsg(1, "Не найден класс, создание Graphic device невозможно."); return E_FAIL;}
	
	if (pHWI->bGraphics == FALSE)
	{
		
		pDirect = NULL;

		ZeroMemory(&pDispaly, sizeof(pDispaly));
		ZeroMemory(&pHWI->d3dParam, sizeof(pHWI->d3dParam));

		if (FAILED(pDirect = Direct3DCreate9(D3D_SDK_VERSION))) { errorMsg(1, "Ошибка pDirect = Direct3DCreate9(...)"); return E_FAIL; }
		if (FAILED(pDirect->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &pDispaly))) { errorMsg(1, "Ошибка GetAdapterDisplayMode(...)"); return E_FAIL; }

		pHWI->d3dParam.Windowed = TRUE;
		pHWI->d3dParam.SwapEffect = D3DSWAPEFFECT_DISCARD;
		pHWI->d3dParam.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		pHWI->d3dParam.AutoDepthStencilFormat = D3DFMT_D16;
		pHWI->d3dParam.EnableAutoDepthStencil = TRUE;
		pHWI->d3dParam.BackBufferFormat = pDispaly.Format;

		if (pHWI->d3dParam.Windowed == FALSE)
		{
			pHWI->d3dParam.BackBufferCount = 3;
			pHWI->d3dParam.FullScreen_RefreshRateInHz = pDispaly.RefreshRate;
			pHWI->d3dParam.BackBufferHeight = pDispaly.Height;
			pHWI->d3dParam.BackBufferWidth = pDispaly.Width;
		}
		pDirect->CreateDevice(	D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, pHWI->hWnd, 
								D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
								&pHWI->d3dParam, &pHWI->pDevice);


		pHWI->pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);		//Параметры глубины
		pHWI->pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);		//Отсечение не попадающих в кадр полигонов 
		pHWI->pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);			//Отключение обработки света 

		pHWI->pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		pHWI->pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pHWI->pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		pHWI->pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pHWI->pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pHWI->pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


		pHWI->bGraphics = TRUE;
	}
	else {errorMsg(1, "Графика уже создана, создание Graphic device невозможно."); return E_FAIL; }

	return S_OK;
}
HRESULT graphics::recreateGraphicWnd(LPSTR ClassName)
{
	bool entrance = false;
	for (unsigned int i = 0; i < tmpWndInfo->size(); i++)
	{
		if (tmpWndInfo->at(i).settingsWnd.lpszClassName == ClassName) 
		{ 
			if (tmpWndInfo->at(i).pDevice != NULL)
			{
				pDirect = NULL;
				tmpWndInfo->at(i).pDevice->Release();

				ZeroMemory(&pDispaly, sizeof(pDispaly));

				if (FAILED(pDirect = Direct3DCreate9(D3D_SDK_VERSION))) { errorMsg(1, "Ошибка pDirect = Direct3DCreate9(...)"); return E_FAIL; }
				if (FAILED(pDirect->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &pDispaly))) { errorMsg(1, "Ошибка GetAdapterDisplayMode(...)"); return E_FAIL; }

				pDirect->CreateDevice(	D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, tmpWndInfo->at(i).hWnd,
										D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
										&tmpWndInfo->at(i).d3dParam, &tmpWndInfo->at(i).pDevice);
				entrance = true;
			}
		}
	}
	if (!entrance) { errorMsg(1, "Ошибка обновления графики окна. Графика не создана"); return E_FAIL;}
	return S_OK;
}
HRESULT graphics::deleteGraphicWnd(LPSTR ClassName)
{
	hWndInfo *pHWI = NULL;
	bool entrance = false;
	for (unsigned int i = 0; i < tmpWndInfo->size(); i++)
	{
		if (tmpWndInfo->at(i).settingsWnd.lpszClassName == ClassName) { pHWI = &tmpWndInfo->at(i); entrance = true; }
	}

	if (entrance && pHWI->pDevice != NULL)
	{
		ZeroMemory(&pHWI->d3dParam, sizeof(pHWI->d3dParam));
		pHWI->bGraphics = false;
		pHWI->pDevice = NULL;

	}
	else { errorMsg(1, "Не найден класс, Удаление Graphic device невозможно."); return E_FAIL; }

	return S_OK;
}

void renderScene(wnd *windows, resources *Data)
{
	hWndInfo *pHWI = NULL;
	pHWI = windows->getSettingsWnd("Main");
	if (pHWI != NULL)
	{
		if (pHWI->bGraphics == TRUE)
		{
			pHWI->pDevice->Clear(0, NULL,  D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(100, 100, 100), 1.0f, 0);
			pHWI->pDevice->BeginScene();
				char keyboardState[256];
				pHWI->pKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

				if (KEYDOWN(keyboardState, DIK_LCONTROL)) pHWI->sCamera.sensetive = 0.0f;
				else pHWI->sCamera.sensetive = 0.3f;
				pHWI->sCamera.speed = 0.2f;

				setCamera(pHWI);
				Data->setShaderVal(pHWI->sCamera.wMatrix, "MatrixWorld", "Data/Shaders/VS.vs", pHWI->pDevice, TV_FLOAT4X4);
				Data->setShaderVal(pHWI->sCamera.pMatrix, "MatrixProjection", "Data/Shaders/VS.vs", pHWI->pDevice, TV_FLOAT4X4);
				Data->setShaderVal(pHWI->sCamera.vMatrix, "MatrixView", "Data/Shaders/VS.vs", pHWI->pDevice, TV_FLOAT4X4);
			
				model *Wolf1 = (model*)Data->getObject("Data/Models/Wolf.dae", OT_MODEL);
				
				updateModelAnim(Wolf1, Wolf1->pSkelet);
				Data->setShaderVal(transform(pHWI->pDevice, 270, NULL, NULL, NULL, NULL, 10.2f), "MatrixWorld", "Data/Shaders/VS.vs", pHWI->pDevice, TV_FLOAT4X4);
				drawModel(Wolf1, pHWI->pDevice, Data);
				model *Wolf2 = (model*)Data->getObject("Data/Models/CODMapShipment.dae", OT_MODEL);
				if (Wolf2 != NULL)
				{
					Data->setShaderVal(transform(pHWI->pDevice, NULL, NULL, NULL, NULL, NULL, 0.2f), "MatrixWorld", "Data/Shaders/VS.vs", pHWI->pDevice, TV_FLOAT4X4);
					drawModel(Wolf2, pHWI->pDevice, Data);
					if(!pHWI->threads->empty())
					{
						pHWI->threads->at(0)->join();
						pHWI->threads->clear();
					}
				}
			pHWI->pDevice->EndScene();
			pHWI->pDevice->Present(NULL, NULL, NULL, NULL);

		}
	}
}
