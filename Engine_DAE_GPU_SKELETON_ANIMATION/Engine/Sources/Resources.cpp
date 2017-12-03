#include "Resources.h"


resources::resources()
{
	pTracks = new std::vector<track>();
	pModels = new std::vector<model>();
	pTextures = new std::vector<texture>();
	pShadersV = new std::vector<shaderV>();
	pShadersP = new std::vector<shaderP>();
}
resources::~resources()
{
}

void resources::parserSkel(std::fstream	*file_data, skeletData *pPrntSkelet, D3DXMATRIX *mParent, bool state)
{
	std::string tmpStr = "";
	std::getline(*file_data, tmpStr);
	int entrance = 1;
	skeletData *perElement = new skeletData();
	while (!std::strstr(tmpStr.c_str(), "</node>") && entrance == 1)
	{
		skeletData *tmpSkel = new skeletData();
		if (std::strstr(tmpStr.c_str(), "<node ") && !std::strstr(tmpStr.c_str(), "/>") && getParam(tmpStr, "type") == "JOINT" && entrance == 1)
		{

			tmpSkel->nameBone = getParam(tmpStr, "name");
			tmpSkel->idBone = getParam(tmpStr, "id");
			tmpSkel->sid = getParam(tmpStr, "sid");

			std::getline(*file_data, tmpStr);
			std::vector<float> tmpMatrixData;
			getArray(tmpStr, &tmpMatrixData);
			tmpSkel->localMatrix = (D3DXMATRIX)tmpMatrixData.data();
			if (state)
			{
				tmpSkel->pParentData = pPrntSkelet->pChildData->pParentData;

				if (perElement == NULL)
				{
					pPrntSkelet->pSiblingData = tmpSkel;
					if (pPrntSkelet->pSiblingData->pChildData != NULL)
						parserSkel(file_data, pPrntSkelet->pSiblingData, &tmpSkel->localMatrix, 1);
					else
						parserSkel(file_data, pPrntSkelet->pSiblingData, &tmpSkel->localMatrix, 0);
				}
				else
				{
					perElement->pSiblingData = tmpSkel;
					if (perElement->pSiblingData->pChildData != NULL)
						parserSkel(file_data, perElement->pSiblingData, &tmpSkel->localMatrix, 1);
					else
						parserSkel(file_data, perElement->pSiblingData, &tmpSkel->localMatrix, 0);
				}
			}
			else
			{
				tmpSkel->pParentData = pPrntSkelet;
				pPrntSkelet->pChildData = tmpSkel;
				if (pPrntSkelet->pChildData->pChildData != NULL)
					parserSkel(file_data, pPrntSkelet->pChildData, &tmpSkel->localMatrix, 1);
				else
					parserSkel(file_data, pPrntSkelet->pChildData, &tmpSkel->localMatrix, 0);
				state = 1;
			}
			perElement = tmpSkel;

			//D3DXMatrixMultiply(&tmpSkel.localMatrix, mParent, (D3DXMATRIX*)tmpMatrixData.data());
			//pPrntSkelet->pChildData->push_back(tmpSkel);
		}
		if (std::strstr(tmpStr.c_str(), "<node "))entrance++;
		else if (std::strstr(tmpStr.c_str(), "</node>"))entrance--;
		std::getline(*file_data, tmpStr);

	}
}

HRESULT resources::setTexture(std::string modelName, std::string objName, std::string textureName)
{
	model *tmp = (model*)getObject(modelName, OT_MODEL);
	if (tmp == NULL)return E_FAIL;
	for (unsigned int i = 0; i < tmp->pBuffer->size(); i++)
	{
		if (tmp->pBuffer->at(i).nameObj == objName)
		{
			texture *tmpT = (texture*)getObject(textureName, OT_TEXTURE);
			if (tmpT == NULL) return E_FAIL;
			tmp->pBuffer->at(i).pTexture = tmpT->data;
			return S_OK;
		}
	}
	return E_FAIL;
}
HRESULT resources::setShaderVert(std::string modelName, std::string objName, std::string shaderName)
{
	model *tmp = (model*)getObject(modelName, OT_MODEL);
	if (tmp == NULL)return E_FAIL;
	for (unsigned int i = 0; i < tmp->pBuffer->size(); i++)
	{
		if (tmp->pBuffer->at(i).nameObj == objName)
		{
			shaderV *tmpT = (shaderV*)getObject(shaderName, OT_VSHADER);
			if (tmpT == NULL) return E_FAIL;
			tmp->pBuffer->at(i).vShaderName = tmpT->vertShaderName;
			tmp->pBuffer->at(i).pVshader = tmpT->pVertShader;
			return S_OK;
		}
	}
	return E_FAIL;
}
HRESULT resources::setDecl(std::string modelName, std::string objName, D3DVERTEXELEMENT9 *pDecl, LPDIRECT3DDEVICE9 pDevice)
{
	model *tmp = (model*)getObject(modelName, OT_MODEL);
	if (tmp == NULL)return E_FAIL;
	for (unsigned int i = 0; i < tmp->pBuffer->size(); i++)
	{
		if (tmp->pBuffer->at(i).nameObj == objName)
		{
			pDevice->CreateVertexDeclaration(pDecl, &tmp->pBuffer->at(i).pDecl);
			return S_OK;
		}
	}
	return E_FAIL;
}
HRESULT resources::setShaderVal(void* data, std::string nameVar, std::string nameShader, LPDIRECT3DDEVICE9 pDevice, typeVar type)
{
	D3DXHANDLE  hConstData;
	shaderP *tmpP = (shaderP*)getObject(nameShader, OT_PSHADER);
	shaderV *tmpV = (shaderV*)getObject(nameShader, OT_VSHADER);
	if (tmpP != NULL)
	{
		hConstData = tmpP->pConstTablePix->GetConstantByName(0, nameVar.c_str());
		switch (type)
		{
		case TV_FLOAT:
			tmpP->pConstTablePix->SetFloat(pDevice, hConstData, (float&)data);
			break;
		case TV_FLOAT4X4:
			tmpP->pConstTablePix->SetMatrix(pDevice, hConstData, (D3DXMATRIX*)data);
			break;
		case TV_INT:
			tmpP->pConstTablePix->SetInt(pDevice, hConstData, (int&)data);
			break;
		}	
		return S_OK;
	}
	else if (tmpV != NULL)
	{
		hConstData = tmpV->pConstTableVert->GetConstantByName(0, nameVar.c_str());
		switch (type)
		{
			case TV_FLOAT:
				tmpV->pConstTableVert->SetFloat(pDevice, hConstData, (float&)data);
				break;
			case TV_FLOAT4X4:
			{
				tmpV->pConstTableVert->SetMatrix(pDevice, hConstData, (D3DXMATRIX*)data);
			}
			break;
			case TV_INT:
				tmpV->pConstTableVert->SetInt(pDevice, hConstData, (int&)data);
				break;
			case TV_MASFLOAT4X4:
			{
				std::vector<D3DXMATRIX> *tmpVec = (std::vector<D3DXMATRIX>*)data;
				tmpV->pConstTableVert->SetMatrixArray(pDevice, hConstData, tmpVec->data(), tmpVec->size());
			}

				break;
		}
		
		return S_OK;
	}
	
	return E_FAIL;
}
HRESULT resources::setShaderPix(std::string modelName, std::string objName, std::string shaderName)
{
	model *tmp = (model*)getObject(modelName, OT_MODEL);
	if (tmp == NULL)return E_FAIL;
	for (unsigned int i = 0; i < tmp->pBuffer->size(); i++)
	{
		if (tmp->pBuffer->at(i).nameObj == objName)
		{
			shaderP *tmpT = (shaderP*)getObject(shaderName, OT_PSHADER);
			if (tmpT == NULL) return E_FAIL;
			tmp->pBuffer->at(i).pShaderName = tmpT->pixShaderName;
			tmp->pBuffer->at(i).pPshader = tmpT->pPixShader;
			return S_OK;
		}
	}
	return E_FAIL;
}
void	resources::setWeight(model *data, UINT iPos, BUFFER *pBuffer, std::string idGeom)
{
	//Получили геометрию в которой нужно искать данные
	geometry* tmpGeom = NULL;
	for (unsigned int i2 = 0; i2 < data->pControllers->size(); i2++)
	{
		if (data->pControllers->at(i2).sourceGeom == idGeom)
		{
			tmpGeom = &data->pControllers->at(i2);
			break;
		}
	}
	//Проверка на существование данных
	if (tmpGeom == NULL) return;

	//Содержит сортированный по vcount массив индексов 
	std::vector<float>		*tmpArr				= &tmpGeom->indexData->at(0).dataIndexes->at(iPos);

	//Описание позиций в индексах (<v>)
	std::vector<input>		*tmpInputICtrl		= tmpGeom->indexData->at(0).inputSettings;

	//локальная позиция веса и индекса матрицы в массиве индексов
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
	//нормализация весов 
	float sum = pBuffer->weights[0] + pBuffer->weights[1] + pBuffer->weights[2] + pBuffer->weights[3];
	sum = 100 / sum;
	for (int i = 0; i < 4; i++)
	{
		pBuffer->weights[i] = (sum * pBuffer->weights[i]) / 100;
	}
}



HRESULT resources::loadObject(std::string name, flagObj flag, LPDIRECT3DDEVICE9 pDevice)
{

	switch (flag)
	{
	case OT_MODEL:
	{
		loadModel(name, pDevice);
	}
	break;
	case OT_TEXTURE:
	{
		loadTexture(name, pDevice);
	}
	break;
	case OT_MUSIC:
	{
		loadTrack(name);
	}
	break;
	case OT_PSHADER:
	{
		loadPixShaders(name, pDevice);
	}
	break;
	case OT_VSHADER:
	{
		loadVertShaders(name, pDevice);
	}
	break;
	case OT_DEFAULT:
		break;
	default:
		break;
	}
	return S_OK;
}
HRESULT resources::loadTrack(std::string nameFile)
{
	if (nameFile == "None")
		return E_FAIL;

	std::ifstream inFile(nameFile, std::ios::binary | std::ios::in);
	if (inFile.bad())
		return E_FAIL;

	DWORD dwChunkId = 0, dwFileSize = 0, dwChunkSize = 0, dwExtra = 0;
	track tmpTrack;

	tmpTrack.trackName = nameFile;

	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(XAudio2Create(&tmpTrack.gEngine, 0, XAUDIO2_DEFAULT_PROCESSOR)))
	{
		tmpTrack.gEngine->Release();
		return E_FAIL;
	}
	if (FAILED(tmpTrack.gEngine->CreateMasteringVoice(&tmpTrack.gMaster)))
	{
		tmpTrack.gEngine->Release();
		return E_FAIL;
	}

	//look for 'RIFF' chunk identifier
	inFile.seekg(0, std::ios::beg);
	inFile.read(reinterpret_cast<char*>(&dwChunkId), sizeof(dwChunkId));
	if (dwChunkId != 'FFIR')
	{
		inFile.close();
		return E_FAIL;
	}
	inFile.seekg(4, std::ios::beg); //get file size
	inFile.read(reinterpret_cast<char*>(&dwFileSize), sizeof(dwFileSize));
	if (dwFileSize <= 16)
	{
		inFile.close();
		return E_FAIL;
	}
	inFile.seekg(8, std::ios::beg); //get file format
	inFile.read(reinterpret_cast<char*>(&dwExtra), sizeof(dwExtra));
	if (dwExtra != 'EVAW')
	{
		inFile.close();
		return E_FAIL;
	}

	//look for 'fmt ' chunk id
	bool bFilledFormat = false;
	for (unsigned int i = 12; i < dwFileSize;)
	{
		inFile.seekg(i, std::ios::beg);
		inFile.read(reinterpret_cast<char*>(&dwChunkId), sizeof(dwChunkId));
		inFile.seekg(i + 4, std::ios::beg);
		inFile.read(reinterpret_cast<char*>(&dwChunkSize), sizeof(dwChunkSize));
		if (dwChunkId == ' tmf')
		{
			inFile.seekg(i + 8, std::ios::beg);
			inFile.read(reinterpret_cast<char*>(&(tmpTrack.mWF)), sizeof(tmpTrack.mWF));
			bFilledFormat = true;
			break;
		}
		dwChunkSize += 8; //add offsets of the chunk id, and chunk size data entries
		dwChunkSize += 1;
		dwChunkSize &= 0xfffffffe; //guarantees WORD padding alignment
		i += dwChunkSize;
	}
	if (!bFilledFormat)
	{
		inFile.close();
		return E_FAIL;
	}

	//look for 'data' chunk id
	BYTE* m_waveData;
	bool bFilledData = false;
	for (unsigned int i = 12; i < dwFileSize;)
	{
		inFile.seekg(i, std::ios::beg);
		inFile.read(reinterpret_cast<char*>(&dwChunkId), sizeof(dwChunkId));
		inFile.seekg(i + 4, std::ios::beg);
		inFile.read(reinterpret_cast<char*>(&dwChunkSize), sizeof(dwChunkSize));
		if (dwChunkId == 'atad')
		{
			m_waveData = new BYTE[dwChunkSize];
			inFile.seekg(i + 8, std::ios::beg);
			inFile.read(reinterpret_cast<char*>(m_waveData), dwChunkSize);
			tmpTrack.mXA.AudioBytes = dwChunkSize;
			tmpTrack.mXA.pAudioData = m_waveData;
			tmpTrack.mXA.PlayBegin = 0;
			tmpTrack.mXA.PlayLength = 0;
			bFilledData = true;
			break;
		}
		dwChunkSize += 8; //add offsets of the chunk id, and chunk size data entries
		dwChunkSize += 1;
		dwChunkSize &= 0xfffffffe; //guarantees WORD padding alignment
		i += dwChunkSize;
	}
	if (!bFilledData)
	{
		inFile.close();
		return E_FAIL;
	}

	if (FAILED(tmpTrack.gEngine->CreateSourceVoice(&(tmpTrack.gSource), &(tmpTrack.mWF))))return E_FAIL;
	pTracks->push_back(tmpTrack);

	inFile.close();
	return S_OK;
}
HRESULT resources::loadModel(std::string nameFile, LPDIRECT3DDEVICE9 pDevice)
{
	std::string tmpStr;
	if (nameFile.empty()) return E_FAIL;
	else
	{
		std::string		str_raw_data;
		std::fstream	file_data;
		model tmpModel;

		tmpModel.modelName = nameFile;
		file_data.open(nameFile);
		if (!file_data.is_open()) return E_FAIL;
		else
		{
			while (!file_data.eof())
			{
				std::getline(file_data, tmpStr);
				
				//не работает корректно <library_effects>
				if (std::strstr(tmpStr.c_str(), "<library_effects>"))
				{
					std::vector<float> tmpDataColor;
					while (!std::strstr(tmpStr.c_str(), "</library_effects>"))
					{
						if (std::strstr(tmpStr.c_str(), "<effect "))
						{
							effect tmpEffect;
							tmpEffect.idEffect = getParam(tmpStr, "id");
							while (!std::strstr(tmpStr.c_str(), "</effect>"))
							{
								if (std::strstr(tmpStr.c_str(), "<emission>"))
								{
									while (!std::strstr(tmpStr.c_str(), "</emission>"))
									{
										if (std::strstr(tmpStr.c_str(), "<color "))
										{
											tmpDataColor.clear();
											getArray(tmpStr, &tmpDataColor);
											if (tmpDataColor.size() == 4)
												tmpEffect.emission = { tmpDataColor.at(0),tmpDataColor.at(1),tmpDataColor.at(2),tmpDataColor.at(3) };
										}
										std::getline(file_data, tmpStr);
									}
								}
								if (std::strstr(tmpStr.c_str(), "<ambient>"))
								{
									while (!std::strstr(tmpStr.c_str(), "</ambient>"))
									{
										if (std::strstr(tmpStr.c_str(), "<color "))
										{
											tmpDataColor.clear();
											getArray(tmpStr, &tmpDataColor);
											if (tmpDataColor.size() == 4)
												tmpEffect.ambient = { tmpDataColor.at(0),tmpDataColor.at(1),tmpDataColor.at(2),tmpDataColor.at(3) };
										}
										std::getline(file_data, tmpStr);
									}
								}
								if (std::strstr(tmpStr.c_str(), "<diffuse>"))
								{
									while (!std::strstr(tmpStr.c_str(), "</diffuse>"))
									{
										if (std::strstr(tmpStr.c_str(), "<color "))
										{
											tmpDataColor.clear();
											getArray(tmpStr, &tmpDataColor);
											if (tmpDataColor.size() == 4)
												tmpEffect.diffuse = { tmpDataColor.at(0),tmpDataColor.at(1),tmpDataColor.at(2),tmpDataColor.at(3) };
										}
										std::getline(file_data, tmpStr);
									}
								}
								if (std::strstr(tmpStr.c_str(), "<index_of_refraction>"))
								{
									while (!std::strstr(tmpStr.c_str(), "</index_of_refraction>"))
									{
										if (std::strstr(tmpStr.c_str(), "<float "))
										{
											tmpDataColor.clear();
											getArray(tmpStr, &tmpDataColor);
											if (tmpDataColor.size() == 1)
												tmpEffect.indexOfRefraction = tmpDataColor.at(0);
										}
										std::getline(file_data, tmpStr);
									}
								}
								if (std::strstr(tmpStr.c_str(), "<shininess>"))
								{
									while (!std::strstr(tmpStr.c_str(), "</shininess>"))
									{
										if (std::strstr(tmpStr.c_str(), "<float "))
										{
											tmpDataColor.clear();
											getArray(tmpStr, &tmpDataColor);
											if (tmpDataColor.size() == 1)
												tmpEffect.shininess = tmpDataColor.at(0);
										}
										std::getline(file_data, tmpStr);
									}
								}
								std::getline(file_data, tmpStr);
							}
							tmpModel.pEffects->push_back(tmpEffect);
						}
						std::getline(file_data, tmpStr);
					}
				} 
				else if (std::strstr(tmpStr.c_str(), "<library_materials>"))
				{
					while (!std::strstr(tmpStr.c_str(), "</library_materials>"))
					{
						if (std::strstr(tmpStr.c_str(), "<material "))
						{
							material tmpMaterial;
							tmpMaterial.idMaterial = getParam(tmpStr, "id");
							tmpMaterial.nameMaterial = getParam(tmpStr, "name");

							while (!std::strstr(tmpStr.c_str(), "</material>"))
							{
								if (std::strstr(tmpStr.c_str(), "<instance_effect "))
								{
									tmpMaterial.instanceEffect = getParam(tmpStr, "url");
								}

								std::getline(file_data, tmpStr);
							}							
							tmpModel.pMaterials->push_back(tmpMaterial);
						}
						std::getline(file_data, tmpStr);
					}
				}
				else if (std::strstr(tmpStr.c_str(), "<library_animations>"))
				{
					while (!std::strstr(tmpStr.c_str(), "</library_animations>"))
					{
						if (std::strstr(tmpStr.c_str(), "<animation "))
						{
							animation tmpAnimation;
							tmpAnimation.nameAnimation = getParam(tmpStr, "id");

							while (!std::strstr(tmpStr.c_str(), "</animation>"))
							{
								if (std::strstr(tmpStr.c_str(), "<source "))
								{
									source tmpSource;
									tmpSource.sourceId = getParam(tmpStr, "id");
									while (!std::strstr(tmpStr.c_str(), "</source>"))
									{
										if (std::strstr(tmpStr.c_str(), "<float_array "))
										{
											tmpSource.arrayId = getParam(tmpStr, "id");
											getArray(tmpStr, &tmpSource.data);
										}
										if (std::strstr(tmpStr.c_str(), "<Name_array "))
										{
											tmpSource.arrayId = getParam(tmpStr, "id");
											getArrayName(tmpStr, &tmpSource.dataName);
										}
										if (std::strstr(tmpStr.c_str(), "<technique_common>"))
										{
											while (!std::strstr(tmpStr.c_str(), "</technique_common>"))
											{
												if (std::strstr(tmpStr.c_str(), "<accessor "))
												{
													tmpSource.count = std::stoi(getParam(tmpStr, "count"));
													tmpSource.stride = std::stoi(getParam(tmpStr, "stride"));
												}
												if (std::strstr(tmpStr.c_str(), "<param "))
												{
													tmpSource.type = getParam(tmpStr, "name");
													if (tmpSource.type == "TRANSFORM")
													{
														formatData2(&tmpSource, tmpAnimation.pDataMatrix);
													}
													if (tmpSource.type == "TIME")
													{
														tmpAnimation.pDataTime->assign(tmpSource.data.begin(), tmpSource.data.end());
													}
												}
												std::getline(file_data, tmpStr);
											}
										}
										std::getline(file_data, tmpStr);
									}
									tmpAnimation.pData->push_back(tmpSource);
								}
								if (std::strstr(tmpStr.c_str(), "<sampler "))
								{
									while (!std::strstr(tmpStr.c_str(), "</sampler>"))
									{
										if (std::strstr(tmpStr.c_str(), "<input "))
										{
											input tmpInput;
											tmpInput.semantic = getParam(tmpStr, "semantic");
											tmpInput.source = getParam(tmpStr, "source");
											tmpInput.offset = -1;
											tmpAnimation.pInputs->push_back(tmpInput);
										}
										std::getline(file_data, tmpStr);
									}
								}
								if (std::strstr(tmpStr.c_str(), "<channel "))
								{
									tmpAnimation.nameBone = formatNum1(&getParam(tmpStr, "target"));
								}
								std::getline(file_data, tmpStr);
							}
							tmpModel.pAnimation->push_back(tmpAnimation);
						}
						std::getline(file_data, tmpStr);
					}
				}
				else if (std::strstr(tmpStr.c_str(), "<library_images>"))
				{
					while (!std::strstr(tmpStr.c_str(), "</library_images>"))
					{
						if (std::strstr(tmpStr.c_str(), "<image "))
						{
							image tmpImage;
							tmpImage.nameImage = getParam(tmpStr, "id");
							while (!std::strstr(tmpStr.c_str(), "</image>"))
							{
								if (std::strstr(tmpStr.c_str(), "<init_from>"))
								{
									//tmpImage.filePath = getDataArr(tmpStr);
								}
								std::getline(file_data, tmpStr);
							}
						}
						std::getline(file_data, tmpStr);
					}
				}
				else if (std::strstr(tmpStr.c_str(), "<library_geometries>"))
				{
					while (!std::strstr(tmpStr.c_str(), "</library_geometries>"))
					{
						if (std::strstr(tmpStr.c_str(), "<geometry "))
						{
							geometry tmpGeom;
							tmpGeom.id = getParam(tmpStr, "id");
							tmpGeom.name = getParam(tmpStr, "name");
							while (!std::strstr(tmpStr.c_str(), "</geometry>"))
							{
								if (std::strstr(tmpStr.c_str(), "<source"))
								{
									source tmpSrc;
									tmpSrc.sourceId = getParam(tmpStr, "id");
									while (!std::strstr(tmpStr.c_str(), "</source>"))
									{
										if (std::strstr(tmpStr.c_str(), "<float_array "))
										{
											tmpSrc.arrayId= getParam(tmpStr, "id");
											getArray(tmpStr, &tmpSrc.data);
										}
										if (std::strstr(tmpStr.c_str(), "<accessor "))
										{
											tmpSrc.count = std::stoi(getParam(tmpStr, "count"));
											tmpSrc.stride = std::stoi(getParam(tmpStr, "stride"));
										}
										if (std::strstr(tmpStr.c_str(), "<param "))
										{
											if(tmpSrc.type.empty())
											tmpSrc.type = getParam(tmpStr, "type");
										}

										std::getline(file_data, tmpStr);
									}
									tmpGeom.data->push_back(tmpSrc);
								}								
								if (std::strstr(tmpStr.c_str(), "<vertices "))
								{
									tmpGeom.veticesData->verticesName = getParam(tmpStr, "id");

									while (!std::strstr(tmpStr.c_str(), "</vertices>"))
									{
										if (std::strstr(tmpStr.c_str(), "<input "))
										{
											input tmpInput;
											tmpInput.semantic = getParam(tmpStr, "semantic");
											tmpInput.source	= getParam(tmpStr, "source");

											tmpGeom.veticesData->inputSettings->push_back(tmpInput);
										}
										std::getline(file_data, tmpStr);
									}
								}
								if (std::strstr(tmpStr.c_str(), "<polylist ") || 
									std::strstr(tmpStr.c_str(), "<triangles "))
								{
									indexlist tmpPolylist;
									while ( !(std::strstr(tmpStr.c_str(), "</polylist>") ||
											std::strstr(tmpStr.c_str(), "</triangles>")) )
									{
										
										if (std::strstr(tmpStr.c_str(), "<vcount>"))
										{
											getArray(tmpStr, tmpPolylist.dataVcounts);
										}
										if (std::strstr(tmpStr.c_str(), "<input "))
										{
											input tmpIndexIn;

											tmpIndexIn.offset = std::stoi(getParam(tmpStr, "offset"));
											tmpIndexIn.semantic = getParam(tmpStr, "semantic");
											tmpIndexIn.source = getParam(tmpStr, "source");

											tmpPolylist.inputSettings->push_back(tmpIndexIn);
										}
										if (std::strstr(tmpStr.c_str(), "<p>"))
										{
											
											std::vector<float> tmpMas;

											getArray(tmpStr, &tmpMas);

											if (tmpPolylist.dataVcounts->empty())
											{
												int sizeStack = 0;
												for (UINT i2 = 0; i2 < tmpPolylist.inputSettings->size(); i2++)
												{
													if (tmpPolylist.inputSettings->at(i2).offset > sizeStack) sizeStack = tmpPolylist.inputSettings->at(i2).offset;
												}
												for (UINT i = 0; i < (tmpMas.size()) / (sizeStack + 1); i++)
													tmpPolylist.dataVcounts->push_back(sizeStack + 1.0f);
											}

											int shift = 0, offsetmax = 0;
											for (UINT i = 0; i < tmpPolylist.inputSettings->size(); i++)
											{
												if (tmpPolylist.inputSettings->at(i).offset > offsetmax) offsetmax = tmpPolylist.inputSettings->at(i).offset;
											}
											for (unsigned int i = 0; i < tmpPolylist.dataVcounts->size(); i++)
											{
												std::vector<float> tmpVectData;
												for (unsigned int j = 0; j < tmpPolylist.dataVcounts->at(i); j++)
												{
													for (int k = 0; k < offsetmax+1; k++)
													{
														tmpVectData.push_back(tmpMas.at(shift + k));
													}
													shift += (int)offsetmax + 1;
												}
												tmpPolylist.dataIndexes->push_back(tmpVectData);
											}
										}

										std::getline(file_data, tmpStr);
									}
									tmpGeom.indexData->push_back(tmpPolylist);
								}
						
								std::getline(file_data, tmpStr);
							}

							tmpModel.pObjects->push_back(tmpGeom);
						}
						std::getline(file_data, tmpStr);
					}
				}
				else if (std::strstr(tmpStr.c_str(), "<library_controllers>"))
				{
					while (!std::strstr(tmpStr.c_str(), "</library_controllers>"))
					{
						if (std::strstr(tmpStr.c_str(), "<controller "))
						{
							while (!std::strstr(tmpStr.c_str(), "</controller>"))
							{
								geometry tmpGeom;
								tmpGeom.id = getParam(tmpStr, "id");
								tmpGeom.name = getParam(tmpStr, "name");
								if (std::strstr(tmpStr.c_str(), "<skin "))
								{
									tmpGeom.sourceGeom = getParam(tmpStr, "source");
									while (!std::strstr(tmpStr.c_str(), "</skin>") )
									{
										if (std::strstr(tmpStr.c_str(), "<source"))
										{
											source tmpSrc;
											tmpSrc.sourceId = getParam(tmpStr, "id");
											while (!std::strstr(tmpStr.c_str(), "</source>"))
											{
												if (std::strstr(tmpStr.c_str(), "<float_array "))
												{
													tmpSrc.arrayId = getParam(tmpStr, "id");
													getArray(tmpStr, &tmpSrc.data);
												}
												if (std::strstr(tmpStr.c_str(), "<Name_array "))
												{
													tmpSrc.arrayId = getParam(tmpStr, "id");
													getArrayName(tmpStr, &tmpSrc.dataName);
												}
												if (std::strstr(tmpStr.c_str(), "<accessor "))
												{
													tmpSrc.count = std::stoi(getParam(tmpStr, "count"));
													tmpSrc.stride = std::stoi(getParam(tmpStr, "stride"));
												}
												if (std::strstr(tmpStr.c_str(), "<param "))
												{
													if (tmpSrc.type.empty())
														tmpSrc.type = getParam(tmpStr, "name");
												}

												std::getline(file_data, tmpStr);
											}

											tmpGeom.data->push_back(tmpSrc);
										}
										if (std::strstr(tmpStr.c_str(), "<vertex_weights "))
										{
											indexlist tmpWeightlist;
											while (!(std::strstr(tmpStr.c_str(), "</vertex_weights>")))
											{
												if (std::strstr(tmpStr.c_str(), "<input "))
												{
													input tmpIndexIn;

													tmpIndexIn.offset = std::stoi(getParam(tmpStr, "offset"));
													tmpIndexIn.semantic = getParam(tmpStr, "semantic");
													tmpIndexIn.source = getParam(tmpStr, "source");

													tmpWeightlist.inputSettings->push_back(tmpIndexIn);
												}
												if (std::strstr(tmpStr.c_str(), "<vcount>"))
												{
													getArray(tmpStr, tmpWeightlist.dataVcounts);
												}
												if (std::strstr(tmpStr.c_str(), "<v>"))
												{
													std::vector<float> tmpMas;

													getArray(tmpStr, &tmpMas);
													int shift = 0, offsetmax = 0;
													for (size_t i = 0; i < tmpWeightlist.inputSettings->size(); i++)
													{
														if (tmpWeightlist.inputSettings->at(i).offset > offsetmax) offsetmax = tmpWeightlist.inputSettings->at(i).offset;
													}

													for (unsigned int i = 0; i < tmpWeightlist.dataVcounts->size(); i++)
													{
														std::vector<float> tmpVectData;
														for (unsigned int j = 0; j < tmpWeightlist.dataVcounts->at(i); j++)
														{
															for (int k = 0; k < offsetmax + 1; k++)
															{
																tmpVectData.push_back(tmpMas.at(shift + k));
															}
															shift += (int)offsetmax + 1;
														}
														tmpWeightlist.dataIndexes->push_back(tmpVectData);
													}
												}
												std::getline(file_data, tmpStr);
											}
											tmpGeom.indexData->push_back(tmpWeightlist);
										}
										if (std::strstr(tmpStr.c_str(), "<joints>"))
										{
											//indexlist tmpPolylist;
											while (!(std::strstr(tmpStr.c_str(), "</joints>")))
											{
												std::vector<float> tmpArrInd;
												// связи 1 к 1
												std::getline(file_data, tmpStr);
											}
											//StmpGeom.indexData->push_back(tmpPolylist);
										}
										std::getline(file_data, tmpStr);
									}
									tmpModel.pControllers->push_back(tmpGeom);
								}
								std::getline(file_data, tmpStr);
							}
						}
						std::getline(file_data, tmpStr);
					}
				}
				else if (std::strstr(tmpStr.c_str(), "<library_visual_scenes>"))
				{
					std::streampos tmpPos = 0;
					while (!std::strstr(tmpStr.c_str(), "</library_visual_scenes>"))
					{
						if (std::strstr(tmpStr.c_str(), "<visual_scene "))
						{
							visualGeometry tmpVGeometry;
							tmpVGeometry.sceneId = getParam(tmpStr, "id");
							while (!std::strstr(tmpStr.c_str(), "</visual_scene>"))
							{
								if (std::strstr(tmpStr.c_str(), "<node ") && !std::strstr(tmpStr.c_str(), "/>") && getParam(tmpStr, "type") == "JOINT")
								{
									int entrance = 1;
									skeletData *tmpSkel = new skeletData();
									if (std::strstr(tmpStr.c_str(), "<node ") && !std::strstr(tmpStr.c_str(), "/>") && getParam(tmpStr, "type") == "JOINT" && entrance == 1 && tmpModel.pSkelet == NULL)
									{
										tmpSkel->pParentData = NULL;
										tmpSkel->nameBone = getParam(tmpStr, "name");
										tmpSkel->idBone = getParam(tmpStr, "id");
										tmpSkel->sid = getParam(tmpStr, "sid");

										std::getline(file_data, tmpStr);
										std::vector<float> tmpMatrixData;
										getArray(tmpStr, &tmpMatrixData);
										tmpSkel->localMatrix = tmpMatrixData.data();
										tmpModel.pSkelet = tmpSkel;
										parserSkel(&file_data, tmpModel.pSkelet, &tmpSkel->localMatrix, 0);
									}
									if (std::strstr(tmpStr.c_str(), "<node "))entrance++;
									else if (std::strstr(tmpStr.c_str(), "</node>"))entrance--;
								}
								if (std::strstr(tmpStr.c_str(), "<node ") && !std::strstr(tmpStr.c_str(), "/>") && getParam(tmpStr, "type") == "NODE")
								{
									//Записать presets моделей
								}
								tmpPos = file_data.tellg();
								std::getline(file_data, tmpStr);
							}
						}
						tmpPos = file_data.tellg();
						std::getline(file_data, tmpStr);
					}
				}
			}
			convertData(&tmpModel, pDevice);
			setInverseMatrix(&tmpModel, tmpModel.pSkelet);
			pModels->push_back(tmpModel);
			file_data.close();
		}
	}

	return S_OK;
}
HRESULT resources::loadTexture(std::string nameFile, LPDIRECT3DDEVICE9 pDevice)
{
	if (pDevice == NULL) return E_FAIL;
	texture tmpTexture;
	tmpTexture.textureName = nameFile.c_str();
	//D3DXCreateTextureFromFileEx(pDevice, nameFile.c_str(), 0, 0, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0xFF000000, NULL, NULL, &tmpTexture.data);
	D3DXCreateTextureFromFile(pDevice, nameFile.c_str(), &tmpTexture.data);
	pTextures->push_back(tmpTexture);
	return S_OK;
}
HRESULT resources::loadPixShaders(std::string nameFile, LPDIRECT3DDEVICE9 pDevice)
{
	shaderP tmpShader;
	tmpShader.pixShaderName = nameFile;

	LPD3DXBUFFER pPixBuffer;
	D3DXCompileShaderFromFile(nameFile.c_str(), 0, 0, "ps_main", "ps_3_0", D3DXSHADER_DEBUG, &pPixBuffer, NULL, &tmpShader.pConstTablePix);
	pDevice->CreatePixelShader((DWORD*)pPixBuffer->GetBufferPointer(), &tmpShader.pPixShader);
	pPixBuffer->Release();
	pShadersP->push_back(tmpShader);
	return S_OK;
}
HRESULT resources::loadVertShaders(std::string nameFile, LPDIRECT3DDEVICE9 pDevice)
{
	shaderV tmpShader;
	tmpShader.vertShaderName = nameFile;

	LPD3DXBUFFER pVertBuffer;
	D3DXCompileShaderFromFile(nameFile.c_str(), 0, 0, "vs_main", "vs_3_0", D3DXSHADER_DEBUG, &pVertBuffer, NULL, &tmpShader.pConstTableVert);
	pDevice->CreateVertexShader((DWORD*)pVertBuffer->GetBufferPointer(), &tmpShader.pVertShader);
	pVertBuffer->Release();
	pShadersV->push_back(tmpShader);
	return S_OK;
}

std::string	resources::getParam(std::string source, std::string param)
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
						if(source.at(i) != '=' && 
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
float		resources::getArrElem(geometry *tmpGeom, std::string name, DWORD pos, DWORD id)
{
	std::vector<source> *tmpSrc = tmpGeom->data;

	for (unsigned int i1 = 0; i1 < tmpSrc->size(); i1++)
	{
		if (std::strstr(tmpSrc->at(i1).sourceId.c_str(), name.c_str()))
		{
			return tmpSrc->at(i1).data.at(	( pos*tmpSrc->at(i1).stride) + (id - 1) );
		}
	}
	return 0.0f;
}
HRESULT		resources::getArray(std::string rawData, std::vector<float>* tmpArray)
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
			if(!tmp_string.empty())
				tmpArray->push_back(std::stof(tmp_string));
			tmp_string.clear();
			if(rawData.at(i) != '<') i++;
			else break;
		}
		tmp_string.push_back(rawData.at(i));
	}
	
	return NULL;
}
HRESULT		resources::getArrayName(std::string rawData, std::vector<std::string>* tmpArray)
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
void*		resources::getAllObject(flagObj flag)
{
	switch (flag)
	{
	case OT_MODEL:
	{
		return pModels;
	}
	break;
	case OT_TEXTURE:
	{
	}
	break;
	case OT_MUSIC:
	{
		return pTracks;
	}
	break;
	case OT_DEFAULT:
		break;
	default:
		break;
	}
	return NULL;
}
void*		resources::getObject(std::string name, flagObj flag)
{
	switch (flag)
	{
	case OT_MODEL:
	{
		if (pModels->empty()) return NULL;
		for (unsigned int i = 0; i < pModels->size(); i++)
		{
			if (pModels->at(i).modelName == name)
			{
				return &pModels->at(i);
				break;
			}
		}
	}
	break;
	case OT_TEXTURE:
	{
		if (pTextures->empty()) return NULL;
		for (unsigned int i = 0; i < pTextures->size(); i++)
		{
			if (pTextures->at(i).textureName == name)
			{
				return &pTextures->at(i);
				break;
			}
		}
	}
	break;
	case OT_MUSIC:
	{
		if (pTracks->empty()) return NULL;
		for (unsigned int i = 0; i < pTracks->size(); i++)
		{
			if (pTracks->at(i).trackName == name)
			{
				return &pTracks->at(i);
				break;
			}
		}
	}
	break;
	case OT_PSHADER:
	{
		if (pShadersP->empty()) return NULL;
		for (unsigned int i = 0; i < pShadersP->size(); i++)
		{
			if (pShadersP->at(i).pixShaderName == name)
			{
				return &pShadersP->at(i);
				break;
			}
		}
	}
	break;
	case OT_VSHADER:
	{
		if (pShadersV->empty()) return NULL;
		for (unsigned int i = 0; i < pShadersV->size(); i++)
		{
			if (pShadersV->at(i).vertShaderName == name)
			{
				return &pShadersV->at(i);
				break;
			}
		}
	}
	break;
	case OT_DEFAULT:
		break;
	default:
		break;
	}
	return NULL;
}

void	resources::convertData(model *data, LPDIRECT3DDEVICE9 pDevice)
{
	std::vector<geometry> *tmpObjects = data->pObjects;
	for (UINT indexObj = 0; indexObj < tmpObjects->size(); indexObj++)
	{
		bufferObj tmpBufObj;
		tmpBufObj.nameObj = tmpObjects->at(indexObj).name;
		tmpBufObj.idObj = tmpObjects->at(indexObj).id;
		tmpBufObj.pMasBoneForShader = InitMasBoneForShader(data, tmpObjects->at(indexObj).id);

		std::string NameMasVert;
		for (UINT i = 0; i < tmpObjects->at(indexObj).veticesData->inputSettings->size(); i++)
		{
			if (tmpObjects->at(indexObj).veticesData->inputSettings->at(i).semantic == "POSITION")
				NameMasVert = tmpObjects->at(indexObj).veticesData->inputSettings->at(i).source;
		}
		if (NameMasVert.empty())
		{
			for (UINT i = 0; i < tmpObjects->at(indexObj).indexData->at(0).inputSettings->size(); i++)
			{
				if (tmpObjects->at(indexObj).indexData->at(0).inputSettings->at(i).semantic == "POSITION") 
					NameMasVert = tmpObjects->at(indexObj).indexData->at(0).inputSettings->at(i).source;
			}
		}
		std::vector<BUFFER> VB_data_o;


		DWORD countVertMas = 0;
		for (UINT i = 0; i < tmpObjects->at(indexObj).data->size(); i++)
		{
			if (tmpObjects->at(indexObj).data->at(i).sourceId == NameMasVert) countVertMas = tmpObjects->at(indexObj).data->at(i).count;
		}
		BUFFER *VB_data = new BUFFER[countVertMas];
		std::vector<WORD> IB_data;

		std::vector<indexlist> *tmpIndexData = tmpObjects->at(indexObj).indexData;
		for (UINT indexDI = 0; indexDI < tmpIndexData->size(); indexDI++) //Цикл прогоняет все наборы указателейы
		{
			int sizeStack = 0;
			int sizeStackAll = 0;
			std::vector<patternBuf> patBufI;
			std::vector<input>* tmpInputV = tmpObjects->at(indexObj).veticesData->inputSettings;
			std::vector<input>* tmpInputI = tmpIndexData->at(indexDI).inputSettings;
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

			
			std::vector<std::vector<float>> *masIndeces = tmpIndexData->at(indexDI).dataIndexes; //Массив по VCount Хранящий указатели на данные
			for (UINT indexPoly = 0; indexPoly < masIndeces->size(); indexPoly++) // Цикл прогоняет указатели на данные каждного полигона
			{

				std::vector<std::vector<float>>* tmpDataI = tmpIndexData->at(indexDI).dataIndexes;
				std::vector<float>* iDataPoly = &masIndeces->at(indexPoly); //Хранит все указатели для построения одного полигона 
				for (UINT indexVert = 0; indexVert < iDataPoly->size() / (sizeStack + 1); indexVert++)
				{
					//3 раза для одного вертекса
					BUFFER tmpBuffer;
					int PosVert = 0;

					for (UINT indexElem = 0; indexElem < patBufI.size(); indexElem++)
					{
						DWORD tmpPos = (DWORD)tmpDataI->at(indexPoly).at( (sizeStackAll * indexVert) + patBufI.at(indexElem).pos ); //indexPoly Указывает на набор точек 1 полигона; (patBufI.size() * indexVert) Смещает на количество значений одной точки ; patBufI.at(indexElem).pos смещает на номер элемента
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
						if (patBufI.at(indexElem).semantic == "TEXCOORD" )
						{
							tmpBuffer.tu = getArrElem(&tmpObjects->at(indexObj), patBufI.at(indexElem).name, tmpPos, 1);
							tmpBuffer.tv = getArrElem(&tmpObjects->at(indexObj), patBufI.at(indexElem).name, tmpPos, 2);
						}
					}
					if (!data->pControllers->empty())
					{
						DWORD tmpPos = (DWORD)tmpDataI->at(indexPoly).at((sizeStackAll * indexVert));
						setWeight(data, tmpPos, &tmpBuffer, tmpObjects->at(indexObj).id);
					}
					VB_data[PosVert] = tmpBuffer;
					IB_data.push_back(PosVert);

					VB_data_o.push_back(tmpBuffer);
				}
			}	
		}
		VOID* pBV;
		tmpBufObj.sizeVB = countVertMas;
		pDevice->CreateVertexBuffer(countVertMas*sizeof(BUFFER), 0, NULL, D3DPOOL_MANAGED, &tmpBufObj.p_vb, NULL);
		tmpBufObj.p_vb->Lock(0, 0, (void**)&pBV, 0);
		memcpy(pBV, VB_data, countVertMas*sizeof(BUFFER));
		tmpBufObj.p_vb->Unlock();

		tmpBufObj.sizeVB_only = VB_data_o.size();
		pDevice->CreateVertexBuffer(VB_data_o.size()*sizeof(BUFFER), 0, NULL, D3DPOOL_MANAGED, &tmpBufObj.p_vb_only, NULL);
		tmpBufObj.p_vb_only->Lock(0, 0, (void**)&pBV, 0);
		memcpy(pBV, VB_data_o.data(), VB_data_o.size()*sizeof(BUFFER));
		tmpBufObj.p_vb_only->Unlock();

		tmpBufObj.sizeIB = IB_data.size();
		pDevice->CreateIndexBuffer(IB_data.size()*sizeof(WORD), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &tmpBufObj.p_ib, NULL);
		VOID* pBI;
		tmpBufObj.p_ib->Lock(0, 0, (void**)&pBI, 0);
		memcpy(pBI, IB_data.data(), IB_data.size()*sizeof(WORD));
		tmpBufObj.p_ib->Unlock();

		data->pBuffer->push_back(tmpBufObj);
	}
}

std::vector<skeletData*> *InitMasBoneForShader(model * pModel, std::string objId)
{
	for (UINT i = 0; i < pModel->pControllers->size(); i++)
	{
		if (pModel->pControllers->at(i).sourceGeom == objId)
		{
			for (UINT j = 0; j < pModel->pControllers->at(i).data->size(); j++)
			{
				if (pModel->pControllers->at(i).data->at(j).type == "JOINT")
				{
					std::vector<std::string> *tmpMas = &pModel->pControllers->at(i).data->at(j).dataName;
					std::vector<skeletData*> *tmpMasMat = new std::vector<skeletData*>();
					for (UINT k = 0; k < tmpMas->size(); k++)
					{
						skeletData *tmpMatrix = getPointerToBone(pModel->pSkelet, tmpMas->at(k));
						tmpMasMat->push_back(tmpMatrix);
					}
					return tmpMasMat;
				}
			}
		}
	}
	return NULL;
}
skeletData* getPointerToBone(skeletData* pSkelet, std::string name)
{
	if (pSkelet->sid == name)
		return pSkelet;
	skeletData* tmpBone;
	if (pSkelet->pChildData != NULL)
	{
		tmpBone = getPointerToBone(pSkelet->pChildData, name);
		if (tmpBone != NULL) return tmpBone;
	}
	if (pSkelet->pSiblingData != NULL)
	{
		tmpBone = getPointerToBone(pSkelet->pSiblingData, name);
		if (tmpBone != NULL) return tmpBone;
	}
	return NULL;
}

HRESULT resources::deleteObject(std::string name, flagObj flag)
{

	switch (flag)
	{
	case OT_MODEL:
	{
		if (pModels->empty()) return E_FAIL;
		for (unsigned int i = 0; i < pModels->size(); i++)
		{
			if (pModels->at(i).modelName == name)
			{
				pModels->erase(pModels->begin() + i);
				break;
			}
		}
	}
	break;
	case OT_TEXTURE:
	{
	}
	break;
	case OT_MUSIC:
	{
		if (pTracks->empty()) return E_FAIL;
		for (unsigned int i = 0; i < pTracks->size(); i++)
		{
			if (pTracks->at(i).trackName == name)
			{
				pTracks->erase(pTracks->begin() + i);
				break;
			}
		}
	}
	break;
	case OT_DEFAULT:
		break;
	default:
		break;
	}
	return S_OK;
}

void setInverseMatrix(model *pModel, skeletData *pBone)
{
	if (pBone == NULL) return;
	pBone->mInversePos = *getStockBone(pBone->sid, pModel);

	if (pBone->pChildData != NULL)
	{
		setInverseMatrix(pModel, pBone->pChildData);
	}
	if (pBone->pSiblingData != NULL)
	{
		setInverseMatrix(pModel, pBone->pSiblingData);
	}
}
D3DXMATRIX *getStockBone(std::string sid,  model * pModel)
{
	D3DXMATRIX *tmpMatrix = new D3DXMATRIX();
	for (UINT i = 0; i < pModel->pControllers->size(); i++)
	{
		for (UINT j = 0; j < pModel->pControllers->at(i).data->size(); j++)
		{
			if (pModel->pControllers->at(i).data->at(j).type == "JOINT")
			{
				std::vector<std::string> tmpMas = pModel->pControllers->at(i).data->at(j).dataName;
				for (UINT k = 0; k < tmpMas.size(); k++)
				{
					if (tmpMas.at(k) == sid)
					{
						for (UINT d = 0; d < pModel->pControllers->at(i).data->size(); d++)
						{
							if (pModel->pControllers->at(i).data->at(d).type == "TRANSFORM")
							{
								std::vector<float> tmpMasFloat;
								for (UINT g = 0; g < 16; g++)
									tmpMasFloat.push_back(pModel->pControllers->at(i).data->at(d).data.at(k * 16 + g));
								*tmpMatrix = tmpMasFloat.data();
								return tmpMatrix;
							}
						}
					}
				}
			}
		}
	}
	D3DXMatrixIdentity(tmpMatrix);
	return tmpMatrix;
}
std::string formatNum1(std::string* Str)
{
	std::string tmpStr;

	for (UINT i = 0; i < Str->size(); i++)
	{
		if (Str->at(i) == '/')
		{
			return tmpStr;
		}
		else tmpStr.push_back(Str->at(i));
	}
	tmpStr = "Error";
	return tmpStr;
}

void formatData2(source * tmpSource, std::vector<D3DXMATRIX> * pDataMatrix)
{
	std::vector<float> tmpFloatMas;
	tmpFloatMas.clear();

	for (UINT i = 0; i < tmpSource->data.size(); i++)
	{
		tmpFloatMas.push_back(tmpSource->data.at(i));
		if (tmpFloatMas.size() == 16)
		{
			pDataMatrix->push_back(tmpFloatMas.data());
			tmpFloatMas.clear();
		}
	}
}
