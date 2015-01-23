#pragma once 

#include "resource.h"

//////////////////////////////////////////////////////////////////////////
// Класс диалога параметров программы
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

		// Конструктор по-умолчанию
		CDlgSettings(CDlgMain *pMainDlg);

		// Инициализация шаблона диалога
		VOID DoInitTemplate();

		// Инициализация контролов диалога
		VOID DoInitControls();

	private:

		CDlgMain*     m_pMainDlg;       // указатель на главный диалог
		CDlgFlag*     m_pCaretFlag;     // указатель на окно-индикатор раскладки
		CMapWordStr*  m_pMapLanguages;  // указатель на массив доступных языков
		WORD          m_wLanguageID;    // ИД текущего языка программы
		CComboBox     m_cbLanguages;    // выпадающий список языков
		CHyperLink    m_hlEmail;        // гиперссылка на e-mail
		CHyperLink    m_hlWWW;          // гиперссылка на WWW
		COLORREF      m_crText;         // цвет текста
		COLORREF      m_crBackground;   // цвет фона
		CButton       m_btnViewText;    // отображать на окне-индикаторе текст
		CButton       m_btnViewFlag;    // отображать на окне-индикаторе флаг
		CButton       m_btnViewSmall;   // отображать маленькое окно-индикатор
		CButton       m_btnViewLarge;   // отображать увеличенное окно-индикатор
		CButton       m_btnTransparent; // сделать фон прозрачным
		CTrackBarCtrl m_tbTransparency; // ползунок изменения индекса прозрачности
		CStatic       m_sTransparency;  // текст для вывода процента прозрачности
		CButton       m_btnColor;       // цвет текста 
		CButton       m_btnBGColor;     // цвет фона
		CButton       m_btnAutostart;   // автостарт
		CButton       m_btnLeftTop;     // позиции окна-индикатора
		CButton       m_btnLeftCenter;  //
		CButton       m_btnLeftBottom;  //
		CButton       m_btnRightTop;    //
		CButton       m_btnRightCenter; // 
		CButton       m_btnRightBottom; //
		CStatic       m_sTextCaret1;    // изображение текстового курсора #1 для отображение позиции
		CStatic       m_sTextCaret2;    // изображение текстового курсора #1 для выбора позиции

	protected:

		// Инициализация диалога
		LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		// Обработка WM_DESTROY
		LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		// Обработка WM_TIMER - создание мигающего курсора
		LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		// Обработка IDOK - сохранение настроек
		LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// Обработка IDCANCEL
		LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// Закрытие диалога
		VOID CloseDialog(int nVal);

		// Обработка смены языка
		LRESULT OnChangeLanguage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// Смена цвета текста
		LRESULT OnChangeColor(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// Смена цвета фона
		LRESULT OnChangeBackground(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// Смена режима отображения
		LRESULT OnChangeView(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// Изменение прозрачности
		LRESULT OnChangeTransparent(INT wID, LPNMHDR pnmh, BOOL& bHandled);

		// Изменение позиции окна-индикатора
		LRESULT OnChangePos(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

		// Обновляет изображение окна-индикатора раскладки
		VOID UpdateTestCaretFlag();
};



