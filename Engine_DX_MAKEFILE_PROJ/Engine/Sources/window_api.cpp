#include "window_api.h"


window_api::window_api()
{
}
window_api::~window_api()
{
}

int window_api::setup_window()
{
	CoInitialize(NULL);

	window_class.cbClsExtra = 0;											//��������
	window_class.cbWndExtra = 0;											//��������
	window_class.cbSize = sizeof(WNDCLASSEX);								//������ ���������
	window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);		//���� ���� ����
	window_class.hCursor = NULL;											//������ �� ���������
	window_class.hIcon = NULL;												//Icon �����������
	window_class.hIconSm = NULL;											//Icon ��������� �� ������ �����
	window_class.hInstance = get_h_inst();									//��������� ���� ����������� ��� ����� � ������� win_main()
	window_class.lpfnWndProc = win_proc;									//���������� ������� � ��� ��� ������� win_main_proc()
	window_class.lpszClassName = "Class_name";								//�������� ������ ���� �������� ��� ���������� ���������� � ������� ������������
	window_class.lpszMenuName = "Menu_name";								//�������� ��� ��� � ��� ������ ������� ���� �� �����
	window_class.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;	//����� ����

	//������������ ���� ���������
	if (!RegisterClassEx(&window_class)) return FALSE;		//������������ ��������� ������ ����

	//������� ����
	if (!(h_window = CreateWindowEx(NULL,					//������������ ��� ����������� ���� ������ ���������
									"Class_name",			//��� ������ ������
									"Window",				//��������� ����
									WS_VISIBLE,				//����� ���� WS_VISIBLE - ���� ������ ����������, WS_BORDER - �������� ����� ���� �� ����� ������
									0, 0,					//������������ ���� x,y
									500, 400,				//������ ���� x,y
									NULL,					//��� ���� �� ����������� �� ������
									NULL,					//���� �� ����� ����
									get_h_inst(),			//�������� ����
									NULL))) return FALSE;
	//���������� ����
	ShowWindow(h_window, WS_VISIBLE);
	ShowCursor(false);
	//��������� ����
	UpdateWindow(h_window);
	//������� ��������� �������� ���������
	CoUninitialize();

	return 0;
}
int window_api::handler_proc()
{

	// ���� ��������� ���������
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		global_handler();
	}
	return (msg.wParam);
}
HWND window_api::get_hwnd()
{
	return h_window;
}
