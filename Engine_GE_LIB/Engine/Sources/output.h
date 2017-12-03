#ifndef OUTPUT_H
#define OUTPUT_H

#include <Windows.h>
#include <vector>
#include <string>

#include "manager.h"
#include "resources.h"
#include "processing.h"



namespace out
{
	//HRESULT dModel(model* data, D3DPRIMITIVETYPE type);
	//HRESULT dObject(bufferObj *data, D3DPRIMITIVETYPE type);
	//HRESULT dArray(std::vector<BUFFER> data, D3DPRIMITIVETYPE type);

	//HRESULT dScObject(object *data, D3DXMATRIX *transformMatrix = NULL);
	//HRESULT dScene(scene* data);

	HRESULT dText(const char *text, RECT coord = {10,10,800,600}, D3DCOLOR color = D3DCOLOR_ARGB( 255,255,255,255 ), DWORD format = DT_WORDBREAK);
	HRESULT dText(const wchar_t * text, RECT coord = { 10,10,800,600 }, D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 255, 255), DWORD format = DT_WORDBREAK);

	HRESULT dO(object *data, int flag, D3DXMATRIX *transformMatrix = NULL);
	HRESULT dS(scene* data);

	HRESULT drMRTDraw();
	HRESULT drAddToOut(object *data, D3DXMATRIX *transformMatrix = NULL);
	HRESULT drAddToOut(scene *data);

}
#endif