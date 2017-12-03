#include "Scene.h"

void initComponents(wnd *wInfo)
{

	graphics directx(wInfo->getAllSetWnd());
	devices device(wInfo->getAllSetWnd());

	wInfo->createWnd("Main");
	directx.createGraphicWnd("Main");
	device.сreateDevices("Main");

	hWndInfo *setWnd = wInfo->getSettingsWnd("Main");

	resources Data;
	std::thread *func2;


	//std::thread func(testThread,&Data, setWnd);
	//Data.loadObject("Data/Models/sky.dae", OT_MODEL, setWnd->pDevice);
	//Data.loadObject("Data/Textures/skybox_texture.jpg", OT_TEXTURE, setWnd->pDevice);
	//Data.setTexture("Data/Models/sky.dae", "Cube.001", "Data/Textures/skybox_texture.jpg");
	

	//Data.loadObject("Data/Models/House.dae", OT_MODEL, setWnd->pDevice);
	Data.loadObject("Data/Textures/floor3.jpg", OT_TEXTURE, setWnd->pDevice);
	//Data.setTexture("Data/Models/House.dae", "Floor", "Data/Textures/floor3.jpg");

	//Data.loadObject("Data/Textures/Wall2.jpg", OT_TEXTURE, setWnd->pDevice);
	//Data.setTexture("Data/Models/House.dae", "Wall", "Data/Textures/Wall2.jpg");

	//Data.loadObject("Data/Textures/OutWall.jpg", OT_TEXTURE, setWnd->pDevice);
	//Data.setTexture("Data/Models/House.dae", "OutWall", "Data/Textures/OutWall.jpg");

	//Data.loadObject("Data/Textures/Window.jpg", OT_TEXTURE, setWnd->pDevice);
	//Data.setTexture("Data/Models/House.dae", "Window", "Data/Textures/Window.jpg");

	Data.loadObject("Data/Shaders/VS.vs", OT_VSHADER, setWnd->pDevice);
	Data.loadObject("Data/Shaders/PS_type_1.ps", OT_PSHADER, setWnd->pDevice);

	Data.loadObject("Data/Tracks/Track1.wav", OT_MUSIC);
	Data.loadObject("Data/Tracks/Track2.wav", OT_MUSIC);

	Data.loadObject("Data/Models/man2.dae", OT_MODEL, setWnd->pDevice);
	Data.loadObject("Data/Models/Wolf.dae", OT_MODEL, setWnd->pDevice);

	D3DVERTEXELEMENT9 pDecl[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
		{ 0, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },

		D3DDECL_END()
	};

	//Data.loadObject("Data/Models/UZI.dae", OT_MODEL, setWnd->pDevice);
	model *model1 = (model*)Data.getObject("Data/Models/Wolf.dae", OT_MODEL);
	for (UINT i = 0; i < model1->pBuffer->size(); i++)
	{
		Data.setShaderVert(model1->modelName, model1->pBuffer->at(i).nameObj, "Data/Shaders/VS.vs");
		Data.setShaderPix(model1->modelName, model1->pBuffer->at(i).nameObj, "Data/Shaders/PS_type_1.ps");
		Data.setDecl(model1->modelName, model1->pBuffer->at(i).nameObj, pDecl, setWnd->pDevice);
		Data.setTexture(model1->modelName, model1->pBuffer->at(i).nameObj, "Data/Textures/floor3.jpg");

	}
	for (size_t i = 0; i < model1->pAnimation->size(); i++)
	{
		model1->pAnimation->at(i).stateAnim = true;
	}

		func2 = new std::thread(testThread, &Data, setWnd);
	setWnd->threads->push_back(func2);


	settingCam set;
	set.axisHeight		= NORMAL_Y;
	set.methodRenders	= RENDER_REF;
	set.control			= CC_MANUAL;
	set.view			= VIEW_3D;
	setWnd->sCamera		= set;

	initFont(setWnd, 20, "Courier New");



	startHandlerProc(wInfo, &Data);
}

void testThread(resources *Data, hWndInfo *setWnd)
{
	D3DVERTEXELEMENT9 pDecl[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
		{ 0, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },

		D3DDECL_END()
	};
	Data->loadObject("Data/Models/CODMapShipment.dae", OT_MODEL, setWnd->pDevice);
	model *model1 = (model*)Data->getObject("Data/Models/CODMapShipment.dae", OT_MODEL);
	for (UINT i = 0; i < model1->pBuffer->size(); i++)
	{
		Data->setShaderVert(model1->modelName, model1->pBuffer->at(i).nameObj, "Data/Shaders/VS.vs");
		Data->setShaderPix(model1->modelName, model1->pBuffer->at(i).nameObj, "Data/Shaders/PS_type_1.ps");
		Data->setDecl(model1->modelName, model1->pBuffer->at(i).nameObj, pDecl, setWnd->pDevice);
		//Data->setTexture(model1->modelName, model1->pBuffer->at(i).nameObj, "Data/Textures/floor3.jpg");

	}
}