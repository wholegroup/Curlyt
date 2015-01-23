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
#include "resource.h"
#include "DlgMain.h"

// �������� ������
CAppModule g_Module;
CDlgMain*  g_pDlgMain;

//////////////////////////////////////////////////////////////////////////
// ������ ���������
//
INT Run(LPTSTR /*lpstrCmdLine*/ = NULL, BOOL bSettings = FALSE)
{
	// ������ ����� ���������
	CMessageLoop theLoop;

	g_Module.AddMessageLoop(&theLoop);

	// �������� �������� �������
	if(NULL == g_pDlgMain->Create(NULL))
	{
		ATLTRACE(_T("Main dialog creation failed!\n"));
		
		return 0;
	}

	// ����� ������� �������� (� ������ ���� ���� ����� ����� ���������)
	if (bSettings)
	{
		g_pDlgMain->PostMessage(WM_COMMAND, IDB_SETTINGS);
	}

	// ��� ��������
	INT iRet = theLoop.Run();

	// �������� ������� ���������
	g_Module.RemoveMessageLoop();

	return iRet;
}


//////////////////////////////////////////////////////////////////////////
// ������� ����� � ���������
//
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
	// ��������� ������ � ������� ������
	#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif

	// �������� ���������� ������� �������������� ������ ��������� ����� � �������� ������������ ������
	HANDLE  hMutexRun   = NULL;
	wstring wsMutexName = L"Local\\{9FAF9D80-453E-4b98-A67B-1FD6776755EE}";

	// ���� ������ ����, ������ ����� ��������� ��� ��������
	hMutexRun = OpenMutex(0, FALSE, wsMutexName.c_str());
	if (NULL != hMutexRun)
	{
		ATLVERIFY(CloseHandle(hMutexRun));
		_RPT_API_ERROR(OpenMutex);
		return 0;
	}

	if (ERROR_ACCESS_DENIED == GetLastError())
	{
		return 0;
	}

	// ������� ����� ������
	hMutexRun = CreateMutex(NULL, TRUE, wsMutexName.c_str());
	if (NULL == hMutexRun)
	{
		_RPT_API_ERROR(CreateMutex);
		return 0;
	}

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	// ���������� ������ ��� ��������� ���������
	AtlInitCommonControls(ICC_BAR_CLASSES);

	// ������������� COM
	HRESULT hRes = ::CoInitialize(NULL);
	ATLASSERT(SUCCEEDED(hRes));

	// ������������� �������
	hRes = g_Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	// ������
	INT iRet = 0;
	
	do 
	{
		g_pDlgMain = new CDlgMain();
		
		iRet = Run(lpstrCmdLine, (CHANGE_LANGUAGE == iRet) ? TRUE : FALSE);

		if (g_pDlgMain)
		{
			if (g_pDlgMain->IsWindow())
			{
				g_pDlgMain->DestroyWindow();
			}

			delete g_pDlgMain;
			g_pDlgMain = NULL;
		}
	} while (CHANGE_LANGUAGE == iRet);

	// ��������� ���������
	g_Module.Term();

	// ������� �������� COM
	::CoUninitialize();

	return iRet;
}
