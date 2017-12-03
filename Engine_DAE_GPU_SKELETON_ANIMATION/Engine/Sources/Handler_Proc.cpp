#include "Handler_Proc.h"

int startHandlerProc(wnd *window, resources *Data)
{
	MSG msg;
	memset(&msg, 0, sizeof(MSG));

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		renderScene(window, Data);
		startHandlerDevice(window);
	}
	return (msg.wParam);
}
