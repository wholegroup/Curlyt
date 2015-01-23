#pragma once 

#include "resource.h"

//////////////////////////////////////////////////////////////////////////
// ����� ������� ���������� ���������
//
class CDlgSettings :
	public CIndirectDialogImpl<CDlgSettings, CResDlgTemplate>
{
	public:

		enum {IDD = IDD_SETTINGS};

		BEGIN_MSG_MAP(CDlgSettings)
			MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
			MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
			MESSAGE_HANDLER(WM_TIMER, OnTimer)
			COMMAND_ID_HANDLER(IDOK, OnOK)
			COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
			COMMAND_ID_HANDLER(IDC_BUTTON_COLOR_TEXT, OnChangeColor)
			COMMAND_ID_HANDLER(IDC_BUTTON_COLOR_BACKGROUND, OnChangeBackground)
			COMMAND_ID_HANDLER(IDC_RADIO_VIEW_TEXT, OnChangeView)
			COMMAND_ID_HANDLER(IDC_RADIO_VIEW_FLAG, OnChangeView)
			COMMAND_ID_HANDLER(IDC_RADIO_VIEW_SMALL, OnChangeView)
			COMMAND_ID_HANDLER(IDC_RADIO_VIEW_LARGE, OnChangeView)
			COMMAND_ID_HANDLER(IDC_CHECK_TRANSPARENT, OnChangeView)
			COMMAND_ID_HANDLER(IDC_RADIO_LEFT_TOP, OnChangePos)
			COMMAND_ID_HANDLER(IDC_RADIO_LEFT_CENTER, OnChangePos)
			COMMAND_ID_HANDLER(IDC_RADIO_LEFT_BOTTOM, OnChangePos)
			COMMAND_ID_HANDLER(IDC_RADIO_RIGHT_TOP, OnChangePos)
			COMMAND_ID_HANDLER(IDC_RADIO_RIGHT_CENTER, OnChangePos)
			COMMAND_ID_HANDLER(IDC_RADIO_RIGHT_BOTTOM, OnChangePos)
			COMMAND_HANDLER(IDC_COMBO_LANGUAGES, CBN_SELCHANGE, OnChangeLanguage)
			NOTIFY_ID_HANDLER(IDC_SLIDER_TRANSPARENCY, OnChangeTransparent)
		END_MSG_MAP()

		// ����������� ��-���������
		CDlgSettings(CDlgMain *pMainDlg);

		// ������������� ������� �������
		VOID DoInitTemplate();

		// ������������� ��������� �������
		VOID DoInitControls();

	private:

		CDlgMain*     m_pMainDlg;       // ��������� �� ������� ������
		CDlgFlag*     m_pCaretFlag;     // ��������� �� ����-��������� ���������
		CMapWordStr*  m_pMapLanguages;  // ��������� �� ������ ��������� ������
		WORD          m_wLanguageID;    // �� �������� ����� ���������
		CComboBox     m_cbLanguages;    // ���������� ������ ������
		CHyperLink    m_hlEmail;        // ����������� �� e-mail
		CHyperLink    m_hlWWW;          // ����������� �� WWW
		COLORREF      m_crText;         // ���� ������
		COLORREF      m_crBackground;   // ���� ����
		CButton       m_btnViewText;    // ���������� �� ����-���������� �����
		CButton       m_btnViewFlag;    // ���������� �� ����-���������� ����
		CButton       m_btnViewSmall;   // ���������� ��������� ����-���������
		CButton       m_btnViewLarge;   // ���������� ����������� ����-���������
		CButton       m_btnTransparent; // ������� ��� ����������
		CTrackBarCtrl m_tbTransparency; // �������� ��������� ������� ������������
		CStatic       m_sTransparency;  // ����� ��� ������ �������� ������������
		CButton       m_btnColor;       // ���� ������ 
		CButton       m_btnBGColor;     // ���� ����
		CButton       m_btnAutostart;   // ���������
		CButton       m_btnLeftTop;     // ������� ����-����������
		CButton       m_btnLeftCenter;  //
		CButton       m_btnLeftBottom;  //
		CButton       m_btnRightTop;    //
		CButton       m_btnRightCenter; // 
		CButton       m_btnRightBottom; //
		CStatic       m_sTextCaret1;    // ����������� ���������� ������� #1 ��� ����������� �������
		CStatic       m_sTextCaret2;    // ����������� ���������� ������� #1 ��� ������ �������

	protected:

		// ������������� �������
		LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		// ��������� WM_DESTROY
		LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		// ��������� WM_TIMER - �������� ��������� �������
		LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		// ��������� IDOK - ���������� ��������
		LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// ��������� IDCANCEL
		LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// �������� �������
		VOID CloseDialog(int nVal);

		// ��������� ����� �����
		LRESULT OnChangeLanguage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// ����� ����� ������
		LRESULT OnChangeColor(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// ����� ����� ����
		LRESULT OnChangeBackground(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// ����� ������ �����������
		LRESULT OnChangeView(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// ��������� ������������
		LRESULT OnChangeTransparent(INT wID, LPNMHDR pnmh, BOOL& bHandled);

		// ��������� ������� ����-����������
		LRESULT OnChangePos(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// ��������� ����������� ����-���������� ���������
		VOID UpdateTestCaretFlag();
};



