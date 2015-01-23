#pragma once

#include "stdafx.h"

// наименование компании
#define COMPANY_NAME L"www.wholegroup.com"

// наименование программы
#define PROGRAM_NAME L"Curlyt"

// язык по умолчанию
#define DEFAULT_LANGUAGE MAKELANGID(LANG_ENGLISH, SUBLANG_NEUTRAL)

// задержка работы треда слежения за раскладкой (в ms)
#define THREADLANG_CYCLE 300

// тип списка языков (ИД, название)
typedef map<DWORD, const wstring> CMapWordStr;

// тип списка иконок (ИД, HICON*)
typedef map<DWORD, const HICON> CMapWordPHIcon;

// код выхода для смены языка
#define CHANGE_LANGUAGE 763

// сообщение для смены языка программы
#define WM_USER_CHANGE_LANGUAGE WM_USER + 0x801

// название класса окна для консоли
#define CONSOLE_WINDOW_CLASS L"ConsoleWindowClass"

// автозапуск программы
#define DEFAULT_AUTOSTART TRUE

// показывать текущую раскладку в трее
#define DEFAULT_VIEW_TRAY_LANG TRUE

// показывать индикатор раскладки флажком
#define DEFAULT_VIEW_FLAG TRUE

// показывать увеличенный индикатор 24x24
#define DEFAULT_VIEW_LARGE FALSE

// шрифт для формирование иконки-акронима
#define DEFAULT_ACRONYM_ICON_FONT "Trebuchet MS"

// цвет фона у иконки-акронима
#define DEFAULT_ACRONYM_ICON_BKCOLOR 0xFFFFFFFF

// цвет текста у иконки-акронима
#define DEFAULT_ACRONYM_ICON_COLOR 0x00000000

// прозрачность по-умолчанию
#define DEFAULT_TRANSPARENCY     100
#define DEFAULT_TRANSPARENCY_MIN 25
#define DEFAULT_TRANSPARENCY_MAX 255


// типы GUI окон для ввода текста
enum EDITUITYPE
{
	EDITUITYPE_UNKNOW = 0, // неизвестный тип
	EDITUITYPE_EDIT,       // стандартный Edit
	EDITUITYPE_RICHEDIT,   // RichEdit
	EDITUITYPE_MSOFFICE,   // Microsoft Office
	EDITUITYPE_IEXPLORER,  // Internet Explorer
	EDITUITYPE_FIREFOX,    // Mozilla Firefox
	EDITUITYPE_OPERA,      // Opera
	EDITUITYPE_OOFFICE,    // Open Office
	EDITUITYPE_INVALID     // ошибочное поле ввода (не будет обрабатываться в принципе)
};

// позиция окна индикатора относительно каретки
enum ICONPOSITION
{
	ICONPOSITION_RIGHT_CENTER = 0,
	ICONPOSITION_RIGHT_TOP,
	ICONPOSITION_RIGHT_BOTTOM,
	ICONPOSITION_LEFT_CENTER,
	ICONPOSITION_LEFT_TOP,
	ICONPOSITION_LEFT_BOTTOM
};

// позиция окна индикатора по-умолчанию
#define DEFAULT_ICONPOSITION ICONPOSITION_RIGHT_BOTTOM

// сообщение для изменения позиции каретки в консоли
#define WM_USER_CARET_CONSOLE_REPOS WM_USER + 0x802

// сообщение для изменения позиции каретки в GUI
#define WM_USER_CARET_GUI_REPOS WM_USER + 0x803

// сообщение для сокрытия каретки
#define WM_USER_CARET_HIDE WM_USER + 0x804

// задержка перед сокрытием курсора
#define DEFAULT_TIMEOUT_HIDE_ICONCARET 50

// функция возвращает раскладку консоли
typedef BOOL (NTAPI *LPGETCONKEYBLAYNAME)(LPWSTR pwszLayout);

// функция аттача к консоли
typedef BOOL (NTAPI *LPATTACHCONSOLE)(DWORD dwProcessId);

// функция детачча от консоли
typedef BOOL (NTAPI *LPFREECONSOLE)(void);


//////////////////////////////////////////////////////////////////////////
// Класс для создания диалогов из DLGTEMPLATE
// Используется при загрузке диалога в зависимости от языка
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
// Перевод строки в верхний регистр
// (с нормальной поддержкой Unicode)
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

// макросы для отладки
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
