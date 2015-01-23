#pragma once

// выключение варнингов
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

// Change these values to use different versions
#define WINVER        0x0500
#define _WIN32_WINNT  0x0501
#define _WIN32_IE     0x0501
#define _RICHEDIT_VER 0x0200

// STL
#include <map>
#include <utility>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>

using namespace std;

// отлов утечек памяти
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC

	#include <stdlib.h>
	#include <crtdbg.h>

	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

// WTL
#include <atlbase.h>
#include <atlapp.h>
#include <atlframe.h>
#include <atlwin.h>
#include <atlmisc.h>
#include <atlctrls.h>
#include <atlctrlx.h>

// AGG
#pragma warning(push)
#pragma warning(disable: 4267) // conversion x64

#include "agg_rendering_buffer.h"
#include "agg_pixfmt_rgba.h"
#include "agg_renderer_base.h"
#include "agg_renderer_scanline.h"
#include "agg_scanline_p.h"
#include "agg-2.4\font_win32_tt\agg_font_win32_tt.h"

#pragma warning(pop)

typedef agg::font_engine_win32_tt_int16                       font_engine_type;
typedef agg::font_cache_manager<font_engine_type>             font_manager_type;
typedef agg::conv_curve<font_manager_type::path_adaptor_type> conv_font_curve_type;

// для создания иконки в трее
#include "CTrayIconImpl.h"

// для создания тредов
#include "CThreadImpl.h"

// для использования настроек программы
#include "WTLAddons/CSettingsImpl.h"

// главный модуль программы
extern CAppModule g_Module;

// версия программы (генерируется при сборке MSBuild)
#include "Version.h"

// различные константы, определения типов
#include "Defines.h"

// манифест
#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif


