#include "direct_shader.h"

direct_shader::direct_shader()
{
}
direct_shader::~direct_shader()
{
}
void direct_shader::compile_shader(LPDIRECT3DDEVICE9 p_direct3d_device,char* name_vs, char* name_ps)
{
	D3DXCompileShaderFromFile(name_vs, 0, 0, "vs_main", "vs_2_0", D3DXSHADER_DEBUG, &pv_shader_buffer, NULL, &const_table_v);
	p_direct3d_device->CreateVertexShader((DWORD*)pv_shader_buffer->GetBufferPointer(), &pv_shader);
	pv_shader_buffer->Release();

	D3DXCompileShaderFromFile(name_ps, 0, 0, "ps_main", "ps_2_0", D3DXSHADER_DEBUG, &pp_shader_buffer, NULL, &const_table_p);
	p_direct3d_device->CreatePixelShader((DWORD*)pp_shader_buffer->GetBufferPointer(), &pp_shader);
	pp_shader_buffer->Release();
}
void direct_shader::set_shaders(LPDIRECT3DDEVICE9 p_direct3d_device, D3DXMATRIX m_view, D3DXMATRIX m_world, D3DXMATRIX m_proj)
{


	D3DXHANDLE  h_const_data_v;
	h_const_data_v = get_const_table_v()->GetConstantByName(0, "MatrixWorld");
	get_const_table_v()->SetMatrixArray(p_direct3d_device, h_const_data_v, &m_world, 4);

	h_const_data_v = get_const_table_v()->GetConstantByName(0, "MatrixProjection");
	get_const_table_v()->SetMatrixArray(p_direct3d_device, h_const_data_v, &m_proj, 4);

	h_const_data_v = get_const_table_v()->GetConstantByName(0, "MatrixView");
	get_const_table_v()->SetMatrixArray(p_direct3d_device, h_const_data_v, &m_view, 4);

	p_direct3d_device->SetVertexShader(pv_shader);
	p_direct3d_device->SetPixelShader(pp_shader);
}
ID3DXConstantTable* direct_shader::get_const_table_p()
{
	return const_table_p;
}
ID3DXConstantTable* direct_shader::get_const_table_v()
{
	return const_table_v;
}