#pragma once
#include <windows.h>

#include <fstream>
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9core.h>
#include <d3dx9mesh.h>
#include <dinput.h>
#include <stdio.h>
#include <mmsystem.h>

//#include <xaudio2.h>


#define KEYDOWN(name, key)(name[key]&0x80)

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
