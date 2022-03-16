// Этот файл нужно везде включать последним
#pragma once


#include <crtdbg.h>

// нужно для того чтобы при утечках памяти в выходном окне, 
// отображались номера строк и имена исходных файлов.
#ifdef _DEBUG
	#ifndef NEW_INLINE_WORKAROUND
		#define NEW_INLINE_WORKAROUND new ( _NORMAL_BLOCK ,__FILE__, __LINE__ )
		#define new NEW_INLINE_WORKAROUND
	#endif
#endif // _DEBUG