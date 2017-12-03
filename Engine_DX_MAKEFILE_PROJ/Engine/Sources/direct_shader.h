#pragma once
#include "header.h"

class direct_shader
{
private:
	LPDIRECT3DPIXELSHADER9 pp_shader;
	LPDIRECT3DVERTEXSHADER9 pv_shader;

	LPD3DXBUFFER pv_shader_buffer;
	LPD3DXBUFFER pp_shader_buffer;

	ID3DXConstantTable* const_table_v;
	ID3DXConstantTable* const_table_p;
	D3DXHANDLE h_const_v;
	D3DXHANDLE h_const_p;
public:
	direct_shader();
	~direct_shader();
	void compile_shader(LPDIRECT3DDEVICE9, char* name_vs, char* name_ps);
	ID3DXConstantTable* get_const_table_p();
	ID3DXConstantTable* get_const_table_v();
	void set_shaders(LPDIRECT3DDEVICE9 p_direct3d_device, D3DXMATRIX m_view, D3DXMATRIX m_world, D3DXMATRIX m_proj);
};
