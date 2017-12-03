#include "pErrors.h"

void errorMsg(int codeError ,std::string text)
{
	const char *convertText;
	convertText = text.c_str();

	switch (codeError)
	{
		case 1:
		{
			int msgbox_id = MessageBox(NULL, convertText, "Ошибка", MB_OK);
		}
		break;
	}
}
