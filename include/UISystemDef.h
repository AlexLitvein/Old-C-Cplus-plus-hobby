#pragma once

#include "d3dUtil.h"
#include "Vertex.h"
#include "d3d_DI.h"
#include "UIDefine.h"
#include "TextureManager.h"

extern DirectInput* gDInput;

class MyFont
{
public:
	MyFont(void);
	MyFont(char *name, int h, int interStr);	
	virtual ~MyFont(void);
	void Destroy(void);
	
	int ScanFont(const char* name, int hInPix);
	int LoadFont(const char* name, int h);

	//const char* DrawStringM(/*int x, int y,*/ const char* str, RECT &rcClip, size_t align, 
	//				const TMItem *pTexItemDst, size_t clrFont, size_t clrBkg);

	void DrawStringM2(int y, TEXTROW &textrow, RECT &rcClip, size_t align, 
					const TMItem *pTexItemDst, size_t clrFont, size_t clrBkg);

	//void DrawTextM(int x, int y, const char* str, RECT &rcClip, SIZE &tmAlign, 
	//				const TMItem *pTexItemDst, size_t clrFont, size_t clrBkg, bool bWrap);

	void DrawTextM2(/*int x, int y,*/ TEXTBLOCK* pTextBlock, RECT &rcClip, size_t align, 
					const TMItem *pTexItemDst, size_t clrFont, size_t clrBkg, bool bWrap);

	//const char* MeasureString(const char* str, const RECT &rcClip, RECT &rcBound_out, size_t &lenght_out);
	//void MeasureText(const char* str, const RECT &bound, bool bWrap, RECT &size);
	//int CharOut(/*HDC hDC,*/ int x, int y, int ch, KFontHeader20* pH, int sx=1, int sy=1);
	//void MeasureText2(const char* str, const RECT &rcClip, RECT &rcBound, bool bWrap);
	void ProcessText(const char* str, const RECT &rcClip, TEXTBLOCK* pTextBlock, bool bWrap);
	const char* ProcessString(const char* str, const RECT &rcClip, TEXTROW &textrow);
	//void OutSymbols();

protected:
	char				m_name[MAXCHAR_FONTNAME];
	int					m_h;
	int					m_interStr;
	SymbolDef			m_arrSymbDefs[MAXCHAR_INFONT];
	
	void scanSymbol(SymbolDef* pSymbDef, size_t w, size_t pitchInPix, size_t* pTexl);
	void defineSymbol(SymbolDef* pSymbDef, size_t w, size_t pitch, size_t* pTexl);
	void drawChar(unsigned char ch, size_t w, size_t clrBckgnd, size_t color, size_t* pDst, size_t pitchDst);
	/*void drawChar2(size_t x, size_t y, unsigned char ch, RECT &rcClip, 
					   size_t color, size_t* pDst, size_t pitchDst);*/
};

class Base_Elm // делаем с запасом на все будущие типы(вместе с массивом указателей на доч-ие эл-ты)
{
public:
	Base_Elm(void);
	Base_Elm(Base_Elm* pParent, int x, int y, int w, int h, int maxW, int maxH, char* name);
	virtual ~Base_Elm(void);
	virtual void Destroy();

	Base_Elm*	Parent() { return m_pParent; }
	size_t		State(void) { return m_state; }
	size_t		IdxTex(void) { return m_idxTex; } //???
	int			X(void) { return m_x; }
	int			Y(void) { return m_y; }
	int			W(void) { return m_w; }
	int			H(void) { return m_h; }
	//void		CurrRect(RECT &rc) { rc.left = 0; rc.top = 0; rc.right = m_w; rc.bottom = m_h; }
	inline void	CurrRect(RECT &rc, bool bLocal); // разделить на два метода(без if)
	int			MaxW() { return m_maxW; }
	int			MaxH() { return m_maxH; }
	//void		Resize();
	void		SetAnchor(size_t flags) { SET_ANCHOR(flags, m_align); }
	void		SetDock(size_t flags)	{ SET_DOCK(flags, m_align); }
	void		SetAlign(size_t flags)	{ SET_ALIGN(flags, m_align); }
	//inline void		SetSize(size_t w, size_t h);
	//size_t		GetAlign() { return m_align; }
	inline void	SetState(ELEMENT_STATE state, bool bit);
	inline void SetProperty(ELEMENT_PROP prop);
	void		AlignRect(RECT* pDstRC, RECT* pSrcRC);
	inline void		Activate();
	inline void		SetFocus();

	virtual void		Update(float dt);
	virtual void		UpdateState(float dt);
	virtual void		UpdateView(); // ???
	IDirect3DTexture9*  Texture() { return g_pTexMngr->GetTexData(m_idxTex)->_pTex; }
	

protected:
	int					m_idx;		// ???
	char				m_name[ELEMENT_NAME_MAXCHAR]; // надо для таблицы
	Base_Elm*			m_pParent;
	size_t				m_state;
	size_t				m_property;
	size_t				m_messages;
	size_t				m_bckColor;
	size_t				m_align;	// | no use | align item | dock | anchor |
	//bool b_wrap
	//SIZE				m_anchor;	
	int					m_x;
	int					m_y;
	size_t				m_w;
	size_t				m_h;
	size_t				m_maxW;
	size_t				m_maxH;
	size_t				m_minSize; // одно для всех! static?
	size_t				m_idxTex;

	size_t				tmpFPS; // temp

public:
	// Указатели на ф-ции
	void (*OnMBDown)	(Base_Elm* pSender/*, MOUSE_DATA ms*/);
	void (*OnMBUp)		(Base_Elm* pSender);
	void (*OnMMove)		(Base_Elm* pSender);
	void (*OnKeyDown)	(Base_Elm* pSender);
	void (*OnResize)	(Base_Elm* pSender);

protected:

	bool isAnchor(size_t flags) { return (m_align & flags) != 0; }
	bool isDock(size_t flags) { return (m_align & (flags << 8)) != 0; }
	bool isAlign(size_t flags) { return (m_align & (flags << 16)) != 0; }

	void doAnchor();
	void doDock();
	//void align();

	virtual void MBDown(/*MOUSE_DATA*/) {}
	virtual void MBUp(/*MOUSE_DATA*/) {}
	virtual void MMove(/*MOUSE_DATA*/) {}
	virtual void KeyDown(/*KEYBOARD_DATA*/) {}
	virtual void Resize(size_t w, size_t h);
};

//void Base_Elm::SetSize(size_t w, size_t h)
//{
//	m_minSize = 10;
//
//	m_maxW = w;
//	m_maxH = h;
//
//	if(w < m_minSize) m_w = m_minSize;
//	if(h < m_minSize) m_h = m_minSize;
//	if(w > m_maxW) m_w = m_maxW;
//	if(h > m_maxH) m_h = m_maxH;
//}
void Base_Elm::Activate()
{
	//SET_BIT(m_state, ES_ONTOP);
	//SET_BIT(m_state, ES_MOVE);
}
void Base_Elm::SetFocus()
{
	SET_BIT(m_state, STATE_TOP);
	SET_BIT(m_state, STATE_FOCUS);	
	SetState(STATE_MOVE, (m_property & PROP_MOVABLE) != 0);

	if((m_property & PROP_RESIZEABLE) != 0 && gDInput->MouseX() >= m_x + m_w - 5) 
	{
		SET_BIT(m_state, STATE_RESIZE);	
	}
	//SetState(STATE_RESIZING, (m_property & PROP_RESIZEABLE) != 0);
}
void Base_Elm::CurrRect(RECT &rc, bool bLocal) 
{
	int toX = 0, toY = 0;
	if(bLocal == 0) { toX = m_x; toY = m_y; }
	rc.left = toX; rc.top = toY; rc.right = rc.left + m_w; rc.bottom = rc.top + m_h; 
}
void Base_Elm::SetState(ELEMENT_STATE state, bool bit)
{
	if(bit) SET_BIT(m_state, state);
	else RESET_BIT(m_state, state);
}
void Base_Elm::SetProperty(ELEMENT_PROP prop)
{
	m_property |= prop;
}

//=====================LABEL=================================

//class LabelMy : public Base_Elm
//{
//public:
//	LabelMy(){};
//	LabelMy(char* name, char* text, SIZE pos, SIZE size, 
//			size_t align, size_t dock, size_t anchor, Base_Elm* pParent);
//	
//	void SetText(const char* text){ strncpy(m_text, text, MAXCHAR); }
//	char* GetText(){ return m_text; }
//protected:
//	char m_text[MAXCHAR];
//};

//=====================PANEL=================================

class UIPanelNew
{
public:
			UIPanelNew(void);
			UIPanelNew(size_t w, size_t h, size_t maxElm);
	virtual ~UIPanelNew(void);
	void	Destroy(void);
	void	Draw(void);

	void	Draw_fx();
	void	build_fx();

	void	Update(float dt);
	
	size_t	AddElement(Base_Elm* pElm);
	void	OnLostDevice(void);
	void	OnResetDevice(void);
	size_t  W(void) { return m_w; }
	size_t  H(void) { return m_h; }

	size_t  GetColor(UIPANEL_COLOR color){ return m_arrSysColor[color]; }
	MyFont* GetFont(UIPANEL_FONT font){ return m_arrFonts[font]; }

	MyFont			m_font;

protected:
	size_t					m_w;
	size_t					m_h;
	int						m_cntWnd; // текущ кол-во окон в m_arrWndIdx
	size_t					m_szElm;
	size_t					m_state;
	IDirect3DVertexBuffer9* m_VB;
	IDirect3DIndexBuffer9*  m_IB;	

	Papo::MyTable			m_typeTbl;	
	Papo::MyTable			m_dataTbl;
	void*					m_pMem;
	int						m_idxTopWnd;
	Base_Elm*				m_arrWndPtr[PANEL_MAX_WND];	
	D3DCOLOR				m_arrSysColor[CLR_COUNT];
	MyFont*					m_arrFonts[FONT_COUNT];

	/*ID3DXEffect*	m_FX;
	D3DXHANDLE		m_hTech;
	D3DXHANDLE		m_hTex0;
	D3DXHANDLE		m_hElmData;*/

	HRESULT	createVIBuffers(int cntElm);
	size_t addWind(Base_Elm* pWnd); // 0 - OK; 1 - FAIL
	void	updateVBuff(int idx, Base_Elm* pElm);

	void	updateVBuff_2(VertexRHW* v, Base_Elm* pElm);
	//HRESULT	createVIBuffers_2(int cntElm);
	HRESULT createVIBuffers_fx();
};

extern UIPanelNew* g_pUIPanel;