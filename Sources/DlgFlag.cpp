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

//////////////////////////////////////////////////////////////////////////
// Конструктор по-умолчанию
//
CDlgFlag::CDlgFlag()
{
	m_bmpInfo.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER); 
	m_bmpInfo.bmiHeader.biPlanes        = 1; 
	m_bmpInfo.bmiHeader.biBitCount      = 32; 
	m_bmpInfo.bmiHeader.biCompression   = BI_RGB; 
	m_bmpInfo.bmiHeader.biSizeImage     = 0; 
	m_bmpInfo.bmiHeader.biXPelsPerMeter = 0; 
	m_bmpInfo.bmiHeader.biYPelsPerMeter = 0; 
	m_bmpInfo.bmiHeader.biClrUsed       = 0; 
	m_bmpInfo.bmiHeader.biClrImportant  = 0; 

	m_dcMem = ::CreateCompatibleDC(NULL); 

	m_blend.BlendOp     = AC_SRC_OVER;
	m_blend.BlendFlags  = 0;
	m_blend.AlphaFormat = AC_SRC_ALPHA;
}


//////////////////////////////////////////////////////////////////////////
// Деструктор по-умолчанию
//
CDlgFlag::~CDlgFlag()
{
	::DeleteDC(m_dcMem);
}


//////////////////////////////////////////////////////////////////////////
// смена иконки
//
BOOL CDlgFlag::ChangeIcon(HICON hNewIcon, BYTE bTransparent)
{
	if (NULL == hNewIcon)
	{
		ATLASSERT(FALSE);
		return FALSE;
	}

	// получение оконных координат
	GetClientRect(&m_rectClient);
	GetWindowRect(&m_rectWindow);

	// создание битмапа
	m_bmpInfo.bmiHeader.biWidth  = m_rectClient.Width(); 
	m_bmpInfo.bmiHeader.biHeight = m_rectClient.Height(); 

	HBITMAP hBmp    = ::CreateDIBSection(m_dcMem, &m_bmpInfo, DIB_RGB_COLORS, &pvBuf, 0, 0);
	HBITMAP hBmpOld = static_cast<HBITMAP>(::SelectObject(m_dcMem, hBmp));

	ZeroMemory(pvBuf, m_rectClient.Width() * m_rectClient.Height() << 2);

	// отрисовка иконки
	if (FALSE == DrawIconEx(m_dcMem, 0, 0, hNewIcon, 0, 0, 0, NULL, DI_NORMAL))
	{
		_RPT_API_ERROR(DrawIcon);
	}

	// обновление окна
	m_blend.SourceConstantAlpha = bTransparent;

	POINT pDst = m_rectWindow.TopLeft();
	POINT pSrc = m_rectClient.TopLeft();
	SIZE  sWnd = m_rectClient.Size();
	
	if (FALSE == UpdateLayeredWindow(*this, NULL, &pDst, &sWnd, m_dcMem, &pSrc, NULL, &m_blend, ULW_ALPHA))
	{
		_RPT_API_ERROR(UpdateLayeredWindow);
	}

	// удаление созданных ресурсов
	::SelectObject(m_dcMem, hBmpOld);
	::DeleteObject(hBmp);

	return TRUE;
}
