#pragma once

// ��������� ����
__declspec(dllexport) BOOL SetHook();

// ������ ����
__declspec(dllexport) BOOL UnSetHook();

// ���������� ��������� ���������� ����������
__declspec(dllexport) HKL GetLastLayout();
