#ifndef LOADER_OG_H
#define LOADER_OG_H

#include <windows.h>
#include <d3dx9math.h>
#include <vector>
#include <string>
#include "Resources.h"

#include "Init_Window.h"
#include "Handler_Devices.h"

#pragma comment(lib, "winmm.lib") //для получения времени 

enum flagtype { DM_TIME, DM_MATRIX};

HRESULT setCamera(hWndInfo *pHWI);
void manualControl(hWndInfo *pHWI);
void initFont(hWndInfo *pHWI, int size, char* name);
void printText(char* text, int posX, int posY, int entryFX, int entryFY, D3DCOLOR color, hWndInfo *pHWI);
D3DXMATRIX transform(LPDIRECT3DDEVICE9 p_direct3d_device, float angle_x, float angle_y, float pos_x, float pos_y, float pos_z, float size);


HRESULT drawModel(model *data, LPDIRECT3DDEVICE9 pDevice, resources *res);

int getPosAnimation(animation* pAnim, int time);
D3DXMATRIX getMatrixOfTime(D3DXMATRIX matrixBegin, D3DXMATRIX matrixEnd, int time, int timeNow);

std::vector<D3DXMATRIX> *collectMasMatrix(bufferObj *pBuffer);

void updateModelAnim(model *pModel, skeletData* pSkelet);


#endif