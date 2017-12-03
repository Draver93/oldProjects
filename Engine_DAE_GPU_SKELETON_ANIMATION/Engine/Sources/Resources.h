#ifndef RESOURCES_H
#define RESOURCES_H

#include <windows.h>
#include <d3dx9math.h>
#include <vector>
#include <string>
#include <xaudio2.h>
#include <fstream>
#include <thread>

#include "Init_Window.h"
#include "Handler_Devices.h"

#pragma comment(lib, "winmm.lib") //для получения времени 

enum flagObj { OT_MODEL, OT_TEXTURE, OT_MUSIC, OT_VSHADER, OT_PSHADER, OT_DEFAULT };
enum typeVar { TV_INT, TV_FLOAT4X4, TV_FLOAT, TV_MASFLOAT4X4};

struct skeletData
{
	std::string idBone;
	std::string nameBone;
	std::string sid;

	D3DXMATRIX localMatrix;
	D3DXMATRIX mInversePos;
	D3DXMATRIX mIntrpolate;
	D3DXMATRIX mFinal;

	skeletData *pParentData;
	skeletData *pSiblingData;
	skeletData *pChildData;
	skeletData()
	{
		nameBone = "None";
		idBone = "None";
		sid = "None";
		D3DXMatrixIdentity(&localMatrix);
		D3DXMatrixIdentity(&mInversePos);
		D3DXMatrixIdentity(&mIntrpolate);
		D3DXMatrixIdentity(&mFinal);
		pParentData = NULL;
		pChildData = NULL;
		pSiblingData = NULL;
	}
	~skeletData()
	{

	}
};
struct trans
{
	bool state;
	float x, y, z;
	float angleX, angleY;
	float size;
	trans()
	{
		state = false;
		x = y = z = 0.0f;
		angleX = angleY = 0.0f;
		size = 1.0f;
	}
};
struct BUFFER
{
	FLOAT x, y, z;
	FLOAT nx, ny, nz;
	FLOAT tu, tv;
	FLOAT weights[4];
	FLOAT indeces[4];
	BUFFER()
	{
		x = 0.0f, y = 1000.0f, z = 0.0f;
		nx = 0.0f, ny = 0.0f, nz = 0.0f;
		tu = 0.0f, tv = 0.0f;
		weights[0] = 0.0f;
		weights[1] = 0.0f;
		weights[2] = 0.0f;
		weights[3] = 0.0f;
		indeces[0] = 0.0f;
		indeces[1] = 0.0f;
		indeces[2] = 0.0f;
		indeces[3] = 0.0f;
	}
	~BUFFER()
	{}
};
struct bufferObj
{
	std::string nameObj;
	std::string idObj;

	trans baseTransform;
	LPDIRECT3DTEXTURE9 pTexture;
	std::string vShaderName;
	LPDIRECT3DVERTEXSHADER9 pVshader;
	
	std::string pShaderName;
	LPDIRECT3DPIXELSHADER9 pPshader;

	std::vector<skeletData*> *pMasBoneForShader;

	LPDIRECT3DVERTEXDECLARATION9 pDecl;

	LPDIRECT3DVERTEXBUFFER9 p_vb_only;
	DWORD sizeVB_only;

	LPDIRECT3DVERTEXBUFFER9 p_vb;
	LPDIRECT3DINDEXBUFFER9	p_ib;
	DWORD sizeVB;
	DWORD sizeIB;

	bufferObj()
	{
		nameObj = "None";
		vShaderName = "None";
		pShaderName = "None";
		pTexture = NULL;
		pPshader = NULL;
		pVshader = NULL;
		pDecl = NULL;

		pMasBoneForShader = new std::vector<skeletData*>();

		sizeVB = 0;
		sizeIB = 0;
		sizeVB_only = 0;
		p_vb = NULL;
		p_ib = NULL;
	}
	~bufferObj()
	{
	}
};
struct patternBuf
{
	int pos;
	std::string semantic;
	std::string name;
};

struct source
{
	std::string sourceId;
	std::string arrayId;
	std::vector<float> data;
	std::vector<std::string> dataName;
	DWORD count;
	DWORD stride;
	std::string type;
};

struct input
{
	std::string semantic;
	std::string	source;
	int offset;
	input()
	{
		offset = -1;
	}
};
struct vertices
{
	std::string verticesName;
	std::vector<input>* inputSettings;
	vertices()
	{
		inputSettings = new std::vector<input>();
	}
};
struct indexlist
{
	std::vector<input>* inputSettings;
	std::vector<std::vector<float>>* dataIndexes;
	std::vector<float>* dataVcounts;
	indexlist()
	{
		dataIndexes = new std::vector<std::vector<float>>();
		dataVcounts = new std::vector<float>();
		inputSettings = new std::vector<input>();
	}
};

struct node
{
	std::string nodeId;
	D3DXMATRIXA16 transform;
	std::vector<std::string> *pMaterials;
	node()
	{
		pMaterials = new std::vector<std::string>();
	}
	~node()
	{
	}
};
struct visualGeometry
{
	std::string sceneId;
	std::vector<node>* pNodes;
	visualGeometry()
	{
		pNodes = new std::vector<node>();
	}
	~visualGeometry()
	{
	}
};
struct geometry
{
	std::string id, name, sourceGeom;

	vertices	*veticesData;
	std::vector<source>		*data;	
	std::vector<indexlist>	*indexData;

	geometry()
	{
		indexData	= new std::vector<indexlist>();
		data		= new std::vector<source>();
		veticesData = new vertices();
		id = name = sourceGeom = "None";
	}
	~geometry()
	{}
};
struct material
{
	std::string nameMaterial;
	std::string idMaterial;

	std::string instanceEffect;
	std::string textureName;
};
struct effect
{
	std::string idEffect;
	D3DXVECTOR4 emission;
	D3DXVECTOR4 ambient;
	D3DXVECTOR4 diffuse;
	float shininess;
	float indexOfRefraction;
	effect()
	{
		idEffect = "None";
		emission = { 0, 0, 0, 0 };
		ambient = { 0, 0, 0, 0 };
		diffuse = { 0, 0, 0, 0 };
		shininess = 0;
		indexOfRefraction = 0;
	}
};
struct image
{
	std::string nameImage;
	std::string filePath;
	LPDIRECT3DTEXTURE9 textureData;
};

struct animation
{
	std::string nameAnimation;
	std::string nameBone;
	int timeCounter;
	int timeLast;
	bool stateAnim;
	float speedAnim;
	bool directAnim; // true ahead false back

	std::vector<float> *pDataTime;
	std::vector<D3DXMATRIX> *pDataMatrix;
	std::vector<source> *pData;
	std::vector<input> *pInputs;
	animation()
	{
		timeCounter = 0;
		timeLast = 0;
		nameAnimation = "None";
		nameBone = "None";
		stateAnim = false;
		directAnim = true;
		speedAnim = 1.0f;
		pDataTime = new std::vector<float>();
		pDataMatrix = new std::vector<D3DXMATRIX>();
		pData = new std::vector<source>();
		pInputs = new std::vector<input>();
	}
	~animation()
	{
	}

};

struct model
{
	std::string modelName;

	std::vector<geometry>*	pObjects;
	std::vector<geometry>*	pControllers;
	std::vector<animation>* pAnimation;
 	std::vector<material>*	pMaterials;
	std::vector<image>*		pImages;
	std::vector<effect>*	pEffects;
	skeletData *pSkelet;

	std::vector<bufferObj>* pBuffer;
	model()
	{
		pObjects	= new std::vector<geometry>();
		pControllers = new std::vector<geometry>();
		pAnimation	= new std::vector<animation>();
		pMaterials	= new std::vector<material>();
		pEffects	= new std::vector<effect>();
		pImages		= new std::vector<image>();
		pBuffer		= new std::vector<bufferObj>();

		pSkelet		= NULL;
	}
	~model()
	{}
	void SetAnimParam(std::string name, bool state, float speed, bool direct)
	{
		for (UINT i = 0; i < pAnimation->size(); i++)
		{
			if (pAnimation->at(i).nameAnimation == name)
			{
				pAnimation->at(i).stateAnim = state;
				pAnimation->at(i).speedAnim = speed;
				pAnimation->at(i).directAnim = direct;
				break;
			}
		}
	}
};
struct track
{
	WAVEFORMATEX mWF;
	XAUDIO2_BUFFER mXA;
	IXAudio2SourceVoice *gSource;
	IXAudio2MasteringVoice *gMaster;
	IXAudio2 *gEngine;

	std::string trackName;
	track()
	{
		ZeroMemory(&mXA, sizeof(mXA));
		ZeroMemory(&mWF, sizeof(mWF));
		gSource = NULL;
		gMaster = NULL;
		gEngine = NULL;
		trackName = "None";
	}
	~track()
	{
	}
};
struct texture
{
	std::string textureName;
	LPDIRECT3DTEXTURE9 data;
};
struct shaderP
{
	std::string pixShaderName;
	LPDIRECT3DPIXELSHADER9 pPixShader;
	LPD3DXCONSTANTTABLE pConstTablePix;
};
struct shaderV
{
	std::string vertShaderName;
	LPDIRECT3DVERTEXSHADER9 pVertShader;
	LPD3DXCONSTANTTABLE pConstTableVert;
};

class resources
{
private:

	std::vector<track> *pTracks;
	std::vector<texture> *pTextures;
	std::vector<shaderV> *pShadersV;
	std::vector<shaderP> *pShadersP;

	HRESULT loadTrack( std::string nameFile );
	HRESULT loadModel( std::string nameFile, LPDIRECT3DDEVICE9 pDevice);
	HRESULT loadTexture( std::string nameFile, LPDIRECT3DDEVICE9 pDevice);
	HRESULT loadPixShaders( std::string nameFile, LPDIRECT3DDEVICE9 pDevice);
	HRESULT loadVertShaders( std::string nameFile, LPDIRECT3DDEVICE9 pDevice);

	std::string getParam( std::string source, std::string param );
	float		getArrElem( geometry *tmpGeom, std::string name, DWORD pos, DWORD id );
	HRESULT		getArray( std::string rawData, std::vector<float>* tmpArray );
	HRESULT		getArrayName(std::string rawData, std::vector<std::string>* tmpArray);
	void		convertData( model *data, LPDIRECT3DDEVICE9 pDevice);
	void		setWeight(model *data, UINT iPos, BUFFER *pBuffer, std::string idGeom);
	void		parserSkel(std::fstream * file_data, skeletData *tmpSkelet, D3DXMATRIX *mParent, bool state);
public:
	std::vector<model> *pModels;
	resources();
	~resources();
	HRESULT setTexture(std::string modelName, std::string objName, std::string textureName);
	HRESULT setShaderVert(std::string modelName, std::string objName, std::string shaderName);
	HRESULT setDecl(std::string modelName, std::string objName, D3DVERTEXELEMENT9 *pDecl, LPDIRECT3DDEVICE9 pDevice);
	HRESULT setShaderPix(std::string modelName, std::string objName, std::string shaderName);
	HRESULT setShaderVal(void* data, std::string nameVar, std::string nameShader, LPDIRECT3DDEVICE9 pDevice, typeVar type);
	HRESULT loadObject( std::string name, flagObj flag, LPDIRECT3DDEVICE9 pDevice = NULL);
	HRESULT deleteObject( std::string name, flagObj flag );
	void* getAllObject( flagObj flag );
	void* getObject( std::string name, flagObj flag );
};

std::string formatNum1(std::string* Str);
void formatData2(source *tmpSource, std::vector<D3DXMATRIX> *pDataMatrix);

skeletData* getPointerToBone(skeletData* pSkelet, std::string name);
std::vector<skeletData*> *InitMasBoneForShader(model * pModel, std::string objId);

void setInverseMatrix(model *pModel, skeletData *pBone);
D3DXMATRIX* getStockBone(std::string idBone, model * pModel);

#endif