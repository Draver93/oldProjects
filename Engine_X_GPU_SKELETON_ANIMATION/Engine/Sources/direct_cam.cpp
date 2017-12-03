#include "direct_cam.h"


direct_cam::direct_cam()
{
	//pos_x = -236;
	//pos_y = 53;
	//pos_z = 181;
}
direct_cam::~direct_cam()
{
}

void direct_cam::set_cam(LPDIRECT3DDEVICE9 p_direct3d_device, char* keyboard_state)
{
	new_time = timeGetTime();
	mouse_control(keyboard_state);
	old_time = timeGetTime();
	D3DXMatrixLookAtLH(&matrix_view, &D3DXVECTOR3(pos_x, pos_y, pos_z),
									&D3DXVECTOR3(look_x, look_y, look_z),
									&D3DXVECTOR3(0, 1, 0));
	D3DXMatrixPerspectiveFovLH(&matrix_projection, D3DX_PI / 4, 16.0f / 10.0f, 1.0f, 10000.0f);
}
void direct_cam::mouse_control(char* keyboard_state)
{
	DWORD time = new_time - old_time;
	if (KEYDOWN(keyboard_state, DIK_W))
	{
		pos_x += time * sin(rad_y)*sin(rad_x);
		pos_y += time * cos(rad_y);
		pos_z += time * sin(rad_y)*cos(rad_x);
	}
	if (KEYDOWN(keyboard_state, DIK_S))
	{
		pos_x -= time * sin(rad_y)*sin(rad_x);
		pos_y -= time * cos(rad_y);
		pos_z -= time * sin(rad_y)*cos(rad_x);
	}
	if (KEYDOWN(keyboard_state, DIK_A))
	{
		pos_x += time * sin(rad_x - (180 * (D3DX_PI / 360)));
		pos_z += time * cos(rad_x - (180 * (D3DX_PI / 360)));
	}
	if (KEYDOWN(keyboard_state, DIK_D))
	{
		pos_x -= time * sin(rad_x - (180 * (D3DX_PI / 360)));
		pos_z -= time * cos(rad_x - (180 * (D3DX_PI / 360)));
	}


	GetCursorPos(&cursor_new);
	if (cursor_new.x > 600) SetCursorPos(20, cursor_new.y);
	if (cursor_new.y > 400) SetCursorPos(cursor_new.x, 20);
	if (cursor_new.x < 18) SetCursorPos(598, cursor_new.y);
	if (cursor_new.y < 18) SetCursorPos(cursor_new.x, 398);

	if (cursor_old.x > cursor_new.x)	{ angle_x -= (cursor_old.x - cursor_new.x) ; }
	if (cursor_old.x < cursor_new.x)	{ angle_x += (cursor_new.x - cursor_old.x) ; }

	if (cursor_old.y > cursor_new.y)	{ angle_y -= (cursor_old.y - cursor_new.y) ; }
	if (cursor_old.y < cursor_new.y)	{ angle_y += (cursor_new.y - cursor_old.y) ; }

	GetCursorPos(&cursor_old);

	rad_x = angle_x*(D3DX_PI / 360);
	rad_y = angle_y*(D3DX_PI / 360);


	look_x = pos_x + 1.0f * sin(rad_y)*sin(rad_x);
	look_y = pos_y + 1.0f * cos(rad_y);
	look_z = pos_z + 1.0f * sin(rad_y)*cos(rad_x);

	normal_x = pos_x + 1.0f * sin( rad_y - (90*(D3DX_PI / 180)) )*sin(rad_x);
	normal_y = pos_y + 1.0f * cos( rad_y - (90*(D3DX_PI / 180)) );
	normal_z = pos_z + 1.0f * sin( rad_y - (90*(D3DX_PI / 180)) )*cos(rad_x);

}
D3DXMATRIX direct_cam::get_view_matrix()
{
	return matrix_view;
}
D3DXMATRIX direct_cam::get_proj_matrix()
{
	return matrix_projection;
}
float direct_cam::get_cam_pos(char xyz)
{
	switch (xyz)
	{
	case 'x':
		return pos_x;
	break;
	case 'y':
		return pos_y;
	break;
	case 'z':
		return pos_z;
	break;
	default:
		return 0;
	break;
	}
}