#include "header.h"

HINSTANCE h_inst_global;
LPSTR lp_cmd_str_global;
int n_cmd_show_global;

int WINAPI WinMain(HINSTANCE h_inst, HINSTANCE, LPSTR lp_cmd_str, int n_cmd_show)
{
	h_inst_global = h_inst;
	lp_cmd_str_global = lp_cmd_str;
	n_cmd_show_global = n_cmd_show;

	main_function();
	return (0);
}
LRESULT CALLBACK win_proc(HWND hw, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);		//Сообщение обработчику о завершении цикла обработки 
		return(0);
	break;
	default:
		break;
	}
	return (DefWindowProc(hw, msg, wp, lp));
}
HINSTANCE get_h_inst()
{
	return h_inst_global;
}
LPSTR get_lp_str()
{
	return lp_cmd_str_global;
}
int get_n_cmd_show()
{
	return n_cmd_show_global;
}



