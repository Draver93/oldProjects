#include "header.h"

window_api wnd;
direct_api d_wnd;
direct_font d_font;
direct_control d_control;
direct_cam d_cam;

parser_x test_mesh;
direct_shader d_shader;
LPDIRECT3DTEXTURE9 p_texture;

D3DXFRAME_EX *g_Frame;
D3DXMESHCONTAINER_EX *g_Mesh;
D3DXFRAME_EX *floor_f;
D3DXMESHCONTAINER_EX *floor_m;

animation g_anim;
//////////////////////////////////////////
DWORD maxVertInfluences = 0;
DWORD numBoneComboEntries = 0;
LPD3DXBUFFER boneComboTable = 0;
//////////////////////////////////////////
D3DVERTEXELEMENT9 p_decl[] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
	{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	{ 0, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
	{ 0, 48, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },

	D3DDECL_END()
};
LPDIRECT3DVERTEXDECLARATION9 p_vd;


void main_function()
{
	wnd.setup_window();
	d_wnd.create_direct(wnd.get_hwnd(), TRUE);
	d_font.create_font(d_wnd.get_device(), 18);

	d_shader.compile_shader(d_wnd.get_device(), "Shaders/VertexShader.vs", "Shaders/PixelShader.ps");
	D3DXCreateTextureFromFile(d_wnd.get_device(), "Test_model/Tiny_skin.bmp", &p_texture);

	d_control.init_device(get_h_inst());
	d_control.create_keyboard(wnd.get_hwnd());
	d_control.create_mouse(wnd.get_hwnd());

	load_mesh(&g_Mesh, &g_Frame, d_wnd.get_device(), "Test_model/tiny.x", "Test_model");

	g_Mesh->pSkinInfo->ConvertToIndexedBlendedMesh(g_Mesh->MeshData.pMesh, D3DXMESH_MANAGED | D3DXMESH_WRITEONLY,
		35, NULL, NULL, NULL, NULL, &maxVertInfluences, &numBoneComboEntries, &boneComboTable, &g_Mesh->p_skin_mesh);
	if (boneComboTable != NULL)
		boneComboTable->Release();

	load_mesh(&floor_m, &floor_f, d_wnd.get_device(), "Models/Mesh.x", "Test_model");
	g_anim.load("Test_model/tiny.x");
	g_anim.map(g_Frame);

	wnd.handler_proc();
}
void global_handler()
{
	d_control.keyboard_handler();
	d_control.mouse_handler();
	d_wnd.render_scene();
}

void global_render()
{
	static DWORD StartTime = timeGetTime();
	DWORD ThisTime = timeGetTime();
	d_cam.set_cam(d_wnd.get_device(), d_control.get_keyboard_state());
	d_wnd.get_device()->SetTexture(0, p_texture);
	d_wnd.get_device()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	d_wnd.get_device()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	d_wnd.get_device()->CreateVertexDeclaration(p_decl, &p_vd);



	int j = 1;
	int k = 1;
	int h = 0;
	for (DWORD i = 0; i < 1000; i++)
	{
		g_anim.update(NULL, (ThisTime - StartTime) * 3, TRUE);

		if (g_Frame)
			g_Frame->update_hierarchy();

		k++;
		if ((i % 10) == 0) j++;
		if ((i % 10) == 0) k = 1;
		if ((i % 100) == 0) { h++, j = 1; }
		D3DXMATRIX  world_matrix = mesh_transform(d_wnd.get_device(), NULL, NULL, 300.0f*k, 300.0f*h, 300.0f*j, 0.2f);
		LPDIRECT3DVERTEXSHADER9 p_vs = d_shader.set_shaders(d_wnd.get_device(), d_cam.get_view_matrix(), world_matrix, d_cam.get_proj_matrix(), g_Mesh, maxVertInfluences);

		update_mesh(g_Mesh);
		draw_model(g_Mesh, p_vs, p_vd);
	}
	j = 1;
	char str[1024];
	
	sprintf_s(str, sizeof(str), "%d, %d, %d", g_Mesh->MeshData.pMesh->GetNumVertices(), g_Mesh->MeshData.pMesh->GetNumFaces(), g_Mesh->pSkinInfo->GetNumBones());
	d_font.print_text(str, 20, 40, 1440, 900, D3DCOLOR_ARGB(170, 100, 255, 100));
	sprintf_s(str, sizeof(str), "%d", ThisTime-StartTime);
	d_font.print_text(str, 20, 60, 1440, 900, D3DCOLOR_ARGB(170, 100, 255, 100));
	d_font.print_text("Информация о модели:", 20, 20, 1440, 900, D3DCOLOR_ARGB(170, 100, 255, 100));
}
