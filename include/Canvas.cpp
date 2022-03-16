#include "Canvas.h"
#include "TextureMngr.h"
#include "UIPanel.h"
#include "MyHelper.h"

Canvas::Canvas(void){}
Canvas::Canvas(int w, int h/*, DWORD clr*/)
{	
	//m_clr = clr;
	m_offsL = m_offsR = m_offsU = m_offsD = 0;
	m_w = w;
	m_h = h;
	m_cntPxls = m_w * m_h;
	
	m_rcCurr.left = m_rcCurr.top = 0;
	m_rcCurr.right = m_w; 
	m_rcCurr.bottom = m_h;

	m_rcClnt = m_rcCurr;
	m_pClipRect = &m_rcClnt;

	m_TM.x = 0; m_TM.y = 0;	
	// !!!  При создании с D3DPOOL_SYSTEMMEM и рендеринге ее спрайтом,
	//		всегда цвет текстуры ограничен цветом параметра в вызове спрайта.	
	m_pTex = g_pTexMngr->CreateTex(g_pD3Device, m_w, m_h, "xxx", 0xffffffff/*m_clr*/);	
	//m_pFont = g_UIPanel->GetFontPtr(0);	
	
	//D3DXSaveTextureToFile("Canvas.bmp", D3DXIFF_BMP, m_pTex, 0);
	//Clear();
	/*RECT r;
	r.left = r.top = 1;
	r.right = r.bottom = 3;

	FillRect(&r, 0x0);*/
	//m_pParent = 0;
}
void Canvas::Resize(RECT &rc)
{
	m_rcCurr.right = rc.right - rc.left;	
	m_rcCurr.bottom = rc.bottom - rc.top;
	
	m_rcClnt.left = m_offsL;
	m_rcClnt.top = m_offsU;
	m_rcClnt.right = m_rcCurr.right - m_offsR;
	m_rcClnt.bottom = m_rcCurr.bottom - m_offsD;
}
bool Canvas::GetSubRect(RECT* rcTest, RECT &rcDestInOut, RECT &rcSrcInOut)
{
	bool res = 1;
	int sub = rcDestInOut.left - rcTest->left;
	if(sub < 0)
	{
		rcDestInOut.left = rcTest->left;
		rcSrcInOut.left -= sub;
	}
	if(rcDestInOut.left > rcTest->right) res = 0;

	sub = rcDestInOut.top - rcTest->top;
	if(sub < 0) 
	{
		rcDestInOut.top = rcTest->top;
		rcSrcInOut.top -= sub;
	}
	if(rcDestInOut.top > rcTest->bottom) res = 0;

	if(rcDestInOut.right > rcTest->right) rcDestInOut.right = rcTest->right;
	if(rcDestInOut.right < rcTest->left) res = 0;

	if(rcDestInOut.bottom > rcTest->bottom) rcDestInOut.bottom = rcTest->bottom;
	if(rcDestInOut.bottom < rcTest->top) res = 0;

	return res;
}
void Canvas::SetTM(int dx, int dy) { m_TM.x = dx; m_TM.y = dy; }
void Canvas::DrawRC(LPDIRECT3DTEXTURE9 pSrcTex, const RECT &rcSrcIn, int x, int y, DWORD clr)
{
	//Write_Log_String(hLog, "Canvas::DrawRC %f\n", g_time); // !!!

	RECT rcDst;
	RECT rcSrc = rcSrcIn;
	
	rcDst.left	 = x + m_TM.x; 
	rcDst.top	 = y + m_TM.y;
	
	rcDst.right  = rcDst.left + (rcSrc.right - rcSrc.left);
	rcDst.bottom = rcDst.top + (rcSrc.bottom - rcSrc.top);

	if(!GetSubRect(m_pClipRect, rcDst, rcSrc)) return;	

	int dstW, dstH;
	dstW = rcDst.right - rcDst.left;
	dstH = rcDst.bottom - rcDst.top;
	int stpDst = m_w - dstW;

	D3DSURFACE_DESC	sd;
	pSrcTex->GetLevelDesc(0, &sd);
	int stpSrc = sd.Width - dstW;	

	D3DLOCKED_RECT rcDstLock;
	m_pTex->LockRect(0, &rcDstLock, &rcDst, 0);

	D3DLOCKED_RECT rcSrcLock;
	pSrcTex->LockRect(0, &rcSrcLock, &rcSrc, 0);

	DWORD *p32Dst = ( DWORD*)rcDstLock.pBits;
	DWORD *p32Src = ( DWORD*)rcSrcLock.pBits;
	//-----------------------------------------------------
	//DWORD* endPtr = p32Dst + (m_wCanv * (dstH-1) + dstW);
	//int w = 1;
	////while(p32Dst <= endPtr)	
	//for(; p32Dst <= endPtr; )
	//{
	//	DWORD cl = (*p32Src) & clMask;			
	//	if((cl & 0xff000000) != 0) *p32Dst = cl;

	//	p32Dst++;
	//	p32Src++;
	//	w++;
	//	if(w > dstW)
	//	{
	//		w = 1;
	//		p32Dst += stpDst;
	//		p32Src += stpSrc;
	//	}
	//}
	//-----------------------------------------------------
	for( int i = 0; i < dstH; ++i )
	{	
		int j = 0;
		for( ; j < dstW; ++j )
		{			
			DWORD cl = (*p32Src) & clr;			
			if((cl & 0xff000000) != 0) *p32Dst = cl;

			p32Src++;
			p32Dst++;
		}
		p32Src += stpSrc;
		p32Dst += stpDst;
		//p32Src += (sd.Width - j);
		//p32Dst += (m_wCanv - j);
	}
	pSrcTex->UnlockRect(0);
	m_pTex->UnlockRect(0);
}
void Canvas::DrawPattrnLineV(LPDIRECT3DTEXTURE9 pSrcTex, const RECT &rcPattrn, 
						   int x, int y, int l, DWORD clr)
{
	int symH = rcPattrn.right - rcPattrn.left;
	int cntY = l / symH;
	int dy = 0;
	for(int i = 0; i < cntY; ++i)
	{
		DrawRC(pSrcTex, rcPattrn, x, y + dy, clr);
		dy += symH;
	}
}
//void Canvas::DrawString(bool bWrap, RECT *rcBnd, int x, int y, D3DCOLOR cFont, D3DCOLOR cBkg, const char* str)
//{	
//	int w = rcBnd->right - rcBnd->left;	
//	int h = rcBnd->bottom - rcBnd->top;	
//	int dx = 0;
//	int dy = 0;
//
//	const char* end = str + strlen(str);
//	while(str < end)
//	{
//		int dist = 1;
//		if(*str == ' ') dist = (int)(strchr( str + 1, ' ') - str);
//
//		if(dy + m_symH > h) break;
//		if(dx + (m_symW * dist) > w || *str == '\n')
//		{
//			if(!bWrap) break;			
//			dx = 0;
//			dy += (m_symH + m_interStr);			
//		}		
//		else
//		{
//			DrawRC(m_pFont->m_pTex, &m_pFont->m_vCharsTexCoord[/*(unsigned int)*/*str], x + dx, y + dy, cFont);			
//			dx += m_symW;
//		}			
//		str++;		
//	}
//}
//void Canvas::Draw(bool bToClnt, LPDIRECT3DTEXTURE9 pSrcTex, const RECT &rcSrcIn, 
//				  int xWrld, int yWrld, DWORD clMask, DWORD clBkg)
//{	
//	//if(pSrcTex == 0) return;
//	RECT rcDst;
//	RECT rcSrc = rcSrcIn;
//	
//	rcDst.left	 = xWrld + m_TM.x; 
//	rcDst.top	 = yWrld + m_TM.y;
//	rcDst.right  = rcDst.left + (rcSrc.right - rcSrc.left);
//	rcDst.bottom = rcDst.top + (rcSrc.bottom - rcSrc.top);
//
//	RECT* rc = &m_rcCurr;
//	if(bToClnt) rc = &m_rcClnt;
//	//if(!GetSubRect(rc, rcDst, rcSrc)) return;
//	if(rcDst.left < rc->left ||  rcDst.left > rc->right) return;
//	if(rcDst.top < rc->top ||  rcDst.top > rc->bottom) return;
//
//	int dstW, dstH;
//	dstW = rcDst.right - rcDst.left;
//	dstH = rcDst.bottom - rcDst.top;
//	//int stpDst = m_wCanv - dstW;
//
//	D3DSURFACE_DESC	sd;
//	pSrcTex->GetLevelDesc(0, &sd);
//	//int stpSrc = sd.Width - dstW;	
//
//	D3DLOCKED_RECT rcDstLock;
//	m_pTex->LockRect(0, &rcDstLock, &rcDst, 0);
//
//	D3DLOCKED_RECT rcSrcLock;
//	pSrcTex->LockRect(0, &rcSrcLock, &rcSrc, 0);
//
//	DWORD *p32Dst = ( DWORD*)rcDstLock.pBits;
//	DWORD *p32Src = ( DWORD*)rcSrcLock.pBits;
//
//	//DWORD* endPtr = p32Dst + (m_wCanv * (dstH-1) + dstW);
//	//int w = 1;
//
//	//while(p32Dst <= endPtr)	
//	//{			
//	//	//*p32Dst = c;
//	//	DWORD cl = (*p32Src) & clMask;			
//	//	if((cl & 0xff000000) != 0) *p32Dst = cl;
//
//	//	p32Dst++;
//	//	p32Src++;
//	//	w++;
//	//	if(w > dstW)
//	//	{
//	//		w = 1;
//	//		p32Dst += stpDst;
//	//		p32Src += stpSrc;
//	//	}
//	//}
//	for( int i = 0; (i < dstH && i < rc->bottom) ; ++i )
//	{
//		int j = 0;	
//		for( ; (j < dstW && j < rc->right) ; ++j )
//		{			
//			DWORD cl = (*p32Src) & clMask;			
//			if((cl & 0xff000000) != 0) *p32Dst = cl;
//			p32Src++;
//			p32Dst++;
//		}
//		p32Src += (sd.Width - j);
//		p32Dst += (m_wCanv - j);
//	}
//	pSrcTex->UnlockRect(0);
//	m_pTex->UnlockRect(0);
//}
void Canvas::DrawLine(int x, int y, int w, int h, DWORD color)
{
	RECT rc;	
	rc.left	 = x + m_TM.x; 
	rc.top	 = y + m_TM.y;
	rc.right  = rc.left + w;
	rc.bottom = rc.top + h;

	if(!GetClipRect(*m_pClipRect, rc)) return;
	
	int dstW, dstH;
	dstW = rc.right - rc.left;
	dstH = rc.bottom - rc.top;
	int stpDst = m_w - dstW;

	D3DLOCKED_RECT rcDstLock;
	m_pTex->LockRect(0, &rcDstLock, &rc, 0);
	DWORD *p32Dst = ( DWORD*)rcDstLock.pBits;	
	//-----------------------------------------------------
	//DWORD* endPtr = p32Dst + (m_wCanv * (dstH-1) + dstW);
	//int w = 1;
	////while(p32Dst <= endPtr)	
	//for(; p32Dst <= endPtr; )
	//{
	//	DWORD cl = (*p32Src) & clMask;			
	//	if((cl & 0xff000000) != 0) *p32Dst = cl;

	//	p32Dst++;
	//	p32Src++;
	//	w++;
	//	if(w > dstW)
	//	{
	//		w = 1;
	//		p32Dst += stpDst;
	//		p32Src += stpSrc;
	//	}
	//}
	//-----------------------------------------------------
	for( int i = 0; i < dstH; ++i )
	{	
		int j = 0;
		for( ; j < dstW; ++j )
		{			
			*p32Dst = color;
			p32Dst++;
		}		
		p32Dst += stpDst;
		//p32Src += (sd.Width - j);
		//p32Dst += (m_wCanv - j);
	}	
	m_pTex->UnlockRect(0);
}
void Canvas::FastClearAll(DWORD clr)
{
	D3DLOCKED_RECT rcLock;
	m_pTex->LockRect(0, &rcLock, 0, 0);
	Mem_Set_QUAD(( DWORD*)rcLock.pBits, clr/*m_clr*/, m_cntPxls);
	m_pTex->UnlockRect(0);
}
void Canvas::SetClipRC(bool bClient)
{
	m_pClipRect = &m_rcClnt;
	if(!bClient) m_pClipRect = &m_rcCurr;	
}
void Canvas::FillRC(RECT* pRC, DWORD clr)
{	
	int dstW, dstH;	
	RECT rcDst = *pRC;
	RECT rcSrc = rcDst;
	
	rcDst.left	 += m_TM.x; 
	rcDst.top	 += m_TM.y;
	rcDst.right  = rcDst.left + (rcSrc.right - rcSrc.left);
	rcDst.bottom = rcDst.top + (rcSrc.bottom - rcSrc.top);

	/*RECT* rc = &m_rcCurr;
	if(bToClient) rc = &m_rcClnt;*/

	if(!GetSubRect(m_pClipRect, rcDst, rcSrc)) return;
	dstW = rcDst.right - rcDst.left;
	dstH = rcDst.bottom - rcDst.top;	
	int step = m_w - dstW;

	D3DLOCKED_RECT rcLock;
	m_pTex->LockRect(0, &rcLock, &rcDst, 0);
	
	DWORD* p32Dst = ( DWORD*)rcLock.pBits;
	DWORD* endPtr = p32Dst + (m_w * (dstH-1) + dstW);
	int w = 1;
	
	while(p32Dst <= endPtr)	
	{	
		//if(!m_bAlphaDst) // учитыв альфу приемника
		//{
		//	*p32Dst = clr;
		//}
		//else
		//{			
		//	if((*p32Dst & 0xff000000) == 0) *p32Dst = clr;
		//}

		*p32Dst = clr;
		p32Dst++;
		w++;
		if(w > dstW)
		{
			w = 1;
			p32Dst += step;
		}
	}
		
	m_pTex->UnlockRect(0);
}
//void Canvas::ClearRC(RECT* pRC)
//{	
//	int dstW, dstH;	
//	RECT rcDst = *pRC;
//	RECT rcSrc = rcDst;
//	
//	rcDst.left	 += m_TM.x; 
//	rcDst.top	 += m_TM.y;
//	rcDst.right  = rcDst.left + (rcSrc.right - rcSrc.left);
//	rcDst.bottom = rcDst.top + (rcSrc.bottom - rcSrc.top);
//
//	/*RECT* rc = &m_rcCurr;
//	if(bToClient) rc = &m_rcClnt;*/
//
//	if(!GetSubRect(m_pClipRect, rcDst, rcSrc)) return;
//	dstW = rcDst.right - rcDst.left;
//	dstH = rcDst.bottom - rcDst.top;	
//	int step = m_w - dstW;
//
//	D3DLOCKED_RECT rcLock;
//	m_pTex->LockRect(0, &rcLock, &rcDst, 0);
//	
//	DWORD* p32Dst = ( DWORD*)rcLock.pBits;
//	DWORD* endPtr = p32Dst + (m_w * (dstH-1) + dstW);
//	int w = 1;
//	
//	while(p32Dst <= endPtr)	
//	{			
//		*p32Dst = m_clr;
//		p32Dst++;
//		w++;
//		if(w > dstW)
//		{
//			w = 1;
//			p32Dst += step;
//		}
//	}
//		
//	m_pTex->UnlockRect(0);
//}
Canvas::~Canvas(void)
{
	// !!! ничего тут не уничтожать а, то нельзя будет создавать на стеке
	//m_pTex->Release();
	//m_pTex = 0;
}
