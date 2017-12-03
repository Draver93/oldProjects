#ifndef PROCESSING_H
#define PROCESSING_H

#include <d3d9.h>
#include <d3dx9.h>

#include <Windows.h>
#include <vector>
#include <string>
#include <fstream>

#include "output.h"
#include "manager.h"
#include "resources.h"

#pragma comment(lib, "winmm.lib") //для получения времени 

struct TRI
{
	D3DXVECTOR3 p1, p2, p3;
};
struct PICKING
{
	bool active;
	std::vector<TRI> triangles;
	PICKING()
	{
		active = false;
	}
	~PICKING()
	{
	}
};


void ctrl(trans *tmp, float speed = 1.0f);

D3DXMATRIX transform(float angle_x, float angle_y, float angle_z, float pos_x, float pos_y, float pos_z, float size);
void manualControl();
void holdMouseInRect(RECT rc);

bool pick(object *obj, D3DXVECTOR3 vecBegin, D3DXVECTOR3 vecEnd);
bool pick(object *obj);

int listen(std::string *tmpStr);

POINT getTextSize(std::string str);
POINT getTextSize(std::wstring str);

//LoadModel Functions
std::string getParameter(std::string source, std::string param);
float		getArrElem(geometry *tmpGeom, std::string name, DWORD pos, DWORD id);
HRESULT		getArray(std::string rawData, std::vector<float>* tmpArray);
HRESULT		getArrayName(std::string rawData, std::vector<std::string>* tmpArray);
void		convertData(model *data, LPDIRECT3DDEVICE9 pDevice);
void		setWeight(model *data, UINT iPos, BUFFER *pBuffer, std::string idGeom);
void		parserSkel(std::fstream * file_data, std::string tmpStr, bone *pPrntNode);
void		setInverseMatrix(model *pModel, bone *pBone);

std::vector<bone*>	InitMasBoneForShader(model * pModel, std::string objId);
bone*				getPointerToBone(bone* pSkelet, std::string name);
D3DXMATRIX			getStockBone(std::string idBone, model * pModel);


int getPosAnimation(animation* pAnim, float time);
D3DXMATRIX getMatrixOfTime(D3DXMATRIX matrixBegin, D3DXMATRIX matrixEnd, float time, float timeNow);
std::vector<D3DXMATRIX> *collectMasMatrix(bufferObj *pBuffer);
void updateSkelet(object *pModel, bone* pBone);
void updateFrame(object *pModel);

bufferObj* createRndPanel(D3DXVECTOR2 pt);

#endif // !PROCESSING_H