#include "MyHelper.h"

#pragma warning(disable : 4996)

TCHAR g_buff[GBUFF_SIZE];

float GetRandomFloat(float a, float b)
{
	if( a >= b ) // bad input
		return a;

	// Get random float in [0, 1] interval.
	float f = (rand()%10001) * 0.0001f;

	return (f*(b-a))+a;
}

int GetRandomInt(int a, int b)
{
	if( a >= b ) // bad input
		return a;
	return  rand()%(b - a);
}

HANDLE Open_Log_File(TCHAR * filename)
{
	HANDLE file;
	SYSTEMTIME time;
	TCHAR strTime[64];	
	DWORD dwWritten;

	file = CreateFile(filename, GENERIC_WRITE,NULL,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(file == INVALID_HANDLE_VALUE) return 0;	
	GetLocalTime(&time);
	_tcprintf(strTime, L"Создан: %.2d:%.2d:%.2d %.2d/%.2d/%d\n", time.wHour,time.wMinute,time.wSecond,time.wDay,time.wMonth,time.wYear);

	WriteFile(file,strTime,lstrlen(strTime),&dwWritten,NULL);
	
	return file;
}

 int Write_Log_String(HANDLE file, TCHAR *string, ...)
{
	TCHAR buffer[1024];
	DWORD dwWritten;
	va_list arglist; // variable argument list
	
	if (!string) return(0);
	// print out the string using the variable number of arguments on stack
	va_start(arglist,string);	
	_vstprintf(buffer, string, arglist);
	
	va_end(arglist);
	
	if(file == INVALID_HANDLE_VALUE) return 0;
	WriteFile(file,&buffer,lstrlen(buffer),&dwWritten,NULL);
	
	return(1);
} 

int Close_Log_File(HANDLE &file)
{
	SYSTEMTIME time;
	TCHAR strTime[64];
	DWORD dwWritten;
	
	if (file == INVALID_HANDLE_VALUE) return 0;
	GetLocalTime(&time);
	_tcprintf(strTime, L"\nЗакрыт: %.2d:%.2d:%.2d %.2d/%.2d/%d\n", time.wHour,time.wMinute,time.wSecond,time.wDay,time.wMonth,time.wYear);
	
	WriteFile(file, strTime, lstrlen(strTime), &dwWritten, NULL);
	CloseHandle(file);
	file=NULL;

	return 1;
} 

void SplitPath(const char *strIn, char* out_path, char *out_name)
{
    const char *find = strrchr(strIn, '\\');
	if(out_name)
	{
		*out_name = 0;
		if(find) strcpy(out_name, find + 1);
		else strcpy(out_name, strIn);
	}	
	if(out_path)
	{
		*out_path = 0; // обнуляем первый элемент
		if(find)
		{
			int len = find - strIn + 1;
			if(len < MAX_PATH)
			{
				strncpy(out_path, strIn, len);
				out_path[len] = 0;
			}
		}
	}	
}

int BuildFullFileName(const TCHAR* in_path, const TCHAR* in_name, TCHAR** out_fullName)
{
	int len_p = _tcsnlen(in_path, GBUFF_SIZE);	
	int len_n = _tcsnlen(in_name, GBUFF_SIZE);

	if(len_p + len_n + 2 > GBUFF_SIZE) return 0;
	_tcsnccpy_s(g_buff, GBUFF_SIZE, in_path, GBUFF_SIZE);

	if( *(in_path + len_p - 1) != '\\') _tcscat_s(g_buff, GBUFF_SIZE, L"\\");

	_tcscat_s(g_buff, GBUFF_SIZE, in_name);
	*out_fullName = g_buff;

	return 1;
}


int	MapFile(HANDLE h, HANDLE &hMap, void** pVOF, DWORD szByte)
{
	//if(h == INVALID_HANDLE_VALUE) return 0;
	// TODO: !!PAGE_READWRITE если файл открыт только для чтения, то с этим флагом будет fail
	hMap = CreateFileMapping(h, NULL, PAGE_READWRITE, 0, szByte /*fSize*/, NULL);
	//ShowLastErrorWin("AAA!");
	if(hMap == 0) return 0;
	
	*pVOF = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);	
	if(*pVOF == 0) return 0; // TODO: убрать, проверять успех по pVOF в вызывающем коде
	return 1;
}


void UnMapFile(HANDLE hMap, void* pVOF)
{
	if(pVOF != 0) UnmapViewOfFile(pVOF);
	CloseHandle(hMap);
	//pVOF = 0; // смысл? передается по значению
}

void MinRect(D2D1_RECT_F &rcOut, D2D1_RECT_F &rc1, D2D1_RECT_F &rc2)
{
	rcOut.left = max(rc1.left, rc2.left);
	rcOut.top = max(rc1.top, rc2.top);
	rcOut.right = min(rc1.right, rc2.right);	
	rcOut.bottom = min(rc1.bottom, rc2.bottom);

	if (rcOut.left > rcOut.right) rcOut.left = rcOut.right = 0;
	if (rcOut.top > rcOut.bottom) rcOut.top = rcOut.bottom = 0;
	if (rcOut.right < rcOut.left) rcOut.left = rcOut.right = 0;
	if (rcOut.bottom < rcOut.top) rcOut.top = rcOut.bottom = 0;
}

bool ClipRect(int x, int y,RECT* rcInOut, const RECT* rcClip, RECT* rcOut)
{
	int tmX, tmY;

	tmX = rcInOut->left - x;
	tmY = rcInOut->top - y;
	//if(x >= rcClip->right || y >= rcClip->bottom) return 0;

	rcOut->left = x;
	rcOut->top = y;
	rcOut->right = rcOut->left + rcInOut->right - rcInOut->left;	
	rcOut->bottom = rcOut->top + rcInOut->bottom - rcInOut->top;

	if(0 != IntersectRect(rcInOut, rcOut, rcClip)) // если пересекаются
	{
		*rcOut = *rcInOut;

		rcInOut->left  = rcOut->left + tmX;
		rcInOut->top  = rcOut->top + tmY;
		rcInOut->right  = rcOut->right + tmX;
		rcInOut->bottom  = rcOut->bottom + tmY;
		return 1;
	}
	return 0;
}

