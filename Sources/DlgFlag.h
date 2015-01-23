#pragma once 

#include "resource.h"

class CDlgFlag:
	public CWindowImpl<CDlgFlag>
{
	public:

		DECLARE_WND_CLASS(NULL)

		// ����������� ��-���������
		CDlgFlag();

		// ���������� ��-���������
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

		// ����� ������
		BOOL ChangeIcon(HICON hNewIcon, BYTE bTransparent);
};
