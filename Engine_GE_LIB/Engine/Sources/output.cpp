#include "output.h"
/*
HRESULT out::dModel(model* data, D3DPRIMITIVETYPE type)
{
	if (data == NULL) return E_FAIL;

	PDIRECT3DDEVICE9 pDevice = manager.getDevice();
	CAMERA *cam = manager.getCamSetting();


	for (UINT i = 0; i < data->pBuffer.size(); i++)
	{
		pDevice->SetTransform(D3DTS_WORLD, &cam->wMatrix);
		pDevice->SetStreamSource(0, data->pBuffer.at(i)->p_vb_only, 0, sizeof(BUFFER));		
		//pDevice->SetIndices(data->pBuffer.at(i)->p_ib);	

		pDevice->DrawPrimitive(type, 0, data->pBuffer.at(i)->sizeVB_only);
		//pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, data->pBuffer.at(i)->sizeVB, 0, data->pBuffer.at(i)->sizeIB/3);

		D3DXMatrixIdentity(&cam->wMatrix);
	}
	return S_OK;
}

HRESULT out::dObject(bufferObj *data, D3DPRIMITIVETYPE type)
{
	if (data == NULL) return E_FAIL;

	PDIRECT3DDEVICE9 pDevice = manager.getDevice();
	CAMERA *cam = manager.getCamSetting();

	pDevice->SetTransform(D3DTS_WORLD, &cam->wMatrix);
	pDevice->SetStreamSource(0, data->p_vb_only, 0, sizeof(BUFFER));
	pDevice->DrawPrimitive(type, 0, data->sizeVB_only);
	D3DXMatrixIdentity(&cam->wMatrix);

	return S_OK;
}

HRESULT out::dArray(std::vector<BUFFER> data, D3DPRIMITIVETYPE type)
{
	if (data.empty()) return E_FAIL;

	PDIRECT3DDEVICE9 pDevice = manager.getDevice();
	CAMERA *cam = manager.getCamSetting();

	VOID* pBV;
	LPDIRECT3DVERTEXBUFFER9 vb;
	pDevice->CreateVertexBuffer(data.size() * sizeof(BUFFER), 0, NULL, D3DPOOL_MANAGED, &vb, NULL);
	vb->Lock(0, 0, (void**)&pBV, 0);
	memcpy(pBV, data.data(), data.size() * sizeof(BUFFER));
	vb->Unlock();

	pDevice->SetTransform(D3DTS_WORLD, &cam->wMatrix);
	pDevice->SetStreamSource(0, vb, 0, sizeof(BUFFER));
	pDevice->DrawPrimitive(type, 0, data.size());
	D3DXMatrixIdentity(&cam->wMatrix);

	vb->Release();
	return S_OK;
}

HRESULT out::dScene(scene* data)
{
	if (data == NULL) return E_FAIL;
	if (!data->active) return S_OK;
	for (UINT i = 0; i < data->objects.size(); i++)
	{
		dScObject(data->objects.at(i));
	}
	return S_OK;
}

HRESULT out::dScObject(object *data, D3DXMATRIX *transformMatrix)
{
	if (data == NULL) return E_FAIL;

	PDIRECT3DDEVICE9 pDevice = manager.getDevice();

	if (!data->active) return S_OK;

	CAMERA *cam = manager.getCamSetting();
	trans *tmpTrans = &data->SettingsTransform;
	D3DXMATRIX locWMAtrix;
	D3DXMatrixIdentity(&locWMAtrix);
	if (tmpTrans->state == TRUE)
		locWMAtrix = transform(tmpTrans->angleX, tmpTrans->angleY, tmpTrans->angleZ, tmpTrans->x,
			tmpTrans->y, tmpTrans->z, tmpTrans->size);

	if (transformMatrix != NULL)
		D3DXMatrixMultiply(&locWMAtrix, &locWMAtrix, transformMatrix);

	if (data->pVshader != NULL)
	{
		D3DXHANDLE tmpHandel;
		pDevice->SetVertexShader(data->pVshader->pVertShader);

		tmpHandel = data->pVshader->pConstTableVert->GetConstantByName(0, "MatrixWorld");
		data->pVshader->pConstTableVert->SetMatrix(pDevice, tmpHandel, &locWMAtrix);

		tmpHandel = data->pVshader->pConstTableVert->GetConstantByName(0, "MatrixProjection");
		data->pVshader->pConstTableVert->SetMatrix(pDevice, tmpHandel, &cam->pMatrix);

		tmpHandel = data->pVshader->pConstTableVert->GetConstantByName(0, "MatrixView");
		data->pVshader->pConstTableVert->SetMatrix(pDevice, tmpHandel, &cam->vMatrix);
		for (UINT j = 0; j < data->vs_attr.size(); j++)
		{
			attr tmp = data->vs_attr.at(j);
			D3DXHANDLE hl = NULL;

			if (!tmp.data_var.empty())
			{
				hl = data->pVshader->pConstTableVert->GetConstantByName(0, tmp.name.c_str());
				data->pVshader->pConstTableVert->SetFloatArray(pDevice, hl, tmp.data_var.data(), tmp.data_var.size());
			}
		}
	}


	if (data->pPshader != NULL)
	{
		pDevice->SetPixelShader(data->pPshader->pPixShader);
		for (UINT j = 0; j < data->ps_attr.size(); j++)
		{
			attr tmp = data->ps_attr.at(j);
			D3DXHANDLE hl = NULL;
			if (!tmp.data_var.empty())
			{
				hl = data->pPshader->pConstTablePix->GetConstantByName(0, tmp.name.c_str());
				data->pPshader->pConstTablePix->SetFloatArray(pDevice, hl, tmp.data_var.data(), tmp.data_var.size());
			}
		}
	}

	model *pModel = (model*)resources.getObject(data->nameModel, OT_MODEL);
	if (pModel != NULL)
	{
		for (UINT j = 0; j < pModel->pNodes.size(); j++)
		{
			if (pModel->pNodes.at(j).pRootBone != NULL)
				updateSkelet(data, pModel->pNodes.at(j).pRootBone);
		}
		if (!pModel->pAnimation.empty() && data->pVshader != NULL)
		{
			D3DXHANDLE tmpHandel;
			std::vector<D3DXMATRIX> *pMasMatrix = collectMasMatrix(pModel->pBuffer.at(0));
			if (pMasMatrix != NULL)
			{
				tmpHandel = data->pVshader->pConstTableVert->GetConstantByName(0, "MatBones");
				data->pVshader->pConstTableVert->SetMatrixArray(manager.getDevice(), tmpHandel, pMasMatrix->data(), pMasMatrix->size());
			}
			delete pMasMatrix;
		}
	}

	if (data->pTexture != NULL)
	{
		pDevice->SetTexture(0, data->pTexture->data);
	}

	if (data->mesh != NULL)
	{
		pDevice->SetStreamSource(0, data->mesh->p_vb_only, 0, sizeof(BUFFER));
		if (data->prType == D3DPT_FORCE_DWORD)
			pDevice->DrawPrimitive(data->prType, 0, data->mesh->sizeVB_only);
		else
			pDevice->DrawPrimitive(data->prType, 0, data->mesh->sizeVB_only);
	}
	pDevice->SetTexture(0, NULL);
	pDevice->SetVertexShader(NULL);
	pDevice->SetPixelShader(NULL);


	if (data->handler != NULL)
	{
		if(1 == data->handler(data))  return S_OK;
	}

	if (!data->pChildObjects.empty())
		for (UINT j = 0; j < data->pChildObjects.size(); j++)
			dScObject(data->pChildObjects.at(j), &locWMAtrix);


	return S_OK;
}
*/
HRESULT out::dO(object *data, int flag, D3DXMATRIX *transformMatrix) 
{
	if (data == NULL) return E_FAIL;
	if (!data->active) return S_OK;
	LPDIRECT3DDEVICE9 pDevice = manager.getDevice();
	LPD3DXEFFECT rowEffect = data->plData.pEffects->data;
	CAMERA *cam = manager.getCamSetting();
	LPDIRECT3DSURFACE9 pSf = NULL;

	if (flag == 1)
	{
		manager.cTexture->GetSurfaceLevel(0, &pSf);
		pDevice->SetRenderTarget(0, pSf);
		pSf->Release();
		manager.nTexture->GetSurfaceLevel(0, &pSf);
		pDevice->SetRenderTarget(1, pSf);
		pSf->Release();
		manager.pTexture->GetSurfaceLevel(0, &pSf);
		pDevice->SetRenderTarget(2, pSf);
		pSf->Release();
		manager.dTexture->GetSurfaceLevel(0, &pSf);
		pDevice->SetRenderTarget(3, pSf);
		pSf->Release();
	} //Переключение RT
	trans *tmpTrans = &data->SettingsTransform;
	D3DXMatrixIdentity(&cam->wMatrix);
	if (tmpTrans->state == TRUE) cam->wMatrix = transform(tmpTrans->angleX, tmpTrans->angleY, tmpTrans->angleZ, tmpTrans->x, tmpTrans->y, tmpTrans->z, tmpTrans->size);
	if (transformMatrix != NULL) D3DXMatrixMultiply(&cam->wMatrix, &cam->wMatrix, transformMatrix);

	if (data->mesh != NULL)
	{
		pDevice->SetStreamSource(0, data->mesh->p_vb_only, 0, sizeof(BUFFER));

		LPD3DXEFFECT rowEffect = data->plData.pEffects->data;
		
		manager.tmpData->GetSurfaceLevel(0, &pSf);
		pDevice->SetRenderTarget(0, pSf);
		pSf->Release();

		data->plData.setAllUniform();
		sTechnique *pTec = data->plData.pEffects->getTechByName("firstPE");
		rowEffect->SetTechnique(pTec->hl);
		rowEffect->Begin(NULL, 0);
		for (UINT j = 0; j < pTec->desc.Passes; j++)
		{
			rowEffect->BeginPass(j);
				rowEffect->CommitChanges();
				pDevice->DrawPrimitive(data->prType, 0, data->mesh->sizeVB_only);
			rowEffect->EndPass();
		}
		rowEffect->End();

		pDevice->SetRenderTarget(0, manager.oldbuff);
		data->plData.setAllUniform();

		pTec = data->plData.pEffects->getTechByName("secondPE");
		rowEffect->SetTechnique(pTec->hl);
		rowEffect->Begin(NULL, 0);
		for (UINT j = 0; j < pTec->desc.Passes; j++)
		{
			rowEffect->BeginPass(j);
				rowEffect->CommitChanges();
				pDevice->DrawPrimitive(data->prType, 0, data->mesh->sizeVB_only);
			rowEffect->EndPass();
		}
		rowEffect->End();
		
	}

	if (flag == 1)
	{
		pDevice->SetRenderTarget(0, manager.oldbuff);
		pDevice->SetRenderTarget(1, NULL);
		pDevice->SetRenderTarget(2, NULL);
		pDevice->SetRenderTarget(3, NULL);
	} //RT back

	if (!data->pChildObjects.empty())
		for (UINT j = 0; j < data->pChildObjects.size(); j++)
			dO(data->pChildObjects.at(j), flag, &cam->wMatrix);
	return S_OK;
}
HRESULT out::dS(scene* data)
{
	if (data == NULL) return E_FAIL;
	if (!data->active) return S_OK;
	for (UINT i = 0; i < data->objects.size(); i++)
	{
		if (data->objects.at(i)->plData.plFunc != NULL)
			dO(data->objects.at(i), 1);
	}
	return S_OK;
}


//эффект должнен быть типа MRT (Color, Normal, Position, Depth)
HRESULT out::drAddToOut(object * data, D3DXMATRIX *transformMatrix)
{
	if (!data->active) return S_OK;
	D3DXMATRIX tmpTM;
	bool pMatrix = false;
	sDFElement tmpElement;
	D3DXMatrixIdentity(&tmpElement.wMatrix);
	if (transformMatrix != NULL) { tmpTM = *transformMatrix; pMatrix = true; }
	if (data->SettingsTransform.state == true)
	{
		trans *tmpTrans = &data->SettingsTransform;
		if (tmpTrans->state == TRUE) tmpElement.wMatrix = transform(tmpTrans->angleX, tmpTrans->angleY, tmpTrans->angleZ, tmpTrans->x, tmpTrans->y, tmpTrans->z, tmpTrans->size);
	}
	if (pMatrix) D3DXMatrixMultiply(&tmpElement.wMatrix, &tmpElement.wMatrix, &tmpTM);

	tmpElement.data = data;
	manager.drArray.push_back(tmpElement);

	if (!data->pChildObjects.empty())
		for (UINT j = 0; j < data->pChildObjects.size(); j++)
			drAddToOut(data->pChildObjects.at(j), &tmpElement.wMatrix);
	return S_OK;
}
HRESULT out::drAddToOut(scene * data)
{
	for (UINT i = 0; i < data->objects.size(); i++)
	{
		drAddToOut(data->objects.at(i));
	}
	return S_OK;
}
HRESULT out::drMRTDraw()
{
	LPDIRECT3DDEVICE9 pDevice = manager.getDevice();
	LPDIRECT3DSURFACE9 pSf = NULL;
	CAMERA *cam = manager.getCamSetting();

	for (UINT k = 0; k < manager.drArray.size(); k++)
	{
		object *data = manager.drArray.at(k).data;

		if (!data->active)continue;
		if (data->handler != NULL) if(1 == data->handler(data)) return S_OK;
		if (data->mesh == NULL)continue;
		if (data->plData.pEffects == NULL) continue;

		pDevice->SetStreamSource(0, data->mesh->p_vb_only, 0, sizeof(BUFFER));

		LPD3DXEFFECT rowEffect = data->plData.pEffects->data;

		manager.cTexture->GetSurfaceLevel(0, &pSf);
		pDevice->SetRenderTarget(0, pSf);
		pSf->Release();
		manager.nTexture->GetSurfaceLevel(0, &pSf);
		pDevice->SetRenderTarget(1, pSf);
		pSf->Release();
		manager.pTexture->GetSurfaceLevel(0, &pSf);
		pDevice->SetRenderTarget(2, pSf);
		pSf->Release();
		manager.dTexture->GetSurfaceLevel(0, &pSf);
		pDevice->SetRenderTarget(3, pSf);
		pSf->Release();

		cam->wMatrix = manager.drArray.at(k).wMatrix;
		data->plData.setAllUniform();

		sTechnique *tmpTec;
		tmpTec = data->plData.pEffects->getTechByName(data->plData.thechnique);
		rowEffect->SetTechnique(tmpTec->hl);
		rowEffect->Begin(NULL, 0);
		for (UINT j = 0; j < tmpTec->desc.Passes; j++)
		{
			rowEffect->BeginPass(j);
				rowEffect->CommitChanges();
				pDevice->DrawPrimitive(data->prType, 0, data->mesh->sizeVB_only);
			rowEffect->EndPass();
		}
		rowEffect->End();
	}

	pDevice->SetRenderTarget(0, manager.oldbuff);
	pDevice->SetRenderTarget(1, NULL);
	pDevice->SetRenderTarget(2, NULL);
	pDevice->SetRenderTarget(3, NULL);

	return S_OK;
}

/*
HRESULT out::TestFX(object *data, D3DXMATRIX *transformMatrix)
{
	if (data == NULL) return E_FAIL;
	if (!data->active) return S_OK;
	PDIRECT3DDEVICE9 pDevice = manager.getDevice();

	CAMERA *cam = manager.getCamSetting();
	trans *tmpTrans = &data->SettingsTransform;
	D3DXMATRIX locWMAtrix;
	D3DXMatrixIdentity(&locWMAtrix);
	if (tmpTrans->state == TRUE)
		locWMAtrix = transform(tmpTrans->angleX, tmpTrans->angleY, tmpTrans->angleZ, tmpTrans->x,
			tmpTrans->y, tmpTrans->z, tmpTrans->size);
	
	if (transformMatrix != NULL)
		D3DXMatrixMultiply(&locWMAtrix, &locWMAtrix, transformMatrix);
	if (data->mesh != NULL)
	{
		pDevice->SetStreamSource(0, data->mesh->p_vb_only, 0, sizeof(BUFFER));

		UINT iPass, cPasses;
		sGlobal *gl = data->pEffect->getGlobalByName("MatrixWorld"); //locWMAtrix
		data->pEffect->data->SetMatrix(gl->hl, &locWMAtrix);

		gl = data->pEffect->getGlobalByName("MatrixProjection"); //cam->pMatrix
		data->pEffect->data->SetMatrix(gl->hl, &cam->pMatrix);

		gl = data->pEffect->getGlobalByName("MatrixView"); //cam->vMatrix
		data->pEffect->data->SetMatrix(gl->hl, &cam->vMatrix);
		HRESULT hr = data->pEffect->data->ValidateTechnique(data->pEffect->techniques.at(0).hl);
		if (hr == E_FAIL) 
			return E_FAIL;

		manager.dRenderObj.pSourceBuffer = NULL;
		pDevice->GetRenderTarget(0, &manager.dRenderObj.pSourceBuffer);
		manager.dRenderObj.pDestBuffer = NULL;
		manager.dRenderObj.tDepth->GetSurfaceLevel(0, &manager.dRenderObj.pDestBuffer); //Depth
		pDevice->SetRenderTarget(0, manager.dRenderObj.pDestBuffer);
		data->pEffect->data->SetTechnique(data->pEffect->techniques.at(0).hl);
		data->pEffect->data->Begin(&cPasses, 0);
			
			data->pEffect->data->BeginPass(0);
				data->pEffect->data->CommitChanges();
				pDevice->DrawPrimitive(data->prType, 0, data->mesh->sizeVB_only);
			data->pEffect->data->EndPass();
			///			
			
			//pDevice->StretchRect(manager.dRenderObj.pSourceBuffer, NULL, manager.dRenderObj.pDestBuffer, NULL, D3DTEXF_NONE);

			gl = data->pEffect->getGlobalByName("tmpTxr");
			data->pEffect->data->SetTexture(gl->hl, manager.dRenderObj.tDepth);
				///
		data->pEffect->data->End();
		pDevice->SetRenderTarget(0, manager.dRenderObj.pSourceBuffer);

		pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(110, 110, 0), 1.0f, 0);
		data->pEffect->data->Begin(&cPasses, 0);
			data->pEffect->data->BeginPass(1);
			data->pEffect->data->CommitChanges();
			pDevice->DrawPrimitive(data->prType, 0, data->mesh->sizeVB_only);
			data->pEffect->data->EndPass();
			///
			manager.dRenderObj.pSourceBuffer = NULL;
			pDevice->GetRenderTarget(0, &manager.dRenderObj.pSourceBuffer);
			manager.dRenderObj.pDestBuffer = NULL;
			manager.dRenderObj.tColor->GetSurfaceLevel(0, &manager.dRenderObj.pDestBuffer); //COLOR
			pDevice->StretchRect(manager.dRenderObj.pSourceBuffer, NULL, manager.dRenderObj.pDestBuffer, NULL, D3DTEXF_NONE);
			gl = data->pEffect->getGlobalByName("tmpTxr");
			data->pEffect->data->SetTexture(gl->hl, manager.dRenderObj.tColor);	
			///	
		data->pEffect->data->End();
		pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(110, 110, 0), 1.0f, 0);

		//mergeTexture();
	}

	return S_OK;
}

HRESULT mergeTexture()
{
	object *data = manager.rDisplay;

	if (data == NULL) return E_FAIL;
	if (!data->active) return S_OK;
	PDIRECT3DDEVICE9 pDevice = manager.getDevice();

	CAMERA *cam = manager.getCamSetting();
	trans *tmpTrans = &data->SettingsTransform;
	D3DXMATRIX locWMAtrix;
	D3DXMatrixIdentity(&locWMAtrix);
	if (tmpTrans->state == TRUE)
		locWMAtrix = transform(tmpTrans->angleX, tmpTrans->angleY, tmpTrans->angleZ, tmpTrans->x,
			tmpTrans->y, tmpTrans->z, tmpTrans->size);

	manager.updateCam(VIEW_2D);
	if (data->mesh != NULL)
	{
		pDevice->SetStreamSource(0, data->mesh->p_vb_only, 0, sizeof(BUFFER));

		UINT iPass, cPasses;
		sGlobal *gl = data->pEffect->getGlobalByName("MatrixWorld"); //locWMAtrix
		data->pEffect->data->SetMatrix(gl->hl, &locWMAtrix);

		gl = data->pEffect->getGlobalByName("MatrixProjection"); //cam->pMatrix
		data->pEffect->data->SetMatrix(gl->hl, &cam->pMatrix);

		gl = data->pEffect->getGlobalByName("MatrixView"); //cam->vMatrix
		data->pEffect->data->SetMatrix(gl->hl, &cam->vMatrix);
		HRESULT hr = data->pEffect->data->ValidateTechnique(data->pEffect->techniques.at(0).hl);
		if (hr == E_FAIL)
			return E_FAIL;

		gl = data->pEffect->getGlobalByName("tCDest");
		data->pEffect->data->SetTexture(gl->hl, manager.dFinalRender.tColor);
		gl = data->pEffect->getGlobalByName("tCSrc");
		data->pEffect->data->SetTexture(gl->hl, manager.dRenderObj.tColor);
		gl = data->pEffect->getGlobalByName("tDDest");
		data->pEffect->data->SetTexture(gl->hl, manager.dFinalRender.tDepth);
		gl = data->pEffect->getGlobalByName("tDSrc");
		data->pEffect->data->SetTexture(gl->hl, manager.dRenderObj.tDepth);

		data->pEffect->data->SetTechnique(data->pEffect->techniques.at(0).hl);
		data->pEffect->data->Begin(&cPasses, 0);
		for (iPass = 0; iPass < cPasses; iPass++)
		{
			data->pEffect->data->BeginPass(iPass);
				data->pEffect->data->CommitChanges();
				pDevice->DrawPrimitive(data->prType, 0, data->mesh->sizeVB_only);
			data->pEffect->data->EndPass();


			///
			manager.dFinalRender.pSourceBuffer = NULL;
			pDevice->GetRenderTarget(0, &manager.dFinalRender.pSourceBuffer);

			for (int i = 0; i < manager.dFinalRender.tColor->GetLevelCount(); i++) //Не производительный метод
			{
				manager.dFinalRender.pDestBuffer = NULL;
				if (iPass == 0)
					manager.dFinalRender.tColor->GetSurfaceLevel(i, &manager.dFinalRender.pDestBuffer);
				else if (iPass == 1)
					manager.dFinalRender.tDepth->GetSurfaceLevel(i, &manager.dFinalRender.pDestBuffer);
				else if (iPass == 2)
					manager.dFinalRender.tNormal->GetSurfaceLevel(i, &manager.dFinalRender.pDestBuffer);

				pDevice->StretchRect(manager.dFinalRender.pSourceBuffer, NULL, manager.dFinalRender.pDestBuffer, NULL, D3DTEXF_NONE);
			}
			///
		}
		data->pEffect->data->End();
		pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
		manager.updateCam(VIEW_3D);
	}
	return S_OK;
}

*/
HRESULT out::dText(const char * text, RECT coord, D3DCOLOR color, DWORD format)
{
	LPD3DXFONT pFont = manager.getFont();
	if (pFont == NULL) 	return E_FAIL;

	int a = pFont->DrawTextA(NULL,
	text,
	-1,
	&coord,
	format,
	color);

	return S_OK;
}
HRESULT out::dText(const wchar_t * text, RECT coord, D3DCOLOR color, DWORD format)
{
	LPD3DXFONT pFont = manager.getFont();
	if (pFont == NULL) 	return E_FAIL;
	std::wstring name;
	name.c_str();
	int a = pFont->DrawTextW(NULL,
		text,
		-1,
		&coord,
		format,
		color);

	return S_OK;
}