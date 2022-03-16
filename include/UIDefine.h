#pragma once

//=====================PANEL=================================

#define PANEL_MAX_WND 64

enum UIPANEL_STATE : size_t // UIPANEL_SIGNALS ???
{
	PS_NONE			= 0,
	PS_UPDVBUFF		= 1 // ������� ������� �� ����(�� ��� � m_arrWndPtr �� �����-�� ��� � m_VB) � �����
	// ��� resize\move �������� ��� ��� ��������� ����� ����� �������������� �������� �� ��� ������� ��������
	// �������� ���� ���� ����� ���� ���� ����� ���� ��� ������ ��������� �� UIPanelNew � ������ Base_Elm
};

enum UIPANEL_FONT : size_t
{
	FONT_LABEL = 0,
	FONT_ELM1,
	FONT_ELM2,
	FONT_ELM3,
	FONT_ELM4,
	FONT_ELM5,
	FONT_ELM6,
	FONT_ELM7,
	FONT_COUNT	
};

enum UIPANEL_COLOR : size_t
{
	CLR_TEXT = 0,
	CLR_BCKGND,
	CLR_ELM2,
	CLR_ELM3,
	CLR_ELM4,
	CLR_ELM5,
	CLR_ELM6,
	CLR_ELM7,
	CLR_COUNT	
};

//=====================ALIGN=================================

#define DOCK_NONE		0
#define DOCK_LEFT		1
#define DOCK_TOP		2
#define DOCK_RIGHT		4
#define DOCK_BOTTOM		8
#define DOCK_FILL		16

#define ANCHOR_LEFT_TOP		1
#define ANCHOR_LEFT_BOTTOM	2
#define ANCHOR_RIGHT_TOP	4
#define ANCHOR_RIGHT_BOTTOM	8

#define ALIGN_LEFT		1
#define ALIGN_TOP		2
#define ALIGN_RIGHT		4
#define ALIGN_BOTTOM	8
#define ALIGN_MIDDLE_X	16
#define ALIGN_MIDDLE_Y	32

#define ALIGN_DEFAULT 0 | (((ALIGN_LEFT | ALIGN_TOP) << 16) | ANCHOR_LEFT_TOP)

#define SET_ANCHOR(flag, anchor) anchor = anchor & 0x00ff0000 | flag;
#define SET_DOCK(flag, dock) dock = dock & 0x00ff0000 | (flag << 8) | ANCHOR_LEFT_TOP;
#define SET_ALIGN(flag, align) align = align & 0x0000ffff | (flag << 16);

//=====================FONT=================================

#define MAXCHAR_FONTNAME 32
#define MAXCHAR_INFONT 256
#define SYMB_COL 28
#define SYMB_ROWS 32
#define MAX_ROWS 32

struct TEXTROW
{
	TEXTROW() : text(0), count(0), width(0) {}
	const char* text;
	int count;	// ���-�� ��������
	int width;	// ������ � ��������
	//int visible;
};

struct TEXTBLOCK
{	
	int x;
	int y;
	int cntRow;
	TEXTROW rows[MAX_ROWS];
};

struct SymbolLine // sizeof = 32
{	
	size_t cnt;
	unsigned char data[SYMB_COL];
};

struct SymbolDef // sizeof = 1056
{
	SymbolDef() { Mem_Set_QUAD(this, 0, sizeof(SymbolDef) >> 2); }
	size_t width;
	SymbolLine lines[SYMB_ROWS];
	char reserve[28]; // �������� �� �� 32 ����
};

//=====================ELEMENT=================================

#define ELEMENT_MAX_SIZE 256
#define ELEMENT_NAME_MAXCHAR 16
#define ELEMENT_MAX_CHILD 32

// � ������� �� ������� ���������� � ������� �����
enum ELEMENT_STATE : size_t // ... � ��� ����� �����
{
	STATE_HIDE		= 1,
	STATE_LOCK		= 2,	
	STATE_TOP		= 4, // ������ ������ �� ���������
	STATE_MOVE		= 8, // �� � ������
	STATE_MOUSEHIT	= 16, // ������ ������ � �������
	STATE_FOCUS		= 32, // ����� ����� �����(���� ����� ���� � �� ���), ���� ����� ���� � �� ���
	STATE_SELECT	= 64,
	STATE_RESIZE	= 128
};

enum ELEMENT_SIGNAL : size_t
{
	SIGNAL_REDRAW	= 1,  // ??
	//MSG_UPDVBUFF = 2,  // ??
	MSG_MOUSEHIT		= 2,
	MSG_MOVE		= 4,// ??
	MSG_RESIZE		= 8,// ??
	MSG_CHANGE_BCKGND_COLOR	= 16,
	MSG_ALIGN		= 32 // 
};

// �������� �������� ��� �������� �������� � �� ���������� � ������� �����
enum ELEMENT_PROP : size_t
{
	PROP_MOVABLE	= 1,
	PROP_RESIZEABLE = 2
	//MSG_MOUSEHIT		= 2,
	//MSG_MOVE		= 4,
	//MSG_RESIZE		= 8,
	//MSG_CHANGE_BCKGND_COLOR	= 16
};
