#include "main.h"
// ����� �����

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpStrCmd, int iShowCmd)
{
	if (!(hInst || lpStrCmd || iShowCmd)) errorMsg(1, "������ WinMain �����");

	wnd window(hInst);
	initComponents(&window);

	return 0;
}