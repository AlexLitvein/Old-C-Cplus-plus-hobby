#pragma once

//#include "Debug_first.h"
#include <windows.h>


#include <stdio.h>
#include <stdarg.h>

#include <TCHAR.H>
//#include <Strsafe.h>

#pragma warning(disable : 4996) // _CRT_SECURE_NO_WARNINGS

extern TCHAR g_buff[];
const size_t GBUFF_SIZE = 512;

#define SWAP(a,b,t) {t=a; a=b; b=t;}
#define MAKEDWORD(a, b) ((LONG)(((WORD)(((DWORD_PTR)(b)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(a)) & 0xffff))) << 16))

#define SET_BIT(word,bit_flag)   ((word)=((word) | (bit_flag)))
#define RESET_BIT(word,bit_flag) ((word)=((word) & (~bit_flag)))

//===================Randomness=======================================
float GetRandomFloat(float a, float b);
int GetRandomInt(int a, int b);
//====================================================================
inline size_t ColorARGB(size_t a, size_t r, size_t g, size_t b)
{
	size_t out = 0;
	out = ((b & 0x000000ff) | ((g & 0x000000ff) << 8) | ((r & 0x000000ff) << 16) | ((a & 0x000000ff) << 24));
	return out;
}
inline DWORD NearBigMult(DWORD num, DWORD x)
{
	//DWORD add = 0;
	/*if(num < x) num = x;
	if(num % x) num += x;
	return (num / x) * x;*/

	DWORD nBlock = 0;
	nBlock = num / x;
	if(num % x) nBlock += 1;
	return nBlock * x;
}

inline size_t log2( size_t x ) 
{   
	size_t ans = 0 ;   
	while(x >>= 1) ans++;   
	return ans ; 
} 

inline size_t power( size_t b, size_t e ) 
{	
    if (e == 0) return 1;
    else        return b * power(b, e-1);
}

inline size_t NearBigMult2( size_t x ) 
{   
	size_t tmp, ans = 0 ; 
	ans = log2(x);
	tmp = power(2, ans);
	if( tmp < x ) tmp *= 2;	  
	return log2(tmp); 
} 

inline bool PointTest(float ptX, float ptY, float rcX, float rcY, float rcW, float rcH)
{
	bool res = false;
	if (ptX >= rcX && ptX <= rcX + rcW) { res = true; }
	return res & (ptY >= rcY && ptY <= rcY + rcH);
}


void MinRect(D2D1_RECT_F &rcOut, D2D1_RECT_F &rc1, D2D1_RECT_F &rc2);

// прямоугольники дб в непротиворечивом состоянии (left < right and top < bottom && >= 0!!!) 
bool ClipRect(int x, int y,RECT* rcInOut, const RECT* rcClip, RECT* rcOut);
inline void Mem_Mov_QUAD(void *src, void *dest, size_t count)
{
	// this function fills or sets unsigned 32-bit aligned memory
	// count is number of quads
	_asm 
	{ 
		mov esi, src   
		mov edi, dest   ; edi points to destination memory
		mov ecx, count  ; number of 32-bit words to move		
		rep movsd       ; move data
	}
}

inline void Mem_Set_QUAD(void *dest, UINT data, int count)
{
	// this function fills or sets unsigned 32-bit aligned memory
	// count is number of quads
	_asm 
	{ 
		mov edi, dest   ; edi points to destination memory
		mov ecx, count  ; number of 32-bit words to move
		mov eax, data   ; 32-bit data
		rep stosd       ; move data
	}
}
//-------------------------------------------------------------------------------------------------
HANDLE Open_Log_File(TCHAR * filename);
int Write_Log_String(HANDLE file, TCHAR *string, ...);
int Close_Log_File(HANDLE &file);
//-------------------------------------------------------------------------------------------------
void SplitPath(const char *strIn, char* out_path, char *out_name);
// 0 - OK, 1 - fail
int BuildFullFileName(const TCHAR* in_path, const TCHAR* in_name, TCHAR** out_fullName);
//-------------------------------------------------------------------------------------------------
int	MapFile(HANDLE h, HANDLE &hMap, void** pVOF, DWORD szByte);
void UnMapFile(HANDLE hMap, void* pVOF);

