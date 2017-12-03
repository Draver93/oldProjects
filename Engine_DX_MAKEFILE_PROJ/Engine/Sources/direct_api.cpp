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
	if (FAILED(p_direct3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,		//����� ������������� ������� �������� default �������� ��������� �������
												&display					//��������������� ��������� �� ���������  
												))) return E_FAIL;			//� ������� �������� ���� � ������� �������� ��������

	direct3d_parametr.Windowed					= fullscrean;				//������� ����� TRUE �������, FALSE ��������
	direct3d_parametr.BackBufferFormat			= display.Format;			//������������� ��� ������� ������ ������ ������ �������
	direct3d_parametr.SwapEffect				= D3DSWAPEFFECT_DISCARD;	//��������� ������� ������
	direct3d_parametr.EnableAutoDepthStencil	= TRUE;						//��������� �������
	direct3d_parametr.AutoDepthStencilFormat	= D3DFMT_D16;				//��������� �������
	direct3d_parametr.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;

	if (direct3d_parametr.Windowed == FALSE)
	{
		direct3d_parametr.BackBufferCount			= 3;						//���������� ������ ������� 
		direct3d_parametr.BackBufferHeight			= display.Height;			//���������� ������
		direct3d_parametr.BackBufferWidth			= display.Width;			//���������� ������
		direct3d_parametr.FullScreen_RefreshRateInHz = display.RefreshRate;		//��������� FPS
	}
	else
	{
		SetWindowPos(h_window, 0, 0, 0, 800, 600, SWP_SHOWWINDOW);	//������ �������� ������
	}

	p_direct3d->CreateDevice(D3DADAPTER_DEFAULT,	//����������� ������� ����������
		D3DDEVTYPE_HAL,								//�������� ��� ����������. D3DDEVTYPE_HAL ���������, D3DDEVTYPE_REF ����������
		h_window,									//���������� �������� ����
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,		//������ ��������� ������ HARDWARE ����������, SOFTWARE ����������� ���������
		&direct3d_parametr,							//��� ��������� ����� ���������� ����������
		&p_direct3d_device							//� ��� ���������� ������������� ���������
		);

	p_direct3d_device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);		//��������� �������
	p_direct3d_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);		//��������� �� ���������� � ���� ��������� 
	p_direct3d_device->SetRenderState(D3DRS_LIGHTING, FALSE);			//���������� ��������� ����� 
	
	p_direct3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	p_direct3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//����� ����� �� ����� ���������

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
	// ������� ������
	p_direct3d_device->Clear(0,											//����� ������� ��������������� �������������� �������
							NULL,										//����� ������� �������������� 
							D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,			//�� ������ �������� �������
							D3DCOLOR_RGBA(150, 150, 255, 1),					//���� ������� ������� �����
							1.0f,										//������� Z 
							0);											//��������(�� ����������)
	p_direct3d_device->BeginScene();	//������ ���������� �����

	global_render();

	p_direct3d_device->EndScene();		//���������� ���������� �����
	p_direct3d_device->Present(NULL, NULL, NULL, NULL);		//��� � ����� ��� ���������� ���� ������� ���� ������ ������
}
IDirect3DDevice9* direct_api::get_device()
{
	return p_direct3d_device;
}