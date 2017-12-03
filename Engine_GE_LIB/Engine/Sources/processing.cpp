#include "processing.h"

void ctrl(trans *tmp, float speed)
{
	sTimer st = manager.getTimer(1000);
	double dt = st.getDeltaTime(UOT_MILI);
	manager.setTimer(1000);

	float radY = 0, radZ = 0, radX = 0;
	radY = tmp->angleY*(D3DX_PI / 180);
	radZ = tmp->angleZ*(D3DX_PI / 180);
	radX = tmp->angleX*(D3DX_PI / 180);

	if (KEYDOWN(manager.keyboard_data, DIK_W))
	{
		tmp->x += -sin(radY) * (speed * dt);
		tmp->z += cos(radY)  * (speed * dt);
	}
	if (KEYDOWN(manager.keyboard_data, DIK_S))
	{
		tmp->x -= -sin(radY) * (speed * dt);
		tmp->z -= cos(radY)  * (speed * dt);
	}
	if (KEYDOWN(manager.keyboard_data, DIK_A))
	{
		tmp->x += -sin(radY - 90 * (D3DX_PI / 180)) * (speed * dt);
		tmp->z += cos(radY - 90 * (D3DX_PI / 180))  * (speed * dt);
	}
	if (KEYDOWN(manager.keyboard_data, DIK_D))
	{
		tmp->x -= -sin(radY - 90 * (D3DX_PI / 180)) * (speed * dt);
		tmp->z -= cos(radY - 90 * (D3DX_PI / 180))  * (speed * dt);
	}
}
D3DXMATRIX transform(float angle_x, float angle_y, float angle_z, float pos_x, float pos_y, float pos_z, float size)
{
	D3DXMATRIX	matrix_world, 
		matrix_worldT,
		matrix_worldRX,
		matrix_worldRY,
		matrix_worldRZ,
		matrix_worldS;

	float	angle_rad_x = angle_x*(D3DX_PI / 180),
		angle_rad_y = angle_y*(D3DX_PI / 180),
		angle_rad_z = angle_z*(D3DX_PI / 180);

	double num = 0.5;
	double crat = 10;
	if (size > 0)
		for (int i = 0; i < size; i++)
		{
			num += 1.0f / (float)crat;
		}
	else
		for (int i = 0; i > size; i--)
		{
			double tmp = (num - (1.0 / crat));
			if (tmp < (1.0 / crat))
				crat *= 10;
			num -= 1.0 / crat;
		}

	D3DXMatrixIdentity(&matrix_world);
	D3DXMatrixScaling(&matrix_worldS, (float)num, (float)num, (float)num);
	D3DXMatrixMultiply(&matrix_world, &matrix_world, &matrix_worldS);
	D3DXMatrixRotationX(&matrix_worldRX, angle_rad_x);
	D3DXMatrixRotationY(&matrix_worldRY, angle_rad_y);
	D3DXMatrixRotationZ(&matrix_worldRZ, angle_rad_z);
	D3DXMatrixMultiply(&matrix_world, &matrix_world, &matrix_worldRX);
	D3DXMatrixMultiply(&matrix_world, &matrix_world, &matrix_worldRY);
	D3DXMatrixMultiply(&matrix_world, &matrix_world, &matrix_worldRZ);
	D3DXMatrixTranslation(&matrix_worldT, pos_z, pos_y, pos_x);
	D3DXMatrixMultiply(&matrix_world, &matrix_world, &matrix_worldT);


	return matrix_world;
}

void manualControl()
{
	CAMERA *cam = manager.getCamSetting();
	cam->timeNew = timeGetTime();

	DWORD time = cam->timeNew - cam->timeOld;

	if (KEYDOWN(manager.keyboard_data, DIK_W))
	{
		cam->posX += (time * sin(cam->radY)*sin(cam->radX))* cam->speed;
		cam->posY += (time * cos(cam->radY))* cam->speed;
		cam->posZ += (time * sin(cam->radY)*cos(cam->radX))* cam->speed;
	}
	if (KEYDOWN(manager.keyboard_data, DIK_S))
	{
		cam->posX -= (time * sin(cam->radY)*sin(cam->radX))* cam->speed;
		cam->posY -= (time * cos(cam->radY))* cam->speed;
		cam->posZ -= (time * sin(cam->radY)*cos(cam->radX))* cam->speed;
	}
	if (KEYDOWN(manager.keyboard_data, DIK_A))
	{
		cam->posX += (time * sin(cam->radX - (180 * (D3DX_PI / 360))))* cam->speed;
		cam->posZ += (time * cos(cam->radX - (180 * (D3DX_PI / 360))))* cam->speed;
	}
	if (KEYDOWN(manager.keyboard_data, DIK_D))
	{
		cam->posX -= (time * sin(cam->radX - (180 * (D3DX_PI / 360))))* cam->speed;
		cam->posZ -= (time * cos(cam->radX - (180 * (D3DX_PI / 360))))* cam->speed;
	}


	cam->angleX += (float)manager.mouse_data.lX;
	cam->angleY += (float)manager.mouse_data.lY;

	RECT rect;
	GetWindowRect(manager.getHWnd(), &rect);
	holdMouseInRect(rect);

	MSG *msg = manager.getMessage();
	if (msg->message == WM_MOUSEWHEEL)
	{
		if (GET_WHEEL_DELTA_WPARAM(msg->wParam) > 0)
		{

		}
		else if (GET_WHEEL_DELTA_WPARAM(msg->wParam) < 0)
		{

		}
	}

	cam->radX = cam->angleX * (D3DX_PI / 360);
	if (cam->angleY > 359) 
		cam->angleY = 359;
	if (cam->angleY < 1) 
		cam->angleY = 1;

	cam->radY = cam->angleY * (D3DX_PI / 360);

	cam->lookX = cam->posX + 1.0f * sin(cam->radY)*sin(cam->radX);
	cam->lookY = cam->posY + 1.0f * cos(cam->radY);
	cam->lookZ = cam->posZ + 1.0f * sin(cam->radY)*cos(cam->radX);

	cam->timeOld = timeGetTime();
}

void holdMouseInRect(RECT rc)
{
	POINT posMouse;
	GetCursorPos(&posMouse);
	
	if (posMouse.x > rc.right) SetCursorPos(rc.left, posMouse.y);
	if (posMouse.y > rc.bottom) SetCursorPos(posMouse.x, rc.top);
	if (posMouse.x < rc.left) SetCursorPos(rc.right, posMouse.y);
	if (posMouse.y < rc.top) SetCursorPos(posMouse.x, rc.bottom);
}

POINT getTextSize(std::string str)
{
	RECT rc = {0,0,0,0};
	LPD3DXFONT pFont = manager.getFont();

	pFont->DrawText(NULL,
		str.c_str(),
		-1,
		&rc,
		DT_CALCRECT,
		D3DCOLOR_ARGB(0,0,0,0));

	POINT pt;
	pt.x = rc.right - rc.left;
	pt.y = rc.bottom - rc.top;
	return pt;
}
POINT getTextSize(std::wstring str)
{
	RECT rc = { 0,0,0,0 };
	LPD3DXFONT pFont = manager.getFont();

	pFont->DrawTextW(NULL,
		str.c_str(),
		-1,
		&rc,
		DT_CALCRECT,
		D3DCOLOR_ARGB(0, 0, 0, 0));

	POINT pt;
	pt.x = rc.right - rc.left;
	pt.y = rc.bottom - rc.top;
	return pt;
}
std::string	getParameter(std::string source, std::string param)
{
	bool entrance = false, takeParam = true;
	std::string retStr;

	for (unsigned int i = 0; i < source.length(); i++)
	{
		if (source.at(i) == '<') entrance = true;
		if (entrance)
		{
			if (source.at(i) == ' ')
			{
				takeParam = true;
				for (unsigned int j = 0; j < param.length(); j++)
				{
					i++;
					if (source.at(i) != param.at(j)) takeParam = false;
				}
				if (takeParam)
				{
					int countSym = 0;
					while (source.at(i) != source.back())
					{
						if (source.at(i) == '\"') countSym++;
						if (countSym == 2) break;
						i++;
						if (source.at(i) != '=' &&
							source.at(i) != '\\' &&
							source.at(i) != '\"' &&
							source.at(i) != ' ' &&
							source.at(i) != '>'&&
							source.at(i) != '#')
							retStr.push_back(source.at(i));
					}
					return retStr;
				}
			}
		}
	}
	return "None";
}
float	getArrElem(geometry *tmpGeom, std::string name, DWORD pos, DWORD id)
{
	std::vector<source> *tmpSrc = &tmpGeom->data;

	for (unsigned int i1 = 0; i1 < tmpSrc->size(); i1++)
	{
		if (std::strstr(tmpSrc->at(i1).sourceId.c_str(), name.c_str()))
		{
			return tmpSrc->at(i1).data.at((pos*tmpSrc->at(i1).stride) + (id - 1));
		}
	}
	return 0.0f;
}
HRESULT	getArray(std::string rawData, std::vector<float>* tmpArray)
{
	if (rawData.empty()) return NULL;

	std::string tmp_string = "-1";
	tmp_string.clear();
	unsigned int i = 0;
	while (rawData.at(i) != '>' && rawData.size() != i) i++;

	while (rawData.at(i) != '<' && rawData.size() != i)
	{
		i++;

		if (rawData.at(i) == ' ' || i == rawData.length() || rawData.at(i) == '<')
		{
			if (!tmp_string.empty())
				tmpArray->push_back(std::stof(tmp_string));
			tmp_string.clear();
			if (rawData.at(i) != '<') i++;
			else break;
		}
		tmp_string.push_back(rawData.at(i));
	}

	return NULL;
}
HRESULT	getArrayName(std::string rawData, std::vector<std::string>* tmpArray)
{
	if (rawData.empty()) return NULL;

	std::string tmp_string;
	tmp_string.clear();
	unsigned int i = 0;
	while (rawData.at(i) != '>' || rawData.size() == i) i++;

	while (rawData.at(i) != '<' || rawData.size() == i)
	{
		i++;

		if (rawData.at(i) == ' ' || i == rawData.length() || rawData.at(i) == '<')
		{
			tmpArray->push_back(tmp_string);
			tmp_string.clear();
			if (rawData.at(i) != '<') i++;
			else break;
		}
		tmp_string.push_back(rawData.at(i));
	}

	return NULL;
}
void	setWeight(model *data, UINT iPos, BUFFER *pBuffer, std::string idGeom)
{
	//ѕолучили геометрию в которой нужно искать данные
	geometry* tmpGeom = NULL;
	for (unsigned int i2 = 0; i2 < data->pControllers.size(); i2++)
	{
		if (data->pControllers.at(i2).sourceGeom == idGeom)
		{
			tmpGeom = &data->pControllers.at(i2);
			break;
		}
	}
	//ѕроверка на существование данных
	if (tmpGeom == NULL) return;

	//—одержит сортированный по vcount массив индексов 
	std::vector<float>		*tmpArr = &tmpGeom->indexData.at(0).dataIndexes.at(iPos);

	//ќписание позиций в индексах (<v>)
	std::vector<input>		*tmpInputICtrl = &tmpGeom->indexData.at(0).inputSettings;

	//локальна€ позици€ веса и индекса матрицы в массиве индексов
	int weightPos = 0, indexPos = 0;
	for (UINT i1 = 0; i1 < tmpInputICtrl->size(); i1++)
	{
		if (tmpInputICtrl->at(i1).semantic == "WEIGHT") weightPos = tmpInputICtrl->at(i1).offset;
		if (tmpInputICtrl->at(i1).semantic == "JOINT") indexPos = tmpInputICtrl->at(i1).offset;
	}


	for (UINT i0 = 0; i0 < tmpArr->size() / tmpInputICtrl->size(); i0++)
	{
		bool entrance = false;

		for (UINT i1 = 0; i1 < tmpInputICtrl->size(); i1++)
		{
			if (tmpInputICtrl->at(i1).semantic == "WEIGHT")
			{
				for (UINT i2 = 0; i2 < 4; i2++)
				{
					if (pBuffer->weights[i2] == 0)
					{
						pBuffer->weights[i2] = getArrElem(tmpGeom, tmpInputICtrl->at(i1).source, (DWORD)tmpArr->at((i0*tmpInputICtrl->size()) + i1), 1);
						pBuffer->indeces[i2] = tmpArr->at((i0*tmpInputICtrl->size()) + i1 + (indexPos - weightPos));
						entrance = true;
						break;
					}
				}
				if (!entrance)
				{
					float tmpVal = getArrElem(tmpGeom, tmpInputICtrl->at(i1).source, (DWORD)tmpArr->at((i0*tmpInputICtrl->size()) + i1), 1);
					for (UINT i3 = 0; i3 < 4; i3++)
					{
						if (pBuffer->weights[i3] < tmpVal)
						{
							pBuffer->weights[i3] = tmpVal;
							pBuffer->indeces[i3] = tmpArr->at((i0*tmpInputICtrl->size()) + i1 + (indexPos - weightPos));
							break;
						}
					}
				}
			}
		}
	}
	//нормализаци€ весов 
	float sum = pBuffer->weights[0] + pBuffer->weights[1] + pBuffer->weights[2] + pBuffer->weights[3];
	sum = 100 / sum;
	for (int i = 0; i < 4; i++)
	{
		pBuffer->weights[i] = (sum * pBuffer->weights[i]) / 100;
	}
}
void	convertData(model *data, LPDIRECT3DDEVICE9 pDevice)
{
	if (pDevice == NULL) return;
	std::vector<geometry> *tmpObjects = &data->pObjects;
	for (UINT indexObj = 0; indexObj < tmpObjects->size(); indexObj++)
	{
		bufferObj *tmpBufObj = new bufferObj();
		tmpBufObj->nameObj = tmpObjects->at(indexObj).name;
		tmpBufObj->idObj = tmpObjects->at(indexObj).id;
		tmpBufObj->pMasBoneForShader = InitMasBoneForShader(data, tmpObjects->at(indexObj).id);

		std::string NameMasVert;
		for (UINT i = 0; i < tmpObjects->at(indexObj).veticesData.inputSettings.size(); i++)
		{
			if (tmpObjects->at(indexObj).veticesData.inputSettings.at(i).semantic == "POSITION")
				NameMasVert = tmpObjects->at(indexObj).veticesData.inputSettings.at(i).source;
		}
		if (NameMasVert.empty())
		{
			for (UINT i = 0; i < tmpObjects->at(indexObj).indexData.at(0).inputSettings.size(); i++)
			{
				if (tmpObjects->at(indexObj).indexData.at(0).inputSettings.at(i).semantic == "POSITION")
					NameMasVert = tmpObjects->at(indexObj).indexData.at(0).inputSettings.at(i).source;
			}
		}
		std::vector<BUFFER> VB_data_o;


		DWORD countVertMas = 0;
		for (UINT i = 0; i < tmpObjects->at(indexObj).data.size(); i++)
		{
			if (tmpObjects->at(indexObj).data.at(i).sourceId == NameMasVert) countVertMas = tmpObjects->at(indexObj).data.at(i).count;
		}
		BUFFER *VB_data = new BUFFER[countVertMas];
		std::vector<WORD> IB_data;

		std::vector<indexlist> *tmpIndexData = &tmpObjects->at(indexObj).indexData;
		for (UINT indexDI = 0; indexDI < tmpIndexData->size(); indexDI++) //÷икл прогон€ет все наборы указателейы
		{
			int sizeStack = 0;
			int sizeStackAll = 0;
			std::vector<patternBuf> patBufI;
			std::vector<input>* tmpInputV = &tmpObjects->at(indexObj).veticesData.inputSettings;
			std::vector<input>* tmpInputI = &tmpIndexData->at(indexDI).inputSettings;
			for (unsigned int i2 = 0; i2 < tmpInputI->size(); i2++)
			{
				if (tmpInputI->at(i2).semantic == "VERTEX")
				{
					for (unsigned int i3 = 0; i3 < tmpInputV->size(); i3++)
					{
						patternBuf tmpBuf;
						tmpBuf.name = tmpInputV->at(i3).source;
						tmpBuf.pos = tmpInputI->at(i2).offset;
						tmpBuf.semantic = tmpInputV->at(i3).semantic;
						patBufI.push_back(tmpBuf);
					}
				}
				else
				{
					patternBuf tmpBuf;
					tmpBuf.name = tmpInputI->at(i2).source;
					tmpBuf.pos = tmpInputI->at(i2).offset;
					tmpBuf.semantic = tmpInputI->at(i2).semantic;
					patBufI.push_back(tmpBuf);
				}
				if (tmpInputI->at(i2).offset > sizeStack) sizeStack = tmpInputI->at(i2).offset;
			}
			for (UINT i = 0; i < patBufI.size(); i++)
			{
				if (patBufI.at(i).pos > sizeStackAll) sizeStackAll++;
			}
			sizeStackAll++;


			std::vector<std::vector<float>> *masIndeces = &tmpIndexData->at(indexDI).dataIndexes; //ћассив по VCount ’ран€щий указатели на данные
			for (UINT indexPoly = 0; indexPoly < masIndeces->size(); indexPoly++) // ÷икл прогон€ет указатели на данные каждного полигона
			{

				std::vector<std::vector<float>>* tmpDataI = &tmpIndexData->at(indexDI).dataIndexes;
				std::vector<float>* iDataPoly = &masIndeces->at(indexPoly); //’ранит все указатели дл€ построени€ одного полигона 
				for (UINT indexVert = 0; indexVert < iDataPoly->size() / (sizeStack + 1); indexVert++)
				{
					//3 раза дл€ одного вертекса
					BUFFER tmpBuffer;
					int PosVert = 0;

					for (UINT indexElem = 0; indexElem < patBufI.size(); indexElem++)
					{
						DWORD tmpPos = (DWORD)tmpDataI->at(indexPoly).at((sizeStackAll * indexVert) + patBufI.at(indexElem).pos); //indexPoly ”казывает на набор точек 1 полигона; (patBufI.size() * indexVert) —мещает на количество значений одной точки ; patBufI.at(indexElem).pos смещает на номер элемента
						if (patBufI.at(indexElem).semantic == "POSITION")
						{
							PosVert = (int)iDataPoly->at(indexVert * (sizeStack + 1));

							tmpBuffer.x = getArrElem(&tmpObjects->at(indexObj), patBufI.at(indexElem).name, tmpPos, 1);
							tmpBuffer.y = getArrElem(&tmpObjects->at(indexObj), patBufI.at(indexElem).name, tmpPos, 2);
							tmpBuffer.z = getArrElem(&tmpObjects->at(indexObj), patBufI.at(indexElem).name, tmpPos, 3);
						}
						if (patBufI.at(indexElem).semantic == "NORMAL")
						{
							tmpBuffer.nx = getArrElem(&tmpObjects->at(indexObj), patBufI.at(indexElem).name, tmpPos, 1);
							tmpBuffer.ny = getArrElem(&tmpObjects->at(indexObj), patBufI.at(indexElem).name, tmpPos, 2);
							tmpBuffer.nz = getArrElem(&tmpObjects->at(indexObj), patBufI.at(indexElem).name, tmpPos, 3);
						}
						if (patBufI.at(indexElem).semantic == "TEXCOORD")
						{
							tmpBuffer.tu = getArrElem(&tmpObjects->at(indexObj), patBufI.at(indexElem).name, tmpPos, 1);
							tmpBuffer.tv = getArrElem(&tmpObjects->at(indexObj), patBufI.at(indexElem).name, tmpPos, 2);
						}
					}
					if (!data->pControllers.empty())
					{
						DWORD tmpPos = (DWORD)tmpDataI->at(indexPoly).at((sizeStackAll * indexVert));
						setWeight(data, tmpPos, &tmpBuffer, tmpObjects->at(indexObj).id);
					}
					VB_data[PosVert] = tmpBuffer;
					IB_data.push_back(PosVert);
					if (VB_data_o.size() > VB_data_o.max_size() - 10)
					{
						VB_data_o.push_back(tmpBuffer);
					}
					VB_data_o.push_back(tmpBuffer);
				}
			}
		}
		VOID* pBV;
		tmpBufObj->sizeVB = countVertMas;
		pDevice->CreateVertexBuffer(countVertMas * sizeof(BUFFER), 0, NULL, D3DPOOL_MANAGED, &tmpBufObj->p_vb, NULL);
		tmpBufObj->p_vb->Lock(0, 0, (void**)&pBV, 0);
		memcpy(pBV, VB_data, countVertMas * sizeof(BUFFER));
		tmpBufObj->p_vb->Unlock();

		tmpBufObj->sizeVB_only = VB_data_o.size();
		pDevice->CreateVertexBuffer(VB_data_o.size() * sizeof(BUFFER), 0, NULL, D3DPOOL_MANAGED, &tmpBufObj->p_vb_only, NULL);
		tmpBufObj->p_vb_only->Lock(0, 0, (void**)&pBV, 0);
		memcpy(pBV, VB_data_o.data(), VB_data_o.size() * sizeof(BUFFER));
		tmpBufObj->p_vb_only->Unlock();

		tmpBufObj->sizeIB = IB_data.size();
		pDevice->CreateIndexBuffer(IB_data.size() * sizeof(WORD), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &tmpBufObj->p_ib, NULL);
		VOID* pBI;
		tmpBufObj->p_ib->Lock(0, 0, (void**)&pBI, 0);
		memcpy(pBI, IB_data.data(), IB_data.size() * sizeof(WORD));
		tmpBufObj->p_ib->Unlock();

		tmpBufObj->rawVectorData = VB_data_o;
		delete[] VB_data;
		data->pBuffer.push_back(tmpBufObj);
	}
}
void	parserSkel(std::fstream *file_data, std::string tmpStr, bone *pPrntBone)
{
	bone *tmpSkel;										//
	if (pPrntBone->nameBone != "Root")					// ѕроверка на существование родител€
		tmpSkel = new bone();							// ≈сли родитель пуст то мен€ем указатель времнной кости на указатель на родител€.
	else												// »наче заполн€ем временную кость
		tmpSkel = pPrntBone;							//

	tmpSkel->nameBone = getParameter(tmpStr, "name");
	tmpSkel->idBone = getParameter(tmpStr, "id");
	tmpSkel->sid = getParameter(tmpStr, "sid");

	if (pPrntBone->pChildBone == NULL && pPrntBone != tmpSkel) //ƒоп проверка на равенство указателей временной кости и родителс€кой 
	{
		pPrntBone->pChildBone = tmpSkel;
		tmpSkel->pParentBone = pPrntBone;
	}
	else if (pPrntBone != tmpSkel)
	{
		bone * lSib = pPrntBone->pChildBone->findLSib();
		lSib->pSiblingBone = tmpSkel;
		tmpSkel->pParentBone = pPrntBone;
	}



	while (!std::strstr(tmpStr.c_str(), "</node>"))
	{
		std::getline(*file_data, tmpStr);
		if (std::strstr(tmpStr.c_str(), "<node ") && !std::strstr(tmpStr.c_str(), "/>") && getParameter(tmpStr, "type") == "JOINT")
		{
			parserSkel(file_data, tmpStr, tmpSkel);
		}
		if (std::strstr(tmpStr.c_str(), "<matrix"))
		{
			std::vector<float> tmpVec;
			getArray(tmpStr, &tmpVec);
			if (!tmpVec.empty())
			{
				tmpSkel->localMatrix = (D3DXMATRIX)tmpVec.data();
			}
		}
	}
}
std::vector<bone*> InitMasBoneForShader(model * pModel, std::string objId)
{
	std::vector<bone*> tmpMasMat;
	for (UINT i = 0; i < pModel->pControllers.size(); i++)
	{
		if (pModel->pControllers.at(i).sourceGeom == objId)
		{
			for (UINT j = 0; j < pModel->pControllers.at(i).data.size(); j++)
			{
				if (pModel->pControllers.at(i).data.at(j).type == "JOINT")
				{
					std::vector<std::string> *tmpMas = &pModel->pControllers.at(i).data.at(j).dataName;
					for (UINT k = 0; k < tmpMas->size(); k++)
					{
						bone *tmpMatrix = NULL;
						for (UINT y = 0; y < pModel->pNodes.size(); y++)
						{
							if (pModel->pNodes.at(y).pRootBone != NULL)
							{
								tmpMatrix = getPointerToBone(pModel->pNodes.at(y).pRootBone, tmpMas->at(k));
								break;
							}
						}
						if (tmpMatrix != NULL)
							tmpMasMat.push_back(tmpMatrix);
					}
					return tmpMasMat;
				}
			}
		}
	}
	tmpMasMat.clear();
	return tmpMasMat;
}
void setInverseMatrix(model *pModel, bone *pBone)
{
	pBone->mInversePos = getStockBone(pBone->sid, pModel);

	if (pBone->pChildBone != NULL)
	{
		setInverseMatrix(pModel, pBone->pChildBone);
	}
	if (pBone->pSiblingBone != NULL)
	{
		setInverseMatrix(pModel, pBone->pSiblingBone);
	}
}
D3DXMATRIX getStockBone(std::string sid, model * pModel)
{
	D3DXMATRIX tmpMatrix;
	for (UINT i = 0; i < pModel->pControllers.size(); i++)
	{
		for (UINT j = 0; j < pModel->pControllers.at(i).data.size(); j++)
		{
			if (pModel->pControllers.at(i).data.at(j).type == "JOINT")
			{
				std::vector<std::string> tmpMas = pModel->pControllers.at(i).data.at(j).dataName;
				for (UINT k = 0; k < tmpMas.size(); k++)
				{
					if (tmpMas.at(k) == sid)
					{
						for (UINT d = 0; d < pModel->pControllers.at(i).data.size(); d++)
						{
							if (pModel->pControllers.at(i).data.at(d).type == "TRANSFORM")
							{
								std::vector<float> tmpMasFloat;
								for (UINT g = 0; g < 16; g++)
									tmpMasFloat.push_back(pModel->pControllers.at(i).data.at(d).data.at(k * 16 + g));
								tmpMatrix = tmpMasFloat.data();
								return tmpMatrix;
							}
						}
					}
				}
			}
		}
	}
	D3DXMatrixIdentity(&tmpMatrix);
	return tmpMatrix;
}
bone* getPointerToBone(bone* pSkelet, std::string name)
{
	if (pSkelet->sid == name)
		return pSkelet;
	bone* tmpBone;
	if (pSkelet->pChildBone != NULL)
	{
		tmpBone = getPointerToBone(pSkelet->pChildBone, name);
		if (tmpBone != NULL) return tmpBone;
	}
	if (pSkelet->pSiblingBone != NULL)
	{
		tmpBone = getPointerToBone(pSkelet->pSiblingBone, name);
		if (tmpBone != NULL) return tmpBone;
	}
	return NULL;
}

bool pick(object *obj)
{
	trans *tmpTrans = &obj->SettingsTransform;


	D3DXMATRIX wMatrix;
	D3DXMatrixIdentity(&wMatrix);

	object *pTmpObj = obj;
	while (pTmpObj != NULL)
	{
		trans *tmpTrans = &pTmpObj->SettingsTransform;
		D3DXMATRIX locWMAtrix;
		D3DXMatrixIdentity(&locWMAtrix);
		if (tmpTrans->state == TRUE)
			locWMAtrix = transform(tmpTrans->angleX, tmpTrans->angleY, tmpTrans->angleZ, tmpTrans->x,
				tmpTrans->y, tmpTrans->z, tmpTrans->size);

		D3DXMatrixMultiply(&wMatrix, &wMatrix, &locWMAtrix);
		pTmpObj = pTmpObj->pParentObject;
	}

	LPDIRECT3DDEVICE9 pDevice = manager.getDevice();
	CAMERA *cam = manager.getCamSetting();
	
	D3DXMATRIX pMatrixP = cam->pMatrix;
	D3DXMATRIX pMatrixWV = wMatrix * cam->vMatrix;

	POINT ptCursor;
	RECT pRec;
	GetWindowRect(manager.getHWnd(), &pRec);
	GetCursorPos(&ptCursor);
	int TopBord = GetSystemMetrics(SM_CYSIZE);
	int BorderH = GetSystemMetrics(SM_CYBORDER);
	int BorderW = GetSystemMetrics(SM_CXBORDER);

	WINDOWINFO pwi;
	GetWindowInfo(manager.getHWnd(), &pwi);
	int width = pwi.rcClient.right - pwi.rcClient.left;
	int height = pwi.rcClient.bottom - pwi.rcClient.top; // Ќе тестил 

	D3DXVECTOR3 v;
	if (manager.getGParameters().Windowed == TRUE)
	{
		v.x = (((2.0f * (ptCursor.x - pwi.rcClient.left)) / width) - 1) / pMatrixP._11;
		v.y = -(((2.0f * (ptCursor.y - pwi.rcClient.top)) / height) - 1) / pMatrixP._22;
		v.z = 1.0f;
	}
	else
	{
		v.x = (((2.0f * (ptCursor.x)) / manager.getGParameters().BackBufferWidth) - 1) / pMatrixP._11;
		v.y = -(((2.0f * (ptCursor.y)) / manager.getGParameters().BackBufferHeight) - 1) / pMatrixP._22;
		v.z = 1.0f;
	}
	D3DXMATRIX mMatrix;
	D3DXMatrixInverse(&mMatrix, NULL, &pMatrixWV);
	D3DXVECTOR3 vPickDir, vPickOrign;

	vPickDir.x = v.x * mMatrix._11 + v.y * mMatrix._21 + v.z * mMatrix._31;
	vPickDir.y = v.x * mMatrix._12 + v.y * mMatrix._22 + v.z * mMatrix._32;
	vPickDir.z = v.x * mMatrix._13 + v.y * mMatrix._23 + v.z * mMatrix._33;
	vPickOrign.x = mMatrix._41;
	vPickOrign.y = mMatrix._42;
	vPickOrign.z = mMatrix._43;

	D3DXVECTOR3 pt1, pt2, pt3;

	for (UINT j = 0; j < obj->mesh->rawVectorData.size(); j += 3)
	{
		pt1 = D3DXVECTOR3(obj->mesh->rawVectorData.at(j).x, obj->mesh->rawVectorData.at(j).y, obj->mesh->rawVectorData.at(j).z);
		pt2 = D3DXVECTOR3(obj->mesh->rawVectorData.at(j + 1).x, obj->mesh->rawVectorData.at(j + 1).y, obj->mesh->rawVectorData.at(j + 1).z);
		pt3 = D3DXVECTOR3(obj->mesh->rawVectorData.at(j + 2).x, obj->mesh->rawVectorData.at(j + 2).y, obj->mesh->rawVectorData.at(j + 2).z);
		if (D3DXIntersectTri(&pt1, &pt2, &pt3, &vPickOrign, &vPickDir, NULL, NULL, NULL))
		{
			return TRUE;
		}
	}
	return FALSE;
}
bool pick(object *obj, D3DXVECTOR3 vecBegin, D3DXVECTOR3 vecEnd)
{

	CAMERA *cam = manager.getCamSetting();

	object *pTmpObj = obj;
	D3DXMatrixIdentity(&cam->wMatrix);
	std::vector<D3DXMATRIX> masMtx;
	while (pTmpObj != NULL)
	{
		trans *tmpTrans = &pTmpObj->SettingsTransform;
		D3DXMATRIX locWMAtrix;
		D3DXMatrixIdentity(&locWMAtrix);
		if (tmpTrans->state == TRUE)
			locWMAtrix = transform(tmpTrans->angleX, tmpTrans->angleY, tmpTrans->angleZ, tmpTrans->x,
				tmpTrans->y, tmpTrans->z, tmpTrans->size);

		masMtx.insert(masMtx.begin(), locWMAtrix);
		pTmpObj = pTmpObj->pParentObject;

	}
	for(UINT i = 0; i < masMtx.size(); i++)
		D3DXMatrixMultiply(&cam->wMatrix, &masMtx.at(i), &cam->wMatrix);

	D3DXVECTOR3 pt1, pt2, pt3;
	for (UINT j = 0; j < obj->mesh->rawVectorData.size(); j += 3)
	{

		D3DXVec3TransformCoord(&pt1, &D3DXVECTOR3(obj->mesh->rawVectorData.at(j).x, obj->mesh->rawVectorData.at(j).y, obj->mesh->rawVectorData.at(j).z), &cam->wMatrix);
		D3DXVec3TransformCoord(&pt2, &D3DXVECTOR3(obj->mesh->rawVectorData.at(j + 1).x, obj->mesh->rawVectorData.at(j + 1).y, obj->mesh->rawVectorData.at(j + 1).z), &cam->wMatrix);
		D3DXVec3TransformCoord(&pt3, &D3DXVECTOR3(obj->mesh->rawVectorData.at(j + 2).x, obj->mesh->rawVectorData.at(j + 2).y, obj->mesh->rawVectorData.at(j + 2).z), &cam->wMatrix);
		
		if (D3DXIntersectTri(&pt1, &pt2, &pt3, &vecBegin, &vecEnd, NULL, NULL, NULL))
		{
			return TRUE;
		}
	}
	return FALSE;
}
void updateSkelet(object *pObj, bone* pBone)
{
	bool ent = false;
	model *pModel = (model*)resources.getObject(pObj->nameModel, OT_MODEL);
	DWORD ThisTime = timeGetTime();
	float time = 0;
	if (pObj->SettingsAnimation.timeLast != 0)
		time = (float)(ThisTime - pObj->SettingsAnimation.timeLast);
	time *= pObj->SettingsAnimation.speed;


	if (pObj->SettingsAnimation.activate == TRUE) 
	{
		if(pObj->SettingsAnimation.direction == TRUE)
			pObj->SettingsAnimation.timeCounter += (DWORD)time;
		else
			pObj->SettingsAnimation.timeCounter -= (DWORD)time;
	}
		
	for (UINT j = 0; j < pModel->pAnimation.size(); j++)
	{
		if (pModel->pAnimation.at(j).pDataMatrix.empty()) return;
		if (pModel->pAnimation.at(j).nameObj == pBone->sid) //»щет все запущенные анимации вли€ющие на кость
		{
			ent = true;
			std::vector<float>* masTime = &pModel->pAnimation.at(j).pDataTime;
			if (pObj->SettingsAnimation.endTime == -1)pObj->SettingsAnimation.endTime = (DWORD)(masTime->back() * 1000);
			if (pObj->SettingsAnimation.startTime == -1)pObj->SettingsAnimation.startTime = (DWORD)(masTime->at(0) * 1000);

			if ((pObj->SettingsAnimation.timeCounter > masTime->back() * 1000 || pObj->SettingsAnimation.timeCounter > pObj->SettingsAnimation.endTime )
				&& pObj->SettingsAnimation.direction == TRUE)
			{
				pObj->SettingsAnimation.timeCounter = 0;
				pObj->SettingsAnimation.timeLast = 0;
				if (pObj->SettingsAnimation.loop == FALSE)
					pObj->SettingsAnimation.activate = false;
			}
			if ((pObj->SettingsAnimation.timeCounter < 0 || pObj->SettingsAnimation.timeCounter < pObj->SettingsAnimation.startTime) 
				&& pObj->SettingsAnimation.direction == FALSE)
			{
				pObj->SettingsAnimation.timeCounter = pObj->SettingsAnimation.endTime;
				pObj->SettingsAnimation.timeLast = 0;
				if (pObj->SettingsAnimation.loop == FALSE)
					pObj->SettingsAnimation.activate = false;
			}
			//”знать между какими двум€ матрицами это врем€ находитс€ 
			DWORD PosAnim = 1;
			PosAnim = getPosAnimation(&pModel->pAnimation.at(j), (float)pObj->SettingsAnimation.timeCounter); //¬озвращает количество полностью пройденных анимациий + неполностью выполненную
			float timeCombine = (masTime->at(PosAnim) - masTime->at(PosAnim - 1)) * 1000;

			//–асчитываем врем€ не полного перехода 
			float timeNow = 0;
			if (pObj->SettingsAnimation.timeCounter != 0)
				timeNow = pObj->SettingsAnimation.timeCounter - (masTime->at(PosAnim - 1) * 1000);

			//расчитать преобразованную матрицу от времени
			pBone->mIntrpolate = getMatrixOfTime(	pModel->pAnimation.at(j).pDataMatrix.at(PosAnim - 1),
													pModel->pAnimation.at(j).pDataMatrix.at(PosAnim), timeCombine, timeNow);

			//ѕотом умножаетс€ на InverseBindMatrix
			pObj->SettingsAnimation.timeLast = timeGetTime();
		}
	}

	if (pBone->pParentBone != NULL)
	{
		if (!ent)
			D3DXMatrixMultiply(&pBone->mFinal, &pBone->pParentBone->mFinal, &pBone->localMatrix);
		else
			D3DXMatrixMultiply(&pBone->mFinal, &pBone->pParentBone->mFinal, &pBone->mIntrpolate);
	}
	else
	{
		if (!ent)
			pBone->mFinal = pBone->localMatrix;
		else
			pBone->mFinal = pBone->mIntrpolate;
	}
	if (pBone->pChildBone != NULL)
	{
		updateSkelet(pObj, pBone->pChildBone);
	}
	if (pBone->pSiblingBone != NULL)
	{
		updateSkelet(pObj, pBone->pSiblingBone);
	}
}
/*void updateFrame(object *pObj)
{
	DWORD ThisTime = timeGetTime();
	float time = 0;
	if (pObj->SettingsAnimation.timeLast != 0)
		time = (float)(ThisTime - pObj->SettingsAnimation.timeLast);
	if (time != 0)
		time *= pObj->SettingsAnimation.speed;// -0.94f; // вариант изменени€ скрости

	pObj->SettingsAnimation.timeCounter += time;
	for (UINT j = 0; j < pObj->pModel->pAnimation.size(); j++)
	{
		if (pObj->pModel->pAnimation.at(j).pDataFloat.empty()) return;
		std::vector<float>* masTime = &pObj->pModel->pAnimation.at(j).pDataTime;
		if (pObj->SettingsAnimation.timeCounter > masTime->back() * 1000)
		{
			pObj->SettingsAnimation.timeCounter = 0;
			pObj->SettingsAnimation.timeLast = 0;
			if (!pObj->SettingsAnimation.loop)
				pObj->SettingsAnimation.activate = false;
		}
		DWORD PosAnim = 0;
		PosAnim = getPosAnimation(&pObj->pModel->pAnimation.at(j), pObj->SettingsAnimation.timeCounter); //¬озвращает количество полностью пройденных анимациий + неполностью выполненную
		float timeCombine = (masTime->at(PosAnim) * 1000 - masTime->at(PosAnim - 1) * 1000);

		//–асчитываем врем€ не полного перехода 
		float timeNow = 0;
		if (pObj->SettingsAnimation.timeCounter != 0)
			timeNow = (pObj->SettingsAnimation.timeCounter - masTime->at(PosAnim - 1) * 1000);

		float val = pObj->pModel->pAnimation.at(j).pDataFloat.at(PosAnim - 1) +
			(pObj->pModel->pAnimation.at(j).pDataFloat.at(PosAnim) - pObj->pModel->pAnimation.at(j).pDataFloat.at(PosAnim - 1)) / timeCombine * timeNow;

		if (pObj->pModel->pAnimation.at(j).typeAnim == "location.X") pObj->SettingsAnimation.PRS.x = val;
		if (pObj->pModel->pAnimation.at(j).typeAnim == "location.Y") pObj->SettingsAnimation.PRS.y = val;
		if (pObj->pModel->pAnimation.at(j).typeAnim == "location.Z") pObj->SettingsAnimation.PRS.z = val;
		if (pObj->pModel->pAnimation.at(j).typeAnim == "rotationX.ANGLE") pObj->SettingsAnimation.PRS.angleX = val;
		if (pObj->pModel->pAnimation.at(j).typeAnim == "rotationY.ANGLE") pObj->SettingsAnimation.PRS.angleY = val;
		if (pObj->pModel->pAnimation.at(j).typeAnim == "rotationZ.ANGLE") pObj->SettingsAnimation.PRS.angleZ = val;
	}
	pObj->SettingsAnimation.timeLast = timeGetTime();
}*/
std::vector<D3DXMATRIX> *collectMasMatrix(bufferObj *pBuffer)
{
	if (pBuffer->pMasBoneForShader.empty()) return NULL; //все врем€ выходит
	std::vector<D3DXMATRIX> *ResultMas = new std::vector<D3DXMATRIX>();
	D3DXMATRIX tmpMasMatrix;
	for (UINT i = 0; i < pBuffer->pMasBoneForShader.size(); i++)
	{
		D3DXMatrixMultiplyTranspose(&tmpMasMatrix, &pBuffer->pMasBoneForShader.at(i)->mFinal, &pBuffer->pMasBoneForShader.at(i)->mInversePos);
		ResultMas->push_back(tmpMasMatrix);
		D3DXMatrixIdentity(&tmpMasMatrix);
	}
	return ResultMas;
}
D3DXMATRIX getMatrixOfTime(D3DXMATRIX matrixBegin, D3DXMATRIX matrixEnd, float time, float timeNow)
{
	return matrixBegin + (matrixEnd - matrixBegin) / time * timeNow;
}
int getPosAnimation(animation* pAnim, float time)
{
	for (UINT i = 0; i < pAnim->pDataTime.size(); i++)
	{
		if ((pAnim->pDataTime.at(i) * 1000) > time && i < pAnim->pDataTime.size())
		{
			if(i != 0)
				return i;
		}
	}
	return 1;
}

int listen(std::string *tmpStr)
{
	MSG *msg = manager.getMessage();
	if (msg->message == WM_CHAR)
	{
		if (msg->wParam == 0x08 && tmpStr->size() != 0) tmpStr->pop_back(); //backspace
		if (msg->wParam == 0x0D && tmpStr->size() != 0) { return 1; } //Enter
		else if (msg->wParam != 0x08 && msg->wParam != 0x0D)
		{
			tmpStr->push_back(msg->wParam);
		}
	}
	if (msg->message == WM_KEYDOWN)
	{
	}
	return 0;
}

bufferObj* createRndPanel(D3DXVECTOR2 pt)
{
	POINT tmpPt;
	tmpPt.x = 0;
	tmpPt.y = 0;


	std::vector<BUFFER> tmpBuffer;
	BUFFER tmp;

	tmp.x = (-pt.x / 2) + tmpPt.x;
	tmp.y = 0.0f;
	tmp.z = (-pt.y / 2) + tmpPt.y;
	tmp.tv = 0;
	tmp.tu = 0;
	tmpBuffer.push_back(tmp);

	tmp.x = (pt.x / 2) + tmpPt.x;
	tmp.y = 0.0f;;
	tmp.z = (-pt.y / 2) + tmpPt.y;
	tmp.tv = 0;
	tmp.tu = 1;
	tmpBuffer.push_back(tmp);

	tmp.x = (pt.x / 2) + tmpPt.x;
	tmp.y = 0.0f;
	tmp.z = (pt.y / 2) + tmpPt.y;
	tmp.tv = 1;
	tmp.tu = 1;
	tmpBuffer.push_back(tmp);


	tmp.x = (-pt.x / 2) + tmpPt.x;
	tmp.y = 0.0f;
	tmp.z = (pt.y / 2) + tmpPt.y;
	tmp.tv = 1;
	tmp.tu = 0;
	tmpBuffer.push_back(tmp);

	tmp.x = (-pt.x / 2) + tmpPt.x;
	tmp.y = 0.0f;
	tmp.z = (-pt.y / 2) + tmpPt.y;
	tmp.tv = 0;
	tmp.tu = 0;
	tmpBuffer.push_back(tmp);


	tmp.x = (pt.x / 2) + tmpPt.x;
	tmp.y = 0.0f;
	tmp.z = (pt.y / 2) + tmpPt.y;
	tmp.tv = 1;
	tmp.tu = 1;
	tmpBuffer.push_back(tmp);



	bufferObj *tmpBO = new bufferObj();
	tmpBO->idObj = "NONE";
	tmpBO->nameObj = "Panel";
	tmpBO->sizeVB_only = tmpBuffer.size();

	VOID* pBV;
	LPDIRECT3DDEVICE9 pDevice = manager.getDevice();
	pDevice->CreateVertexBuffer(tmpBO->sizeVB_only * sizeof(BUFFER), 0, NULL, D3DPOOL_MANAGED, &tmpBO->p_vb_only, NULL);
	tmpBO->p_vb_only->Lock(0, 0, (void**)&pBV, 0);
	memcpy(pBV, tmpBuffer.data(), tmpBO->sizeVB_only * sizeof(BUFFER));
	tmpBO->p_vb_only->Unlock();
	tmpBO->p_vb_only;
	tmpBO->rawVectorData = tmpBuffer;
	return tmpBO;
}