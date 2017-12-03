#pragma once
#include "header.h"
#include "rmxfguid.h"

extern unsigned char D3DRM_XTEMPLATES[];
#define D3DRM_XTEMPLATE_BYTES 3278
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
		Name				= NULL;
		MeshData.pMesh		= NULL;
		pMaterials			= NULL;
		pEffects			= NULL;
		NumMaterials		= NULL;
		pAdjacency			= NULL;
		pSkinInfo			= NULL;
		pNextMeshContainer	= NULL;
		p_textures			= NULL;
		p_skin_mesh			= NULL;
		pp_frame_matrices	= NULL;
		p_bone_matrices		= NULL;
	}
	~D3DXMESHCONTAINER_EX()
	{
		if (p_textures && NumMaterials)
		{
			for (DWORD i = 0; i < NumMaterials; i++) ReleaseCOM(p_textures[i]);
		}
		delete[] p_textures;			p_textures	= NULL;
		NumMaterials = 0;
		delete[] Name;					Name				= NULL;
		delete[] pMaterials;			pMaterials			= NULL;
		delete pEffects;				pEffects			= NULL;

		delete[] pAdjacency;			pAdjacency			= NULL;
		delete[] pp_frame_matrices;		pp_frame_matrices	= NULL;
		delete[] p_bone_matrices;		p_bone_matrices		= NULL;

		ReleaseCOM(MeshData.pMesh);
		ReleaseCOM(p_skin_mesh);
		ReleaseCOM(pSkinInfo);

		delete pNextMeshContainer;		pNextMeshContainer	= NULL;
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

struct c_anim_vec_key
{
	DWORD m_time;
	D3DXVECTOR3 m_vec_key;
};		//element ainmation
struct c_anim_quat_key
{
	DWORD m_time;
	D3DXQUATERNION m_quat_key;
};		//element ainmation
struct c_anim_matrix_key
{
	DWORD m_time;
	D3DXMATRIX m_mat_key;
};		//element ainmation

struct c_animation
{
	char			*m_name;
	D3DXFRAME_EX	*m_bone;
	c_animation		*m_next;

	DWORD               m_num_translation_keys;
	c_anim_vec_key     *m_translation_keys;

	DWORD               m_num_scale_keys;
	c_anim_vec_key     *m_scale_keys;

	DWORD               m_num_rotation_keys;
	c_anim_quat_key	   *m_rotation_keys;

	DWORD               m_num_matrix_keys;
	c_anim_matrix_key  *m_matrix_keys;

	c_animation()
	{
		m_name = NULL;
		m_bone = NULL;
		m_next = NULL;

		m_num_matrix_keys		= 0;
		m_num_rotation_keys		= 0;
		m_num_scale_keys		= 0;
		m_num_translation_keys	= 0;

		m_matrix_keys		= NULL;
		m_rotation_keys		= NULL;
		m_scale_keys		= NULL;
		m_translation_keys	= NULL;
	}
	~c_animation()
	{
		delete[] m_name; 
		m_name = NULL;
		delete m_next; 
		m_next = NULL;

		delete[] m_scale_keys;
		m_scale_keys		= NULL;
		delete[] m_translation_keys;
		m_translation_keys	= NULL;
		delete[] m_rotation_keys;
		m_rotation_keys		= NULL;
		delete[] m_matrix_keys;
		m_matrix_keys		= NULL;
	}
};			//element ainmation
struct c_animation_set
{
	char *m_name;
	DWORD m_lenght;
	c_animation_set *m_next;

	DWORD m_num_animations;
	c_animation *m_animations;

	c_animation_set()
	{
		m_name			= NULL;
		m_lenght			= 0;
		m_next			= NULL;

		m_num_animations	= 0;
		m_animations	= NULL;
	}
	~c_animation_set()
	{
		delete[] m_name;
		delete m_next;
		delete m_animations;

		m_name = NULL;
		m_next = NULL;
		m_animations = NULL;
	}
};		//element ainmation

class parser_x
{
public:
	IDirect3DDevice9	*p_direct3d_device;
	DWORD				 m_flag;
	char                 *m_texture_path;
	DWORD                m_new_FVF;
	DWORD                m_load_flags;

	D3DXMESHCONTAINER_EX	*m_root_mesh;
	D3DXFRAME_EX			*m_root_frame;
	DWORD m_num_animation_set;		//element ainmation
	c_animation_set *m_anim_set;	//element ainmation

protected:
	BOOL parse_object(LPD3DXFILEDATA p_data_obj, LPD3DXFILEDATA p_data_ptr_obj, DWORD depth, void** data, BOOL reference);
	BOOL parse_child_object(LPD3DXFILEDATA p_data_obj, DWORD depth, void** data, BOOL reference = FALSE);
public:
	parser_x();
	~parser_x();
	BOOL parse(char *name, void** data = NULL);

	void free();														//element ainmation
	void map_anim(D3DXFRAME_EX *root_frame);							//element ainmation
	void update_anim(char *anim_set_name, DWORD time, BOOL loop);		//element ainmation
	void set_animation(DWORD num_anim, c_animation_set *animation);

	GUID get_obj_guid(LPD3DXFILEDATA p_data_obj);
	char *get_obj_name(LPD3DXFILEDATA p_data_obj);
	void *get_obj_data(LPD3DXFILEDATA p_data_obj, DWORD *size);
};

HRESULT load_mesh(	D3DXMESHCONTAINER_EX **pp_mesh, 
					D3DXFRAME_EX **pp_frame, 
					DWORD *num_animation_set, 
					c_animation_set **m_anim_set,
					LPDIRECT3DDEVICE9 p_direct3d_device, 
					char *file_name, 
					char *texture_path, 
					DWORD new_FVF = 0, 
					DWORD load_flags = D3DXMESH_SYSTEMMEM);
HRESULT load_mesh(	D3DXMESHCONTAINER_EX **pp_mesh, 
					LPDIRECT3DDEVICE9 p_direct3d_device, 
					LPD3DXFILEDATA p_data_obj, 
					char *texture_path,
					DWORD new_FVF = 0,
					DWORD load_flags = D3DXMESH_SYSTEMMEM );

HRESULT draw_model(D3DXMESHCONTAINER_EX *p_mesh);
HRESULT update_mesh(D3DXMESHCONTAINER_EX *p_mesh);
D3DXMATRIX mesh_transform(LPDIRECT3DDEVICE9, float angle_x, float angle_y, float pos_x, float pos_y, float pos_z, float size);
