#include "Graphics.h"

HRESULT setCamera(hWndInfo *pHWI)
{
	D3DXVECTOR3 normal;

	if (pHWI->sCamera.control == CC_MANUAL) manualControl(pHWI);
	else if (pHWI->sCamera.control == CC_STATIC) 
	{
		//¬ разработке
	}
	else if (pHWI->sCamera.control == CC_PATH) 
	{
		//¬ разработке
	}

	D3DXMatrixIdentity(&pHWI->sCamera.wMatrix);
	if (pHWI->sCamera.axisHeight == NORMAL_X)	normal = { 1, 0, 0 };
	else if (pHWI->sCamera.axisHeight == NORMAL_Y)	normal = { 0, 1, 0 };
	else if (pHWI->sCamera.axisHeight == NORMAL_Z)	normal = { 0, 0, 1 };

	D3DXMatrixLookAtLH(&pHWI->sCamera.vMatrix, &D3DXVECTOR3(pHWI->sCamera.posX, pHWI->sCamera.posY, pHWI->sCamera.posZ),
		&D3DXVECTOR3(pHWI->sCamera.lookX, pHWI->sCamera.lookY, pHWI->sCamera.lookZ),
		&normal);

	if (pHWI->sCamera.view == VIEW_2D)
		D3DXMatrixOrthoLH(&pHWI->sCamera.pMatrix, (float)pHWI->settingsWnd.width, (float)pHWI->settingsWnd.height, 1.0f, 10000.0f);
	if (pHWI->sCamera.view == VIEW_3D)
		D3DXMatrixPerspectiveFovLH(&pHWI->sCamera.pMatrix, 0.9f, (float)pHWI->settingsWnd.height/(float)pHWI->settingsWnd.width, 1.0f, 10000.0f);

	if (pHWI->sCamera.methodRenders == RENDER_REF)
	{
		pHWI->pDevice->SetTransform(D3DTS_WORLD, &pHWI->sCamera.wMatrix);
		pHWI->pDevice->SetTransform(D3DTS_VIEW, &pHWI->sCamera.vMatrix);
		pHWI->pDevice->SetTransform(D3DTS_PROJECTION, &pHWI->sCamera.pMatrix);
	}
	if (pHWI->sCamera.methodRenders == RENDER_HAL)
	{
		// ¬ разработке
	}
	return S_OK;
}
void manualControl(hWndInfo *pHWI)
{
	pHWI->sCamera.timeNew = timeGetTime();
	char keyboardState[256];
	DIMOUSESTATE mouseState;
	DWORD time = pHWI->sCamera.timeNew - pHWI->sCamera.timeOld;

	pHWI->pKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);
	pHWI->pMouse->GetDeviceState(sizeof(mouseState), (LPVOID)&mouseState);

	if (KEYDOWN(keyboardState, DIK_W))
	{
		pHWI->sCamera.posX += (time * sin(pHWI->sCamera.radY)*sin(pHWI->sCamera.radX))* pHWI->sCamera.speed;
		pHWI->sCamera.posY += (time * cos(pHWI->sCamera.radY))* pHWI->sCamera.speed;
		pHWI->sCamera.posZ += (time * sin(pHWI->sCamera.radY)*cos(pHWI->sCamera.radX))* pHWI->sCamera.speed;
	}
	if (KEYDOWN(keyboardState, DIK_S))
	{
		pHWI->sCamera.posX -= (time * sin(pHWI->sCamera.radY)*sin(pHWI->sCamera.radX))* pHWI->sCamera.speed;
		pHWI->sCamera.posY -= (time * cos(pHWI->sCamera.radY))* pHWI->sCamera.speed;
		pHWI->sCamera.posZ -= (time * sin(pHWI->sCamera.radY)*cos(pHWI->sCamera.radX))* pHWI->sCamera.speed;
	}
	if (KEYDOWN(keyboardState, DIK_A))
	{
		pHWI->sCamera.posX += (time * sin(pHWI->sCamera.radX - (180 * (D3DX_PI / 360))))* pHWI->sCamera.speed;
		pHWI->sCamera.posZ += (time * cos(pHWI->sCamera.radX - (180 * (D3DX_PI / 360))))* pHWI->sCamera.speed;
	}
	if (KEYDOWN(keyboardState, DIK_D))
	{
		pHWI->sCamera.posX -= (time * sin(pHWI->sCamera.radX - (180 * (D3DX_PI / 360))))* pHWI->sCamera.speed;
		pHWI->sCamera.posZ -= (time * cos(pHWI->sCamera.radX - (180 * (D3DX_PI / 360))))* pHWI->sCamera.speed;
	}

	GetCursorPos(&pHWI->sCamera.cPosNew);
	RECT rect;
	GetWindowRect(pHWI->hWnd, &rect);
	if (pHWI->sCamera.cPosNew.x > rect.right - 5) SetCursorPos(rect.left + 5, pHWI->sCamera.cPosNew.y);
	if (pHWI->sCamera.cPosNew.y > rect.bottom - 5) SetCursorPos(pHWI->sCamera.cPosNew.x, rect.top + 5);
	if (pHWI->sCamera.cPosNew.x < rect.left + 5) SetCursorPos(rect.right - 5, pHWI->sCamera.cPosNew.y);
	if (pHWI->sCamera.cPosNew.y < rect.top + 5) SetCursorPos(pHWI->sCamera.cPosNew.x, rect.bottom - 5);

	if (pHWI->sCamera.cPosOld.x > pHWI->sCamera.cPosNew.x) { pHWI->sCamera.angleX -= (pHWI->sCamera.cPosOld.x - pHWI->sCamera.cPosNew.x)*pHWI->sCamera.sensetive; }
	if (pHWI->sCamera.cPosOld.x < pHWI->sCamera.cPosNew.x) { pHWI->sCamera.angleX += (pHWI->sCamera.cPosNew.x - pHWI->sCamera.cPosOld.x)*pHWI->sCamera.sensetive; }
	if (pHWI->sCamera.cPosOld.y > pHWI->sCamera.cPosNew.y && pHWI->sCamera.angleY > 5) { pHWI->sCamera.angleY -= (pHWI->sCamera.cPosOld.y - pHWI->sCamera.cPosNew.y)*pHWI->sCamera.sensetive; }
	if (pHWI->sCamera.cPosOld.y < pHWI->sCamera.cPosNew.y && pHWI->sCamera.angleY < 355) { pHWI->sCamera.angleY += (pHWI->sCamera.cPosNew.y - pHWI->sCamera.cPosOld.y)*pHWI->sCamera.sensetive; }

	if (pHWI->sCamera.angleY < 5)pHWI->sCamera.angleY = 5;
	if (pHWI->sCamera.angleY > 355)pHWI->sCamera.angleY = 355;

	GetCursorPos(&pHWI->sCamera.cPosOld);

	pHWI->sCamera.radX = pHWI->sCamera.angleX *(D3DX_PI / 360);
	pHWI->sCamera.radY = pHWI->sCamera.angleY*(D3DX_PI / 360);

	pHWI->sCamera.lookX = pHWI->sCamera.posX + 1.0f * sin(pHWI->sCamera.radY)*sin(pHWI->sCamera.radX);
	pHWI->sCamera.lookY = pHWI->sCamera.posY + 1.0f * cos(pHWI->sCamera.radY);
	pHWI->sCamera.lookZ = pHWI->sCamera.posZ + 1.0f * sin(pHWI->sCamera.radY)*cos(pHWI->sCamera.radX);

	pHWI->sCamera.timeOld = timeGetTime();
}
void initFont(hWndInfo *pHWI, int size, char * name)
{
	D3DXCreateFont(pHWI->pDevice,
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
		&pHWI->pFont
		);
}
void printText(char * text, int posX, int posY, int entryFX, int entryFY, D3DCOLOR color, hWndInfo *pHWI)
{
	RECT rec;
	ZeroMemory(&rec, sizeof(rec));
	rec.left = posX;
	rec.top = posY;
	rec.right = entryFX;
	rec.bottom = entryFY;

	if (pHWI->pFont != NULL)
	{
		pHWI->pFont->DrawText(NULL,
			text,
			-1,
			&rec,
			DT_WORDBREAK,
			color);
	}
}
D3DXMATRIX transform(LPDIRECT3DDEVICE9 pDevice, float angle_x, float angle_y, float pos_x, float pos_y, float pos_z, float size)
{
	D3DXMATRIX	matrix_world,
		matrix_worldX, matrix_worldY,
		matrix_worldT,
		matrix_worldS;
	float		angle_rad_x,
		angle_rad_y;
	angle_rad_x = angle_x*(D3DX_PI / 180);
	angle_rad_y = angle_y*(D3DX_PI / 180);
	D3DXMatrixRotationX(&matrix_worldX, angle_rad_x);
	D3DXMatrixRotationY(&matrix_worldY, angle_rad_y);
	D3DXMatrixMultiply(&matrix_world, &matrix_worldX, &matrix_worldY);

	D3DXMatrixTranslation(&matrix_worldT, pos_x, pos_y, pos_z);
	D3DXMatrixMultiply(&matrix_world, &matrix_world, &matrix_worldT);
	D3DXMatrixScaling(&matrix_worldS, size, size, size);
	D3DXMatrixMultiply(&matrix_world, &matrix_world, &matrix_worldS);
	if (!angle_x && !angle_y && !pos_x && !pos_y && !pos_z && !size) D3DXMatrixIdentity(&matrix_world);

	pDevice->SetTransform(D3DTS_WORLD, &matrix_world);
	return matrix_world;
}
HRESULT drawModel(model *data, LPDIRECT3DDEVICE9 pDevice, resources *res)
{	
	if (!data->pAnimation->empty()) updateModelAnim(data, data->pSkelet);

	for (UINT i = 0; i < data->pBuffer->size(); i++)
	{
		std::vector<D3DXMATRIX> *pMasMatrix = collectMasMatrix(&data->pBuffer->at(i));
		if (pMasMatrix != NULL)
			res->setShaderVal(pMasMatrix, "MatBones", data->pBuffer->at(i).vShaderName, pDevice, TV_MASFLOAT4X4);
		delete pMasMatrix;

																								
		pDevice->SetStreamSource(0, data->pBuffer->at(i).p_vb_only, 0, sizeof(BUFFER));
		//pDevice->SetIndices(data->pBuffer->at(i).p_ib);

		if (data->pBuffer->at(i).pDecl == NULL)
		{
			pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
		}
		else
		{
			pDevice->SetFVF(NULL);
			pDevice->SetVertexDeclaration(data->pBuffer->at(i).pDecl);
		}
		if (data->pBuffer->at(i).pVshader != NULL && data->pBuffer->at(i).pPshader != NULL)
		{
			pDevice->SetVertexShader(data->pBuffer->at(i).pVshader);
			pDevice->SetPixelShader(data->pBuffer->at(i).pPshader);
		}

		pDevice->SetTexture(0, data->pBuffer->at(i).pTexture);
		pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

		pDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0, data->pBuffer->at(i).sizeVB_only);
		//pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, data->pBuffer->at(i).sizeVB, 0, data->pBuffer->at(i).sizeIB/3);

		pDevice->SetFVF(NULL);
		pDevice->SetVertexDeclaration(NULL);
		pDevice->SetVertexShader(NULL);
		pDevice->SetPixelShader(NULL);
		pDevice->SetTexture(0, NULL);
		pDevice->SetStreamSource(0, NULL, 0, 0);
		pDevice->SetIndices(0);
	}

	return S_OK;
}

void updateModelAnim(model *pModel, skeletData* pSkelet)
{
	bool ent = false;
	for (UINT j = 0; j < pModel->pAnimation->size(); j++)
	{
		if (pModel->pAnimation->at(j).stateAnim && pModel->pAnimation->at(j).nameBone == pSkelet->sid) //»щет все запущенные анимации вли€ющие на кость
		{
			ent = true;
			DWORD ThisTime = timeGetTime();

			//¬ычисть из времени последнего вызова анимации
			DWORD time = 0;
			if (pModel->pAnimation->at(j).timeLast != 0)
				time = ThisTime - pModel->pAnimation->at(j).timeLast;

			time /= (DWORD)pModel->pAnimation->at(j).speedAnim; // вариант изменени€ скрости

			std::vector<float>* masTime = pModel->pAnimation->at(j).pDataTime;

			//ѕрибавить вычесленное врем€ к времени что хранит состо€ние анимации сейчас 
			pModel->pAnimation->at(j).timeCounter += time;
			if (pModel->pAnimation->at(j).timeCounter > masTime->back() * 1000)
			{
				pModel->pAnimation->at(j).timeCounter = 0;
				pModel->pAnimation->at(j).timeLast = 0;
				//pModel->pAnimation->at(j).stateAnim = false; 
			}

			//”знать между какими двум€ матрицами это врем€ находитс€ 
			DWORD PosAnim = 1;
			PosAnim = getPosAnimation(&pModel->pAnimation->at(j), pModel->pAnimation->at(j).timeCounter); //¬озвращает количество полностью пройденных анимациий + неполностью выполненную
			DWORD timeCombine = (DWORD)(masTime->at(PosAnim) * 1000 - masTime->at(PosAnim - 1) * 1000);

			//–асчитываем врем€ не полного перехода 
			DWORD timeNow = 0;
			if(pModel->pAnimation->at(j).timeCounter != 0)
				timeNow = (DWORD)(pModel->pAnimation->at(j).timeCounter - masTime->at(PosAnim - 1) * 1000);

			//расчитать преобразованную матрицу от времени
			pSkelet->mIntrpolate = getMatrixOfTime(	pModel->pAnimation->at(j).pDataMatrix->at(PosAnim - 1), 
													pModel->pAnimation->at(j).pDataMatrix->at(PosAnim), timeCombine, timeNow);

											//ѕотом умножаетс€ на InverseBindMatrix
			pModel->pAnimation->at(j).timeLast = timeGetTime();
		}
	}

	if (pSkelet->pParentData != NULL)
	{
		if (!ent)
			D3DXMatrixMultiply(&pSkelet->mFinal, &pSkelet->pParentData->mFinal, &pSkelet->localMatrix);
		else
			D3DXMatrixMultiply(&pSkelet->mFinal, &pSkelet->pParentData->mFinal, &pSkelet->mIntrpolate);
	}
	else
	{
		if (!ent)
			pSkelet->mFinal = pSkelet->localMatrix;
		else
			pSkelet->mFinal = pSkelet->mIntrpolate;
	}

	if (pSkelet->pChildData != NULL)
	{
		updateModelAnim(pModel, pSkelet->pChildData);
	}
	if (pSkelet->pSiblingData != NULL)
	{
		updateModelAnim(pModel, pSkelet->pSiblingData);
	}
}

std::vector<D3DXMATRIX> *collectMasMatrix(bufferObj *pBuffer)
{
	if (pBuffer->pMasBoneForShader == NULL) return NULL;
	std::vector<D3DXMATRIX> *ResultMas = new std::vector<D3DXMATRIX>();
	D3DXMATRIX tmpMasMatrix;
	for (UINT i = 0; i < pBuffer->pMasBoneForShader->size(); i++)
	{
		D3DXMatrixMultiplyTranspose(&tmpMasMatrix, &pBuffer->pMasBoneForShader->at(i)->mFinal, &pBuffer->pMasBoneForShader->at(i)->mInversePos);
		ResultMas->push_back(tmpMasMatrix);
		D3DXMatrixIdentity(&tmpMasMatrix);
	}
	return ResultMas;
} 

D3DXMATRIX getMatrixOfTime(D3DXMATRIX matrixBegin, D3DXMATRIX matrixEnd, int time, int timeNow)
{
	return matrixBegin + (matrixEnd - matrixBegin) / (float)time * (float)timeNow;
}
int getPosAnimation(animation* pAnim, int time)
{
	bool entrance = false;
	for (UINT i = 0; i < pAnim->pDataTime->size(); i++)
	{
		if ( (pAnim->pDataTime->at(0) * 1000) > time) return 1;
		else
		if ( (pAnim->pDataTime->at(i)*1000) > time && i < pAnim->pDataTime->size() )
		{
			return i;
			entrance = true;
		}
	}
	if (!entrance) 
		return pAnim->pDataTime->size() - 1;

	return 0;
}

