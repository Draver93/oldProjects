#include "header.h"
//#define or || 
window_api wnd;
direct_api d_wnd;
direct_font d_font;
direct_control d_control;
direct_cam d_cam;

DWORD m_num_animation_set;		//element ainmation
c_animation_set *m_anim_set;	//element ainmation
parser_x test_mesh;
direct_shader d_shader;
LPDIRECT3DTEXTURE9 p_texture;

D3DXFRAME_EX *g_Frame;
D3DXMESHCONTAINER_EX *g_Mesh;

DWORD num_anim;		//element ainmation
c_animation_set *animation;	//element ainmation

LPDIRECT3DVERTEXDECLARATION9 pVD = NULL;

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

	load_mesh(&g_Mesh, &g_Frame, &num_anim, &animation, d_wnd.get_device(), "Test_model/tiny.x", "Test_model");
	test_mesh.set_animation(num_anim, animation);
	test_mesh.map_anim(g_Frame);

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
	d_cam.set_cam(d_wnd.get_device(), d_control.get_keyboard_state());
	
	static DWORD StartTime = timeGetTime();
	DWORD ThisTime = timeGetTime();
	test_mesh.update_anim(NULL, (ThisTime - StartTime) * 3, TRUE);

	if (g_Frame)
		g_Frame->update_hierarchy();
	update_mesh(g_Mesh);

	d_wnd.get_device()->SetTexture(0, p_texture);
	d_wnd.get_device()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	d_wnd.get_device()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	D3DXMATRIX  world_matrix = mesh_transform(d_wnd.get_device(), NULL, NULL, NULL, NULL, NULL, 0.2f);
	d_shader.set_shaders(d_wnd.get_device(), d_cam.get_view_matrix(), world_matrix, d_cam.get_proj_matrix());

	draw_model(g_Mesh);
	char str[1024];
	sprintf_s(str, sizeof(str), "%f, %f, %f", d_cam.get_cam_pos('x'), d_cam.get_cam_pos('y'), d_cam.get_cam_pos('z'));
	d_font.print_text(str, 20, 40, 1440, 900, D3DCOLOR_ARGB(170, 100, 255, 100));
	d_font.print_text("Позиция камеры:", 20, 20, 1440, 900, D3DCOLOR_ARGB(170, 100, 255, 100));
}
