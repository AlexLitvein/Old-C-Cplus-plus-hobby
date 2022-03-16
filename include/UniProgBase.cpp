//#include "stdafx.h"
#include "ProgBase.h"

// use ports: 37A, 37b, 37c

void Reset()
{
	__asm
	{
		xor eax,eax
		mov al,00h
		mov dx,37Ah
		out dx,al
		mov al,04h
		out dx,al
	}
};

void WriteDataPort(int addr, int data)
{	
	__asm
	{	
		xor eax,eax
		mov al,byte ptr addr
		mov dx,37bh
		out dx,al	

		xor eax,eax
		mov al,byte ptr data
		mov dx,37ch
		out dx,al		
	}
	
};

char ReadDataPort(int addr)
{
	unsigned char data;
	
	__asm
	{	
		xor eax,eax
		mov al,byte ptr addr
		mov dx,37bh
		out dx,al
	

		xor eax,eax
		mov al,byte ptr data
		mov dx,37ch
		in al,dx
		mov data,al
	}
	
	return data;
};

void SetDAC(int progSrc,int dacData)
{
	switch (progSrc)
    {
        case 1: WriteDataPort(C3 | BA,dacData);
        break;
        case 2: WriteDataPort(C4 | BA,dacData);
        break;
        case 3: WriteDataPort(C5 | BA,dacData);
        break;
        case 4: WriteDataPort(C5 | BB,dacData);
        break;
        default: ;
    }
	
};

void InitProg()
{
	Reset();
	WriteDataPort(C1 & C2 | RUS, 0x90);
	WriteDataPort(C3 & C4 & C5 | RUS, 0x80);
	WriteDataPort(C1 | BB, 0x00);
	WriteDataPort(C1 | BC, 0xFF);	
	WriteDataPort(C2 & C3 & C4 | BB, 0x00);
	WriteDataPort(C2 & C3 & C4 & C5 | BC, 0xFF);	
	SetDAC(1, DACOFF);
	SetDAC(2, DACOFF);
	SetDAC(3, DACOFF);
	SetDAC(4, DACOFF);
};