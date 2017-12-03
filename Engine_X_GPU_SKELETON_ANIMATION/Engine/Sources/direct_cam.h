#pragma once
#include "header.h"

class direct_cam
{
private:
	D3DXMATRIX matrix_view;
	D3DXMATRIX matrix_projection;

	float angle_x, angle_y;
	float rad_x, rad_y;
	float pos_x, pos_y, pos_z;
	float look_x, look_y, look_z;
	POINT cursor_old, cursor_new;
	float move_x, move_y, move_z;
	float normal_x, normal_y, normal_z;
	DWORD new_time, old_time;
public:
	direct_cam();
	~direct_cam();
	void set_cam(LPDIRECT3DDEVICE9, char* keyboard_state);
	void mouse_control(char* keyboard_state);
	D3DXMATRIX direct_cam::get_proj_matrix();
	D3DXMATRIX direct_cam::get_view_matrix();
	float get_cam_pos(char);
};

