#pragma once
#include "header.h"
#include "rmxfguid.h"

extern unsigned char D3DRM_XTEMPLATES[];
#define D3DRM_XTEMPLATE_BYTES 3278



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
	IDirect3DDevice9		*p_direct3d_device;
	char					*m_texture_path;
	DWORD					 m_new_FVF;
	DWORD					 m_load_flags;

	DWORD					 m_flag;

	D3DXMESHCONTAINER_EX	*m_root_mesh;
	D3DXFRAME_EX			*m_root_frame;

protected:
	BOOL parse_object(LPD3DXFILEDATA p_data_obj, LPD3DXFILEDATA p_data_ptr_obj, DWORD depth, void** data, BOOL reference);
	BOOL parse_child_object(LPD3DXFILEDATA p_data_obj, DWORD depth, void** data, BOOL reference = FALSE);
public:
	parser_x();
	~parser_x();
	BOOL parse(char *name, void** data = NULL);

	GUID get_obj_guid(LPD3DXFILEDATA p_data_obj);
	char *get_obj_name(LPD3DXFILEDATA p_data_obj);
	void *get_obj_data(LPD3DXFILEDATA p_data_obj, DWORD *size);
};
class parser
{
protected:
	virtual BOOL begin_parse(void **data) { return TRUE; }
	virtual BOOL end_parse(void **data) { return TRUE; }

	virtual BOOL parse_object(	LPD3DXFILEDATA p_data_obj,
								LPD3DXFILEDATA p_obj_ptr, 
								DWORD depth,
								void **data, BOOL reference)
	{
		return parse_child_object(p_data_obj, depth, data, reference);
	}
	BOOL parse_child_object(	LPD3DXFILEDATA p_data_obj,
							DWORD depth, void **data,
							BOOL force_reference = FALSE);
public:
	BOOL parse(char *file_name, void **data = NULL);

	GUID get_obj_guid(LPD3DXFILEDATA p_data_obj);
	char *get_obj_name(LPD3DXFILEDATA p_data_obj);
	void *get_obj_data(LPD3DXFILEDATA p_data_obj, DWORD *size);
};
class animation : public parser
{
protected:
	DWORD				m_num_animation_set;  // # animation sets
	c_animation_set		*m_anim_set;     // Animation sets

protected:
	BOOL parse_object(LPD3DXFILEDATA p_data_obj, LPD3DXFILEDATA p_data_ptr_obj, DWORD depth, void** data, BOOL reference);
	//D3DXFRAME_EX *findframe(D3DXFRAME *Frame, char *Name);
public:
	animation();
	~animation();

	void free();
	BOOL load(char *Filename);
	void map(D3DXFRAME_EX *RootFrame);
	void update(char *AnimationSetName, DWORD Time, BOOL Loop);
};
HRESULT load_mesh(	D3DXMESHCONTAINER_EX **pp_mesh, 
					D3DXFRAME_EX **pp_frame, 
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

HRESULT draw_model(D3DXMESHCONTAINER_EX *p_mesh, LPDIRECT3DVERTEXSHADER9 p_shader, LPDIRECT3DVERTEXDECLARATION9 p_decl);
HRESULT update_mesh(D3DXMESHCONTAINER_EX *p_mesh);
D3DXMATRIX mesh_transform(LPDIRECT3DDEVICE9, float angle_x, float angle_y, float pos_x, float pos_y, float pos_z, float size);
