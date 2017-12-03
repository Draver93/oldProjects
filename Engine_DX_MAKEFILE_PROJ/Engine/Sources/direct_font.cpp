#include "direct_font.h"


direct_font::direct_font()
{
}
direct_font::~direct_font()
{
}
void direct_font::create_font(IDirect3DDevice9* p_direct3d_device, int size)
{
	D3DXCreateFont(p_direct3d_device,
		size,
		0,
		FW_NORMAL,
		1,
		false,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		"Courier New",
		&p_font
		);
}
void direct_font::print_text(char* text, int pos_x, int pos_y, int entry_field_x, int entry_field_y, D3DCOLOR color)
{
	rec.left	= pos_x;
	rec.top		= pos_y;
	rec.right	= entry_field_x;
	rec.bottom	= entry_field_y;

	p_font->DrawText(NULL, 
		text, 
		-1, 
		&rec, 
		DT_WORDBREAK, 
		color);
}