#include "resources.h"

cResources resources;

cResources::cResources()
{
}

cResources::~cResources()
{
}

void cResources::setObject(void *pointer, flagObj objType)
{
	if (pointer == NULL) return;
	switch (objType)
	{
		case OT_MODEL:
		{
			model* pLocModel = (model*)pointer;
			for (UINT i = 0; i < pModels.size(); i++)
				if (pModels.at(i)->modelFullName == pLocModel->modelFullName) break;

			pModels.push_back((model*)pointer);
			break;
		}
		case OT_OBJECT:
		{
			object* pLocObject = (object*)pointer;
			for (UINT i = 0; i < pObjects.size(); i++)
				if (pObjects.at(i)->nameModel == pLocObject->nameModel) break;

			pObjects.push_back((object*)pointer);
			break;
		}
		case OT_SCENE:
		{
			scene* pLocScene = (scene*)pointer;
			for (UINT i = 0; i < pScene.size(); i++)
				if (pScene.at(i)->sceneName == pLocScene->sceneName) break;

			pScene.push_back((scene*)pointer);
			break;
		}
		case OT_TEXTURE:
		{
			texture* pLocTexture = (texture*)pointer;
			for (UINT i = 0; i < pTextures.size(); i++)
				if (pTextures.at(i)->textureName == pLocTexture->textureName) break;

			pTextures.push_back((texture*)pointer);
			break;
		}
		case OT_EFFECT:
		{
			sEffect* pLocEffect = (sEffect*)pointer;
			for (UINT i = 0; i < pEffects.size(); i++)
				if (pEffects.at(i)->pathName == pLocEffect->pathName) break;

			pEffects.push_back((sEffect*)pointer);
			break;
		}
		case OT_MUSIC:
		{
			pTracks.push_back((track*)pointer);
			break;
		}
		case OT_RENDERDATA:
		{
			pRndPatterns.push_back((sRenderData*)pointer);
			break;
		}
	}
}

void * cResources::getObject(std::string name, flagObj flag)
{
	switch (flag)
	{
		case OT_MODEL:
		{
			for (UINT i = 0; i < pModels.size(); i++)
			{
				if (pModels.at(i)->modelName == name)
					return pModels.at(i);
			}
			break;
		}
		case OT_OBJECT:
		{
			for (UINT i = 0; i < pObjects.size(); i++)
			{
				if (pObjects.at(i)->nameObject == name)
					return pObjects.at(i);
			}
			break;
		}
		case OT_SCENE:
		{
			for (UINT i = 0; i < pScene.size(); i++)
			{
				if (pScene.at(i)->sceneName == name)
					return pScene.at(i);
			}
			break;
		}
		case OT_TEXTURE:
		{
			for (UINT i = 0; i < pTextures.size(); i++)
			{
				if (pTextures.at(i)->textureName == name)
					return pTextures.at(i);
			}
			break;
		}
		case OT_EFFECT:
		{
			for (UINT i = 0; i < pEffects.size(); i++)
			{
				if (pEffects.at(i)->pathName == name)
					return pEffects.at(i);
			}
			break;
		}
		case OT_MUSIC:
		{
			for (UINT i = 0; i < pTracks.size(); i++)
			{
				if (pTracks.at(i)->trackName == name)
					return pTracks.at(i);
			}
			break;
		}
		case OT_RENDERDATA:
		{
			for (UINT i = 0; i < pRndPatterns.size(); i++)
			{
				if (pRndPatterns.at(i)->thechnique == name)
					return pRndPatterns.at(i);
			}
			break;
		}
	}
	
	return NULL;
}

void * cResources::getAll(flagObj flag) //Представляет собой вектор элементов выбранного флага example: std::vector<model*> pModels = (std::vector<model*>*)getAll(OT_MODEL);
{
	switch (flag)
	{
		case OT_MODEL:
		{
			return &pModels;
			break;
		}
		case OT_OBJECT:
		{
			return &pObjects;
			break;
		}
		case OT_SCENE:
		{
			return &pScene;
			break;
		}
		case OT_TEXTURE:
		{
			return &pTextures;
			break;
		}
		case OT_EFFECT:
		{
			return &pEffects;
			break;
		}
		case OT_MUSIC:
		{
			return &pTracks;
			break;
		}
		case OT_RENDERDATA:
		{
			return &pRndPatterns;
			break;
		}
	}

	return NULL;
}

void  cResources::deleteObject(std::string name, flagObj flag)
{
	switch (flag)
	{
		case OT_MODEL:
		{
			for (UINT i = 0; i < pModels.size(); i++)
			{
				if (pModels.at(i)->modelName == name)
				{
					pModels.at(i)->release();
					delete pModels.at(i);
					pModels.erase(pModels.begin() + i);
				}
			}
			break;
		}
		case OT_OBJECT:
		{
			for (UINT i = 0; i < pObjects.size(); i++)
			{
				if (pObjects.at(i)->nameObject == name)
				{
					delete pObjects.at(i);
					pObjects.erase(pObjects.begin() + i);
				}
			}
			break;
		}
		case OT_SCENE:
		{
			for (UINT i = 0; i < pScene.size(); i++)
			{
				if (pScene.at(i)->sceneName == name)
				{
					delete pScene.at(i);
					pScene.erase(pScene.begin() + i);
				}
			}
			break;
		}
		case OT_TEXTURE:
		{
			for (UINT i = 0; i < pTextures.size(); i++)
			{
				if (pTextures.at(i)->textureName == name)
				{
					pTextures.at(i)->data->Release();
					delete pTextures.at(i);
					pTextures.erase(pTextures.begin() + i);
					i = 0;
				}
			}
			break;
		}
		case OT_EFFECT:
		{
			for (UINT i = 0; i < pEffects.size(); i++)
			{
				if (pEffects.at(i)->pathName == name)
				{
					pEffects.at(i)->release();
					delete pEffects.at(i);
					pEffects.erase(pEffects.begin() + i);
				}
			}
			break;
		}
		case OT_MUSIC:
		{
			for (UINT i = 0; i < pTracks.size(); i++)
			{
				if (pTracks.at(i)->trackName == name)
				{
					delete pTracks.at(i);
					pTracks.erase(pTracks.begin() + i);
				}
			}
			break;
		}
		case OT_RENDERDATA:
		{
			for (UINT i = 0; i < pRndPatterns.size(); i++)
			{
				if (pRndPatterns.at(i)->thechnique == name)
				{
					//pRndPatterns.at(i)->release();
					delete pRndPatterns.at(i);
					pRndPatterns.erase(pRndPatterns.begin() + i);
				}
			}
			break;
		}
	}
	return;
}
void  cResources::deleteObject(void *pt, flagObj flag)
{
	switch (flag)
	{
		case OT_MODEL:
		{
			for (UINT i = 0; i < pModels.size(); i++)
			{
				if (pModels.at(i) == pt)
				{
					pModels.at(i)->release();
					delete pModels.at(i);
					pModels.erase(pModels.begin() + i);
					break;
				}
			}
			break;
		}
		case OT_OBJECT:
		{
			for (UINT i = 0; i < pObjects.size(); i++)
			{
				if (pObjects.at(i) == pt)
				{
					pObjects.at(i)->release();
					delete pObjects.at(i);
					pObjects.erase(pObjects.begin() + i);
					break;
				}
			}
			break;
		}
		case OT_SCENE:
		{
			for (UINT i = 0; i < pScene.size(); i++)
			{
				if (pScene.at(i) == pt)
				{
					delete pScene.at(i);
					pScene.erase(pScene.begin() + i);
					break;
				}
			}
			break;
		}
		case OT_TEXTURE:
		{
			for (UINT i = 0; i < pTextures.size(); i++)
			{
				if (pTextures.at(i) == pt)
				{
					pTextures.at(i)->data->Release();
					delete pTextures.at(i);
					pTextures.erase(pTextures.begin() + i);
					break;
				}
			}
			break;
		}
		case OT_EFFECT:
		{
			for (UINT i = 0; i < pEffects.size(); i++)
			{
				if (pEffects.at(i) == pt)
				{
					pEffects.at(i)->release();
					delete pEffects.at(i);
					pEffects.erase(pEffects.begin() + i);
					break;
				}
			}
			break;
		}
		case OT_MUSIC:
		{
			for (UINT i = 0; i < pTracks.size(); i++)
			{
				if (pTracks.at(i) == pt)
				{
					delete pTracks.at(i);
					pTracks.erase(pTracks.begin() + i);
					break;

				}
			}
			break;
		}
		case OT_RENDERDATA:
		{
			for (UINT i = 0; i < pRndPatterns.size(); i++)
			{
				if (pRndPatterns.at(i) == pt)
				{
					//pRndPatterns.at(i)->release();
					delete pRndPatterns.at(i);
					pRndPatterns.erase(pRndPatterns.begin() + i);
				}
			}
			break;
		}
	}

	return;
}
