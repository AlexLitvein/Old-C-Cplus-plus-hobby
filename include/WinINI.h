#pragma once

#include "windows.h"
#include "MyHelper.h"

class WinINI
{
public:
	WinINI(void);
	WinINI(const char* fName);
	~WinINI(void);

	void	SetKeyInt(const char* section, const char* key, int value);
	void	SetKeyString(const char* section, const char* key, const char* strValue);
	int		GetKeyInt(const char* section, const char* key, int nDefault);
	DWORD	GetKeyString(const char* section, const char* key, const char* strDefault, char* buffOut, int buffSize);

	//DWORD	m_error;
	char	m_fName[512]; // полный путь, иначе создастся в папке windows
	//char	m_buffTmp[32];
};	
