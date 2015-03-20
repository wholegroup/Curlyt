/*
 * Copyright (C) 2015 Andrey Rychkov <wholegroup@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "StdAfx.h"
#include "DlgMain.h"
#include "DlgSettings.h"
#include "DlgFlag.h"
#include <time.h>

// подключение хука
// хук работает только в 2k
#include "CLHook/CLHook.h"

// ссылка на главный диалог программы
extern CDlgMain* g_pDlgMain;

// для использования в HandleWinEvent
RECT                 g_rcCaret;
HRESULT              g_hr;
CComPtr<IAccessible> g_pAcc;
CComVariant          g_varChild;

//////////////////////////////////////////////////////////////////////////
// Функция обработки MSAA событий
// (всю обработку передать главному потоку через сообщения)
//
VOID CALLBACK HandleWinEvent(
	HWINEVENTHOOK /*hook*/,          // Handle to an event hook function
	DWORD         dwEvent,           // Specifies the event that occurred
	HWND          hWnd,              // Handle to the window that generates the event
	LONG          idObject,          // Identifies the object associated with the event
	LONG          idChild,           // Identifies whether the event was triggered by an object or a child element of the object
	DWORD         /*dwEventThread*/, // Identifies the thread that generated the event
	DWORD         /*dwmsEventTime*/  // Specifies the time, in milliseconds, that the event was generated
)
{
	// проверка наличия GUI окна
	if (FALSE == ::IsWindow(hWnd))
	{
		return;
	}

	// проверка что получено сообщение от активного окна
	if ((FALSE == IsChild(GetForegroundWindow(), hWnd)) && (GetForegroundWindow() != hWnd))
	{
		return;
	}

	// обработка для консольного окна
	if (EVENT_CONSOLE_CARET == dwEvent)
	{
		g_pDlgMain->SetCaretActivity(TRUE, hWnd);
		PCOORD pcCaret = reinterpret_cast<PCOORD>(&idChild);
		PostMessage(g_pDlgMain->m_hWnd, WM_USER_CARET_CONSOLE_REPOS, pcCaret->X, pcCaret->Y);
		return;
	}

	// Получаем указатель на IAccessible объект 
	// стоит тут, т.к. нужно для Firefox'a, иначе он не начинал посылать сообщения,
	// когда AccessibleObjectFromEvent стоял для EVENT_OBJECT_SHOW и EVENT_OBJECT_LOCATIONCHANGE
	if (EVENT_OBJECT_DESTROY != dwEvent)
	{
		g_pAcc.Release(); g_pAcc = NULL;
		g_varChild.Clear();
		g_hr = AccessibleObjectFromEvent(hWnd, idObject, idChild, &g_pAcc, &g_varChild);

		if ((S_OK != g_hr) || (NULL == g_pAcc))
		{
			return;
		}
	}

	// выход, если событие не связано с кареткой
	if (OBJID_CARET != idObject)
	{
		return;
	}

	// обработка события
	switch(dwEvent)
	{
		// событие уничтожения курсора
		case EVENT_OBJECT_DESTROY:
			{
				g_pDlgMain->SetCaretActivity(FALSE, hWnd);
				g_pDlgMain->SetHideEvent();
			}
			break;

		// сообщение смены позиции каретки
		case EVENT_OBJECT_LOCATIONCHANGE:
		case EVENT_OBJECT_SHOW:
			{
				g_pDlgMain->SetCaretActivity(TRUE, hWnd);
				g_pAcc->accLocation(&g_rcCaret.left, &g_rcCaret.top, &g_rcCaret.right, &g_rcCaret.bottom, g_varChild);
				PostMessage(g_pDlgMain->m_hWnd, WM_USER_CARET_GUI_REPOS, MAKEWPARAM(g_rcCaret.left, g_rcCaret.top), MAKELPARAM(g_rcCaret.right, g_rcCaret.bottom));
			}
			break;

		default:
			break;
	}
}


//////////////////////////////////////////////////////////////////////////
// Конструктор по-умолчанию
// 
CDlgMain::CDlgMain() :
	m_hEventHook(0),
	m_hWndActive(NULL),
	m_hWndActiveLast(NULL),
	m_hWndFocus(NULL),
	m_hWndFocusLast(NULL),
	m_dwActProcessId(0),
	m_dwActThreadId(0),
	m_hKernelDll(NULL),
	m_lpAttachConsole(NULL),
	m_lpFreeConsole(NULL),
	m_lpGetConKeybLayName(NULL),
	m_eEditUIType(EDITUITYPE_UNKNOW),
	m_pCaretFlag(NULL),
	m_bViewLarge(DEFAULT_VIEW_LARGE),
	m_crIconAcronym(DEFAULT_ACRONYM_ICON_COLOR),
	m_crBGIconAcronym(DEFAULT_ACRONYM_ICON_BKCOLOR),
	m_bTransparent(DEFAULT_TRANSPARENCY),
	m_eIconPosition(DEFAULT_ICONPOSITION),
	m_hEventExit(NULL),
	m_hEventCaretHide(NULL),
	m_thCaret(this, &m_hEventExit, &m_hEventCaretHide),
	m_bCaretActivity(FALSE),
	m_hWndCaretActivity(NULL),
	m_hKLCurrent(0),
	m_pSettingsDlg(NULL)
{
	// открытие настроек
	#ifdef _DEBUG
	OpenSettings(COMPANY_NAME, PROGRAM_NAME L" (debug)");
	#else
	OpenSettings(COMPANY_NAME, PROGRAM_NAME);
	#endif

	// язык программы
	m_wLanguageID = static_cast<WORD>(GetSettingsInteger(L"m_wLanguageID", GetUserDefaultLangID()));

	// показывать текущую раскладку в трее
	m_bViewTrayLang = GetSettingsBoolean(L"m_bViewTrayLang", DEFAULT_VIEW_TRAY_LANG);

	// цвет фона индикатора
	m_crBGIconAcronym = static_cast<DWORD>(GetSettingsInteger(L"m_crBKIconAcronym", DEFAULT_ACRONYM_ICON_BKCOLOR));

	// цвет текста индикатора
	m_crIconAcronym = static_cast<DWORD>(GetSettingsInteger(L"m_crIconAcronym", DEFAULT_ACRONYM_ICON_COLOR));

	// показывать индикатор в виде флага
	m_bViewFlag = GetSettingsBoolean(L"m_bViewFlag", DEFAULT_VIEW_FLAG);

	// индекс прозрачности индикатора
	m_bTransparent = static_cast<BYTE>(GetSettingsInteger(L"m_bTransparent", DEFAULT_TRANSPARENCY));

	// показывает увеличенный индикатор 24x24
	m_bViewLarge = GetSettingsBoolean(L"m_bViewLarge", DEFAULT_VIEW_LARGE);

	// проверка первого запуска и установка автостарта
	if (GetSettingsBoolean(L"First", TRUE))
	{
		SetAutostart(DEFAULT_AUTOSTART);
	}
	SetSettingsBoolean(L"First", FALSE);

	// позиция индикатора относительно каретки
	m_eIconPosition = static_cast<ICONPOSITION>(GetSettingsInteger(L"m_eIconPosition", DEFAULT_ICONPOSITION));

	// Поиск всех возможных языков программы
	m_mapLanguages.clear();

	SearchLanguage();

	// установка языка программы
	CMapWordStr::const_iterator item = m_mapLanguages.find(m_wLanguageID);
	if (m_mapLanguages.end() == item)
	{
		m_wLanguageID = DEFAULT_LANGUAGE;
	}

	// поиск установленных языков в системе
	m_mapLangSystem.clear();
	m_mapLangAcronym.clear();

	SearchInstallLanguage();

	// установка стартового языка
	if (0 == m_hKLCurrent)
	{
		m_hKLCurrent = reinterpret_cast<HKL>(m_mapLangAcronym.begin()->first);
	}

	// получение иконок с аббревиатурами установленных языков
	GenerateAcronymIcons(m_crIconAcronym, m_crBGIconAcronym);

	// загрузка иконок флагов
	LoadFlagIcons();

	// запоминаем ИД собственного потока
	m_dwCurrentThread = GetCurrentThreadId();
}


//////////////////////////////////////////////////////////////////////////
// Деструктор по-умолчанию
//
CDlgMain::~CDlgMain()
{
	// удаляем иконки
	for (CMapWordPHIcon::const_iterator 
		hiLarge = m_mapIconsLarge.begin(), 
		hiSmall = m_mapIconsSmall.begin(), 
		hiAcronymSmall = m_mapIconsAcronymSmall.begin(),
		hiAcronymLarge = m_mapIconsAcronymLarge.begin(); 
		hiLarge != m_mapIconsLarge.end(); 
		hiLarge++, hiSmall++, hiAcronymSmall++, hiAcronymLarge++)
	{
		ATLVERIFY(DestroyIcon(hiLarge->second));
		ATLVERIFY(DestroyIcon(hiSmall->second));
		ATLVERIFY(DestroyIcon(hiAcronymSmall->second));
		ATLVERIFY(DestroyIcon(hiAcronymLarge->second));
	}

	// сохраняем настройки программы
	SetSettingsInteger(L"m_wLanguageID", m_wLanguageID);
	SetSettingsBoolean(L"m_bViewTrayLang", m_bViewTrayLang);
	SetSettingsBoolean(L"m_bViewFlag", m_bViewFlag);
	SetSettingsBoolean(L"m_bViewLarge", m_bViewLarge);
	SetSettingsInteger(L"m_bTransparent", m_bTransparent);
	SetSettingsInteger(L"m_crIconAcronym", m_crIconAcronym);
	SetSettingsInteger(L"m_crBKIconAcronym", m_crBGIconAcronym);

	CloseSettings(TRUE);
}


//////////////////////////////////////////////////////////////////////////
// Обработка сообщений
//
BOOL CDlgMain::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}


//////////////////////////////////////////////////////////////////////////
// Фоновая обработка сообщений
// 
BOOL CDlgMain::OnIdle()
{
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
// Инициализация диалога
//
LRESULT CDlgMain::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// загрузка библиотеки kernel32.dll
	m_hKernelDll = LoadLibrary(L"kernel32");
	if (NULL == m_hKernelDll)
	{
		_RPT_API_ERROR(LoadLibrary);
		CloseDialog(0);
		return FALSE;
	}

	// получение указателя на функцию GetConsoleKeyboardLayoutName
	m_lpGetConKeybLayName = reinterpret_cast<LPGETCONKEYBLAYNAME>(
		GetProcAddress(m_hKernelDll, "GetConsoleKeyboardLayoutNameW"));
	if (NULL == m_lpGetConKeybLayName)
	{
		_RPT_API_ERROR(GetProcAddress);
		CloseDialog(0);
		return FALSE;
	}

	// получение указателя на функцию AttachConsole
	m_lpAttachConsole = reinterpret_cast<LPATTACHCONSOLE>(GetProcAddress(m_hKernelDll, "AttachConsole"));

	// получение указателя на FreeConsole
	m_lpFreeConsole = reinterpret_cast<LPFREECONSOLE>(GetProcAddress(m_hKernelDll, "FreeConsole"));

	// инициализация критической секции
	InitializeCriticalSection(&m_csCaretActivity);

	// создание события завершения треда
	m_hEventExit = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (NULL == m_hEventExit)
	{
		_RPT_API_ERROR(CreateEvent);
		CloseDialog(0);
		return FALSE;
	}

	// создание события сокрытия каретки
	m_hEventCaretHide = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (NULL == m_hEventCaretHide)
	{
		_RPT_API_ERROR(CreateEvent);
		CloseDialog(0);
		return FALSE;
	}

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = g_Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	// установка иконки приложения
	HICON hIcon = (HICON)::LoadImage(g_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);

	HICON hIconSmall = (HICON)::LoadImage(g_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	// создание иконки в трее
	CreateNotifyIcon();
	SetNotifyIconTip(GetStringLang(IDS_TRAYTIP).c_str());

	// выровнять диалог по центру
	CenterWindow();

	// установка хука для получение текущей раскладки (только для Windows 2000)
	if (NULL == m_lpAttachConsole)
	{
		if (FALSE == SetHook())
		{
			_RPT_API_ERROR(SetHook);
			CloseDialog(0);
			return TRUE;
		}
	}

	// Set system-wide screen reader present flag (требуется для OfficeXP)
	SystemParametersInfo(SPI_SETSCREENREADER, TRUE, NULL, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);

	// установка MSAA хука
	m_hEventHook = SetWinEventHook(EVENT_CONSOLE_CARET, EVENT_OBJECT_LOCATIONCHANGE, NULL,
		HandleWinEvent, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
	if (0 == m_hEventHook)
	{
		_RPT_API_ERROR(SetWinEventHook);
		CloseDialog(0);
		return TRUE;
	}

	// установа таймера обновления активной раскладки
	if (!SetTimer(0, THREADLANG_CYCLE, 0))
	{
		_RPT_API_ERROR(SetTimer);
		CloseDialog(0);
		return FALSE;
	}

	// создание индикатора расскладки
	CRect rectCaretFlag(0, 0, 23, 23);
	m_pCaretFlag = new CDlgFlag();
	m_pCaretFlag->Create(HWND_DESKTOP, rectCaretFlag, L"", WS_POPUP | WS_VISIBLE, WS_EX_TOPMOST | 
		WS_EX_NOACTIVATE |  WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
	m_pCaretFlag->ShowWindow(SW_HIDE);

	// запуск треда сокрытия индикатора
	m_thCaret.Resume();

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// Обработка WM_DESTROY
//
LRESULT CDlgMain::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// удаление индикатора
	if (NULL != m_pCaretFlag)
	{
		if (m_pCaretFlag->IsWindow())
		{
			m_pCaretFlag->DestroyWindow();
		}
		delete m_pCaretFlag;
		m_pCaretFlag = NULL;
	}

	// удаление MSAA хука
	if (m_hEventHook)
	{
		ATLVERIFY(UnhookWinEvent(m_hEventHook));
	}

	// удаление хука (только для W2k)
	UnSetHook();

	// посылка сообщения для отключения DLL хука от приложений
	::PostMessage(HWND_BROADCAST, WM_NULL, 0, 0);

	// удаление таймера обновления активной раскладки
	KillTimer(0);

	// удалений фильтров обработки сообщений
	CMessageLoop* pLoop = g_Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);

	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	// удаление треда для сокрытия индикатора
	if (NULL != m_hEventExit)
	{
		// создание событие выхода
		ATLVERIFY(SetEvent(m_hEventExit));

		// ожидание завершения треда
		m_thCaret.Resume();
		m_thCaret.Join(INFINITE);

		// удаление события выхода
		ATLVERIFY(CloseHandle(m_hEventExit));
		m_hEventExit = NULL;
	}

	// удаление события сокрытия каретки
	if (NULL != m_hEventCaretHide)
	{
		ATLVERIFY(CloseHandle(m_hEventCaretHide));
		m_hEventCaretHide = NULL;
	}

	// удаление критической секции
	if (TRUE == TryEnterCriticalSection(&m_csCaretActivity))
	{
		DeleteCriticalSection(&m_csCaretActivity);
	}

	// выгружаем kernel32.dll
	if (NULL != m_hKernelDll)
	{
		FreeLibrary(m_hKernelDll);
		
		m_hKernelDll          = NULL;
		m_lpAttachConsole     = NULL;
		m_lpFreeConsole       = NULL;
		m_lpGetConKeybLayName = NULL;
	}

	SystemParametersInfo(SPI_SETSCREENREADER, FALSE, NULL, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// Обработка IDOK
//
LRESULT CDlgMain::OnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// сворачиваем окно в трей
	ShowWindow(SW_MINIMIZE);

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// Обработка IDCANCEL
//
LRESULT CDlgMain::OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(0);

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// Обработка ID_APP_EXIT
//
LRESULT CDlgMain::OnExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// закрытие диалога
	CloseDialog(0);

	return 0;
}



//////////////////////////////////////////////////////////////////////////
// Закрытие диалога
//
VOID CDlgMain::CloseDialog(INT iVal)
{
	DestroyWindow();

	::PostQuitMessage(iVal);
}


//////////////////////////////////////////////////////////////////////////
// Поиск всех возможных языков программы
//
VOID CDlgMain::SearchLanguage()
{
	if (FALSE == ::EnumResourceLanguages(GetModuleHandle(NULL), RT_DIALOG, MAKEINTRESOURCE(IDD_SETTINGS), 
		(ENUMRESLANGPROC)CDlgMain::SearchLanguageCallback, (LONG_PTR)&m_mapLanguages))
	{
		MessageBox(L"Error::EnumResourceLanguages");
	}
	ATLASSERT(0 < m_mapLanguages.size());
}


BOOL CALLBACK CDlgMain::SearchLanguageCallback(HMODULE hModule, LPCTSTR lpszType, 
	LPTSTR lpszName, WORD wIDLanguage, LONG_PTR lParam)
{
	// неиспользуемые параметры
	UNREFERENCED_PARAMETER(hModule);
	UNREFERENCED_PARAMETER(lpszType);
	UNREFERENCED_PARAMETER(lpszName);

	// проверка входных значений
	ATLASSERT(GetModuleHandle(NULL) == hModule);
	ATLASSERT(RT_DIALOG == lpszType);
	ATLASSERT(MAKEINTRESOURCE(IDD_SETTINGS) == lpszName);

	// получения указателя на массив языков
	CMapWordStr *pMap = (CMapWordStr*)lParam;

	// получение наименования языка по его идентификатору wIDLanguage
	vector<WCHAR> wcBuf(128);

	GetLocaleInfo(MAKELCID(wIDLanguage, 0), LOCALE_SNATIVELANGNAME, &wcBuf.front(), 
		static_cast<int>(wcBuf.size()));

	// перевод первой буквы в верхний регистр
	CharUpperBuff(&wcBuf.front(), 1);

	// добавление языка в массив
	pMap->insert(make_pair(wIDLanguage, &wcBuf.front()));

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// Загружает указанный ресурс
//
HGLOBAL CDlgMain::GetResourceLang(LPWSTR lpResType, DWORD dwResID)
{
	HINSTANCE hAppInstance = GetModuleHandle(NULL);
	HRSRC     hrRes;

	hrRes = FindResourceEx(hAppInstance, lpResType, MAKEINTRESOURCE(dwResID), m_wLanguageID);
	if (NULL == hrRes)
	{
		hrRes = FindResourceEx(hAppInstance, lpResType, MAKEINTRESOURCE(dwResID), DEFAULT_LANGUAGE);
	}

	return LoadResource(hAppInstance, hrRes);
}


//////////////////////////////////////////////////////////////////////////
// Загружает строку по указанному идентификатору в соответствии с 
// установленным языком
//
wstring CDlgMain::GetStringLang(DWORD dwResID)
{
	LPCWSTR lpStr;

	lpStr = (LPCWSTR)GetResourceLang(RT_STRING, 1 + (dwResID >> 4));
	if (NULL == lpStr)
	{
		return L"";
	}

	for (DWORD dwStrPos = 0; dwStrPos < (dwResID & 0x000F); dwStrPos++)
	{
		lpStr += *lpStr + 1;
	}

	std::wstring wsStr = (lpStr + 1);

	wsStr.erase(*lpStr);

	return wsStr;
}


//////////////////////////////////////////////////////////////////////////
// Поиск установленных в системе языков
//
VOID CDlgMain::SearchInstallLanguage()
{
	UINT iLocales = GetKeyboardLayoutList(0, NULL);
	
	if (0 < iLocales)
	{
		HKL* phKL = new HKL[iLocales];

		ATLVERIFY(GetKeyboardLayoutList(iLocales, phKL));

		for (UINT i = 0; i < iLocales; i++)
		{
			vector<WCHAR> wcBuf(128);

			// наименование языка
			ATLVERIFY(GetLocaleInfo(MAKELCID(phKL[i], 0), LOCALE_SNATIVELANGNAME, 
				&wcBuf.front(), static_cast<int>(wcBuf.size())));

			// перевод первой буквы в верхний регистр
			CharUpperBuff(&wcBuf.front(), 1);

			m_mapLangSystem.insert(make_pair(reinterpret_cast<WORD>(phKL[i]), &wcBuf.front()));

			// аббревиатура
			ATLVERIFY(GetLocaleInfo(MAKELCID(phKL[i], 0), LOCALE_SABBREVLANGNAME,
				&wcBuf.front(), static_cast<int>(wcBuf.size())));

			m_mapLangAcronym.insert(make_pair(reinterpret_cast<WORD>(phKL[i]), &wcBuf.front()));

		}
		
		delete [] phKL;
	}
}


//////////////////////////////////////////////////////////////////////////
// Устанавливает флаг текущей раскладки
//
VOID CDlgMain::SetFlagLanguage(HKL hklCurrent, CDlgFlag* pFlagDlg, BOOL bViewFlag, 
	BOOL bViewLarge, BYTE bTrasparency)
{
	// выход, если расскладка не указана
	if (0 == hklCurrent)
	{
		return;
	}

	// иконки
	CMapWordPHIcon::const_iterator iconSmall;
	CMapWordPHIcon::const_iterator iconLarge;
	CMapWordPHIcon::const_iterator iconAcronymSmall;
	CMapWordPHIcon::const_iterator iconAcronymLarge;

	iconSmall = m_mapIconsSmall.find(LOWORD(hklCurrent));
	ATLASSERT(iconSmall != m_mapIconsSmall.end());

	iconAcronymSmall = m_mapIconsAcronymSmall.find(LOWORD(hklCurrent));
	ATLASSERT(iconAcronymSmall != m_mapIconsAcronymSmall.end());

	// меняем иконку в трее
	if (TRUE == m_bViewTrayLang)
	{
		if (TRUE == bViewFlag)
		{
			if (iconSmall != m_mapIconsSmall.end())
			{
				ModifyNotifyIcon(iconSmall->second);
			}
		}
		else
		{
			if (iconAcronymSmall != m_mapIconsAcronymSmall.end())
			{
				ModifyNotifyIcon(iconAcronymSmall->second);
			}
		}
	}

	// меняем иконку на окне-индикаторе каретки
	if (pFlagDlg && pFlagDlg->IsWindow())
	{
		// иконка флага
		if (bViewFlag)
		{
			if (bViewLarge)
			{
				iconLarge = m_mapIconsLarge.find(LOWORD(hklCurrent));
				
				ATLASSERT(iconLarge != m_mapIconsLarge.end());
				if (iconLarge != m_mapIconsLarge.end())
				{
					pFlagDlg->ChangeIcon(iconLarge->second, bTrasparency);
				}
			}
			else
			{
				if (iconSmall != m_mapIconsSmall.end())
				{
					pFlagDlg->ChangeIcon(iconSmall->second, bTrasparency);
				}
			}
		}

		// иконка акронима
		else
		{
			if (bViewLarge)
			{
				iconAcronymLarge = m_mapIconsAcronymLarge.find(LOWORD(hklCurrent));

				ATLASSERT(iconAcronymLarge != m_mapIconsAcronymLarge.end());
				if (iconAcronymLarge != m_mapIconsAcronymLarge.end())
				{
					pFlagDlg->ChangeIcon(iconAcronymLarge->second, bTrasparency);
				}			
			}
			else
			{
				if (iconAcronymSmall != m_mapIconsAcronymSmall.end())
				{
					pFlagDlg->ChangeIcon(iconAcronymSmall->second, bTrasparency);
				}
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// Инициализация шаблона диалога
//
VOID CDlgMain::DoInitTemplate() 
{
	m_Template.Create((LPDLGTEMPLATE)GetResourceLang(RT_DIALOG, this->IDD));
}


//////////////////////////////////////////////////////////////////////////
// Инициализация контролов диалога
//
VOID CDlgMain::DoInitControls() 
{
	ATLASSERT(TRUE);
}



//////////////////////////////////////////////////////////////////////////
// Сообщение о смене языка программы
//
LRESULT CDlgMain::OnChangeLanguage(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// ИД языка
	WORD wLanguageId = static_cast<WORD>(wParam);

	// поиск языка
	CMapWordStr::iterator find = m_mapLanguages.find(wLanguageId);

	// выходим, если указанный язык не поддерживаем
	if (find == m_mapLanguages.end())
	{
		return static_cast<LRESULT>(0);
	}

	// смена языка
	m_wLanguageID = wLanguageId;

	// перезапуск программы
	CloseDialog(CHANGE_LANGUAGE);	

	return static_cast<LRESULT>(0);
}


//////////////////////////////////////////////////////////////////////////
// Загрузка иконок флагов в соответствии установленным языкам
//
VOID CDlgMain::LoadFlagIcons()
{
	ATLASSERT(!m_mapLangAcronym.empty());

	// получение полного пути к каталогу с иконками
	std::wstring       wsPath;
	std::vector<WCHAR> pwcPath(MAX_PATH);

	ATLVERIFY(GetModuleFileName(GetModuleHandle(NULL), &pwcPath.front(), MAX_PATH));

	wsPath = &pwcPath.front();
	wsPath.erase(wsPath.rfind(L"\\") + 1);
	wsPath += L"Icons\\";

	// цикл по всем найденным языкам
	std::wstring wsIconPath = L"";
	HICON        hIconSmall = NULL;
	HICON        hIconLarge = NULL;

	for (CMapWordStr::const_iterator itemLanguage = m_mapLangAcronym.begin(); 
		itemLanguage != m_mapLangAcronym.end(); itemLanguage++)
	{
		// загружаем иконку
		wsIconPath = wsPath + itemLanguage->second + L".ico";

		hIconSmall = (HICON)LoadImage(NULL, wsIconPath.c_str(), IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
		hIconLarge = (HICON)LoadImage(NULL, wsIconPath.c_str(), IMAGE_ICON, 24, 24, LR_LOADFROMFILE);

		if (!hIconLarge)
		{
			HICON hIcon = (HICON)::LoadImage(g_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_UNKNOW), 
				IMAGE_ICON, 24, 24, LR_CREATEDIBSECTION | LR_SHARED);

			hIconLarge = hIcon;
		}

		if (!hIconSmall)
		{
			HICON hIcon = (HICON)::LoadImage(g_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_UNKNOW), 
				IMAGE_ICON, 16, 16, LR_CREATEDIBSECTION | LR_SHARED);

			hIconSmall = hIcon;
		}

		m_mapIconsLarge.insert(make_pair(itemLanguage->first, hIconLarge));
		m_mapIconsSmall.insert(make_pair(itemLanguage->first, hIconSmall));
	}

	return;
}


//////////////////////////////////////////////////////////////////////////
// Функция возвращает хендл иконки сформированной из текста
//
HICON CDlgMain::GetIconFromStr(wstring wsText, BOOL bLarge, COLORREF crText, COLORREF crBackground)
{
	HDC            hMemDC;
	BITMAPV5HEADER bi;
	HBITMAP        hBitmap, hOldBitmap;
	DWORD          *lpBMP;
	LONG           lWidth = bLarge ? 24 : 16;

	// создание контекста для рисования
	HDC hDC = ::GetDC(NULL);

	hMemDC = CreateCompatibleDC(hDC);
	::ReleaseDC(NULL, hDC);

	// создаем битмап для рисования
	ZeroMemory(&bi,sizeof(BITMAPV5HEADER));

	bi.bV5Size        = sizeof(BITMAPV5HEADER);
	bi.bV5Width       = lWidth;
	bi.bV5Height      = lWidth;
	bi.bV5Planes      = 1;
	bi.bV5BitCount    = 32;
	bi.bV5Compression = BI_RGB;

	hBitmap = CreateDIBSection(hMemDC, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void **)&lpBMP, NULL, (DWORD)0);	
	ZeroMemory(lpBMP, (lWidth * lWidth) << 2);

	hOldBitmap = static_cast<HBITMAP>(::SelectObject(hMemDC, hBitmap));

	agg::rendering_buffer aggRendBuf;
	aggRendBuf.attach(reinterpret_cast<PUCHAR>(lpBMP), lWidth, lWidth, lWidth * 4);

	agg::pixfmt_bgra32                     aggPixF(aggRendBuf);
	agg::renderer_base<agg::pixfmt_bgra32> aggRenB(aggPixF);

	// очистка фона
	if (0xFF000000 & crBackground)
	{
		aggRenB.clear(agg::rgba8(GetRValue(crBackground), GetGValue(crBackground), 
			GetBValue(crBackground), (crBackground & 0xFF000000) >> 24));
	}

	agg::renderer_scanline_aa_solid<agg::renderer_base<agg::pixfmt_bgra32>> aggRen(aggRenB);
	agg::rasterizer_scanline_aa<>                                           aggRas;
	agg::scanline_p8                                                        aggSl;

	// инициализация шрифта
	font_engine_type     aggFontEng(hMemDC);
	font_manager_type    aggFontMan(aggFontEng);
	conv_font_curve_type aggFontCurves(aggFontMan.path_adaptor());

	aggFontEng.height(300);
	aggFontEng.weight(FW_NORMAL);
	aggFontEng.create_font(DEFAULT_ACRONYM_ICON_FONT, agg::glyph_ren_outline);

	// получение относительных размеров текста
	double dTextLenX = 0;
	double dTextLenY = 0;

	for (UINT i = 0; i < wsText.length(); i++)
	{
		const agg::glyph_cache* aggGlyph = aggFontMan.glyph(wsText[i]);
		
		dTextLenX += aggGlyph->advance_x;
		double dTmp = aggGlyph->bounds.y2 - aggGlyph->bounds.y1;
		dTmp > dTextLenY ? dTextLenY = dTmp : 0;
	}

	// коээфициент пропорциональности текста с размером иконки
	double dScale = (dTextLenX > dTextLenY) ? (lWidth / dTextLenX) : (lWidth / dTextLenY);
	dScale *= 0.95; // чтобы нормально влезло в иконку
	
	// растеризация текста
	double dFontX = lWidth / 2. - (dTextLenX * dScale) / 2.;
	double dFontY = lWidth / 2. - (dTextLenY * dScale) / 2;

	for (UINT i = 0; i < wsText.length(); i++)
	{
		const agg::glyph_cache* aggGlyph = aggFontMan.glyph(wsText[i]);

		aggFontMan.add_kerning(&dFontX, &dFontY);
		aggFontMan.init_embedded_adaptors(aggGlyph, dFontX, dFontY, dScale);

		aggRas.add_path(aggFontCurves);

		dFontX += aggGlyph->advance_x * dScale;
	}

	aggRen.color(agg::rgba8(GetRValue(crText), GetGValue(crText), GetBValue(crText), 255));
	agg::render_scanlines(aggRas, aggSl, aggRen);

	SelectObject(hMemDC, hOldBitmap);

	// создаем маску для иконки
	HBITMAP hMonoBitmap = CreateBitmap(lWidth, lWidth, 1, 1, NULL);

	//  создаем иконку
	ICONINFO iconInfo;

	iconInfo.fIcon    = TRUE;
	iconInfo.xHotspot = lWidth >> 1;
	iconInfo.yHotspot = lWidth >> 1;
	iconInfo.hbmMask  = hMonoBitmap;
	iconInfo.hbmColor = hBitmap;

	HICON hIcon = CreateIconIndirect(&iconInfo);

	// очищаем память
	DeleteObject(hBitmap);          
	DeleteObject(hMonoBitmap); 
	DeleteDC(hMemDC);

	return hIcon;
}


//////////////////////////////////////////////////////////////////////////
// Получение иконок с аббревиатурами установленных языков
//
BOOL CDlgMain::GenerateAcronymIcons(COLORREF crText, COLORREF crBackground)
{
	ATLASSERT(!m_mapLangAcronym.empty());

	// удаляем иконки
	for (CMapWordPHIcon::const_iterator 
		hiAcronymSmall = m_mapIconsAcronymSmall.begin(),
		hiAcronymLarge = m_mapIconsAcronymLarge.begin(); 
		hiAcronymSmall != m_mapIconsAcronymSmall.end(); 
		hiAcronymSmall++, hiAcronymLarge++)
	{
		ATLVERIFY(DestroyIcon(hiAcronymSmall->second));
		ATLVERIFY(DestroyIcon(hiAcronymLarge->second));
	}

	m_mapIconsAcronymSmall.clear();
	m_mapIconsAcronymLarge.clear();

	// создаем новые иконки
	for (CMapWordStr::const_iterator itemLanguage = m_mapLangAcronym.begin(); 
		itemLanguage != m_mapLangAcronym.end(); itemLanguage++)
	{
		std::wstring wsAcronym = (itemLanguage->second).substr(0, 2);
		
		m_mapIconsAcronymSmall.insert(make_pair(itemLanguage->first, 
			GetIconFromStr(wsAcronym, FALSE, crText, crBackground)));
		m_mapIconsAcronymLarge.insert(make_pair(itemLanguage->first, 
			GetIconFromStr(wsAcronym, TRUE, crText, crBackground)));
	}
	
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// Вывод меню по нажатию левой кнопки мыши на значке в трее
//
VOID CDlgMain::OnNotifyIconLButtonUp(const POINT& rpt)
{
	OnNotifyIconRButtonUp(rpt);
}


//////////////////////////////////////////////////////////////////////////
// Вывод меню в трее по нажатию правой кнопки мыши
//
VOID CDlgMain::OnNotifyIconRButtonUp(const POINT& rpt)
{
	// если работает диалог настроек
	// то меню не выводим
	if (m_pSettingsDlg)
	{
		SetForegroundWindow(*m_pSettingsDlg);

		return;
	}

	CMenu menuTray;
	if (FALSE == menuTray.LoadMenuIndirect((const MENUTEMPLATEW*)GetResourceLang(RT_MENU, IDR_CONTEXTMENU)))
	{
		return;
	}

	CMenuHandle menuVisible;
	menuVisible = menuTray.GetSubMenu(0);

	// установка жирным пункта настройки
	menuVisible.SetMenuDefaultItem(IDB_SETTINGS);

	// вывод меню
	SetForegroundWindow(*this); // для того, чтобы меню исчезало при потере фокуса
	menuVisible.TrackPopupMenu(TPM_RIGHTALIGN | TPM_BOTTOMALIGN, rpt.x, rpt.y, m_hWnd, NULL);
	PostMessage(WM_NULL);
}


//////////////////////////////////////////////////////////////////////////
// Проверка активной раскладки по таймеру
//
LRESULT CDlgMain::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// получение хендла активного окна
	m_hWndActive = GetForegroundWindow();
	if (FALSE == ::IsWindow(m_hWndActive))
	{
		return static_cast<LRESULT>(0);
	}

	// если активное окно изменилось, получаем его тип
	if (m_hWndActiveLast != m_hWndActive)
	{
		m_hWndActiveLast = m_hWndActive;

		vector<WCHAR> wcClassName(32);
		wstring       wsClassName;
		int           iCount;

		iCount = GetClassName(m_hWndActive, &wcClassName.front(), static_cast<int>(wcClassName.size()));
		ATLASSERT(0 < iCount);

		if (0 >= iCount)
		{
			return static_cast<LRESULT>(0);
		}

		wsClassName = &wcClassName.front();

		if (CONSOLE_WINDOW_CLASS == wsClassName)
		{
			m_bConsole = TRUE;
		}
		else
		{
			m_bConsole = FALSE;
		}

		// получение ИД активного процесса и треда
		m_dwActThreadId = GetWindowThreadProcessId(m_hWndActive, &m_dwActProcessId);
	}

	// получение активной раскладки
	m_hKLCurrent = GetActiveLayout();

	// смена иконки
	if (m_hKLCurrent != m_hKLLast)
	{
		m_hKLLast = m_hKLCurrent;

		SetFlagLanguage(m_hKLCurrent, m_pCaretFlag, m_bViewFlag, m_bViewLarge, m_bTransparent);
	}

	// получаем окно и его имя класса, на котором находится фокус ввода
	/*
	if (FALSE == m_bConsole)
	{
		AttachThreadInput(m_dwCurrentThread, m_dwActThreadId, TRUE);
		HWND m_hWndFocus = GetFocus();
		AttachThreadInput(m_dwCurrentThread, m_dwActThreadId, FALSE);

		if (m_hWndFocus != m_hWndFocusLast)
		{
			m_hWndFocusLast = m_hWndFocus;

			vector<WCHAR> wcClassName(32);
			wstring       wsClassName;
			int           iCount;
			
			iCount = GetClassName(m_hWndFocus, &wcClassName.front(), wcClassName.size());
			wsClassName = &wcClassName.front();

			if (0 >= iCount)
			{
				m_eEditUIType = EDITUITYPE_INVALID;
			}
			else
			{
				wsClassName = std::upper(wsClassName);
				ATLTRACE(L"%s\n", wsClassName.c_str());
				
				// Edit - стандартное окно ввода
				if (L"EDIT" == wsClassName)
				{
					m_eEditUIType = EDITUITYPE_EDIT;
				}
				else

				// RichEdit
				if (L"RICHEDIT50W" == wsClassName)
				{
					m_eEditUIType = EDITUITYPE_RICHEDIT;
				}
				else

				// Firfox
				if (L"MOZILLAWINDOWCLASS" == wsClassName)
				{
					m_eEditUIType = EDITUITYPE_FIREFOX;
				}
				else

				// Opera
				if (L"OPERAWINDOWCLASS" == wsClassName)
				{
					m_eEditUIType = EDITUITYPE_OPERA;
				}
				else

				// Internet Explorer
				if (L"INTERNET EXPLORER_SERVER" == wsClassName)
				{
					m_eEditUIType = EDITUITYPE_IEXPLORER;
				}
				else

				// Open Office
				if (L"SALFRAME" == wsClassName)
				{
					m_eEditUIType = EDITUITYPE_OOFFICE;
				}

				// Неизвестный тип
				else
				{
					m_eEditUIType = EDITUITYPE_UNKNOW;
				}
			}
		}
	}
	*/

	// вычисление позиции курсора и обновление позиции окна-индикатора
	// только для консоли
	if (m_bConsole)
	{
		CRect rectCursor;
		if (TRUE == GetCursorRect(&rectCursor))
		{
			SetCaretActivity(TRUE, m_hWndActiveLast);
			UpdateCaretFlagPos(rectCursor);
		}
	}

	// скрываем индикатор, если переключились на другое окно
	if (!m_bConsole)
	{
		EnterCriticalSection(&m_csCaretActivity);
		HWND hWnd = m_hWndCaretActivity;
		LeaveCriticalSection(&m_csCaretActivity);

		if ((FALSE == ::IsChild(m_hWndActiveLast, hWnd)) && (m_hWndActiveLast != hWnd))
		{
			SetCaretActivity(FALSE, m_hWndActiveLast);
			SetHideEvent();
		}
	}
	
	return static_cast<LRESULT>(0);
}


//////////////////////////////////////////////////////////////////////////
// Возвращает активную раскладки
//
HKL CDlgMain::GetActiveLayout()
{
	if (!::IsWindow(m_hWndActive))
	{
		return static_cast<HKL>(NULL);
	}

	if (m_bConsole)
	{
		if (NULL == m_lpAttachConsole)
		{
			return GetLastLayout();
		}

		HKL hKLConsole = NULL;

		while (TRUE, TRUE)
		{
			// аттач к консоли
			if (!m_lpAttachConsole(m_dwActProcessId))
			{
				_RPT_API_ERROR(AttachConsole);
				break;
			}

			// получение раскладки
			WCHAR wcLayout[9];
			if (FALSE == m_lpGetConKeybLayName(reinterpret_cast<LPWSTR>(&wcLayout)))
			{
				_RPT_API_ERROR(GetConsoleKeyboardLayoutName);
			}

			wistringstream inStr(wcLayout);
			HANDLE         hKL;
			
			inStr.setf(wistringstream::hex);
			inStr >> hKL;
			hKLConsole = static_cast<HKL>(hKL);
			
			// отсоединяемся от консоли
			ATLVERIFY(m_lpFreeConsole());

			break;
		}

		return hKLConsole;
	}
	else
	{
		return GetKeyboardLayout(m_dwActThreadId);
	}
}


//////////////////////////////////////////////////////////////////////////
// Возвращает координаты текстового курсора
//
BOOL CDlgMain::GetCursorRect(PRECT pRect)
{
	ATLASSERT(pRect);

	// инициализация кода возврата
	BOOL bReturn = FALSE;

	// проверка существования окна
	if (!::IsWindow(m_hWndActive))
	{
		return FALSE;
	}

	// получение позиции курсора консоли
	if (m_bConsole)
	{
		// аттач к консоли
		if ((NULL == m_lpAttachConsole) || !m_lpAttachConsole(m_dwActProcessId))
		{
			return FALSE;
		}

		while (TRUE, TRUE)
		{
			// хендл вывода консоли 
			HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

			// получение текстовых координат курсора
			
			if (!GetConsoleScreenBufferInfo(hOutput, &m_csbiInfo))
			{
				_RPT_API_ERROR(GetConsoleScreenBufferInfo);
				break;
			}

			// позиция scrollbar'ов
			m_iConScrollPosH = ::GetScrollPos(m_hWndActive, SB_HORZ);
			m_iConScrollPosV = ::GetScrollPos(m_hWndActive, SB_VERT);

			// получение шрифта консоли
			if (!GetCurrentConsoleFont(hOutput, FALSE, &m_cciFont))
			{
				_RPT_API_ERROR(GetCurrentConsoleFont);
				break;
			}

			// получение размера шрифта в пикселах
			m_sizeConFont = GetConsoleFontSize(hOutput, m_cciFont.nFont);

			// координаты окна вывода
			if (!::GetClientRect(m_hWndActive, &m_rectConClient))
			{
				_RPT_API_ERROR(GetClientRect);
				break;
			}

			// преобразование координат в экранные
			::ClientToScreen(m_hWndActive, &m_rectConClient.TopLeft());
			::ClientToScreen(m_hWndActive, &m_rectConClient.BottomRight());

			// вычисление позиции и размера курсора
			bReturn = GetConsoleCaretPos(m_csbiInfo.dwCursorPosition.X, 
				m_csbiInfo.dwCursorPosition.Y, *pRect);

			break;
		}

		ATLVERIFY(m_lpFreeConsole());

		return bReturn;
	}

	/*
	switch(m_eEditUIType)
	{
		// вообще непонятное окно
		case EDITUITYPE_INVALID:
			return FALSE;
			break;
		
		// получение курсора в Edit контролах
		case EDITUITYPE_EDIT:

		// получение курсора в RichEdit контролах
		case EDITUITYPE_RICHEDIT:

		// получение курсора в Internet Explorer
		case EDITUITYPE_IEXPLORER:

		// неизвестный тип окна - пробуем получить координаты курсора
		// MSOffice замечательно работает
		case EDITUITYPE_UNKNOW:
			{
				// размер курсора
				GUITHREADINFO guiInfo;

				ZeroMemory(&guiInfo, sizeof(guiInfo));
				guiInfo.cbSize = sizeof(guiInfo);

				if (FALSE == GetGUIThreadInfo(m_dwActThreadId, &guiInfo))
				{
					_RPT_API_TRACE(GetGUIThreadInfo);
					return FALSE;
				}

				// перевод в экранные координаты
				CRect rectCaret = guiInfo.rcCaret;
				::ClientToScreen(guiInfo.hwndCaret, &rectCaret.TopLeft());
				::ClientToScreen(guiInfo.hwndCaret, &rectCaret.BottomRight());

				// для Internet Explorer если высота курсора 1 или меньше, то курсор не выводится
				if (1 >= rectCaret.Height())
				{
					//return FALSE;
				}

				pRect->left   = rectCaret.right;
				pRect->right  = pRect->left;
				pRect->top    = rectCaret.top;
				pRect->bottom = rectCaret.bottom;

				return TRUE;
			}
			break;

		// получение курсора в Mozilla Firefox
		case EDITUITYPE_FIREFOX:
			{
				// курсор выдирается через MSAA
				ATLTRACE(L"Firefox\n");
				return FALSE;
			}
			break;

		// получение курсора в Opera
		case EDITUITYPE_OPERA:
			{
				ATLTRACE(L"Opera\n");
				return FALSE;
			}
			break;

		case EDITUITYPE_OOFFICE:
			{
				ATLTRACE(L"Open Office\n");
				return FALSE;
			}
			break;

		default:
			return FALSE;
			break;
	}
	*/

	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
// Вызов диалога настроек
//
LRESULT CDlgMain::OnSettings(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (m_pSettingsDlg)
	{
		if (m_pSettingsDlg->IsWindow())
		{
			return static_cast<LRESULT>(0);			
		}

		delete m_pSettingsDlg;
	}
	
	m_pSettingsDlg = new CDlgSettings(this);
	
	if (NULL != m_pSettingsDlg)
	{
		if (CHANGE_LANGUAGE != m_pSettingsDlg->DoModal(*this))
		{
			GenerateAcronymIcons(m_crIconAcronym, m_crBGIconAcronym);
			SetFlagLanguage(m_hKLCurrent, m_pCaretFlag, m_bViewFlag, m_bViewLarge, m_bTransparent);
		}
	}

	delete [] m_pSettingsDlg;
	m_pSettingsDlg = NULL;

	return static_cast<LRESULT>(0);
}


//////////////////////////////////////////////////////////////////////////
// Обновляет позицию окна текущей раскладки
// rectNew - позиция курсора
//
BOOL CDlgMain::UpdateCaretFlagPos(CRect rectNew)
{
	if (!m_pCaretFlag || !m_pCaretFlag->IsWindow())
	{
		return FALSE;
	}

	switch (m_eIconPosition)
	{
		//
		case ICONPOSITION_RIGHT_CENTER:
		{
			rectNew.MoveToX(rectNew.left + rectNew.Width());
			rectNew.MoveToY(rectNew.top - ((m_bViewLarge ? 24 : 16) - rectNew.Height()) / 2);

			break;
		}

		//
		case ICONPOSITION_RIGHT_TOP:
		{
			rectNew.MoveToX(rectNew.left + rectNew.Width());
			rectNew.MoveToY(rectNew.top - (m_bViewLarge ? 24 : 16));

			break;
		}

		//
		case ICONPOSITION_RIGHT_BOTTOM:
		{
			rectNew.MoveToX(rectNew.left + rectNew.Width());
			rectNew.MoveToY(rectNew.top + rectNew.Height());

			break;
		}

		//
		case ICONPOSITION_LEFT_CENTER:
		{
			rectNew.MoveToX(rectNew.left - (m_bViewLarge ? 24 : 16));
			rectNew.MoveToY(rectNew.top - ((m_bViewLarge ? 24 : 16) - rectNew.Height()) / 2);

			break;
		}

		//
		case ICONPOSITION_LEFT_TOP:
		{
			rectNew.MoveToX(rectNew.left - (m_bViewLarge ? 24 : 16));
			rectNew.MoveToY(rectNew.top - (m_bViewLarge ? 24 : 16));

			break;
		}

		//
		case ICONPOSITION_LEFT_BOTTOM:
		{
			rectNew.MoveToX(rectNew.left - (m_bViewLarge ? 24 : 16));
			rectNew.MoveToY(rectNew.top + rectNew.Height());

			break;
		}
	}

	m_pCaretFlag->SetWindowPos(NULL, &rectNew, SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// Возвращает координаты каретки консольного окна
//
BOOL CDlgMain::GetConsoleCaretPos(SHORT sX, SHORT sY, RECT& rcCaret)
{
	sX = sX - static_cast<SHORT>(m_iConScrollPosH);
	sY = sY - static_cast<SHORT>(m_iConScrollPosV);

	// проверка попадания курсора в видимую область
	if ((sX < 0) || (sY < 0) || (sX > (m_csbiInfo.srWindow.Right - m_csbiInfo.srWindow.Left)) || 
		(sY > (m_csbiInfo.srWindow.Bottom - m_csbiInfo.srWindow.Top)))
	{
		return FALSE;
	}

	// вычисление абсолютных координат каретки
	rcCaret.left   = m_rectConClient.left + m_rectConClient.Width() / 
		(m_csbiInfo.srWindow.Right - m_csbiInfo.srWindow.Left + 1) * sX;
	rcCaret.right  = rcCaret.left + m_sizeConFont.X;
	rcCaret.top    = m_rectConClient.top + m_rectConClient.Height() /
		(m_csbiInfo.srWindow.Bottom - m_csbiInfo.srWindow.Top + 1) * sY;
	rcCaret.bottom = rcCaret.top + m_sizeConFont.Y;

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// Изменение позиции каретки в GUI
//
LRESULT CDlgMain::OnCaretGUIRepos(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	m_rectTmpCaret.left    = static_cast<LONG>(LOWORD(wParam));
	m_rectTmpCaret.top     = static_cast<LONG>(HIWORD(wParam));
	m_rectTmpCaret.right   = static_cast<LONG>(LOWORD(lParam)) + m_rectTmpCaret.left;
	m_rectTmpCaret.bottom  = static_cast<LONG>(HIWORD(lParam)) + m_rectTmpCaret.top;

	ATLVERIFY(UpdateCaretFlagPos(m_rectTmpCaret));

	return static_cast<LRESULT>(0);
}


//////////////////////////////////////////////////////////////////////////
// Изменение позиции каретки в консоли
//
LRESULT CDlgMain::OnCaretConsoleRepos(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (GetConsoleCaretPos(static_cast<SHORT>(wParam), static_cast<SHORT>(lParam), m_rectTmpCaret))
	{
		ATLVERIFY(UpdateCaretFlagPos(m_rectTmpCaret));
	}

	return static_cast<LRESULT>(0);
}


//////////////////////////////////////////////////////////////////////////
// Сокрытие каретки
//
LRESULT CDlgMain::OnCaretHide(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// если курсор еще не появлялся, то нужно его скрыть
	if (FALSE == GetCaretActivity())
	{
		if (m_pCaretFlag && m_pCaretFlag->IsWindow())
		{
			m_pCaretFlag->ShowWindow(SW_HIDE);
		}
	}

	return static_cast<LRESULT>(0);
}


//////////////////////////////////////////////////////////////////////////
// Устанавливает время последней активности каретки
//
VOID CDlgMain::SetCaretActivity(BOOL bActivity, HWND hWnd)
{
	EnterCriticalSection(&m_csCaretActivity);
	m_bCaretActivity    = bActivity;
	m_hWndCaretActivity = hWnd;
	LeaveCriticalSection(&m_csCaretActivity);
}


//////////////////////////////////////////////////////////////////////////
// Возвращает время последней активности каретки
//
BOOL CDlgMain::GetCaretActivity()
{
	BOOL bActivity;
	EnterCriticalSection(&m_csCaretActivity);
	bActivity = m_bCaretActivity;
	LeaveCriticalSection(&m_csCaretActivity);
	return bActivity;
}


//////////////////////////////////////////////////////////////////////////
// Возвращает TRUE, если программа запускается вместе с Windows
//
BOOL CDlgMain::GetAutostart()
{
	BOOL bAutostart = FALSE;
	HKEY hKey;

	if (ERROR_SUCCESS == RegOpenKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey))
	{
		if (ERROR_SUCCESS == RegQueryValueEx(hKey, PROGRAM_NAME, NULL, NULL, NULL, NULL))
		{
			bAutostart = TRUE;
		}
		else
		{
			bAutostart = FALSE;
		}

		RegCloseKey(hKey);
	}

	return bAutostart;
}


//////////////////////////////////////////////////////////////////////////
// Устанавливает режим автостарта при запуске Windows
//
VOID CDlgMain::SetAutostart(BOOL bAutostart)
{
	HKEY  hKey;
	WCHAR ptcFileName[MAX_PATH];

	if (ERROR_SUCCESS == RegOpenKey(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), &hKey))
	{
		if (bAutostart)
		{
			ZeroMemory(ptcFileName, sizeof(WCHAR) * MAX_PATH);
			GetModuleFileName(NULL, ptcFileName, sizeof(WCHAR) * (MAX_PATH - 1));
			RegSetValueEx(hKey, PROGRAM_NAME, 0, REG_SZ, (BYTE*)ptcFileName, 
				static_cast<DWORD>(wcslen(ptcFileName) * sizeof(WCHAR)));
		}
		else
		{
			RegDeleteValue(hKey, PROGRAM_NAME);
		}

		RegCloseKey(hKey);
	}
}


//////////////////////////////////////////////////////////////////////////
// обработка WM_SETTINGCHANGE
//
LRESULT CDlgMain::OnSettingsChange(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (WAIT_OBJECT_0 == WaitForSingleObject(m_hEventExit, 0))
	{
		return static_cast<LRESULT>(0);
	}

	// If someone else turns off the system-wide screen reader
	// flag, we want to turn it back on.
	if (SPI_SETSCREENREADER == wParam)
	{
		BOOL fSPI = FALSE;

		SystemParametersInfo(SPI_GETSCREENREADER, 0, &fSPI, 0 );

		if (FALSE == fSPI)
		{
			SystemParametersInfo(SPI_SETSCREENREADER, TRUE, NULL, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
		}
	}

	return static_cast<LRESULT>(0);
}

//////////////////////////////////////////////////////////////////////////
// Конструктор с передачей указатель на главный диалог
//
CExistCaret::CExistCaret(CDlgMain* pMainDlg, PHANDLE phEventExit, PHANDLE phEventHide)
	:CThreadImpl<CExistCaret>(CREATE_SUSPENDED)
{
	ATLASSERT(NULL != pMainDlg);
	m_pMainDlg = pMainDlg;

	ATLASSERT(NULL != phEventExit);
	m_phEventExit = phEventExit;

	ATLASSERT(NULL != phEventHide);
	m_phEventHide = phEventHide;
}


//////////////////////////////////////////////////////////////////////////
// Цикл слежения за кареткой
//
DWORD CExistCaret::Run()
{
	const HANDLE pHandles[] = {*m_phEventExit, *m_phEventHide};
	DWORD        dwReturn;

	while (TRUE, TRUE)
	{
		dwReturn = WaitForMultipleObjects(2, pHandles, FALSE, 1000);

		// отвалились по таймауту, продолжаем цикл
		if (WAIT_TIMEOUT == dwReturn)
		{
			continue;
		}
		else

		// если было событие выхода, то завершаем работу
		if (WAIT_OBJECT_0 == dwReturn)
		{
			break;
		}
		else

		// если событие сокрытия, выжидаем паузу
		if ((WAIT_OBJECT_0 + 1) == dwReturn)
		{
			Sleep(DEFAULT_TIMEOUT_HIDE_ICONCARET);
			m_pMainDlg->PostMessage(WM_USER_CARET_HIDE);
		}
		else

		// ошибка
		{
			ATLASSERT(FALSE);
			break;
		}
	}

	return 0;
}


