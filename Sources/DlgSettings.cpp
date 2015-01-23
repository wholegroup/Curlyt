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

#include "stdafx.h"
#include "DlgMain.h"
#include "DlgFlag.h"
#include "DlgSettings.h"

//////////////////////////////////////////////////////////////////////////
//  онструктор по-умолчанию
//
CDlgSettings::CDlgSettings(CDlgMain *pMainDlg)
{
	ATLASSERT(NULL != pMainDlg);
	m_pMainDlg = pMainDlg;
}


//////////////////////////////////////////////////////////////////////////
// »нициализаци€ диалога
//
LRESULT CDlgSettings::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// выравниваем диалог по центру
	CenterWindow();

	// вставка в заголовок диалога версии программы
	std::vector<WCHAR> wcTitle(GetWindowTextLength() + 1);
	GetWindowText(&wcTitle.front(), static_cast<int>(wcTitle.size()));

	std::wstring wsTitle = &wcTitle.front();
	wsTitle += L" v";
	wsTitle += PROGRAM_VERSION;

	SetWindowText(wsTitle.c_str());

	// заполнение списка €зыков
	m_cbLanguages   = static_cast<CComboBox>(GetDlgItem(IDC_COMBO_LANGUAGES));
	m_wLanguageID   = m_pMainDlg->GetLanguageID();
	m_pMapLanguages = m_pMainDlg->GetMapLanguages();

	for (CMapWordStr::iterator lang = m_pMapLanguages->begin(); lang != m_pMapLanguages->end(); lang++)
	{
		m_cbLanguages.AddString(lang->second.c_str());

		if (lang->first == m_wLanguageID)
		{
			m_cbLanguages.SetCurSel(static_cast<int>(std::distance(m_pMapLanguages->begin(), lang)));
		}
	}

	// текст в виде каретки
	m_sTextCaret1 = GetDlgItem(IDC_STATIC_VIEW_ICON);
	m_sTextCaret2 = GetDlgItem(IDC_STATIC_CARET_POS);

	//	установка режима отображени€
	m_btnViewText  = GetDlgItem(IDC_RADIO_VIEW_TEXT);
	m_btnViewText.SetCheck(!m_pMainDlg->GetViewFlag());
	
	m_btnViewFlag  = GetDlgItem(IDC_RADIO_VIEW_FLAG);
	m_btnViewFlag.SetCheck(m_pMainDlg->GetViewFlag());

	m_btnViewSmall = GetDlgItem(IDC_RADIO_VIEW_SMALL);
	m_btnViewSmall.SetCheck(!m_pMainDlg->GetViewLarge());
	
	m_btnViewLarge = GetDlgItem(IDC_RADIO_VIEW_LARGE);
	m_btnViewLarge.SetCheck(m_pMainDlg->GetViewLarge());

	m_btnTransparent = GetDlgItem(IDC_CHECK_TRANSPARENT);
	m_btnTransparent.SetCheck(m_pMainDlg->BGIsTransparent());

	m_btnColor = GetDlgItem(IDC_BUTTON_COLOR_TEXT);
	m_crText   = m_pMainDlg->GetColorIconActronym();
	
	m_btnBGColor   = GetDlgItem(IDC_BUTTON_COLOR_BACKGROUND);
	m_crBackground = m_pMainDlg->GetBGColorIconActronym();

	m_tbTransparency = GetDlgItem(IDC_SLIDER_TRANSPARENCY);
	m_tbTransparency.SetRange(DEFAULT_TRANSPARENCY_MIN, 255);
	m_tbTransparency.SetPos(m_pMainDlg->GetTransparency());

	std::wostringstream streamTransparency;
	streamTransparency << static_cast<INT>((100. - m_tbTransparency.GetPos() * 100. / m_tbTransparency.GetRangeMax())) << "%";
	m_sTransparency = static_cast<CStatic>(GetDlgItem(IDC_STATIC_PERCENT_TRANSPARENCY));
	m_sTransparency.SetWindowText(streamTransparency.str().c_str());

	m_btnAutostart = GetDlgItem(IDC_CHECK_AUTOSTART);
	m_btnAutostart.SetCheck(m_pMainDlg->GetAutostart());

	PostMessage(WM_COMMAND, IDC_RADIO_VIEW_TEXT);

	// установка позиции окна-индикатора
	m_btnLeftTop = GetDlgItem(IDC_RADIO_LEFT_TOP);
	m_btnLeftTop.SetCheck(ICONPOSITION_LEFT_TOP == m_pMainDlg->GetIconPosition() ? TRUE : FALSE);

	m_btnLeftCenter = GetDlgItem(IDC_RADIO_LEFT_CENTER);
	m_btnLeftCenter.SetCheck(ICONPOSITION_LEFT_CENTER == m_pMainDlg->GetIconPosition() ? TRUE : FALSE);

	m_btnLeftBottom = GetDlgItem(IDC_RADIO_LEFT_BOTTOM);
	m_btnLeftBottom.SetCheck(ICONPOSITION_LEFT_BOTTOM == m_pMainDlg->GetIconPosition() ? TRUE : FALSE);

	m_btnRightTop = GetDlgItem(IDC_RADIO_RIGHT_TOP);
	m_btnRightTop.SetCheck(ICONPOSITION_RIGHT_TOP == m_pMainDlg->GetIconPosition() ? TRUE : FALSE);

	m_btnRightCenter = GetDlgItem(IDC_RADIO_RIGHT_CENTER);
	m_btnRightCenter.SetCheck(ICONPOSITION_RIGHT_CENTER == m_pMainDlg->GetIconPosition() ? TRUE : FALSE);

	m_btnRightBottom = GetDlgItem(IDC_RADIO_RIGHT_BOTTOM);
	m_btnRightBottom.SetCheck(ICONPOSITION_RIGHT_BOTTOM == m_pMainDlg->GetIconPosition() ? TRUE : FALSE);

	m_hlEmail.SetHyperLink(m_pMainDlg->GetStringLang(IDS_EMAIL).c_str());
	m_hlEmail.SubclassWindow(GetDlgItem(IDC_HYPERLINK_EMAIL));

	m_hlWWW.SetHyperLink(m_pMainDlg->GetStringLang(IDS_WWW).c_str());
	m_hlWWW.SubclassWindow(GetDlgItem(IDC_HYPERLINK_WWW));

	// изменение шрифтов
	CLogFont lfBold(static_cast<CStatic>(GetDlgItem(IDC_STATIC_WWW)).GetFont());
	lfBold.SetBold();

	static_cast<CStatic>(GetDlgItem(IDC_STATIC_WWW)).SetFont(lfBold.CreateFontIndirect());
	static_cast<CStatic>(GetDlgItem(IDC_STATIC_EMAIL)).SetFont(lfBold.CreateFontIndirect());
	static_cast<CStatic>(GetDlgItem(IDC_STATIC_LANG)).SetFont(lfBold.CreateFontIndirect());
	static_cast<CStatic>(GetDlgItem(IDC_STATIC_TRANSPARENCY)).SetFont(lfBold.CreateFontIndirect());
	static_cast<CStatic>(GetDlgItem(IDC_STATIC_GROUP_INDICATOR)).SetFont(lfBold.CreateFontIndirect());
	static_cast<CStatic>(GetDlgItem(IDC_STATIC_GROUP_APPERANCE)).SetFont(lfBold.CreateFontIndirect());
	static_cast<CStatic>(GetDlgItem(IDC_STATIC_GROUP_SIZE)).SetFont(lfBold.CreateFontIndirect());
	static_cast<CStatic>(GetDlgItem(IDC_STATIC_GROUP_LOCATION)).SetFont(lfBold.CreateFontIndirect());

	lfBold.MakeBolder(-1);
	lfBold.MakeLarger(6);
	static_cast<CStatic>(m_sTextCaret2).SetFont(lfBold.CreateFontIndirect());

	// создание окна-индикатора раскладки
	CRect rectCaretFlag(0, 0, 23, 23);

	m_pCaretFlag = new CDlgFlag();

	m_pCaretFlag->Create(*this, rectCaretFlag, L"", WS_POPUP | WS_VISIBLE, 
		WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
	m_pCaretFlag->ShowWindow(SW_SHOW);

	// обновление окна-индикатора раскладки
	UpdateTestCaretFlag();

	// установка таймера, дл€ создани€ мигающего курсора
	SetTimer(0, GetCaretBlinkTime());

	return static_cast<LRESULT>(0);
}


//////////////////////////////////////////////////////////////////////////
// ќбработка WM_DESTROY
//
LRESULT CDlgSettings::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// удаление таймера
	KillTimer(0);

	// удаление окна индикатора дл€ каретки
	if (NULL != m_pCaretFlag)
	{
		if (m_pCaretFlag->IsWindow())
		{
			m_pCaretFlag->DestroyWindow();
		}
		delete m_pCaretFlag;
		m_pCaretFlag = NULL;
	}

	return static_cast<LRESULT>(0);
}


//////////////////////////////////////////////////////////////////////////
// ќбработка WM_TIMER - создание мигающего курсора
//
LRESULT CDlgSettings::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_sTextCaret1.ShowWindow(!m_sTextCaret1.IsWindowVisible());
	m_sTextCaret2.ShowWindow(!m_sTextCaret2.IsWindowVisible());

	return static_cast<LRESULT>(0);
}


//////////////////////////////////////////////////////////////////////////
// ќбработка IDOK - сохранение настроек
//
LRESULT CDlgSettings::OnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// сохранение типа окна-индикатора (текст/флаг)
	m_pMainDlg->SetViewFlag(m_btnViewFlag.GetCheck());
	
	// сохранение размера окна-индикатора (маленькое/большое)
	m_pMainDlg->SetViewLarge(m_btnViewLarge.GetCheck());
	
	// сохранение цвета текста окна-индикатора
	m_pMainDlg->SetColorIconActronym(m_crText);
	
	// сохранение цвета фона окна-индикатора
	if (m_btnTransparent.GetCheck())
	{
		m_pMainDlg->SetBGColorIconActronym(0xFFFFFF & m_crBackground);
	}
	else
	{
		m_pMainDlg->SetBGColorIconActronym(0xFF000000 | m_crBackground);
	}

	// сохранение индекса прозрачности окна-индикатора
	m_pMainDlg->SetTransparency(static_cast<BYTE>(m_tbTransparency.GetPos()));

	// сохранение значени€ автостарта программы
	m_pMainDlg->SetAutostart(m_btnAutostart.GetCheck());

	// сохранение позиции окна-индикатора относительно каретки
	ICONPOSITION ipApply = DEFAULT_ICONPOSITION;
	
	ipApply = m_btnLeftTop.GetCheck()     ? ICONPOSITION_LEFT_TOP : ipApply;
	ipApply = m_btnLeftCenter.GetCheck()  ? ICONPOSITION_LEFT_CENTER : ipApply;
	ipApply = m_btnLeftBottom.GetCheck()  ? ICONPOSITION_LEFT_BOTTOM : ipApply;
	ipApply = m_btnRightTop.GetCheck()    ? ICONPOSITION_RIGHT_TOP : ipApply;
	ipApply = m_btnRightCenter.GetCheck() ? ICONPOSITION_RIGHT_CENTER : ipApply;
	ipApply = m_btnRightBottom.GetCheck() ? ICONPOSITION_RIGHT_BOTTOM : ipApply;
	
	m_pMainDlg->SetIconPosition(ipApply);

	// закрытие диалога
	CloseDialog(IDOK);

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// ќбработка IDCANCEL
//
LRESULT CDlgSettings::OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(IDCANCEL);

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// »нициализаци€ шаблона диалога
//
VOID CDlgSettings::DoInitTemplate() 
{
	m_Template.Create((LPDLGTEMPLATE)m_pMainDlg->GetResourceLang(RT_DIALOG, this->IDD));
}


//////////////////////////////////////////////////////////////////////////
// »нициализаци€ контролов диалога
//
VOID CDlgSettings::DoInitControls() 
{
	ATLASSERT(TRUE);
}


//////////////////////////////////////////////////////////////////////////
// «акрытие диалога
//
VOID CDlgSettings::CloseDialog(int nVal)
{
	EndDialog(nVal);
}


//////////////////////////////////////////////////////////////////////////
// —мена €зыка программы
//
LRESULT CDlgSettings::OnChangeLanguage(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	UNREFERENCED_PARAMETER(wNotifyCode);
	ATLASSERT(CBN_SELCHANGE == wNotifyCode);
	ATLASSERT(m_cbLanguages.GetCurSel() < static_cast<INT>(m_pMapLanguages->size()));

	CMapWordStr::const_iterator lang = m_pMapLanguages->begin();
	std::advance(lang, m_cbLanguages.GetCurSel());

	m_pMainDlg->PostMessage(WM_USER_CHANGE_LANGUAGE, static_cast<WPARAM>(lang->first));

	CloseDialog(CHANGE_LANGUAGE);

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// —мена цвета текста
//
LRESULT CDlgSettings::OnChangeColor(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CColorDialog dlgColor(m_crText, CC_FULLOPEN | CC_RGBINIT);

	if (IDOK == dlgColor.DoModal())
	{
		m_crText = dlgColor.GetColor();
		UpdateTestCaretFlag();
	}

	return static_cast<LRESULT>(0);
}


//////////////////////////////////////////////////////////////////////////
// —мена цвета фона
//
LRESULT CDlgSettings::OnChangeBackground(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CColorDialog dlgColor(m_crBackground, CC_FULLOPEN | CC_RGBINIT);

	if (IDOK == dlgColor.DoModal())
	{
		m_crBackground = dlgColor.GetColor();
		UpdateTestCaretFlag();
	}

	return static_cast<LRESULT>(0);
}


//////////////////////////////////////////////////////////////////////////
// —мена режима отображени€
//
LRESULT CDlgSettings::OnChangeView(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (m_btnViewFlag.GetCheck())
	{
		m_btnTransparent.EnableWindow(FALSE);
		m_btnColor.EnableWindow(FALSE);
		m_btnBGColor.EnableWindow(FALSE);
	}
	else
	{
		m_btnTransparent.EnableWindow(TRUE);
		m_btnColor.EnableWindow(TRUE);

		if (m_btnTransparent.GetCheck())
		{
			m_btnBGColor.EnableWindow(FALSE);
		}
		else
		{
			m_btnBGColor.EnableWindow(TRUE);
		}
	}

	UpdateTestCaretFlag();

	return static_cast<LRESULT>(0);
}


//////////////////////////////////////////////////////////////////////////
// »зменение прозрачности
//
LRESULT CDlgSettings::OnChangeTransparent(INT wID, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
{
	UNREFERENCED_PARAMETER(wID);
	ATLASSERT(IDC_SLIDER_TRANSPARENCY == wID);

	std::wostringstream streamTransparency;
	streamTransparency << static_cast<INT>((100. - m_tbTransparency.GetPos() * 100. / m_tbTransparency.GetRangeMax())) << L"%";
	m_sTransparency.SetWindowText(streamTransparency.str().c_str());

	UpdateTestCaretFlag();

	return static_cast<LRESULT>(0);
}


//////////////////////////////////////////////////////////////////////////
// ќбновл€ет изображение окна-индикатора
//
VOID CDlgSettings::UpdateTestCaretFlag()
{
	// если окно-индикатора в виде текста, генерируем иконки
	if (FALSE == m_btnViewFlag.GetCheck())
	{
		if (m_btnTransparent.GetCheck())
		{
			m_pMainDlg->GenerateAcronymIcons(m_crText, 0xFFFFFF & m_crBackground);
		}
		else
		{
			m_pMainDlg->GenerateAcronymIcons(m_crText, 0xFF000000 | m_crBackground);
		}
	}
	
	// установка иконки в окно-индикатор
	m_pMainDlg->SetFlagLanguage(m_pCaretFlag, m_btnViewFlag.GetCheck(), 
		m_btnViewLarge.GetCheck(), static_cast<BYTE>(m_tbTransparency.GetPos()));

	// обновление позиции окна-индикатора
	PostMessage(WM_COMMAND, IDC_RADIO_LEFT_TOP);
}


//////////////////////////////////////////////////////////////////////////
// »зменение позиции окна-индикатора
//
LRESULT CDlgSettings::OnChangePos(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// выход, если окно-индикатор не существует
	if (!m_pCaretFlag || !m_pCaretFlag->IsWindow())
	{
		return static_cast<LRESULT>(0);
	}

	// вычисление новой позиции окна-индикатора
	CRect rectNew;
	LONG  lWidth = m_btnViewLarge.GetCheck() ? 24 : 16;
	
	m_sTextCaret1.GetWindowRect(&rectNew);

	if (m_btnLeftTop.GetCheck())
	{
		rectNew.MoveToX(rectNew.left - lWidth);
		rectNew.MoveToY(rectNew.top - lWidth);
	}
	else

	if (m_btnLeftCenter.GetCheck())
	{
		rectNew.MoveToX(rectNew.left - lWidth);
		rectNew.MoveToY(rectNew.top - (lWidth - rectNew.Height()) / 2);
	}
	else

	if (m_btnLeftBottom.GetCheck())
	{
		rectNew.MoveToX(rectNew.left - lWidth);
		rectNew.MoveToY(rectNew.top + rectNew.Height());
	}
	else

	if (m_btnRightTop.GetCheck())
	{
		rectNew.MoveToX(rectNew.left + rectNew.Width());
		rectNew.MoveToY(rectNew.top - lWidth);
	}
	else

	if (m_btnRightCenter.GetCheck())
	{
		rectNew.MoveToX(rectNew.left + rectNew.Width());
		rectNew.MoveToY(rectNew.top - (lWidth - rectNew.Height()) / 2);
	}
	else

	if (m_btnRightBottom.GetCheck())
	{
		rectNew.MoveToX(rectNew.left + rectNew.Width());
		rectNew.MoveToY(rectNew.top + rectNew.Height());
	}

	// изменение позиции окна-индикатора
	m_pCaretFlag->SetWindowPos(NULL, &rectNew, SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);

	return static_cast<LRESULT>(0);
}
