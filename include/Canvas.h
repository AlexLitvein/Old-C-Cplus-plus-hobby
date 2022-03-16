#pragma once

#include "d3dutil.h"
//#include "UIElement.h"

//class BaseElmt;

class Canvas
{
public:
	Canvas(void);
	Canvas(int w, int h/*, DWORD clr*/);
	~Canvas(void);

	bool GetSubRect(RECT* rcTest, RECT &rcDestInOut, RECT &rcSrcInOut);
	void FastClearAll(DWORD clr);
	//void Clear(bool bClrClient);
	void SetTM(int dx, int dy);
	void SetClipRC(bool bClient);
	void Resize(RECT &rc);
	//void ClearRC(RECT* pRC);
	void FillRC(RECT* pRC, DWORD clr);
	void DrawRC(LPDIRECT3DTEXTURE9 pSrcTex, const RECT &rcSrcIn, int x, int y, DWORD clr);
	//void DrawString(bool bWrap, RECT *rcBnd, int x, int y, D3DCOLOR cFont, D3DCOLOR cBkg, const char* str);
	void DrawLine(int x, int y, int w, int h, DWORD color);
	void DrawPattrnLineV(LPDIRECT3DTEXTURE9 pSrcTex, const RECT &rcPattrn, 
							int x, int y, int l, DWORD clr);

	int					m_offsL;
	int					m_offsR;
	int					m_offsU;
	int					m_offsD;
	//DWORD				m_clr;	
	int					m_w;
	int					m_h;
	int					m_cntPxls;
	RECT				m_rcCurr;
	RECT				m_rcClnt;
	RECT*				m_pClipRect;
	POINT				m_TM;
	LPDIRECT3DTEXTURE9  m_pTex;

	//BaseElmt*			m_pParent;
	//MyFont*				m_pFont;
};
