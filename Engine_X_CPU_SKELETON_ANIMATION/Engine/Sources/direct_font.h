#pragma once
#include "header.h"

class direct_font
{
private:
	LPD3DXFONT p_font;
	RECT rec;
public:
	direct_font();
	~direct_font();
	void create_font(IDirect3DDevice9*, int);
	void print_text(char*, int, int, int, int, D3DCOLOR);
};

