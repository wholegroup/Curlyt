#pragma once

#include "stdafx.h"

// ������������ ��������
#define COMPANY_NAME L"www.wholegroup.com"

// ������������ ���������
#define PROGRAM_NAME L"Curlyt"

// ���� �� ���������
#define DEFAULT_LANGUAGE MAKELANGID(LANG_ENGLISH, SUBLANG_NEUTRAL)

// �������� ������ ����� �������� �� ���������� (� ms)
#define THREADLANG_CYCLE 300

// ��� ������ ������ (��, ��������)
typedef map<DWORD, const wstring> CMapWordStr;

// ��� ������ ������ (��, HICON*)
typedef map<DWORD, const HICON> CMapWordPHIcon;

// ��� ������ ��� ����� �����
#define CHANGE_LANGUAGE 763

// ��������� ��� ����� ����� ���������
#define WM_USER_CHANGE_LANGUAGE WM_USER + 0x801

// �������� ������ ���� ��� �������
#define CONSOLE_WINDOW_CLASS L"ConsoleWindowClass"

// ���������� ���������
#define DEFAULT_AUTOSTART TRUE

// ���������� ������� ��������� � ����
#define DEFAULT_VIEW_TRAY_LANG TRUE

// ���������� ��������� ��������� �������
#define DEFAULT_VIEW_FLAG TRUE

// ���������� ����������� ��������� 24x24
#define DEFAULT_VIEW_LARGE FALSE

// ����� ��� ������������ ������-��������
#define DEFAULT_ACRONYM_ICON_FONT "Trebuchet MS"

// ���� ���� � ������-��������
#define DEFAULT_ACRONYM_ICON_BKCOLOR 0xFFFFFFFF

// ���� ������ � ������-��������
#define DEFAULT_ACRONYM_ICON_COLOR 0x00000000

// ������������ ��-���������
#define DEFAULT_TRANSPARENCY     100
#define DEFAULT_TRANSPARENCY_MIN 25
#define DEFAULT_TRANSPARENCY_MAX 255


// ���� GUI ���� ��� ����� ������
enum EDITUITYPE
{
	EDITUITYPE_UNKNOW = 0, // ����������� ���
	EDITUITYPE_EDIT,       // ����������� Edit
	EDITUITYPE_RICHEDIT,   // RichEdit
	EDITUITYPE_MSOFFICE,   // Microsoft Office
	EDITUITYPE_IEXPLORER,  // Internet Explorer
	EDITUITYPE_FIREFOX,    // Mozilla Firefox
	EDITUITYPE_OPERA,      // Opera
	EDITUITYPE_OOFFICE,    // Open Office
	EDITUITYPE_INVALID     // ��������� ���� ����� (�� ����� �������������� � ��������)
};

// ������� ���� ���������� ������������ �������
enum ICONPOSITION
{
	ICONPOSITION_RIGHT_CENTER = 0,
	ICONPOSITION_RIGHT_TOP,
	ICONPOSITION_RIGHT_BOTTOM,
	ICONPOSITION_LEFT_CENTER,
	ICONPOSITION_LEFT_TOP,
	ICONPOSITION_LEFT_BOTTOM
};

// ������� ���� ���������� ��-���������
#define DEFAULT_ICONPOSITION ICONPOSITION_RIGHT_BOTTOM

// ��������� ��� ��������� ������� ������� � �������
#define WM_USER_CARET_CONSOLE_REPOS WM_USER + 0x802

// ��������� ��� ��������� ������� ������� � GUI
#define WM_USER_CARET_GUI_REPOS WM_USER + 0x803

// ��������� ��� �������� �������
#define WM_USER_CARET_HIDE WM_USER + 0x804

// �������� ����� ��������� �������
#define DEFAULT_TIMEOUT_HIDE_ICONCARET 50

// ������� ���������� ��������� �������
typedef BOOL (NTAPI *LPGETCONKEYBLAYNAME)(LPWSTR pwszLayout);

// ������� ������ � �������
typedef BOOL (NTAPI *LPATTACHCONSOLE)(DWORD dwProcessId);

// ������� ������� �� �������
typedef BOOL (NTAPI *LPFREECONSOLE)(void);


//////////////////////////////////////////////////////////////////////////
// ����� ��� �������� �������� �� DLGTEMPLATE
// ������������ ��� �������� ������� � ����������� �� �����
//
class CResDlgTemplate
{
	public:

		CResDlgTemplate()
			: m_pData(NULL)
		{
		}

		~CResDlgTemplate()
		{
		}

	private:

		LPDLGTEMPLATE m_pData;

	public:

		VOID Create(LPDLGTEMPLATE pData)
		{
			m_pData = pData;
		}

		BOOL IsValid() const
		{
			return (NULL != m_pData);
		}

		LPDLGTEMPLATE GetTemplatePtr()
		{
			return m_pData;
		}
};


//////////////////////////////////////////////////////////////////////////
// ������� ������ � ������� �������
// (� ���������� ���������� Unicode)
//
namespace std
{
	inline std::wstring upper(std::wstring wsText)
	{
		std::vector<std::wstring::value_type> wcText(wsText.length() + 1);

		ZeroMemory(&wcText.front(), (wsText.length() + 1) * sizeof(std::wstring::value_type));
		CopyMemory(&wcText.front(), wsText.c_str(), wsText.length()* sizeof(std::wstring::value_type));

		CharUpper(&wcText.front());

		return &wcText.front();
	}
}

// ������� ��� �������
#ifdef _DEBUG
#define _RPT_API_TRACE(name)                                                     \
	do                                                                               \
{                                                                                \
	unsigned __err = GetLastError();                                              \
	WCHAR    wcMsg[512];                                                          \
	\
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, __err, 0, wcMsg, 512, NULL); \
	\
	_RPTFW3(_CRT_WARN, L"%S() failed; ERR=%d %s\n", #name, __err, wcMsg);         \
	\
	SetLastError(__err);                                                          \
} while(0, 0)
#else
#define _RPT_API_TRACE(name)	__noop
#endif

#ifdef _DEBUG
#define _RPT_API_ERROR(name)	                                                   \
	do                                                                               \
{                                                                                \
	unsigned __err = GetLastError();                                              \
	WCHAR     wcMsg[512];                                                         \
	\
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, __err, 0, wcMsg, 512, NULL); \
	\
	_RPTFW3(_CRT_ERROR, L"%S() failed; ERR=%d %s\n", #name, __err, wcMsg);        \
	\
	SetLastError(__err);                                                          \
} while(0, 0)
#else
#define _RPT_API_ERROR(name)	__noop
#endif
