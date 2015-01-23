#pragma once

#include "resource.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

class CDlgFlag;
class CDlgMain;
class CDlgSettings;

//////////////////////////////////////////////////////////////////////////
// ����� ��� ������������ �������� �������
//
class CExistCaret:
	public CThreadImpl<CExistCaret>
{
	private:
		CDlgMain* m_pMainDlg;    // ������ �� ������� ����� ���������
		PHANDLE   m_phEventExit; // ������� ���������� ���������
		PHANDLE   m_phEventHide; // ������ ��������������� � �������� �������

	public:

		// ����������� � ��������� ��������� �� ������� ������
		CExistCaret(CDlgMain* pMainDlg, PHANDLE phEventExit, PHANDLE phEventHide);

		// ����������
		~CExistCaret(){}

		// ���� �������� �� ��������
		DWORD Run();
};


//////////////////////////////////////////////////////////////////////////
// ����� �������� ����
//
class CDlgMain : 
	public CIndirectDialogImpl<CDlgMain, CResDlgTemplate>,
	public CMessageFilter,
	public CIdleHandler, 
	public CTrayIconImpl<CDlgMain>,
	public CSettingsImpl
{
	public:

		// ����� ���������
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

		// ID �������
		enum { IDD = IDD_MAINDLG };

	protected:

		HMODULE             m_hKernelDll;           //
		LPATTACHCONSOLE     m_lpAttachConsole;      //
		LPFREECONSOLE       m_lpFreeConsole;        //
		LPGETCONKEYBLAYNAME m_lpGetConKeybLayName;  //
		HWINEVENTHOOK       m_hEventHook;           //

		CMapWordStr         m_mapLanguages;         // ������ ������ ���������(ID, ����)
		CMapWordStr         m_mapLangSystem;        // ������ ������ ������������� � �������  
		CMapWordStr         m_mapLangAcronym;       // ������ ����������� ������������� � ������� ������
		CMapWordPHIcon      m_mapIconsLarge;        // ������ ������ 24x24
		CMapWordPHIcon      m_mapIconsSmall;        // ������ ������ 16x16
		CMapWordPHIcon      m_mapIconsAcronymSmall; // ������ ������ � �������������� ������������� ������ 16x16
		CMapWordPHIcon      m_mapIconsAcronymLarge; // ������ ������ � �������������� ������������� ������ 24x24
		
		CDlgFlag*           m_pCaretFlag;           // ��������� �� ����-��������� ��� ��������� �������
		CDlgSettings*       m_pSettingsDlg;         // ��������� �� ������ ��������

		HANDLE              m_hEventExit;           // ������� ���������� 
		CExistCaret         m_thCaret;              // ���� �������� �������

		WORD                m_wLanguageID;          // �� ���������� ����� ���������
		BOOL                m_bViewTrayLang;        // ���������� ������ ��������� � ����
		BOOL                m_bViewFlag;            // ���������� ���� �������� ����� � ����
		BOOL                m_bViewLarge;           // ���������� ������� ���� (24x24) � �������
		DWORD               m_crIconAcronym;        // ���� ������ ������ ��������
		DWORD               m_crBGIconAcronym;      // ���� ���� ������ ��������
		BYTE                m_bTransparent;         // ������������ (0 - ������ ������������)

		DWORD               m_dwCurrentThread;      // �� ������������ ������
		HWND                m_hWndActive;           // ����� �������� ��������� ����
		HWND                m_hWndActiveLast;       // ����� ���������� ��������� ����
		HWND                m_hWndFocus;            // ����� ���� �� ������� ��������� �����
		HWND                m_hWndFocusLast;        // ����� ���� �� ������� ��������� ����� ��������� ���
		BOOL                m_bConsole;             // TRUE, ���� ���� ����������
		HKL                 m_hKLCurrent;           // �������� ���������
		HKL                 m_hKLLast;              // ��������� �������� ���������
		DWORD               m_dwActProcessId;       // �� ��������� ��������
		DWORD               m_dwActThreadId;        // �� ��������� �����
		EDITUITYPE          m_eEditUIType;          // ��� ���� ����� ������
		ICONPOSITION        m_eIconPosition;        // ������� ���� ����������-������

		HANDLE              m_hEventCaretHide;      // ������� �������� �������
		CRITICAL_SECTION    m_csCaretActivity;      // ����������� ������ ��� ��������� �������� ��������� ���������� �������
		BOOL                m_bCaretActivity;       // TRUE, ���� ������� �������
		HWND                m_hWndCaretActivity;    // ����� ���� ��������� ���������� �������
		
		CONSOLE_SCREEN_BUFFER_INFO m_csbiInfo;      // 
		CONSOLE_FONT_INFO   m_cciFont;              // 
		INT                 m_iConScrollPosH;       //
		INT                 m_iConScrollPosV;       //
		COORD               m_sizeConFont;          //
		CRect               m_rectConClient;        //
		
		CRect               m_rectTmpCaret;         // ��������� ��� ���������� �������������

	public:

		// ����������� ��-���������
		CDlgMain();

		// ���������� ��-���������
		~CDlgMain();

		// ������������� ������� �������
		VOID DoInitTemplate();

		// ������������� ��������� �������
		VOID DoInitControls();

	protected:

		// ��������� ���������
		virtual BOOL PreTranslateMessage(MSG* pMsg);

		// ������� ��������� ���������
		virtual BOOL OnIdle();

		// ������������� �������
		LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		// ��������� WM_DESTROY
		LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		// ��������� IDOK
		LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// ��������� IDCANCEL
		LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// ��������� ID_APP_EXIT
		LRESULT OnExit(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

		// �������� �������
		VOID CloseDialog(INT iVal);

		// ����� ���� ��������� ������ ���������
		VOID SearchLanguage();

		static BOOL CALLBACK SearchLanguageCallback(HMODULE hModule, LPCTSTR lpszType, 
			LPTSTR lpszName, WORD wIDLanguage, LONG_PTR lParam);

		// ����� ������������� � ������� ������
		VOID SearchInstallLanguage();

		// ����� �����
		LRESULT OnChangeLanguage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

		// �������� ������ ������
		VOID LoadFlagIcons();

		// ������� ���������� ����� ������ �������������� �� ������
		HICON GetIconFromStr(wstring wsText, BOOL bLarge, COLORREF crText, COLORREF crBackground);

		// �������� �������� ��������� �� �������
		LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		// ���������� �������� ���������
		HKL GetActiveLayout();

		// ���������� ���������� ���������� �������
		BOOL CDlgMain::GetCursorRect(PRECT pRect);

		// ����� ������� ��������
		LRESULT OnSettings(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

		// ������������� ���� ������� ���������
		VOID SetFlagLanguage(HKL hklCurrent, CDlgFlag* pFlagDlg, BOOL bViewFlag, BOOL bViewLarge, BYTE bTrasparency);

		// ��������� ������� ������� � GUI
		LRESULT OnCaretGUIRepos(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

		// ��������� ������� ������� � �������
		LRESULT OnCaretConsoleRepos(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

		// �������� �������
		LRESULT OnCaretHide(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

		// ��������� WM_SETTINGCHANGE
		LRESULT OnSettingsChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	public:

		// ��������� ��������� ������
		HGLOBAL GetResourceLang(LPWSTR lpResType, DWORD dwResID);

		// ��������� ������ �� ���������� ��������������
		wstring GetStringLang(DWORD dwResID);

		// ����� ���� �� ������� ����� ������ ���� �� ������ � ����
		VOID OnNotifyIconLButtonUp(const POINT& rpt);

		// ����� ���� � ���� �� ������� ������ ������ ����
		VOID OnNotifyIconRButtonUp(const POINT& rpt);

		// ���������� ������ ��������� ������
		inline CMapWordStr* GetMapLanguages(){return &m_mapLanguages;}

		// ���������� �� ���������� ����� ���������
		inline WORD GetLanguageID() {return m_wLanguageID;}

		// ���������� true, ���� �������� ���� ����������
		inline BOOL ActiveWndIsConsole() {return m_bConsole;}

		// ���������� ����� ��������� ����
		inline HWND GetActiveWnd() {return m_hWndActive;}

		// ��������� ������� ���� ������� ���������
		BOOL UpdateCaretFlagPos(CRect rectNew);

		// ���������� ���������� ������� ����������� ����
		BOOL GetConsoleCaretPos(SHORT sX, SHORT sY, RECT& rcCaret);

		// ������������� ����� ��������� ���������� �������
		inline VOID SetCaretActivity(BOOL bActivity, HWND hWnd);

		// ���������� ����� ��������� ���������� �������
		inline BOOL GetCaretActivity();

		// ������������� ������� �������� ����-����������
		inline BOOL SetHideEvent(){return SetEvent(m_hEventCaretHide);}

		// ���������� TRUE, ���� ����-��������� ���������� � ���� �����
		inline BOOL GetViewFlag(){return m_bViewFlag;}
		inline VOID SetViewFlag(BOOL bViewFlag){m_bViewFlag = bViewFlag;}

		// ���������� TRUE, ���� ����-��������� �������� �������
		inline BOOL GetViewLarge(){return m_bViewLarge;}
		inline VOID SetViewLarge(BOOL bViewLarge){m_bViewLarge = bViewLarge;}

		// ���������� TRUE, ���� ��� ����-���������� ����������
		inline BOOL BGIsTransparent(){return (m_crBGIconAcronym & 0xFF000000) ? FALSE : TRUE;}

		// ���������� ������ ������������
		inline BYTE GetTransparency(){return m_bTransparent;}
		inline VOID SetTransparency(BYTE bTransparent){m_bTransparent = bTransparent;}

		// ������������� ������ �� ��������� ����-����������
		inline VOID SetFlagLanguage(CDlgFlag* pDlgFlag, BOOL bViewFlag, BOOL bViewLarge, BYTE bTrasparency)
			{return SetFlagLanguage(m_hKLCurrent, pDlgFlag, bViewFlag, bViewLarge, bTrasparency);}

		// ��������� ������ � �������������� ������������� ������
		BOOL GenerateAcronymIcons(COLORREF crText, COLORREF crBackground);

		// ���������� ���� ������ ������ ���������
		inline COLORREF GetColorIconActronym() {return m_crIconAcronym;}
		inline VOID SetColorIconActronym(COLORREF crIconAcronym){m_crIconAcronym = crIconAcronym;}

		// ���������� ���� ����
		inline COLORREF GetBGColorIconActronym() {return m_crBGIconAcronym;}
		inline VOID SetBGColorIconActronym(COLORREF crBGIconAcronym){m_crBGIconAcronym = crBGIconAcronym;}

		// ���������� TRUE, ���� ���������� ���������
		BOOL GetAutostart();

		// ������������� ���������
		VOID SetAutostart(BOOL bAutostart);

		// ���������� ������� ����-���������� ������������ �������
		inline ICONPOSITION GetIconPosition(){return m_eIconPosition;}

		// ������������� ������� ����-���������� ������������ �������
		VOID SetIconPosition(ICONPOSITION eIconPosition){m_eIconPosition = eIconPosition;}
};
