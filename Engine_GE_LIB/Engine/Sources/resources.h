#ifndef RESOURCES_H
#define RESOURCES_H

#include <vector>
#include <string>
#include <xaudio2.h>
#include <d3d9.h>
#include <d3dx9.h>

enum TypeRender { TR_POINTS, TR_TRIANGLES, TR_LINES, TR_LINESTRIP, TR_NONE };
enum flagObj { OT_OBJECT, OT_MODEL, OT_TEXTURE, OT_MUSIC, OT_SCENE, OT_EFFECT, OT_RENDERDATA, OT_DEFAULT };
enum attrType {TYPE_VALUE, TYPE_POINTER};

struct param
{
	std::string name;
	float value;
	std::string strValue;
	param()
	{
		name.clear();
		strValue.clear();
		value = 0.0f;
	}
};
struct parameters
{
	std::string identifier;
	std::vector<param> info;
	param* findParam(std::string name)
	{
		if(this == NULL) return NULL;
		for (UINT i = 0; i < info.size(); i++)
			if (info.at(i).name == name)
				return &info.at(i);

		return NULL;
	}
	parameters()
	{
		identifier = "NONE";
	}
};

struct parameter_ex
{
	std::string name;
	std::vector<std::string> data;
	std::vector<parameter_ex> pSubParameters;
	parameter_ex* findParam(std::string name)
	{
		if (this == NULL) return NULL;
		for (UINT i = 0; i < pSubParameters.size(); i++)
			if (pSubParameters.at(i).name == name && !pSubParameters.at(i).data.empty())
				return &pSubParameters.at(i);
		return NULL;
	}
};

struct trans
{
	bool state;
	float x, y, z;
	float angleX, angleY, angleZ;

	float size;

	trans()
	{
		state = false;
		x = y = z = 0.0f;
		angleX = angleY = angleZ = 0.0f;
		size = 0;
	}
	~trans()
	{}
	const trans operator+(const trans& rv)
	{
		trans tmpTrans;
		tmpTrans.state = state;
		tmpTrans.x = x + rv.x;
		tmpTrans.y = y + rv.y;
		tmpTrans.z = z + rv.z;
		tmpTrans.size = size + rv.size;
		tmpTrans.angleX = angleX + rv.angleX;
		tmpTrans.angleY = angleY + rv.angleY;
		tmpTrans.angleZ = angleZ + rv.angleZ;

		return tmpTrans;
	}
	const trans operator-(const trans& rv)
	{
		trans tmpTrans;
		tmpTrans.state = state;
		tmpTrans.x = x - rv.x;
		tmpTrans.y = y - rv.y;
		tmpTrans.z = z - rv.z;
		tmpTrans.size = size - rv.size;
		tmpTrans.angleX = angleX - rv.angleX;
		tmpTrans.angleY = angleY - rv.angleY;
		tmpTrans.angleZ = angleZ - rv.angleZ;

		return tmpTrans;
	}
};
struct bone
{
	std::string idBone;
	std::string nameBone;
	std::string sid;

	D3DXMATRIX localMatrix;
	D3DXMATRIX mInversePos;
	D3DXMATRIX mIntrpolate;
	D3DXMATRIX mFinal;

	bone *pParentBone;
	bone *pSiblingBone;
	bone *pChildBone;
	bone()
	{
		nameBone = "None";
		idBone = "None";
		sid = "None";
		D3DXMatrixIdentity(&localMatrix);
		D3DXMatrixIdentity(&mInversePos);
		D3DXMatrixIdentity(&mIntrpolate);
		D3DXMatrixIdentity(&mFinal);
		pParentBone = NULL;
		pChildBone = NULL;
		pSiblingBone = NULL;
	}
	~bone()
	{

	}
	void release()
	{
		nameBone.clear();
		idBone.clear();
		sid.clear();
		std::string().swap(nameBone);
		std::string().swap(idBone);
		std::string().swap(sid);

		ZeroMemory(&localMatrix, sizeof(localMatrix));
		ZeroMemory(&mInversePos, sizeof(mInversePos));
		ZeroMemory(&mIntrpolate, sizeof(mIntrpolate));
		ZeroMemory(&mFinal, sizeof(mFinal));

		idBone.clear();
		nameBone.clear();
		sid.clear();
		if (pParentBone != NULL)
			pParentBone->release();
		if (pSiblingBone != NULL)
			pSiblingBone->release();
		if (pChildBone != NULL)
			pChildBone->release();
	}
	bone* findLSib(bone *pBone = NULL)
	{
		if (pBone == NULL)
		{
			if (this->pSiblingBone != NULL) return findLSib(this->pSiblingBone);
			else return this;
		}
		else
		{
			if (pBone->pSiblingBone != NULL) return findLSib(pBone->pSiblingBone);
			else return pBone;
		}
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

	std::vector<bone*> pMasBoneForShader;

	std::vector<BUFFER> rawVectorData;

	LPDIRECT3DVERTEXBUFFER9 p_vb_only;
	DWORD sizeVB_only;

	LPDIRECT3DVERTEXBUFFER9 p_vb;
	LPDIRECT3DINDEXBUFFER9	p_ib;
	DWORD sizeVB;
	DWORD sizeIB;

	bufferObj()
	{
		nameObj = "None";
		sizeVB = 0;
		sizeIB = 0;
		sizeVB_only = 0;
		p_vb_only = NULL;
		p_vb = NULL;
		p_ib = NULL;
	}
	~bufferObj()
	{

	}
	void release()
	{
		if (this == NULL) return;
		nameObj.clear();
		idObj.clear();

		std::string().swap(idObj);
		std::string().swap(nameObj);

		for (UINT i = 0; i < pMasBoneForShader.size(); i++)
			pMasBoneForShader.at(i)->release();
		pMasBoneForShader.clear();
		std::vector<bone*>().swap(pMasBoneForShader);

		rawVectorData.clear();
		std::vector<BUFFER>().swap(rawVectorData);

		sizeVB_only = 0;
		if (p_vb != NULL) { p_vb->Release(); p_vb = NULL; }
		if (p_ib != NULL){ p_ib->Release(); p_ib = NULL; }
		if (p_vb_only != NULL) { p_vb_only->Release(); p_vb_only = NULL; }

		sizeVB = 0;
		sizeIB = 0;
	}
};
struct patternBuf
{
	int pos;
	std::string semantic;
	std::string name;
	patternBuf()
	{}
	~patternBuf()
	{
		std::string().swap(semantic);
		std::string().swap(name);
	}

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
	source()
	{
		count = 0;
		stride = 0;
	}
	~source()
	{

	}
	void release()
	{
		count = 0;
		stride = 0;
		type.clear();
		arrayId.clear();
		sourceId.clear();
		dataName.clear();
		std::string().swap(arrayId);
		std::string().swap(sourceId);
		std::vector<std::string>().swap(dataName);

		data.clear();
		std::vector<float>().swap(data);

	}
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
	~input()
	{
	}
	void release()
	{
		semantic.clear();
		source.clear();
		std::string().swap(semantic);
		std::string().swap(source);
		offset = 0;

	}
};
struct vertices
{
	std::string verticesName;
	std::vector<input> inputSettings;
	vertices()
	{
	}
	~vertices()
	{
	}
	void release()
	{
		std::string().swap(verticesName);

		for (UINT i = 0; i < inputSettings.size(); i++)
			inputSettings.at(i).release();
		inputSettings.clear();
		std::vector<input>().swap(inputSettings);

		verticesName.clear();
	}
};
struct indexlist
{
	std::vector<input> inputSettings;
	std::vector<std::vector<float>> dataIndexes;
	std::vector<float> dataVcounts;
	indexlist()
	{
	}
	~indexlist()
	{
	}
	void release()
	{
		for (UINT i = 0; i < inputSettings.size(); i++)
			inputSettings.at(i).release();
		std::vector<input>().swap(inputSettings);

		inputSettings.clear();
		dataIndexes.clear();
		std::vector<std::vector<float>>().swap(dataIndexes);
		dataVcounts.clear();
		std::vector<float>().swap(dataVcounts);
	}
};
struct node
{
	std::string nodeSid, nodeId, nodeName, nodeType;

	D3DVECTOR location;
	D3DXVECTOR4 rotationX;
	D3DXVECTOR4 rotationY;
	D3DXVECTOR4 rotationZ;
	D3DVECTOR scale;

	bone * pRootBone;

	node()
	{
		ZeroMemory(&location, sizeof(D3DVECTOR));
		ZeroMemory(&rotationX, sizeof(D3DXVECTOR4));
		ZeroMemory(&rotationY, sizeof(D3DXVECTOR4));
		ZeroMemory(&rotationZ, sizeof(D3DXVECTOR4));
		ZeroMemory(&scale, sizeof(D3DVECTOR));

		nodeSid = nodeId = nodeName = nodeType = "NONE";
		pRootBone = NULL;
	}
	~node()
	{
	}
	void release()
	{
		ZeroMemory(&location, sizeof(D3DVECTOR));
		ZeroMemory(&rotationX, sizeof(D3DXVECTOR4));
		ZeroMemory(&rotationY, sizeof(D3DXVECTOR4));
		ZeroMemory(&rotationZ, sizeof(D3DXVECTOR4));
		ZeroMemory(&scale, sizeof(D3DVECTOR));
		nodeSid.clear();
		nodeId.clear();
		nodeName.clear();
		nodeType.clear();

		std::string().swap(nodeType);
		std::string().swap(nodeName);
		std::string().swap(nodeId);
		std::string().swap(nodeSid);

		if (pRootBone != NULL)
			pRootBone->release();
	}
};
struct visualGeometry
{
	std::string sceneId;
	std::vector<node> pNodes;
	visualGeometry()
	{
	}
	~visualGeometry()
	{
	}
	void release()
	{
		sceneId.clear();
		std::string().swap(sceneId);
		for (UINT i = 0; i < pNodes.size(); i++)
			pNodes.at(i).release();
		pNodes.clear();
		std::vector<node>().swap(pNodes);
	}
};
struct geometry
{
	std::string id, name, sourceGeom;

	vertices	veticesData;
	std::vector<source>		data;
	std::vector<indexlist>	indexData;

	geometry()
	{
		id = name = sourceGeom = "None";
	}
	~geometry()
	{
	}
	void release()
	{
		id.clear();
		name.clear();
		sourceGeom.clear();

		std::string().swap(sourceGeom);
		std::string().swap(name);
		std::string().swap(id);

		veticesData.release();
		for (UINT i = 0; i < data.size(); i++)
			data.at(i).release();
		data.clear();
		for (UINT i = 0; i < indexData.size(); i++)
			indexData.at(i).release();
		indexData.clear();
	}
};
struct material
{
	std::string nameMaterial;
	std::string idMaterial;

	std::string instanceEffect;
	std::string textureName;
	material()
	{
	}
	~material()
	{
	}
	void release()
	{
		nameMaterial.clear();
		idMaterial.clear();
		instanceEffect.clear();
		textureName.clear();

		std::string().swap(textureName);
		std::string().swap(instanceEffect);
		std::string().swap(idMaterial);
		std::string().swap(nameMaterial);
	}
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
	~effect()
	{}
	void release()
	{
		idEffect.clear();
		std::string().swap(idEffect);

		ZeroMemory(emission, sizeof(emission));
		ZeroMemory(ambient, sizeof(ambient));
		ZeroMemory(diffuse, sizeof(diffuse));
		shininess = 0;
		indexOfRefraction = 0;
	}
};
struct image
{
	std::string nameImage;
	std::string filePath;
	LPDIRECT3DTEXTURE9 textureData;
	image()
	{
	}
	~image()
	{
	}
};
struct animation
{
	std::string nameAnim;
	std::string nameObj;
	std::string typeAnim;

	std::vector<float> pDataTime;
	std::vector<float> pDataFloat;
	std::vector<D3DXMATRIX> pDataMatrix;
	std::vector<source> pData;
	std::vector<input> pInputs;
	animation()
	{
		nameAnim = "None";
		nameObj = "None";
		typeAnim = "None";
	}
	~animation()
	{
	}
	void release()
	{
		nameAnim.clear();
		nameObj.clear();
		typeAnim.clear();

		std::string().swap(nameAnim);
		std::string().swap(nameObj);
		std::string().swap(typeAnim);

		pDataTime.clear();
		pDataFloat.clear();
		pDataMatrix.clear();
		for (UINT i = 0; i < pData.size(); i++)
			pData.at(i).release();
		pData.clear();
		std::vector<source>().swap(pData);

		for (UINT i = 0; i < pInputs.size(); i++)
			pInputs.at(i).release();
		pInputs.clear();
		std::vector<input>().swap(pInputs);

	}

};
struct model
{
	std::string modelName;
	std::string modelFullName;

	std::vector<geometry>	pObjects;
	std::vector<geometry>	pControllers;
	std::vector<animation>	pAnimation;
	std::vector<material>	pMaterials;
	std::vector<image>		pImages;
	std::vector<effect>		pEffects;
	std::vector<node>		pNodes;
	std::vector<bufferObj*> pBuffer;
	model()
	{
	}
	~model()
	{
		//if (pDecl != NULL) pDecl->Release();
	}
	void release()
	{
		for (UINT i = 0; i < pObjects.size(); i++)
			pObjects.at(i).release();
		pObjects.clear();
		std::vector<geometry>().swap(pObjects);

		for (UINT i = 0; i < pControllers.size(); i++)
			pControllers.at(i).release();
		pControllers.clear();
		std::vector<geometry>().swap(pControllers);

		for (UINT i = 0; i < pAnimation.size(); i++)
			pAnimation.at(i).release();
		pAnimation.clear();
		std::vector<animation>().swap(pAnimation);

		for (UINT i = 0; i < pMaterials.size(); i++)
			pMaterials.at(i).release();
		pMaterials.clear();
		std::vector<material>().swap(pMaterials);

		pImages.clear();
		std::vector<image>().swap(pImages);

		for (UINT i = 0; i < pEffects.size(); i++)
			pEffects.at(i).release();
		pEffects.clear();
		std::vector<effect>().swap(pEffects);

		for (UINT i = 0; i < pNodes.size(); i++)
			pNodes.at(i).release();
		pNodes.clear();
		std::vector<node>().swap(pNodes);

		for (UINT i = 0; i < pBuffer.size(); i++)
			pBuffer.at(i)->release();
		pBuffer.clear();
		std::vector<bufferObj*>().swap(pBuffer);

	}
};
struct setAnimation
{
	bool activate;
	std::string animName;
	DWORD startTime;
	DWORD endTime;

	float speed;

	DWORD timeCounter;
	int timeLast;
	trans frameState;

	bool direction;
	bool loop;

	setAnimation()
	{
		activate = FALSE;
		timeCounter = 0;
		timeLast = 0;
		animName = "None";
		endTime = startTime = -1;
		direction = true;
		speed = 1.0f;
		loop = FALSE;
	}
	~setAnimation()
	{
	}
};
struct texture
{
	std::string textureName;
	LPDIRECT3DTEXTURE9 data;
	texture()
	{
	}
	~texture()
	{
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

struct shaderV
{
	std::string vertShaderName;
	LPDIRECT3DVERTEXSHADER9 pVertShader;
	LPD3DXCONSTANTTABLE pConstTableVert;
	shaderV()
	{}
	~shaderV()
	{}
	void release()
	{
		vertShaderName.clear();
		std::string().swap(vertShaderName);
		if (pVertShader != NULL)
			pVertShader->Release();
		if (pConstTableVert != NULL)
			pConstTableVert->Release();
	}
};
struct shaderP
{
	std::string pixShaderName;
	LPDIRECT3DPIXELSHADER9 pPixShader;
	LPD3DXCONSTANTTABLE pConstTablePix;

	shaderP()
	{}
	~shaderP()
	{}
	void release()
	{
		pixShaderName.clear();
		std::string().swap(pixShaderName);

		if (pPixShader != NULL)
			pPixShader->Release();
		if (pConstTablePix != NULL)
			pConstTablePix->Release();
	}
};

struct sTechnique
{
	D3DXHANDLE hl;
	D3DXTECHNIQUE_DESC desc;
	sTechnique()
	{
		hl = NULL;
	}
};
struct sGlobal
{
	D3DXHANDLE hl;
	D3DXPARAMETER_DESC desc;
	UINT offset;
	sGlobal()
	{
		hl = NULL;
		offset = 0;
	}

};
struct sEffect
{
	std::string pathName;
	LPD3DXEFFECT data;
	std::vector<sGlobal> globals;
	std::vector<sTechnique> techniques;

	void release()
	{
		pathName.clear();
		std::string().swap(pathName);
		if (data != NULL)
			data->Release();
	}
	sGlobal *getGlobalByName(std::string name)
	{
		for (UINT i = 0; i < globals.size(); i++)
		{
			if (globals.at(i).desc.Name == name) return &globals.at(i);
		}
		return NULL;
	}
	sTechnique *getTechByName(std::string name)
	{
		for (UINT i = 0; i < techniques.size(); i++)
		{
			if (techniques.at(i).desc.Name == name) return &techniques.at(i);
		}
		return NULL;
	}
};

struct attr
{
	std::string name;
	attrType type;
	std::vector<float> data_var;
	void* data_ptr;

	void setMatrix(D3DXMATRIX *mt)
	{
		data_var.clear();
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				data_var.push_back(mt->m[i][j]);
		type = TYPE_VALUE;
	}
	void setVar(float var)
	{
		data_var.clear();
		data_var.push_back(var);
		type = TYPE_VALUE;
	}
	void setMas(std::vector<float> *vec)
	{
		data_var.clear();
		data_var = *vec;
		type = TYPE_VALUE;
	}
	void setPtr(void* ptr)
	{
		data_ptr = ptr;
		type = TYPE_POINTER;
	}
};

struct sRenderData
{
	int(*plFunc)(void*);
	sEffect *pEffects;
	std::string thechnique;
	std::vector<attr> cAttr;

	sRenderData()
	{
		pEffects = NULL;
		plFunc = NULL;
	}
	~sRenderData()
	{
	}
	attr* findFxAttr(std::string name)
	{
		for (UINT i = 0; i < cAttr.size(); i++)
			if (cAttr.at(i).name == name) return &cAttr.at(i);
		return NULL;
	}

	void setAllUniform()
	{
		for (UINT i = 0; i < pEffects->globals.size(); i++)
		{
			sGlobal *pGl = &pEffects->globals.at(i);
			attr *pAttr = findFxAttr(pGl->desc.Name);
			if (pAttr != NULL)
			{
				switch (pAttr->type)
				{
					case TYPE_VALUE:
					{
						pEffects->data->SetValue(pGl->hl, pAttr->data_var.data(), pGl->desc.Bytes);
						break;
					}
					case TYPE_POINTER:
					{
						pEffects->data->SetValue(pGl->hl, pAttr->data_ptr, pGl->desc.Bytes);
						break;
					}
				}
			}
		}
	}

};

struct object : parameters
{
	int id;
	bool active;
	int(*handler)(object*);

	D3DPRIMITIVETYPE prType;
	std::string nameModel, nameObject;
	bufferObj *mesh;
	trans SettingsTransform;
	setAnimation SettingsAnimation;
	sRenderData plData;

	object* pParentObject;
	std::vector<object*> pChildObjects; //Do not add from push_back(), use the addSubObject()

	object()
	{
		id = -1;
		pParentObject = NULL;
		pChildObjects.clear();

		handler = NULL;
		prType = D3DPT_TRIANGLELIST;
		active = TRUE;
		mesh = NULL;
	}
	~object()
	{
		
	}
	//Чистит только шейдеры
	void release()
	{
		//vs_attr.clear();
		//ps_attr.clear();

		if(!pChildObjects.empty())
			for (UINT i = 0; i < pChildObjects.size(); i++)
				pChildObjects.at(i)->release();

		delete this;
	}
	std::vector<object*> getAllSubObj()
	{
		if (this->pChildObjects.empty()) return std::vector<object*>();
		std::vector<object*> tmpVec;

		object *tmpObj = this;
		for (UINT i = 0; i < tmpObj->pChildObjects.size(); i++)
		{
			tmpVec.push_back(tmpObj->pChildObjects.at(i));

			std::vector<object*> tmpVec2 = tmpObj->pChildObjects.at(i)->getAllSubObj();
			if(!tmpVec2.empty())
				tmpVec.insert(tmpVec.end(), tmpVec2.begin(), tmpVec2.end());
		}
		return tmpVec;
	}
	void addSubObject(object* obj)
	{
		obj->pParentObject = this;
		pChildObjects.push_back(obj);
		
	}
	trans getSumTrans()
	{
		object *tmpObj = this;
		trans tmpTrans;
		do
		{
			tmpTrans = tmpTrans + tmpObj->SettingsTransform;
			tmpObj = tmpObj->pParentObject;
		} while (tmpObj != NULL);
		return tmpTrans;
	}
	/*attr* findPixAttr(std::string name)
	{
		for (UINT i = 0; i < ps_attr.size(); i++)
			if (ps_attr.at(i).name == name) return &ps_attr.at(i);
		return NULL;
	}
	attr* findVecAttr(std::string name)
	{
		for (UINT i = 0; i < vs_attr.size(); i++)
			if (vs_attr.at(i).name == name) return &vs_attr.at(i);
		return NULL;
	}*/
	attr* findFxAttr(std::string name)
	{
		for (UINT i = 0; i < plData.cAttr.size(); i++)
			if (plData.cAttr.at(i).name == name) return &plData.cAttr.at(i);
		return NULL;
	}
};
struct sDFElement
{
	object *data;
	D3DXMATRIX wMatrix;
};
struct scene
{
	int nextId;
	bool active;
	D3DPRIMITIVETYPE prType;
	std::string sceneName;
	std::vector<object*> objects;
	scene()
	{
		prType = D3DPT_FORCE_DWORD;
		nextId = 0;
		active = TRUE;
	}
	~scene()
	{}
	void addModel(model *tmpModel)
	{
		for (UINT i = 0; i < tmpModel->pBuffer.size(); i++)
		{
			object *tmpObj = new object();
			tmpObj->id = nextId;
			tmpObj->nameModel = tmpModel->modelName;
			tmpObj->nameObject = tmpModel->pBuffer.at(i)->nameObj;
			tmpObj->mesh = tmpModel->pBuffer.at(i);
			objects.push_back(tmpObj);
		}
		nextId += 1;
	}
	void addObj(object *tmpObject)
	{
		tmpObject->id = nextId;
		nextId++;
		objects.push_back(tmpObject);
	}
};

class cResources
{
private:
	std::vector<track*> pTracks;
	std::vector<texture*> pTextures;
	std::vector<model*> pModels;
	std::vector<object*> pObjects;
	std::vector<scene*> pScene;
	std::vector<sEffect*> pEffects;
	std::vector<sRenderData*> pRndPatterns;


public:

	cResources();
	~cResources();
	void setObject(void *pointer, flagObj flag);
	void * getObject(std::string name, flagObj flag);

	void * cResources::getAll(flagObj flag);
	void deleteObject(std::string name, flagObj flag);
	void deleteObject(void* pt, flagObj flag);

};
extern cResources resources;

#endif