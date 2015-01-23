#pragma once

#include "resource.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

class CDlgFlag;
class CDlgMain;
class CDlgSettings;

//////////////////////////////////////////////////////////////////////////
// Класс для отслеживания сокрытия каретки
//
class CExistCaret:
	public CThreadImpl<CExistCaret>
{
	private:
		CDlgMain* m_pMainDlg;    // ссылка на главный класс программы
		PHANDLE   m_phEventExit; // событие завершения программы
		PHANDLE   m_phEventHide; // мутекс сигнализирующий о сокрытии каретки

	public:

		// Конструктор с передачей указатель на главный диалог
		CExistCaret(CDlgMain* pMainDlg, PHANDLE phEventExit, PHANDLE phEventHide);

		// Деструктор
		~CExistCaret(){}

		// Цикл слежения за кареткой
		DWORD Run();
};


//////////////////////////////////////////////////////////////////////////
// Класс главного окна
//
class CDlgMain : 
	public CIndirectDialogImpl<CDlgMain, CResDlgTemplate>,
	public CMessageFilter,
	public CIdleHandler, 
	public CTrayIconImpl<CDlgMain>,
	public CSettingsImpl
{
	public:

		// карта сообщений
		BEGIN_MSG_MAP(CDlgMain)
			CHAIN_MSG_MAP(CTrayIconImpl<CDlgMain>)
			MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
			MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
			MESSAGE_HANDLER(WM_TIMER, OnTimer)
			COMMAND_ID_HANDLER(IDOK, OnOK)
			COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
			COMMAND_ID_HANDLER(ID_APP_EXIT, OnExit)
			MESSAGE_HANDLER(WM_USER_CHANGE_LANGUAGE, OnChangeLanguage)
			MESSAGE_HANDLER(WM_USER_CARET_GUI_REPOS, OnCaretGUIRepos)
			MESSAGE_HANDLER(WM_USER_CARET_CONSOLE_REPOS, OnCaretConsoleRepos)
			MESSAGE_HANDLER(WM_USER_CARET_HIDE, OnCaretHide)
			MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingsChange)
			COMMAND_ID_HANDLER(IDB_SETTINGS, OnSettings)
			REFLECT_NOTIFICATIONS()
		END_MSG_MAP()

	public:

		// ID диалога
		enum { IDD = IDD_MAINDLG };

	protected:

		HMODULE             m_hKernelDll;           //
		LPATTACHCONSOLE     m_lpAttachConsole;      //
		LPFREECONSOLE       m_lpFreeConsole;        //
		LPGETCONKEYBLAYNAME m_lpGetConKeybLayName;  //
		HWINEVENTHOOK       m_hEventHook;           //

		CMapWordStr         m_mapLanguages;         // список языков программы(ID, язык)
		CMapWordStr         m_mapLangSystem;        // список языков установленных в системе  
		CMapWordStr         m_mapLangAcronym;       // список аббревиатур установленных в системе языков
		CMapWordPHIcon      m_mapIconsLarge;        // список иконок 24x24
		CMapWordPHIcon      m_mapIconsSmall;        // список иконок 16x16
		CMapWordPHIcon      m_mapIconsAcronymSmall; // список иконок с аббревиатурами установленных языков 16x16
		CMapWordPHIcon      m_mapIconsAcronymLarge; // список иконок с аббревиатурами установленных языков 24x24
		
		CDlgFlag*           m_pCaretFlag;           // указатель на окно-индикатор для текстовой каретки
		CDlgSettings*       m_pSettingsDlg;         // указатель на диалог настроек

		HANDLE              m_hEventExit;           // событие завершения 
		CExistCaret         m_thCaret;              // тред сокрытия каретки

		WORD                m_wLanguageID;          // ИД выбранного языка программы
		BOOL                m_bViewTrayLang;        // показывать значок раскладки в трее
		BOOL                m_bViewFlag;            // показывать флаг текущего языка в трее
		BOOL                m_bViewLarge;           // показывать большой флаг (24x24) у каретки
		DWORD               m_crIconAcronym;        // цвет текста иконки акронима
		DWORD               m_crBGIconAcronym;      // цвет фона иконки акронима
		BYTE                m_bTransparent;         // прозрачность (0 - полная прозрачность)

		DWORD               m_dwCurrentThread;      // ИД собственного потока
		HWND                m_hWndActive;           // хендл текущего активного окна
		HWND                m_hWndActiveLast;       // хендл последнего активного окна
		HWND                m_hWndFocus;            // хендл окна на котором находится фокус
		HWND                m_hWndFocusLast;        // хендл окна на котором находился фокус последний раз
		BOOL                m_bConsole;             // TRUE, если окно консольное
		HKL                 m_hKLCurrent;           // активная раскладка
		HKL                 m_hKLLast;              // последняя активная раскладка
		DWORD               m_dwActProcessId;       // ИД активного процесса
		DWORD               m_dwActThreadId;        // ИД активного треда
		EDITUITYPE          m_eEditUIType;          // Тип окна ввода текста
		ICONPOSITION        m_eIconPosition;        // Позиция окна индикатора-иконки

		HANDLE              m_hEventCaretHide;      // событие сокрытия каретки
		CRITICAL_SECTION    m_csCaretActivity;      // Критическая секция для изменения значения последней активности каретки
		BOOL                m_bCaretActivity;       // TRUE, если каретка активна
		HWND                m_hWndCaretActivity;    // хендл окна последней активности каретки
		
		CONSOLE_SCREEN_BUFFER_INFO m_csbiInfo;      // 
		CONSOLE_FONT_INFO   m_cciFont;              // 
		INT                 m_iConScrollPosH;       //
		INT                 m_iConScrollPosV;       //
		COORD               m_sizeConFont;          //
		CRect               m_rectConClient;        //
		
		CRect               m_rectTmpCaret;         // структура для временного использования

	public:

		// Конструктор по-умолчанию
		CDlgMain();

		// Деструктор по-умолчанию
		~CDlgMain();

		// Инициализация шаблона диалога
		VOID DoInitTemplate();

		// Инициализация контролов диалога
		VOID DoInitControls();

	protected:

		// Обработка сообщений
		virtual BOOL PreTranslateMessage(MSG* pMsg);

		// Фоновая обработка сообщений
		virtual BOOL OnIdle();

		// Инициализация диалога
		LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		// Обработка WM_DESTROY
		LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		// Обработка IDOK
		LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// Обработка IDCANCEL
		LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// Обработка ID_APP_EXIT
		LRESULT OnExit(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

		// Закрытие диалога
		VOID CloseDialog(INT iVal);

		// Поиск всех возможных языков программы
		VOID SearchLanguage();

		static BOOL CALLBACK SearchLanguageCallback(HMODULE hModule, LPCTSTR lpszType, 
			LPTSTR lpszName, WORD wIDLanguage, LONG_PTR lParam);

		// Поиск установленных в системе языков
		VOID SearchInstallLanguage();

		// Смена языка
		LRESULT OnChangeLanguage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

		// Загрузка иконок флагов
		VOID LoadFlagIcons();

		// Функция возвращает хендл иконки сформированной из текста
		HICON GetIconFromStr(wstring wsText, BOOL bLarge, COLORREF crText, COLORREF crBackground);

		// Проверка активной раскладки по таймеру
		LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		// Возвращает активную раскладки
		HKL GetActiveLayout();

		// Возвращает координаты текстового курсора
		BOOL CDlgMain::GetCursorRect(PRECT pRect);

		// Вызов диалога настроек
		LRESULT OnSettings(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

		// устанавливает флаг текущей раскладки
		VOID SetFlagLanguage(HKL hklCurrent, CDlgFlag* pFlagDlg, BOOL bViewFlag, BOOL bViewLarge, BYTE bTrasparency);

		// Изменение позиции каретки в GUI
		LRESULT OnCaretGUIRepos(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

		// Изменение позиции каретки в консоли
		LRESULT OnCaretConsoleRepos(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

		// Сокрытие каретки
		LRESULT OnCaretHide(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

		// обработка WM_SETTINGCHANGE
		LRESULT OnSettingsChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	public:

		// Загружает указанный ресурс
		HGLOBAL GetResourceLang(LPWSTR lpResType, DWORD dwResID);

		// Загружает строку по указанному идентификатору
		wstring GetStringLang(DWORD dwResID);

		// Вывод меню по нажатию левой кнопки мыши на значке в трее
		VOID OnNotifyIconLButtonUp(const POINT& rpt);

		// Вывод меню в трее по нажатию правой кнопки мыши
		VOID OnNotifyIconRButtonUp(const POINT& rpt);

		// Возвращает список доступных языков
		inline CMapWordStr* GetMapLanguages(){return &m_mapLanguages;}

		// Возвращает ИД выбранного языка программы
		inline WORD GetLanguageID() {return m_wLanguageID;}

		// Возвращает true, если активное окно консольное
		inline BOOL ActiveWndIsConsole() {return m_bConsole;}

		// возвращает хендл активного окна
		inline HWND GetActiveWnd() {return m_hWndActive;}

		// Обновляет позицию окна текущей раскладки
		BOOL UpdateCaretFlagPos(CRect rectNew);

		// Возвращает координаты каретки консольного окна
		BOOL GetConsoleCaretPos(SHORT sX, SHORT sY, RECT& rcCaret);

		// Устанавливает время последней активности каретки
		inline VOID SetCaretActivity(BOOL bActivity, HWND hWnd);

		// Возвращает время последней активности каретки
		inline BOOL GetCaretActivity();

		// Устанавливает событие сокрытия окна-индикатора
		inline BOOL SetHideEvent(){return SetEvent(m_hEventCaretHide);}

		// Возвращает TRUE, если окно-индикатор отображать в виде флага
		inline BOOL GetViewFlag(){return m_bViewFlag;}
		inline VOID SetViewFlag(BOOL bViewFlag){m_bViewFlag = bViewFlag;}

		// Возвращает TRUE, если окно-индикатор большого размера
		inline BOOL GetViewLarge(){return m_bViewLarge;}
		inline VOID SetViewLarge(BOOL bViewLarge){m_bViewLarge = bViewLarge;}

		// Возвращает TRUE, если фон окна-индикатора прозрачный
		inline BOOL BGIsTransparent(){return (m_crBGIconAcronym & 0xFF000000) ? FALSE : TRUE;}

		// Возвращает индекс прозрачности
		inline BYTE GetTransparency(){return m_bTransparent;}
		inline VOID SetTransparency(BYTE bTransparent){m_bTransparent = bTransparent;}

		// Устанавливает иконку на указанном окне-индикаторе
		inline VOID SetFlagLanguage(CDlgFlag* pDlgFlag, BOOL bViewFlag, BOOL bViewLarge, BYTE bTrasparency)
			{return SetFlagLanguage(m_hKLCurrent, pDlgFlag, bViewFlag, bViewLarge, bTrasparency);}

		// Получение иконок с аббревиатурами установленных языков
		BOOL GenerateAcronymIcons(COLORREF crText, COLORREF crBackground);

		// Возвращает цвет текста иконок акронимов
		inline COLORREF GetColorIconActronym() {return m_crIconAcronym;}
		inline VOID SetColorIconActronym(COLORREF crIconAcronym){m_crIconAcronym = crIconAcronym;}

		// Возвращает цвет фона
		inline COLORREF GetBGColorIconActronym() {return m_crBGIconAcronym;}
		inline VOID SetBGColorIconActronym(COLORREF crBGIconAcronym){m_crBGIconAcronym = crBGIconAcronym;}

		// Возвращает TRUE, если установлен автостарт
		BOOL GetAutostart();

		// Устанавливает автостарт
		VOID SetAutostart(BOOL bAutostart);

		// Возвращает позицию окна-индикатора относительно каретки
		inline ICONPOSITION GetIconPosition(){return m_eIconPosition;}

		// Устанавливает позицию окна-индикатора относительно каретки
		VOID SetIconPosition(ICONPOSITION eIconPosition){m_eIconPosition = eIconPosition;}
};
