#include "WinINI.h"

extern char g_buff[];

 WinINI* g_pIniFile = 0;

WinINI::WinINI(void)
{
	//m_error = 0;	
}

WinINI::~WinINI(void)
{
}

// Если имя без пути создастся в тек каталоге процесса
WinINI::WinINI(const char* fName)
{
	//char buff[512];
	this->WinINI::WinINI();

	if(NULL == strchr(fName, '\\')) // не найден, значит имя без пути
	{
		GetModuleFileName(NULL, g_buff, GBUFF_SIZE);
		SplitPath(g_buff, m_fName, 0); // в m_fName путь оканчивается '\'	
		// будет копировать пока не кончится буфер или не встретится 0
		strcat_s(m_fName, fName);
	}
	else
	{
		strncpy_s(m_fName, sizeof(m_fName), fName, sizeof(m_fName)-1);
		m_fName[sizeof(m_fName)-1] = 0;
	}
}

// Если секции нет, то она создается, если ключа нет то он создается
// если *key = 0, то секция со всеми ключами удаляется
void WinINI::SetKeyInt(const char* section, const char* key, int value)
{
	_itoa_s(value, g_buff/*m_buffTmp*/, GBUFF_SIZE, 10);
	// если вместо m_buffTmp вставить NULL, то ключ с именем key удаляется
	WritePrivateProfileString(section, key, g_buff/*m_buffTmp*/, m_fName);	
}
// Если секции нет, то она создается,  If the key does not exist in the specified section, 
// it is created. If key parameter is NULL, the entire section, 
// including all entries within the section, is deleted.
void WinINI::SetKeyString(const char* section, const char* key,  const char* strValue)
{
	// если вместо m_buffTmp вставить NULL, то ключ с именем key удаляется
	WritePrivateProfileString(section, key, strValue, m_fName);	
}

int WinINI::GetKeyInt(const char* section, const char* key, int nDefault)
{
	return GetPrivateProfileInt (section, key, nDefault, m_fName);
}

// The return value is the number of characters copied to the buffer, 
// not including the terminating null character.
DWORD WinINI::GetKeyString(const char* section, const char* key, const char* strDefault, char* buffOut, int buffSize)
{
	return GetPrivateProfileString(section, key, strDefault, buffOut, buffSize, m_fName);
}