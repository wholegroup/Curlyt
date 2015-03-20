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

// создание модуля
CAppModule g_Module;
CDlgMain*  g_pDlgMain;

//////////////////////////////////////////////////////////////////////////
// Запуск программы
//
INT Run(LPTSTR /*lpstrCmdLine*/ = NULL, BOOL bSettings = FALSE)
{
	// запуск цикла сообщений
	CMessageLoop theLoop;

	g_Module.AddMessageLoop(&theLoop);

	// создание главного диалога
	if(NULL == g_pDlgMain->Create(NULL))
	{
		ATLTRACE(_T("Main dialog creation failed!\n"));
		
		return 0;
	}

	// вызов диалога настроек (в случае если была смена языка программы)
	if (bSettings)
	{
		g_pDlgMain->PostMessage(WM_COMMAND, IDB_SETTINGS);
	}

	// код возврата
	INT iRet = theLoop.Run();

	// удаление очереди сообщений
	g_Module.RemoveMessageLoop();

	return iRet;
}


//////////////////////////////////////////////////////////////////////////
// Входная точка в программу
//
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
	// включение отчета о утечках памяти
	#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif

	// создание локального мутекса отслеживающего работу программы сбора в пределах терминальной сессии
	HANDLE  hMutexRun   = NULL;
	wstring wsMutexName = L"Local\\{9FAF9D80-453E-4b98-A67B-1FD6776755EE}";

	// если мутекс есть, значит копия программы уже запущена
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

	// создаем новый мутекс
	hMutexRun = CreateMutex(NULL, TRUE, wsMutexName.c_str());
	if (NULL == hMutexRun)
	{
		_RPT_API_ERROR(CreateMutex);
		return 0;
	}

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	// добавление флагов для поддержки контролов
	AtlInitCommonControls(ICC_BAR_CLASSES);

	// инициализация COM
	HRESULT hRes = ::CoInitialize(NULL);
	ATLASSERT(SUCCEEDED(hRes));

	// инициализация модууля
	hRes = g_Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	// запуск
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

	// завершаем программы
	g_Module.Term();

	// очистка ресурсов COM
	::CoUninitialize();

	return iRet;
}
