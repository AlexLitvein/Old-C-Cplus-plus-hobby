// ���� ���� ����� ����� �������� ���������
#pragma once


#include <crtdbg.h>

// ����� ��� ���� ����� ��� ������� ������ � �������� ����, 
// ������������ ������ ����� � ����� �������� ������.
#ifdef _DEBUG
	#ifndef NEW_INLINE_WORKAROUND
		#define NEW_INLINE_WORKAROUND new ( _NORMAL_BLOCK ,__FILE__, __LINE__ )
		#define new NEW_INLINE_WORKAROUND
	#endif
#endif // _DEBUG