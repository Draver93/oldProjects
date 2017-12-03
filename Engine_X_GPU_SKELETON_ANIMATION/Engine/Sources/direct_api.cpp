#include "direct_api.h"



direct_api::direct_api()
{

}
direct_api::~direct_api()
{
	if (p_direct3d)p_direct3d->Release();
	if (p_direct3d_device)p_direct3d_device->Release();
}

HRESULT direct_api::create_direct(HWND h_window, bool fullscrean)
{

	p_direct3d			= NULL;
	p_direct3d_device	= NULL;

	ZeroMemory(&display, sizeof(display));
	ZeroMemory(&direct3d_parametr, sizeof(direct3d_parametr));

	if (FAILED(p_direct3d = Direct3DCreate9(D3D_SDK_VERSION))) return E_FAIL;
	if (FAILED(p_direct3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,		//Выбор видеоадаптера дисплея значение default выбирает первичний адаптер
												&display					//Устанавливается указатель на структуру  
												))) return E_FAIL;			//в которую помещать инфо о режимах текущего адаптера

	direct3d_parametr.Windowed					= fullscrean;				//Оконный режим TRUE включен, FALSE выключен
	direct3d_parametr.BackBufferFormat			= display.Format;			//Устанавливаем для заднего буфера формат нашего дисплея
	direct3d_parametr.SwapEffect				= D3DSWAPEFFECT_DISCARD;	//Настройка заднего буфера
	direct3d_parametr.EnableAutoDepthStencil	= TRUE;						//Параметры глубины
	direct3d_parametr.AutoDepthStencilFormat	= D3DFMT_D16;				//Параметры глубины
	direct3d_parametr.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;

	if (direct3d_parametr.Windowed == FALSE)
	{
		direct3d_parametr.BackBufferCount			= 3;						//Количество задних буферов 
		direct3d_parametr.BackBufferHeight			= display.Height;			//Разрешение экрана
		direct3d_parametr.BackBufferWidth			= display.Width;			//Разрешение экрана
		direct3d_parametr.FullScreen_RefreshRateInHz = display.RefreshRate;		//Установка FPS
	}
	else
	{
		SetWindowPos(h_window, 0, 0, 0, 800, 600, SWP_SHOWWINDOW);	//Ресайз оконного режима
	}

	p_direct3d->CreateDevice(D3DADAPTER_DEFAULT,	//Стандартный адаптор видеокарта
		D3DDEVTYPE_HAL,								//Желаемый тип устройства. D3DDEVTYPE_HAL аппаратно, D3DDEVTYPE_REF программно
		h_window,									//Дискриптор главного окна
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,		//Способ обработки вершин HARDWARE видеокарта, SOFTWARE программная обработка
		&direct3d_parametr,							//Для установки наших параметров приложению
		&p_direct3d_device							//В эту переменную присваивается результат
		);

	p_direct3d_device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);		//Параметры глубины
	p_direct3d_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);		//Отсечение не попадающих в кадр полигонов 
	p_direct3d_device->SetRenderState(D3DRS_LIGHTING, FALSE);			//Отключение обработки света 
	
	p_direct3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	p_direct3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//Этому сдесь не место перенести

	p_direct3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	p_direct3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	p_direct3d_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);
	p_direct3d_device->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 8);
	p_direct3d_device->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	p_direct3d_device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	p_direct3d_device->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);
	p_direct3d_device->SetSamplerState(1, D3DSAMP_MAXANISOTROPY, 8);
	return S_OK;
}
void direct_api::render_scene()
{
	// Очистка буфера
	p_direct3d_device->Clear(0,											//Номер массива прямоугольников подвергающихся очистке
							NULL,										//Адрес массива прямоугольника 
							D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,			//По какому принцепу очищать
							D3DCOLOR_RGBA(150, 150, 255, 1),					//Цвет которым очищаем буфер
							1.0f,										//Глубина Z 
							0);											//Трафарет(не используем)
	p_direct3d_device->BeginScene();	//Начало построения сцены

	global_render();

	p_direct3d_device->EndScene();		//Завершение построения сцены
	p_direct3d_device->Present(NULL, NULL, NULL, NULL);		//Как я понял это присвоение всей площади окна нашему выводу
}
IDirect3DDevice9* direct_api::get_device()
{
	return p_direct3d_device;
}