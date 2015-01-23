#include "stdafx.h"
#include "CLHook.h"

// STL
//#include <string>
//#include <sstream>

#pragma data_seg(".CurlytQI")
static HHOOK         hMsgHookShell = NULL; // ��� WH_SHELL
//static HHOOK         hKbdHook      = NULL; // ��� WH_KEYBOARD
static HKL           m_hklLast     = 0;    // ��������� ���������� ����������
#pragma data_seg()
#pragma comment(linker, "/SECTION:.CurlytQI,RWS")

#ifdef _MANAGED
#pragma managed(push, off)
#endif 

static HINSTANCE hInstance = NULL;

LRESULT CALLBACK ShellProc(int nCode, WPARAM wParam, LPARAM lParam);
//LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

//////////////////////////////////////////////////////////////////////////
// ����� �����
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
// ��������� ����
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
// ������ ����
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
// ��������� WH_SHELL
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
// ��������� WH_KEYBOARD
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

		// ����� ������ ������� 
		HANDLE hOutput = GetStdHandle(STD_INPUT_HANDLE);

		// ��������� ��������� ��������� �������
		CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
		if (!GetConsoleScreenBufferInfo(hOutput, &csbiInfo))
		{
			break;
		}

		OutputDebugString(L"2");

		// ������� scrollbar'��
		INT iConScrollPosH = ::GetScrollPos(GetForegroundWindow(), SB_HORZ);
		INT iConScrollPosV = ::GetScrollPos(GetForegroundWindow(), SB_VERT);

		OutputDebugString(L"3");

		// ��������� ������ �������
		CONSOLE_FONT_INFO cciFont;

		if (!GetCurrentConsoleFont(hOutput, FALSE, &cciFont))
		{
			break;
		}

		OutputDebugString(L"4");

		// ��������� ������� ������ � ��������
		COORD sizeConFont = GetConsoleFontSize(hOutput, cciFont.nFont);
		
		// ���������� ���� ������
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
// ���������� ��������� ���������� ����������
//
__declspec(dllexport) HKL GetLastLayout()
{
	return m_hklLast;
}
