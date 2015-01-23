#include "stdafx.h"
#include "CLHook.h"

// STL
//#include <string>
//#include <sstream>

#pragma data_seg(".CurlytQI")
static HHOOK         hMsgHookShell = NULL; // хук WH_SHELL
//static HHOOK         hKbdHook      = NULL; // хук WH_KEYBOARD
static HKL           m_hklLast     = 0;    // последн€€ полученна€ расскладка
#pragma data_seg()
#pragma comment(linker, "/SECTION:.CurlytQI,RWS")

#ifdef _MANAGED
#pragma managed(push, off)
#endif 

static HINSTANCE hInstance = NULL;

LRESULT CALLBACK ShellProc(int nCode, WPARAM wParam, LPARAM lParam);
//LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

//////////////////////////////////////////////////////////////////////////
// точка входа
//
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
			hInstance = (HINSTANCE)hModule;
			break;

		case DLL_PROCESS_DETACH:
			break;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// установка хука
//
__declspec(dllexport) BOOL SetHook()
{
	hMsgHookShell = SetWindowsHookEx(WH_SHELL, (HOOKPROC)ShellProc, hInstance, 0);
	if (NULL == hMsgHookShell)
	{
		return FALSE;
	}

	/*
	hKbdHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)KeyboardProc, hInstance, 0);
	if (NULL == hKbdHook)
	{
		return FALSE;
	}
	*/

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// сн€тие хука
//
__declspec(dllexport) BOOL UnSetHook()
{
	if (NULL != hMsgHookShell)
	{
		UnhookWindowsHookEx(hMsgHookShell);

		hMsgHookShell = NULL;
	}

	/*
	if (NULL != hKbdHook)
	{
		UnhookWindowsHookEx(hKbdHook);

		hKbdHook = NULL;
	}
	*/

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// обработка WH_SHELL
//
LRESULT CALLBACK ShellProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (HSHELL_LANGUAGE == nCode)
	{
		m_hklLast = (HKL)lParam;
	}

	return CallNextHookEx(hMsgHookShell, nCode, wParam, lParam);
}


//////////////////////////////////////////////////////////////////////////
// обработка WH_KEYBOARD
//
/*
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	while (TRUE, TRUE)
	{
		POINT ptCaret;
		GetCaretPos(&ptCaret);
		std::wostringstream out;
		out << ptCaret.x << L" " << ptCaret.y;

		OutputDebugString(out.str().c_str());

		// хендл вывода консоли 
		HANDLE hOutput = GetStdHandle(STD_INPUT_HANDLE);

		// получение текстовых координат курсора
		CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
		if (!GetConsoleScreenBufferInfo(hOutput, &csbiInfo))
		{
			break;
		}

		OutputDebugString(L"2");

		// позици€ scrollbar'ов
		INT iConScrollPosH = ::GetScrollPos(GetForegroundWindow(), SB_HORZ);
		INT iConScrollPosV = ::GetScrollPos(GetForegroundWindow(), SB_VERT);

		OutputDebugString(L"3");

		// получение шрифта консоли
		CONSOLE_FONT_INFO cciFont;

		if (!GetCurrentConsoleFont(hOutput, FALSE, &cciFont))
		{
			break;
		}

		OutputDebugString(L"4");

		// получение размера шрифта в пикселах
		COORD sizeConFont = GetConsoleFontSize(hOutput, cciFont.nFont);
		
		// координаты окна вывода
		RECT rectConClient;
		if (!::GetClientRect(GetForegroundWindow(), &rectConClient))
		{
			break;
		}

		OutputDebugString(L"OK!!!");

		break;
	}

	return CallNextHookEx(hMsgHookShell, nCode, wParam, lParam);
}
*/

//////////////////////////////////////////////////////////////////////////
// возвращает последнюю полученную расскладку
//
__declspec(dllexport) HKL GetLastLayout()
{
	return m_hklLast;
}
