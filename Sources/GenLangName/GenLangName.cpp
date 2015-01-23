//////////////////////////////////////////////////////////////////////////
// Вывод списка языков системы
//

#include "stdafx.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

typedef map<const wstring, const wstring> CMapLang;

int wmain(int argc, WCHAR* argv[])
{
	vector<WCHAR> wcAbb(4);
	vector<WCHAR> wcName(20);
	CMapLang      mapLang;

	for (INT i = 1; i < 1000; i++)
	{
		GetLocaleInfo(MAKELCID(i, 0), LOCALE_SABBREVLANGNAME, &wcAbb.front(), (int)wcAbb.size());
		GetLocaleInfo(MAKELCID(i, 0), LOCALE_SENGCOUNTRY, &wcName.front(), (int)wcName.size());

		mapLang.insert(make_pair(&wcAbb.front(), &wcName.front()));
	}

	for (CMapLang::iterator iterLang = mapLang.begin(); iterLang != mapLang.end(); iterLang++)
	{
		printf("%S %S\n", iterLang->first.c_str(), iterLang->second.c_str());
	}

	return 0;
}

