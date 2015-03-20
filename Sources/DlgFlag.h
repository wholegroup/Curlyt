#pragma once 

#include "resource.h"

class CDlgFlag:
	public CWindowImpl<CDlgFlag>
{
	public:

		DECLARE_WND_CLASS(NULL)

		// Конструктор по-умолчанию
		CDlgFlag();

		// Деструктор по-умолчанию
		~CDlgFlag();

		BEGIN_MSG_MAP(CDlgFlag)
		END_MSG_MAP()

	protected:

		CRect         m_rectClient;
		CRect         m_rectWindow;
		BITMAPINFO    m_bmpInfo;
		HDC           m_dcMem;
		PVOID         pvBuf;
		BLENDFUNCTION m_blend;

	public:

		// смена иконки
		BOOL ChangeIcon(HICON hNewIcon, BYTE bTransparent);
};
