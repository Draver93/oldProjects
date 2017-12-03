#pragma once
#include <windows.h>

#include <fstream>
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9core.h>
#include <d3dx9mesh.h>
#include <dinput.h>
#include <stdio.h>
#include <xaudio2.h>

#include <fstream>
#include <string.h>
#include <conio.h>


#pragma comment(lib, "d3d9.lib") 
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#define KEYDOWN(name, key)(name[key]&0x80)
#define ReleaseCOM(x) { if(x!=NULL) x->Release(); x=NULL; }

struct D3DXFRAME_EX : D3DXFRAME
{
	D3DXMATRIX mat_combine;
	D3DXMATRIX mat_origin;

	D3DXFRAME_EX()
	{
		Name = NULL;
		pMeshContainer = NULL;
		pFrameSibling = pFrameFirstChild = NULL;
		D3DXMatrixIdentity(&mat_combine);
		D3DXMatrixIdentity(&mat_origin);
		D3DXMatrixIdentity(&TransformationMatrix);
	}
	~D3DXFRAME_EX()
	{
		delete[] Name;				Name = NULL;
		delete pFrameFirstChild;	pFrameFirstChild = NULL;
		delete pFrameSibling;		pFrameSibling = NULL;
	}
	D3DXFRAME_EX *find(const char *frame_name)
	{
		D3DXFRAME_EX *p_frame, *p_frame_ptr;

		if (Name && frame_name && !strcmp(frame_name, Name)) return this;
		if (p_frame_ptr = (D3DXFRAME_EX*)pFrameSibling)
		{
			if (p_frame = p_frame_ptr->find(frame_name)) return p_frame;
		}
		if (p_frame_ptr = (D3DXFRAME_EX*)pFrameFirstChild)
		{
			if (p_frame = p_frame_ptr->find(frame_name)) return p_frame;
		}

		return NULL;
	}
	void reset()
	{
		D3DXFRAME_EX *p_frame_ptr;

		TransformationMatrix = mat_origin;
		if (p_frame_ptr = (D3DXFRAME_EX*)pFrameSibling) p_frame_ptr->reset();
		if (p_frame_ptr = (D3DXFRAME_EX*)pFrameFirstChild) p_frame_ptr->reset();
	}
	void update_hierarchy(D3DXMATRIX *mat_transform = NULL)
	{
		D3DXFRAME_EX *p_frame_ptr;
		D3DXMATRIX mat_identity;
		if (!mat_transform)
		{
			D3DXMatrixIdentity(&mat_identity);
			mat_transform = &mat_identity;
		}
		mat_combine = TransformationMatrix *(*mat_transform);
		if (p_frame_ptr = (D3DXFRAME_EX*)pFrameSibling)		p_frame_ptr->update_hierarchy(mat_transform);
		if (p_frame_ptr = (D3DXFRAME_EX*)pFrameFirstChild)	p_frame_ptr->update_hierarchy(&mat_combine);
	}
	void count(DWORD *num)
	{
		if (!num) return;
		(*num) += 1;
		D3DXFRAME_EX *p_frame;

		if (p_frame = (D3DXFRAME_EX*)pFrameSibling)		p_frame->count(num);
		if (p_frame = (D3DXFRAME_EX*)pFrameFirstChild)	p_frame->count(num);

	}
};
struct D3DXMESHCONTAINER_EX : D3DXMESHCONTAINER
{
	IDirect3DTexture9	**p_textures;
	ID3DXMesh			*p_skin_mesh;

	D3DXMATRIX			**pp_frame_matrices;
	D3DXMATRIX			*p_bone_matrices;

	D3DXMESHCONTAINER_EX()
	{
		Name = NULL;
		MeshData.pMesh = NULL;
		pMaterials = NULL;
		pEffects = NULL;
		NumMaterials = 0;
		pAdjacency = NULL;
		pSkinInfo = NULL;
		pNextMeshContainer = NULL;
		p_textures = NULL;
		p_skin_mesh = NULL;
		pp_frame_matrices = NULL;
		p_bone_matrices = NULL;
	}
	~D3DXMESHCONTAINER_EX()
	{
		if (p_textures && NumMaterials)
		{
			for (DWORD i = 0; i < NumMaterials; i++) ReleaseCOM(p_textures[i]);
		}
		delete[] p_textures;			p_textures = NULL;
		NumMaterials = 0;
		delete[] Name;					Name = NULL;
		delete[] pMaterials;			pMaterials = NULL;
		delete pEffects;				pEffects = NULL;

		delete[] pAdjacency;			pAdjacency = NULL;
		delete[] pp_frame_matrices;		pp_frame_matrices = NULL;
		delete[] p_bone_matrices;		p_bone_matrices = NULL;

		ReleaseCOM(MeshData.pMesh);
		ReleaseCOM(p_skin_mesh);
		ReleaseCOM(pSkinInfo);

		delete pNextMeshContainer;		pNextMeshContainer = NULL;
	}

	D3DXMESHCONTAINER_EX *find(char *mesh_name)
	{
		D3DXMESHCONTAINER_EX *p_mesh, *p_mesh_ptr;
		if (mesh_name && Name && !strcmp(mesh_name, Name))return this;
		if (p_mesh_ptr = (D3DXMESHCONTAINER_EX*)pNextMeshContainer)
		{
			if (p_mesh = p_mesh_ptr->find(mesh_name)) return p_mesh;
		}

		return NULL;
	}
};

#include "window_api.h"
#include "direct_api.h"
#include "direct_font.h"
#include "direct_control.h"
#include "direct_cam.h"
#include "direct_mesh.h"
#include "direct_shader.h"

//main.cpp
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT CALLBACK win_proc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE get_h_inst();
LPSTR get_lp_str();
int get_n_cmd_show();


//make_components.cpp
void main_function();
void global_handler();
void global_render();
void const_shader();