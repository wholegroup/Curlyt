#pragma once

// установка хука
__declspec(dllexport) BOOL SetHook();

// снятие хука
__declspec(dllexport) BOOL UnSetHook();

// возвращает последнюю полученную расскладку
__declspec(dllexport) HKL GetLastLayout();
