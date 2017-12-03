#include "input.h"

model* in::loadModel(std::string nameFile)
{
	if (nameFile.empty()) return NULL;
	PDIRECT3DDEVICE9 pDevice = manager.getDevice();
	if (pDevice == NULL) return NULL;

	std::fstream	file_data;
	model *tmpModel = new model();

	std::string tmpName;
	UINT i = 0;
	for (i = (nameFile.size() - 1); i > 0; i--)
	{
		if (nameFile.at(i) == '/') break;
	}
	if (i != 0)
	{
		for (i += 1; i < nameFile.size(); i++)
		{
			tmpName.push_back(nameFile.at(i));
		}
		tmpModel->modelName = tmpName;
	}
	else tmpModel->modelName = nameFile;

	tmpModel->modelFullName = nameFile;

	file_data.open(nameFile);
	if (!file_data.is_open()) return NULL;

	std::string tmpStr;
	while (!file_data.eof())
	{
		std::getline(file_data, tmpStr);

		if (std::strstr(tmpStr.c_str(), "<library_effects>"))
		{
			std::vector<float> tmpDataColor;
			while (!std::strstr(tmpStr.c_str(), "</library_effects>"))
			{
				if (std::strstr(tmpStr.c_str(), "<effect "))
				{
					effect tmpEffect;
					tmpEffect.idEffect = getParameter(tmpStr, "id");
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
					tmpModel->pEffects.push_back(tmpEffect);
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
					tmpMaterial.idMaterial = getParameter(tmpStr, "id");
					tmpMaterial.nameMaterial = getParameter(tmpStr, "name");

					while (!std::strstr(tmpStr.c_str(), "</material>"))
					{
						if (std::strstr(tmpStr.c_str(), "<instance_effect "))
						{
							tmpMaterial.instanceEffect = getParameter(tmpStr, "url");
						}

						std::getline(file_data, tmpStr);
					}
					tmpModel->pMaterials.push_back(tmpMaterial);
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
					tmpAnimation.nameAnim = getParameter(tmpStr, "id");

					while (!std::strstr(tmpStr.c_str(), "</animation>"))
					{
						if (std::strstr(tmpStr.c_str(), "<source "))
						{
							source tmpSource;
							tmpSource.sourceId = getParameter(tmpStr, "id");
							while (!std::strstr(tmpStr.c_str(), "</source>"))
							{
								if (std::strstr(tmpStr.c_str(), "<float_array "))
								{
									tmpSource.arrayId = getParameter(tmpStr, "id");
									getArray(tmpStr, &tmpSource.data);
								}
								if (std::strstr(tmpStr.c_str(), "<Name_array "))
								{
									tmpSource.arrayId = getParameter(tmpStr, "id");
									getArrayName(tmpStr, &tmpSource.dataName);
								}
								if (std::strstr(tmpStr.c_str(), "<technique_common>"))
								{
									while (!std::strstr(tmpStr.c_str(), "</technique_common>"))
									{
										if (std::strstr(tmpStr.c_str(), "<accessor "))
										{
											tmpSource.count = std::stoi(getParameter(tmpStr, "count"));
											tmpSource.stride = std::stoi(getParameter(tmpStr, "stride"));
										}
										if (std::strstr(tmpStr.c_str(), "<param "))
										{
											tmpSource.type = getParameter(tmpStr, "name");
											if (tmpSource.type == "TRANSFORM")
											{
												std::vector<float> tmpFloatMas;
												tmpFloatMas.clear();

												for (UINT i = 0; i < tmpSource.data.size(); i++)
												{
													tmpFloatMas.push_back(tmpSource.data.at(i));
													if (tmpFloatMas.size() == 16)
													{
														tmpAnimation.pDataMatrix.push_back(tmpFloatMas.data());
														tmpFloatMas.clear();
													}
												}
											}
											if (tmpSource.type == "TIME")
											{
												tmpAnimation.pDataTime.assign(tmpSource.data.begin(), tmpSource.data.end());
											}
											if (tmpSource.type == "X" ||
												tmpSource.type == "Y" ||
												tmpSource.type == "Z" ||
												tmpSource.type == "ANGLE")
											{
												tmpAnimation.pDataFloat.assign(tmpSource.data.begin(), tmpSource.data.end());
											}
										}
										std::getline(file_data, tmpStr);
									}
								}
								std::getline(file_data, tmpStr);
							}
							tmpAnimation.pData.push_back(tmpSource);
						}
						if (std::strstr(tmpStr.c_str(), "<sampler "))
						{
							while (!std::strstr(tmpStr.c_str(), "</sampler>"))
							{
								if (std::strstr(tmpStr.c_str(), "<input "))
								{
									input tmpInput;
									tmpInput.semantic = getParameter(tmpStr, "semantic");
									tmpInput.source = getParameter(tmpStr, "source");
									tmpInput.offset = -1;
									tmpAnimation.pInputs.push_back(tmpInput);
								}
								std::getline(file_data, tmpStr);
							}
						}
						if (std::strstr(tmpStr.c_str(), "<channel "))
						{
							std::string Str = getParameter(tmpStr, "target");
							std::string tmpStr;
							bool firstStep = FALSE;
							for (UINT i = 0; i < Str.size(); i++)
							{
								if (Str.at(i) == '/' && firstStep == FALSE)
								{
									tmpAnimation.nameObj = tmpStr;
									tmpStr.clear();
									firstStep = TRUE;
								}
								else tmpStr.push_back(Str.at(i));

								if (firstStep == TRUE)
								{
									tmpAnimation.typeAnim = tmpStr;
								}
							}
						}
						std::getline(file_data, tmpStr);
					}
					tmpModel->pAnimation.push_back(tmpAnimation);
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
					tmpImage.nameImage = getParameter(tmpStr, "id");
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
					tmpGeom.id = getParameter(tmpStr, "id");
					tmpGeom.name = getParameter(tmpStr, "name");
					while (!std::strstr(tmpStr.c_str(), "</geometry>"))
					{
						if (std::strstr(tmpStr.c_str(), "<source"))
						{
							source tmpSrc;
							tmpSrc.sourceId = getParameter(tmpStr, "id");
							while (!std::strstr(tmpStr.c_str(), "</source>"))
							{
								if (std::strstr(tmpStr.c_str(), "<float_array "))
								{
									tmpSrc.arrayId = getParameter(tmpStr, "id");
									getArray(tmpStr, &tmpSrc.data);
								}
								if (std::strstr(tmpStr.c_str(), "<accessor "))
								{
									tmpSrc.count = std::stoi(getParameter(tmpStr, "count"));
									tmpSrc.stride = std::stoi(getParameter(tmpStr, "stride"));
								}
								if (std::strstr(tmpStr.c_str(), "<param "))
								{
									if (tmpSrc.type.empty())
										tmpSrc.type = getParameter(tmpStr, "type");
								}

								std::getline(file_data, tmpStr);
							}
							tmpGeom.data.push_back(tmpSrc);
						}
						if (std::strstr(tmpStr.c_str(), "<vertices "))
						{
							tmpGeom.veticesData.verticesName = getParameter(tmpStr, "id");

							while (!std::strstr(tmpStr.c_str(), "</vertices>"))
							{
								if (std::strstr(tmpStr.c_str(), "<input "))
								{
									input tmpInput;
									tmpInput.semantic = getParameter(tmpStr, "semantic");
									tmpInput.source = getParameter(tmpStr, "source");

									tmpGeom.veticesData.inputSettings.push_back(tmpInput);
								}
								std::getline(file_data, tmpStr);
							}
						}
						if (std::strstr(tmpStr.c_str(), "<polylist ") ||
							std::strstr(tmpStr.c_str(), "<triangles "))
						{
							indexlist tmpPolylist;
							while (!(std::strstr(tmpStr.c_str(), "</polylist>") ||
								std::strstr(tmpStr.c_str(), "</triangles>")))
							{

								if (std::strstr(tmpStr.c_str(), "<vcount>"))
								{
									getArray(tmpStr, &tmpPolylist.dataVcounts);
								}
								if (std::strstr(tmpStr.c_str(), "<input "))
								{
									input tmpIndexIn;

									tmpIndexIn.offset = std::stoi(getParameter(tmpStr, "offset"));
									tmpIndexIn.semantic = getParameter(tmpStr, "semantic");
									tmpIndexIn.source = getParameter(tmpStr, "source");

									tmpPolylist.inputSettings.push_back(tmpIndexIn);
								}
								if (std::strstr(tmpStr.c_str(), "<p>"))
								{

									std::vector<float> tmpMas;

									getArray(tmpStr, &tmpMas);

									if (tmpPolylist.dataVcounts.empty())
									{
										int sizeStack = 0;
										for (UINT i2 = 0; i2 < tmpPolylist.inputSettings.size(); i2++)
										{
											if (tmpPolylist.inputSettings.at(i2).offset > sizeStack) sizeStack = tmpPolylist.inputSettings.at(i2).offset;
										}
										for (UINT i = 0; i < (tmpMas.size()) / (sizeStack + 1); i++)
											tmpPolylist.dataVcounts.push_back(sizeStack + 1.0f);
									}

									int shift = 0, offsetmax = 0;
									for (UINT i = 0; i < tmpPolylist.inputSettings.size(); i++)
									{
										if (tmpPolylist.inputSettings.at(i).offset > offsetmax) offsetmax = tmpPolylist.inputSettings.at(i).offset;
									}
									for (unsigned int i = 0; i < tmpPolylist.dataVcounts.size(); i++)
									{
										std::vector<float> tmpVectData;
										for (unsigned int j = 0; j < tmpPolylist.dataVcounts.at(i); j++)
										{
											for (int k = 0; k < offsetmax + 1; k++)
											{
												tmpVectData.push_back(tmpMas.at(shift + k));
											}
											shift += (int)offsetmax + 1;
										}
										tmpPolylist.dataIndexes.push_back(tmpVectData);
									}
								}

								std::getline(file_data, tmpStr);
							}
							tmpGeom.indexData.push_back(tmpPolylist);
						}

						std::getline(file_data, tmpStr);
					}
					tmpModel->pObjects.push_back(tmpGeom);
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
						tmpGeom.id = getParameter(tmpStr, "id");
						tmpGeom.name = getParameter(tmpStr, "name");
						if (std::strstr(tmpStr.c_str(), "<skin "))
						{
							tmpGeom.sourceGeom = getParameter(tmpStr, "source");
							while (!std::strstr(tmpStr.c_str(), "</skin>"))
							{
								if (std::strstr(tmpStr.c_str(), "<source"))
								{
									source tmpSrc;
									tmpSrc.sourceId = getParameter(tmpStr, "id");
									while (!std::strstr(tmpStr.c_str(), "</source>"))
									{
										if (std::strstr(tmpStr.c_str(), "<float_array "))
										{
											tmpSrc.arrayId = getParameter(tmpStr, "id");
											getArray(tmpStr, &tmpSrc.data);
										}
										if (std::strstr(tmpStr.c_str(), "<Name_array "))
										{
											tmpSrc.arrayId = getParameter(tmpStr, "id");
											getArrayName(tmpStr, &tmpSrc.dataName);
										}
										if (std::strstr(tmpStr.c_str(), "<accessor "))
										{
											tmpSrc.count = std::stoi(getParameter(tmpStr, "count"));
											tmpSrc.stride = std::stoi(getParameter(tmpStr, "stride"));
										}
										if (std::strstr(tmpStr.c_str(), "<param "))
										{
											if (tmpSrc.type.empty())
												tmpSrc.type = getParameter(tmpStr, "name");
										}

										std::getline(file_data, tmpStr);
									}

									tmpGeom.data.push_back(tmpSrc);
								}
								if (std::strstr(tmpStr.c_str(), "<vertex_weights "))
								{
									indexlist tmpWeightlist;
									while (!(std::strstr(tmpStr.c_str(), "</vertex_weights>")))
									{
										if (std::strstr(tmpStr.c_str(), "<input "))
										{
											input tmpIndexIn;

											tmpIndexIn.offset = std::stoi(getParameter(tmpStr, "offset"));
											tmpIndexIn.semantic = getParameter(tmpStr, "semantic");
											tmpIndexIn.source = getParameter(tmpStr, "source");

											tmpWeightlist.inputSettings.push_back(tmpIndexIn);
										}
										if (std::strstr(tmpStr.c_str(), "<vcount>"))
										{
											getArray(tmpStr, &tmpWeightlist.dataVcounts);
										}
										if (std::strstr(tmpStr.c_str(), "<v>"))
										{
											std::vector<float> tmpMas;

											getArray(tmpStr, &tmpMas);
											int shift = 0, offsetmax = 0;
											for (size_t i = 0; i < tmpWeightlist.inputSettings.size(); i++)
											{
												if (tmpWeightlist.inputSettings.at(i).offset > offsetmax) offsetmax = tmpWeightlist.inputSettings.at(i).offset;
											}

											for (unsigned int i = 0; i < tmpWeightlist.dataVcounts.size(); i++)
											{
												std::vector<float> tmpVectData;
												for (unsigned int j = 0; j < tmpWeightlist.dataVcounts.at(i); j++)
												{
													for (int k = 0; k < offsetmax + 1; k++)
													{
														tmpVectData.push_back(tmpMas.at(shift + k));
													}
													shift += (int)offsetmax + 1;
												}
												tmpWeightlist.dataIndexes.push_back(tmpVectData);
											}
										}
										std::getline(file_data, tmpStr);
									}
									tmpGeom.indexData.push_back(tmpWeightlist);
								}
								if (std::strstr(tmpStr.c_str(), "<joints>"))
								{
									while (!(std::strstr(tmpStr.c_str(), "</joints>")))
									{
										std::vector<float> tmpArrInd;
										std::getline(file_data, tmpStr);
									}
								}
								std::getline(file_data, tmpStr);
							}
							tmpModel->pControllers.push_back(tmpGeom);
						}
						std::getline(file_data, tmpStr);
					}
				}
				std::getline(file_data, tmpStr);
			}
		}
		else if (std::strstr(tmpStr.c_str(), "<library_visual_scenes>"))
		{
			while (!std::strstr(tmpStr.c_str(), "</library_visual_scenes>"))
			{
				if (std::strstr(tmpStr.c_str(), "<visual_scene "))
				{
					visualGeometry tmpVGeometry;
					tmpVGeometry.sceneId = getParameter(tmpStr, "id");
					while (!std::strstr(tmpStr.c_str(), "</visual_scene>"))
					{
						if (std::strstr(tmpStr.c_str(), "<node ") && !std::strstr(tmpStr.c_str(), "/>"))
						{
							node tmpNode;
							tmpNode.nodeSid = getParameter(tmpStr, "sid");
							tmpNode.nodeName = getParameter(tmpStr, "name");
							tmpNode.nodeId = getParameter(tmpStr, "id");
							tmpNode.nodeType = getParameter(tmpStr, "type");

							while (!std::strstr(tmpStr.c_str(), "</node>"))
							{
								if (std::strstr(tmpStr.c_str(), "<node ") && !std::strstr(tmpStr.c_str(), "/>") && getParameter(tmpStr, "type") == "JOINT")
								{
									tmpNode.pRootBone = new bone();
									tmpNode.pRootBone->nameBone = "Root";
									parserSkel(&file_data, tmpStr, tmpNode.pRootBone);
								}
								if (std::strstr(tmpStr.c_str(), "<translate"))
								{
									std::vector<float> tmpVec;
									getArray(tmpStr, &tmpVec);
									if (!tmpVec.empty())
									{
										tmpNode.location = (D3DXVECTOR3)tmpVec.data();
									}
								}
								if (std::strstr(tmpStr.c_str(), "<rotate"))
								{
									std::vector<float> tmpVec;
									std::string tmpSid = getParameter(tmpStr, "sid");
									getArray(tmpStr, &tmpVec);
									if (!tmpVec.empty())
									{
										if (tmpSid == "rotationX")
										{
											tmpNode.rotationX = (D3DXVECTOR4)tmpVec.data();
										}
										else if (tmpSid == "rotationY")
										{
											tmpNode.rotationY = (D3DXVECTOR4)tmpVec.data();
										}
										else if (tmpSid == "rotationZ")
										{
											tmpNode.rotationZ = (D3DXVECTOR4)tmpVec.data();
										}
									}
								}
								if (std::strstr(tmpStr.c_str(), "<scale"))
								{
									std::vector<float> tmpVec;
									getArray(tmpStr, &tmpVec);
									if (!tmpVec.empty())
									{
										tmpNode.scale = (D3DXVECTOR3)tmpVec.data();
									}
								}
								std::getline(file_data, tmpStr);
							}
							tmpModel->pNodes.push_back(tmpNode);
						}
						std::getline(file_data, tmpStr);
					}
				}
				std::getline(file_data, tmpStr);
			}
		}
	}
	convertData(tmpModel, pDevice);
	for (UINT i = 0; i < tmpModel->pNodes.size(); i++)
	{
		if (tmpModel->pNodes.at(i).pRootBone != NULL)
			setInverseMatrix(tmpModel, tmpModel->pNodes.at(i).pRootBone);
	}
	file_data.close();

	resources.setObject(tmpModel, OT_MODEL);
	return tmpModel;
}

track * in::loadTrack(std::string nameFile)
{
	if (nameFile == "None")
		return NULL;

	std::ifstream inFile(nameFile, std::ios::binary | std::ios::in);
	if (inFile.bad())
		return NULL;

	DWORD dwChunkId = 0, dwFileSize = 0, dwChunkSize = 0, dwExtra = 0;
	track *tmpTrack = new track();

	tmpTrack->trackName = nameFile;

	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(XAudio2Create(&tmpTrack->gEngine, 0, XAUDIO2_DEFAULT_PROCESSOR)))
	{
		tmpTrack->gEngine->Release();
		return NULL;
	}
	if (FAILED(tmpTrack->gEngine->CreateMasteringVoice(&tmpTrack->gMaster)))
	{
		tmpTrack->gEngine->Release();
		return NULL;
	}

	//look for 'RIFF' chunk identifier
	inFile.seekg(0, std::ios::beg);
	inFile.read(reinterpret_cast<char*>(&dwChunkId), sizeof(dwChunkId));
	if (dwChunkId != 'FFIR')
	{
		inFile.close();
		return NULL;
	}
	inFile.seekg(4, std::ios::beg); //get file size
	inFile.read(reinterpret_cast<char*>(&dwFileSize), sizeof(dwFileSize));
	if (dwFileSize <= 16)
	{
		inFile.close();
		return NULL;
	}
	inFile.seekg(8, std::ios::beg); //get file format
	inFile.read(reinterpret_cast<char*>(&dwExtra), sizeof(dwExtra));
	if (dwExtra != 'EVAW')
	{
		inFile.close();
		return NULL;
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
			inFile.read(reinterpret_cast<char*>(&(tmpTrack->mWF)), sizeof(tmpTrack->mWF));
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
		return NULL;
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
			tmpTrack->mXA.AudioBytes = dwChunkSize;
			tmpTrack->mXA.pAudioData = m_waveData;
			tmpTrack->mXA.PlayBegin = 0;
			tmpTrack->mXA.PlayLength = 0;
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
		return NULL;
	}

	if (FAILED(tmpTrack->gEngine->CreateSourceVoice(&(tmpTrack->gSource), &(tmpTrack->mWF))))return NULL;

	inFile.close();
	
	resources.setObject(tmpTrack, OT_MUSIC);
	return tmpTrack;
}

texture* in::loadTexture(std::string nameFile)
{
	if (nameFile.empty()) return NULL;
	PDIRECT3DDEVICE9 pDevice = manager.getDevice();
	if (pDevice == NULL) return NULL;

	texture *tmpTexture = new texture();
	tmpTexture->textureName = nameFile.c_str();
	//D3DXCreateTextureFromFileEx(pDevice, nameFile.c_str(), 0, 0, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0xFF000000, NULL, NULL, &tmpTexture.data);
	D3DXCreateTextureFromFile(pDevice, nameFile.c_str(), &tmpTexture->data);
	resources.setObject(tmpTexture, OT_TEXTURE);
	return tmpTexture;
}

sEffect * in::loadEffect(std::string nameFile)
{
	LPDIRECT3DDEVICE9 pDevice = manager.getDevice();
	if (pDevice == NULL) return NULL;

	sEffect *tmpEffect = new sEffect();
	tmpEffect->pathName = nameFile;
	ID3DXBuffer* errorBuffer = 0;

	DWORD dwShaderFlags = 0;
	//dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
	//dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
	dwShaderFlags |= D3DXSHADER_NO_PRESHADER;
	HRESULT hr = D3DXCreateEffectFromFile(pDevice, nameFile.c_str(), NULL, NULL, dwShaderFlags, NULL, &tmpEffect->data, &errorBuffer);
	if (hr != S_OK)
		return NULL;

	UINT i = 0;
	D3DXHANDLE hl = NULL;
	sGlobal gl;
	hl = tmpEffect->data->GetParameter(NULL, i);
	while (hl != NULL)
	{
		gl.hl = hl;
		tmpEffect->data->GetParameterDesc(hl, &gl.desc);

		UINT tmpOffset = 0;
		if(!tmpEffect->globals.empty())
			gl.offset = tmpEffect->globals.back().offset + tmpEffect->globals.back().desc.Bytes;

		tmpEffect->globals.push_back(gl);
		i++;
		hl = NULL;
		hl = tmpEffect->data->GetParameter(NULL, i);
	}

	if (errorBuffer)
	{
		MessageBox(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
	}

	i = 0;
	hl = 0;
	sTechnique tq;
	hl = tmpEffect->data->GetTechnique(i);
	while (hl != NULL)
	{
		tq.hl = hl;
		tmpEffect->data->GetTechniqueDesc(hl, &tq.desc);
		tmpEffect->techniques.push_back(tq);
		i++;
		hl = NULL;
		hl = tmpEffect->data->GetTechnique(i);
	}

	resources.setObject(tmpEffect, OT_EFFECT);
	return tmpEffect;
}





