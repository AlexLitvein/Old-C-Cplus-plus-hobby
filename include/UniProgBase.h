#ifndef PROGBASE_H
#define PROGBASE_H

enum addr
{
    BA=0x00,
	BB=0x01,
	BC=0x02, 
	RUS=0x03,
	C1=0xF8,
    C2=0xF4,
    C3=0xEC, 
	C4=0xDC,
	C5=0xBC 
};

enum DACcrtl
{
	DACON=0x00,
	DACOFF=0x80,
	CAPON=0x40,
	CAPOFF_DACON=0x00,
	PRGSRC1_5V=0x0B,
	PRGSRC1_11_5V=0x1A,
	PRGSRC1_12_5V=0x1C,
	PRGSRC2_5V=0x0C,
	PRGSRC4_5V=0x0B
};

void Reset();
void WriteDataPort(int addr, int data);
char ReadDataPort(int addr);
void SetDAC(int progSrc,int dacData);
void InitProg();

#endif