#pragma once

#include <windows.h>
#include <Strsafe.h>

extern char err_buff[];
extern size_t g_lastError;
const size_t ERRBUFF_SIZE = 512;
const size_t ERR_CODES = 0xffffffff - 100;

const char g_errStr[][32] =  
{ 
	"M_OK",			// FFFFFF9B
	//"M_NEEDALLOC",
	"M_FAIL",
	"M_FULL",
	"M_HEAPFULL",
	"M_VECFULL",
	"M_HEAP_CREATE_FAIL",
	"M_BIGSTRING",
	"M_NOINIT",	
	"M_COPYTBL_FAIL",
	"M_PATHERROR",
	"M_NONEXTFREE",	
	"M_OUTOFRANGES",
	"M_BADPTR",
	"M_ALLOCFAIL",
	"M_RESIZEFAIL",
	"M_ITEM_EXISTING",
	"M_FILENOFOUND",
	"M_MAPFILE_FAIL"
};

enum ERRCODE : size_t
{
	M_OK = ERR_CODES,
	//M_NEEDALLOC,
	M_FAIL,	
	M_FULL,
	M_HEAPFULL,
	M_VECFULL,
	M_HEAP_CREATE_FAIL,
	M_BIGSTRING,
	M_NOINIT,
	M_COPYTBL_FAIL,
	M_PATHERROR,
	M_NONEXTFREE,
	M_OUTOFRANGES,
	M_BADPTR,
	M_ALLOCFAIL,
	M_RESIZEFAIL,
	M_ITEM_EXISTING,
	M_FILENOFOUND,
	M_MAPFILE_FAIL
};



const char *GetLastErrorText();

void ShowLastErrorWin(LPTSTR lpszFunction) ;

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef MHR
	#define MHR(x)															\
	{																		\
		if(x > M_OK)														\
		{																	\
			sprintf(err_buff, "%s\nстрока: %d", __FILE__, __LINE__);		\
			::MessageBox(0, err_buff, GetLastErrorText(), MB_ICONWARNING);	\
		}																	\
	}
	#endif

#else
	#ifndef MHR
	#define MHR(x) x;
	#endif
#endif
