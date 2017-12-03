#include "window_api.h"


window_api::window_api()
{
}
window_api::~window_api()
{
}

int window_api::setup_window()
{
	window_class.cbClsExtra = 0;											//Заглушка
	window_class.cbWndExtra = 0;											//Заглушка
	window_class.cbSize = sizeof(WNDCLASSEX);								//Размер структуры
	window_class.hbrBackground = (HBRUSH)GetStockObject(HOLLOW_BRUSH);		//Цвет фона окна
	window_class.hCursor = NULL;											//Курсор по умолчанию
	window_class.hIcon = NULL;												//Icon отсутствует
	window_class.hIconSm = NULL;											//Icon маленькая на панели задач
	window_class.hInstance = get_h_inst();									//Экземпляр окна создаваемый при входе в функцию win_main()
	window_class.lpfnWndProc = win_proc;									//Обработчик событий у нас это функция win_main_proc()
	window_class.lpszClassName = "Class_name";								//Название класса окна задается для исключения совпадений с другими приложениями
	window_class.lpszMenuName = "Menu_name";								//Заглушка так как в нам данный элемент окна не нужен
	window_class.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;	//Стиль окна

	//Регистрируем наши настройки
	if (!RegisterClassEx(&window_class)) return FALSE;		//Регистрируем настройки нашего окна

	//Создаем окно
	if (!(h_window = CreateWindowEx(NULL,					//Используется для отображения окна поверх остальных
									"Class_name",			//Имя нашего класса
									"Window",				//Заголовок окна
									WS_VISIBLE,				//Стиль окна WS_VISIBLE - окно видимо изначально, WS_BORDER - изменяет рамку окна на более тонкую
									0, 0,					//Расположение окна x,y
									500, 400,				//Размер окна x,y
									NULL,					//Это окно не наследуется от других
									NULL,					//Окно не имеет меню
									get_h_inst(),			//Экземляр окна
									NULL))) return FALSE;
	//Показываем окно
	ShowWindow(h_window, WS_VISIBLE);
	ShowCursor(false);
	//Обновляем окно
	UpdateWindow(h_window);
	//Очистка структуры хранящей сообщения

	return 0;
}
int window_api::handler_proc()
{

	// Цикл обработки сообщений 
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