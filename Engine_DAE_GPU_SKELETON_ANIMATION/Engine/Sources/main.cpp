#include "main.h"
// Точка входа

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpStrCmd, int iShowCmd)
{
	if (!(hInst || lpStrCmd || iShowCmd)) errorMsg(1, "Ошибка WinMain входа");

	wnd window(hInst);
	initComponents(&window);

	return 0;
}