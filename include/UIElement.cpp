#include "UIElement.h"
#include "DirectInput.h"
#include "MyHelper.h"
#include "UIPanel.h"
//#include "MyHeader.h"

ID3DXSprite* BaseElmt::m_pSprite = 0;

//void UpdSldrRunner(BaseElmt *bf, int dx, int dy, POINT ptMousePos, float dt)
//{
//	float offset;
//	float val;
//	bool trans = 0;
//	MySlider* p = (MySlider*)bf;
//
//	val = p->m_value;
//	
//	if(p->m_bVertical) { dx = 0; offset = dy; }
//	else { dy = 0; offset = dx; }
//	
//	if(p->m_state & BFS_DRAG) 
//	{
//		offset *= p->m_k;
//		if(p->m_bVertical) p->m_value -= offset;
//		else p->m_value += offset;
//
//		if( p->m_value >= p->m_max) 
//		{ 
//			p->m_value = p->m_max;		
//			p->SetRunerPos(p->m_max); 
//		}
//		if( p->m_value <= p->m_min) { p->m_value = p->m_min;		p->SetRunerPos(p->m_min); }
//	}
//}
ImageButton* CreateImgButton(int x, int y, int w, int h, bool bFixPos, DWORD c,
							 MyBorder* pBrd, int idxBrd, 
							 ComImgRes* pImg, int idxImg,
							 char* text)
{
	ImageButton* pButton = new ImageButton(x, y, w, h, bFixPos, c); 
	if(pBrd) pButton->SetBoard(pBrd, idxBrd);	
	if(pImg) pButton->SetImages(pImg, idxImg);
	if(text) pButton->SetText(text);

	return pButton;
}
//==============================================================================
MyFont::MyFont(void){}
MyFont::MyFont(char *fnSymTbl, int symbW, int symbH, int interStr)
{
	//m_pSprite = pSprite;	
	m_pTex = g_pTexMngr->InsertTex(fnSymTbl, true);
	lstrcpy(m_name, fnSymTbl);
	m_w = symbW;
	m_h = symbH;	
	m_interStr = interStr;

	LoadSymbolTbl(/*fnSymTbl, */m_w, m_h, 0, 256, m_pTex, m_vCharsTexCoord);
}
void MyFont::DrawString(Canvas* pCanv, bool bWrap, RECT *rcBnd, int x, int y, 
							D3DCOLOR cFont, D3DCOLOR cBkg, const char* str)
{	
	// Не забудь уст-ть ClipRC в Canvas до этого вызова
	int w = (rcBnd->right - rcBnd->left) - pCanv->m_TM.x;	
	int h = (rcBnd->bottom - rcBnd->top) - pCanv->m_TM.y;	
	int dx = 0;
	int dy = 0;

	const char* end = str + strlen(str);
	while(str < end)
	{
		int dist = 1;
		if(dy > h) break;
		pCanv->DrawRC(m_pTex, m_vCharsTexCoord[(unsigned char)*str], x + dx, y + dy, cFont);

		if(*str == ' ') dist = (int)(strchr( str + 1, ' ') - str);		
		if((bWrap && dx + (m_w * dist) > w) || (*str == '\n'))
		{
			dx = 0;
			dy += (m_h + m_interStr);
		}
		else dx += m_w;		
		str++;		
	}
}
//void MyFont::AlignString(RECT &rcDst, DWORD fAlign, const char* str, RECT &rcOut)
//{
//	rcOut.left = rcOut.top = 0;
//	rcOut.bottom = m_h;
//	int len = m_w * strlen(str);
//	rcOut.right = len;
//	
//	AlignRC(&rcDst, &rcOut, fAlign);
//}
MyFont::~MyFont(void){}
//==============================================================================
MyBorder::MyBorder(void){}
MyBorder::~MyBorder(void){}
MyBorder::MyBorder(DWORD id, char *fileSymbTbl, int strtIdx, int count, int wSym, int hSym, int brdW)
									: ComImgRes(id, fileSymbTbl, strtIdx, count, wSym, hSym)
{	
	m_brdW = brdW;
	//m_offset = 0;	
}
void MyBorder::Draw(Canvas* pCanv, int offset, DWORD clr)
{	
	pCanv->SetClipRC(0);
	POINT tmp = pCanv->m_TM;
	pCanv->m_TM.x = pCanv->m_TM.y = 0;	// сбрасываем ТМ
	RECT rc = pCanv->m_rcCurr;	

	float tmpF = ((float)(rc.bottom - rc.top) / m_symH) - 2.0f;
	int cntY = ceil(tmpF);
	if(tmpF < 0) cntY = 0;

	//int cntY = (rc.bottom - rc.top) / m_symH;
	int dy = m_symH; //0;
	for(int i = 0; i < cntY; ++i)
	{
		pCanv->DrawRC(m_pTex, m_vRcImgs[offset + 6], rc.left, rc.top + dy, clr);
		pCanv->DrawRC(m_pTex, m_vRcImgs[offset + 7], rc.right - m_symW, rc.top + dy, clr);
		dy += m_symH;
	}

	tmpF = ((float)(rc.right - rc.left) / m_symW) - 2.0f;
	int cntX = ceil(tmpF);
	if(tmpF < 0) cntX = 0;

	//int cntX = (rc.right - rc.left) / m_symW;
	int dx = m_symW; //0;
	for(int c = 0; c < cntX; ++c)
	{
		pCanv->DrawRC(m_pTex, m_vRcImgs[offset + 1], rc.left + dx, rc.top, clr);
		pCanv->DrawRC(m_pTex, m_vRcImgs[offset + 4], rc.left + dx, rc.bottom - m_symH, clr);
		dx += m_symW;
	}
	pCanv->DrawRC(m_pTex, m_vRcImgs[offset + 0], rc.left, rc.top, clr);
	pCanv->DrawRC(m_pTex, m_vRcImgs[offset + 2], rc.right - m_symW, rc.top, clr);
	pCanv->DrawRC(m_pTex, m_vRcImgs[offset + 5], rc.right - m_symW, rc.bottom - m_symH, clr);
	pCanv->DrawRC(m_pTex, m_vRcImgs[offset + 3], rc.left, rc.bottom - m_symH, clr);

	pCanv->m_TM = tmp;	// восстанавливаем ТМ
}
//===========================================================================
BaseElmt::BaseElmt(void){}
BaseElmt::BaseElmt(int x, int y, int w, int h, bool bFixSize, int bFixW, int bFixH, D3DCOLOR clr)
{
	m_align		= MAKEDWORD(ALGN_LEFT, ALGN_TOP);
	m_state		= 0;
	m_w			= w;
	m_h			= h;	
	m_clr		= clr;
	m_pParent	= 0;
	m_anyData	= 0;
	m_tabs		= 0;

	strcpy(m_name, "BaseElmt");	
	if(bFixSize) SET_BIT(m_state, BFS_FIXSIZE);
	SET_BIT(m_state, BFS_ALIGN_TO_CLIENT);
	SET_BIT(m_state, BFS_CLIENT_CLIP); // отсекать прямоуг-ом клиена
	SET_BIT(m_state, BFS_VISIBLE);	
		
	int canvW = w;
	int canvH = h;
	if(!bFixSize)
	{
		if(!bFixW) canvW = g_UIPanel->m_wScr;
		if(!bFixH) canvH = g_UIPanel->m_hScr;
	}	
	m_pCanv = new Canvas(canvW, canvH);
	//m_pCanv->m_pParent = this; // !!!

	m_rc.left = x; 
	m_rc.top = y; 
	m_rc.right = x + w; 
	m_rc.bottom = y + h;
	Resize(m_rc);

	SET_BIT(m_state, BFS_REDRAW);
	SET_BIT(m_state, BFS_DRAWTOPRNT);
}
void BaseElmt::PrintName(char* str)
{
	for(int i = 0; i < m_tabs; ++i)
	{
		Write_Log_String(hLog, " ");
	}
	Write_Log_String(hLog, "%s: %s %f\n", m_name, str, g_time);
}
void BaseElmt::Update(int dx, int dy, POINT ptMousePos, float dt) 
{
	//if(m_pParent) m_tabs = m_pParent->m_tabs + 3; //!!!
	//PrintName("-> Update");
}
//void BaseElmt::UpdateHit(DWORD distate, POINT ptMousePos)
//{
//	if(PtInRect(&m_rc, ptMousePos)) SET_BIT(m_state, BFS_HIT);
//	else RESET_BIT(m_state, BFS_HIT);	
//	
//	if((m_pParent->m_state & BFS_FOCUS) && (distate & DI_LMBCLK) && (m_state & BFS_HIT))
//	{			
//		SET_BIT(m_state, BFS_FOCUS);
//	}		
//	if((distate & DI_LMBCLK) && !(m_state & BFS_HIT)) RESET_BIT(m_state, BFS_FOCUS);		
//	if((m_state & BFS_FOCUS) && (distate & DI_LMBDWN)) 
//	{
//		SET_BIT(m_state, BFS_DRAG);
//		RESET_BIT(m_pParent->m_state, BFS_DRAG);			
//	}
//	else RESET_BIT(m_state, BFS_DRAG);
//}
void BaseElmt::Resize(RECT &rc)
{
	m_rc = rc;
	m_w = rc.right - rc.left;
	m_h = rc.bottom - rc.top;
	m_pCanv->Resize(m_rc);

	SET_BIT(m_state, BFS_REDRAW);
}
void BaseElmt::UpdateCanv(void)
{
	m_pCanv->FastClearAll(m_clr);

	/*RESET_BIT(m_state, BFS_REDRAW);
	RESET_BIT(m_state, BFS_DRAWTOPRNT);*/
}
void BaseElmt::SetVisible(bool bVisible)
{
	if(bVisible) SET_BIT(m_state, BFS_VISIBLE);
	else RESET_BIT(m_state, BFS_VISIBLE);
	if(m_pParent) SET_BIT(m_pParent->m_state, BFS_REDRAW);
}
void BaseElmt::Draw()
{
	DWORD color = 0xffffffff;
	//if(m_state & BFS_DISABLED) 
	//	color = SYSCLR_DISABLED;

	if(m_state & BFS_VISIBLE)
	{
	if(m_pParent) 
	{
		/*POINT tmp = m_pParent->m_pCanv->m_TM;
		if(!(m_state & BFS_USE_PARNT_TM))
		{
			m_pParent->m_pCanv->m_TM.x = 0;
			m_pParent->m_pCanv->m_TM.y = 0;
		}*/
		//m_tabs = m_pParent->m_tabs + 3; //!!!
		m_pParent->m_pCanv->SetClipRC(m_state & BFS_CLIENT_CLIP);
		m_pParent->m_pCanv->DrawRC(m_pCanv->m_pTex, m_pCanv->m_rcCurr, m_rc.left, m_rc.top, color);
	
		//D3DXSaveTextureToFile("Canv.bmp", D3DXIFF_BMP, m_pCanv->m_pTex, 0);
		//m_pParent->m_pCanv->m_TM = tmp;
	}
	else m_pSprite->Draw(m_pCanv->m_pTex, &m_pCanv->m_rcCurr, 0, 
							&D3DXVECTOR3((float)m_rc.left, (float)m_rc.top, 0.0f),
							0xFFFFFFFF);
	}
			
	RESET_BIT(m_state, BFS_REDRAW);
	RESET_BIT(m_state, BFS_DRAWTOPRNT);
}
//void BaseElmt::AlignElmt(RECT* pDstRC/*, DWORD fAlign*/)
//{
//	int wSrc = m_rc.right - m_rc.left;
//	int hSrc = m_rc.bottom - m_rc.top;
//	int wDst = pDstRC->right - pDstRC->left;
//	int hDst = pDstRC->bottom - pDstRC->top;
//
//	if(HIWORD(m_align) == ALGN_LEFT)	m_rc.left += m_offsetX;
//	if(LOWORD(m_align) == ALGN_TOP)		m_rc.top  += m_offsetY;
//	if(HIWORD(m_align) == ALGN_RIGHT)	m_rc.left = pDstRC->right - wSrc - m_offsetX;
//	if(LOWORD(m_align) == ALGN_BOTTOM)	m_rc.top  = pDstRC->bottom - hSrc - m_offsetY;
//	if(HIWORD(m_align) == ALGN_MH)		m_rc.left = pDstRC->left + ((wDst - wSrc) >> 1);
//	if(LOWORD(m_align) == ALGN_MV)		m_rc.top  = pDstRC->top + ((hDst - hSrc) >> 1);
//
//	m_rc.right = m_rc.left + wSrc;
//	m_rc.bottom = m_rc.top + hSrc;
//
//	if(HIWORD(m_align) == ALGN_FITH) { m_rc.left = pDstRC->left; m_rc.right = pDstRC->right; }
//	if(LOWORD(m_align) == ALGN_FITV) { m_rc.top = pDstRC->top; m_rc.bottom = pDstRC->bottom; }
//	//!!!SET_BIT(m_state, BFS_REDRAW);
//}
void BaseElmt::AlignElmt(/*RECT* pDstRC*/)
{
	RECT dstRC = m_pParent->m_pCanv->m_rcCurr;
	if(m_state & BFS_ALIGN_TO_CLIENT) dstRC = m_pParent->m_pCanv->m_rcClnt;
	
	//if(HIWORD(m_align) == ALGN_LEFT)	m_rc.left += m_offsetX;
	//if(LOWORD(m_align) == ALGN_TOP)		m_rc.top  += m_offsetY;
	if(HIWORD(m_align) == ALGN_RIGHT)	m_rc.left = dstRC.right - m_w;
	if(LOWORD(m_align) == ALGN_BOTTOM)	m_rc.top  = dstRC.bottom - m_h;
	if(HIWORD(m_align) == ALGN_MH)		m_rc.left = dstRC.left + (((dstRC.right - dstRC.left) - m_w) >> 1);
	if(LOWORD(m_align) == ALGN_MV)		m_rc.top  = dstRC.top + (((dstRC.bottom - dstRC.top) - m_h) >> 1);

	m_rc.right = m_rc.left + m_w;
	m_rc.bottom = m_rc.top + m_h;

	if(HIWORD(m_align) == ALGN_FITH) 
	{ 
		m_rc.left = dstRC.left; 
		m_rc.right = dstRC.right;
		Resize(m_rc);
	}
	if(LOWORD(m_align) == ALGN_FITV) 
	{ 
		m_rc.top = dstRC.top; 
		m_rc.bottom = dstRC.bottom; 
		Resize(m_rc);
	}
	//Resize(m_rc); // !!!
}
void BaseElmt::SetPos(int x, int y)
{
	m_rc.left = x;
	m_rc.top = y;
	m_rc.right = m_rc.left + m_w;
	m_rc.bottom = m_rc.top + m_h;
}
BaseElmt::~BaseElmt(void){ delete m_pCanv; }
//===========================================================================
RichElmt::RichElmt(void){}
RichElmt::RichElmt(int x, int y, int w, int h, bool bFixPos, bool bFixSize, int bFixW, int bFixH, D3DCOLOR clr)
					:BaseElmt(x, y, w, h, bFixSize, bFixW, bFixH, clr)
{	
	m_pfUserUpd	= 0;		
	m_idxBrd	= 0;
	m_pBrd		= 0;

	if(bFixPos)  m_state |= BFS_FIXPOS;
}
void RichElmt::UpdateSize(int dx, int dy, POINT ptMousePos, float dt)
{
	int  brdW = 0;
	if(m_pBrd) brdW = m_pBrd->m_brdW;
	DWORD dwDIStat = gDInput->m_dwDIState;
	
	if(dwDIStat & DI_LMBCLK)
	{
		if(ptMousePos.x >= m_rc.left && ptMousePos.x <= m_rc.left + brdW)		SET_BIT(m_state, BFS_HITLBRD);
		if(ptMousePos.x <= m_rc.right && ptMousePos.x >= m_rc.right - brdW)		SET_BIT(m_state, BFS_HITRBRD);
		if(ptMousePos.y >= m_rc.top && ptMousePos.y <= m_rc.top + brdW)			SET_BIT(m_state, BFS_HITTBRD);
		if(ptMousePos.y <= m_rc.bottom && ptMousePos.y >= m_rc.bottom - brdW)	SET_BIT(m_state, BFS_HITBBRD);

		if((m_state & BFS_HITLBRD) | (m_state & BFS_HITRBRD) |
			(m_state & BFS_HITTBRD)| (m_state & BFS_HITBBRD)) SET_BIT(m_state, BFS_RESIZE);
	}
	if(!((dwDIStat & DI_LMBDWN) && (m_state & BFS_RESIZE))) RESET_BIT(m_state, BFS_RESIZE);
	if((m_state & BFS_RESIZE) /*&& (dx != 0 || dy != 0)*/)
	{
		RESET_BIT(m_state, BFS_DRAG);
		//SET_BIT(m_state, BFS_CLEAR); // !!!
		SET_BIT(m_state, BFS_REDRAW);

		RECT rc = m_rc;		
		if(m_state & BFS_HITLBRD) rc.left += dx;
		if(m_state & BFS_HITRBRD) rc.right += dx;
		if(m_state & BFS_HITTBRD) rc.top += dy;
		if(m_state & BFS_HITBBRD) rc.bottom += dy;
		
		Resize(rc);		
	}
	else 
	{
		RESET_BIT(m_state, BFS_HITLBRD);	RESET_BIT(m_state, BFS_HITRBRD);
		RESET_BIT(m_state, BFS_HITTBRD);	RESET_BIT(m_state, BFS_HITBBRD);
	}
}
void RichElmt::Update(int dx, int dy, POINT ptMousePos, float dt) 
{
	/*RESET_BIT(m_state, BFS_CLEAR);
	RESET_BIT(m_state, BFS_REDRAW);

	if(m_state & BFS_INIT) 
	{
		SET_BIT(m_state, BFS_CLEAR);
		SET_BIT(m_state, BFS_REDRAW);
	}*/
	BaseElmt::Update(dx, dy, ptMousePos, dt); 

	if((m_state & BFS_FOCUS) && !(m_state & BFS_FIXSIZE)) UpdateSize(dx, dy, ptMousePos, dt);
	//if(m_pfUserUpd) m_pfUserUpd(this, dx, dy, ptMousePos, dt);	
	if(!(m_state & BFS_FIXPOS) && (m_state & BFS_DRAG))	TranslateRC(dx, dy, m_rc);

	
}
//void RichElmt::UpdateHit(DWORD distate, POINT ptMousePos)
//{
//	if(PtInRect(&m_rc, ptMousePos)) SET_BIT(m_state, BFS_HIT);
//	else RESET_BIT(m_state, BFS_HIT);
//	
//	if(distate & DI_LMBCLK)
//	{
//		if(m_state & BFS_HIT) SET_BIT(m_state, BFS_FOCUS);
//		else RESET_BIT(m_state, BFS_FOCUS);
//	}
//		
//	if((m_state & BFS_FOCUS) && (distate & DI_LMBDWN)/* && (dx != 0 || dy != 0)*/) 
//	{
//		SET_BIT(m_state, BFS_DRAG);
//	}
//	else RESET_BIT(m_state, BFS_DRAG);
//}
//void RichElmt::Resize(RECT &rc)
//{
//	//Write_Log_String(hLog, "RichElmt::Resize\n");
//	BaseElmt::Resize(rc);
//	if(m_pBrd)
//	{
//		int w = m_pBrd->m_brdW;
//		m_pCanv->m_rcClnt.left = w;
//		m_pCanv->m_rcClnt.top = w;
//		m_pCanv->m_rcClnt.right = m_w - w;
//		m_pCanv->m_rcClnt.bottom = m_h - w;
//	}
//}
void RichElmt::SetBoard(MyBorder* pBrd, int idx) 
{
	int w = pBrd->m_brdW;
	m_pBrd = pBrd;
	m_idxBrd = idx;

	m_pCanv->m_offsL += w;	
	m_pCanv->m_offsU += w;	
	m_pCanv->m_offsR += w;	
	m_pCanv->m_offsD += w;

	Resize(m_rc);
}
void RichElmt::UpdateCanv(void)
{
	if((m_state & BFS_REDRAW/*BFS_CLEAR*/)/* || (m_state & BFS_INIT)*/)
	{
		//m_pCanv->SetClipRC(0); //!!!
		m_pCanv->FastClearAll(m_clr);
		if(m_pBrd) m_pBrd->Draw(m_pCanv, m_idxBrd, 0xffffffff);
		//PrintName("UpdateCanv->BFS_REDRAW");
	}
}
RichElmt::~RichElmt(void){}
//=========================================================================
ElmtContnr::ElmtContnr(void) {}
ElmtContnr::ElmtContnr(int x, int y, int w, int h, bool bFixPos, bool bFixSize, int bFixW, int bFixH, D3DCOLOR clr)
			: RichElmt(x, y, w, h, bFixPos, bFixSize, bFixW, bFixH, clr) 
{
	//m_xScale = m_yScale = 0;
	m_bSort = 1;
	m_maxChild = 32;
	m_numChild = 0;
	//m_bClear = 1;
	Mem_Set_QUAD(m_vChilds, 0, m_maxChild);
	//Mem_Set_QUAD(&m_rcAllChilds, 0, 4);
	strcpy(m_name, "ElmtContnr");
	//m_pVScroll = 0;
	//m_pHScroll = 0;
	//SET_BIT(m_state, BFS_RESIZE);
}
void ElmtContnr::AddScroll(BaseElmt* pScroll)
{
	MyVScroll* p = dynamic_cast<MyVScroll*>(pScroll);
	if(p)
	{
		p->m_pParent = this;
		//m_pVScroll = p;
	}
}
void ElmtContnr::UpdateChild(int dx, int dy, 
							 DWORD dwDIStat, POINT ptMousePos, 
							 float dt, BaseElmt* p, int &idxFnd)
{
	if(m_state & BFS_REDRAW /*BFS_RESIZE*/) p->AlignElmt();
	if(!(m_state & BFS_DISABLED))
	{

	if(PtInRect(&p->m_rc, ptMousePos)) SET_BIT(p->m_state, BFS_HIT);
	else RESET_BIT(p->m_state, BFS_HIT);
	
	if((m_state & BFS_FOCUS) && (dwDIStat & DI_LMBCLK) && !idxFnd && (p->m_state & BFS_HIT))
	{			
		SET_BIT(p->m_state, BFS_FOCUS);
		idxFnd = 1;			
	}		
	if((dwDIStat & DI_LMBCLK) && !(p->m_state & BFS_HIT)) RESET_BIT(p->m_state, BFS_FOCUS);		
	if(!(p->m_state & BFS_FIXPOS) && (p->m_state & BFS_FOCUS) && (dwDIStat & DI_LMBDWN) && (dx != 0 || dy != 0)) 
	{
		SET_BIT(p->m_state, BFS_DRAG);
		RESET_BIT(m_state, BFS_DRAG);
	}
	else RESET_BIT(p->m_state, BFS_DRAG);

	//if(m_state & BFS_REDRAW /*BFS_RESIZE*/) p->AlignElmt();

	/*if(!(m_state & BFS_DISABLED)) */p->Update(dx, dy, ptMousePos, dt);

	if(p->m_state & BFS_REDRAW) SET_BIT(m_state, BFS_DRAWTOPRNT);
	if(p->m_state & BFS_DRAG || p->m_state & BFS_RESIZE) SET_BIT(m_state, BFS_REDRAW);

	}
}
void ElmtContnr::Update(int dx, int dy, POINT ptMousePos, float dt)
{
	
	BaseElmt* p = 0;
	DWORD dwDIStat = gDInput->m_dwDIState;
	Mem_Set_QUAD(&m_rcAllChilds, 0, 4);	

	POINT ptMPosLoc;
	ptMPosLoc.x = (ptMousePos.x - m_rc.left)- m_pCanv->m_TM.x;
	ptMPosLoc.y = (ptMousePos.y - m_rc.top) - m_pCanv->m_TM.y;

	RichElmt::Update(dx, dy, ptMousePos, dt);
	//if(m_pfUserUpd) m_pfUserUpd(this, dx, dy, ptMousePos, dt);
	/*if((m_state & BFS_FOCUS) && (dwDIStat & DI_MMBDWN))
	{
		m_pCanv->m_TM.x += dx;
		m_pCanv->m_TM.y += dy;	
		m_bClear = 1;
	}*/

	int idxFnd = -1;
	int bFind = 0;
	for(int i = 0; i < m_numChild; ++i)
	{
		p = m_vChilds[i];

		//MyVScroll* ptr = dynamic_cast<MyVScroll*>(p);
		//if(!ptr) 
		//{
			m_rcAllChilds.left		= min(m_rcAllChilds.left, p->m_rc.left);
			m_rcAllChilds.right		= max(m_rcAllChilds.right, p->m_rc.right);
			m_rcAllChilds.top		= min(m_rcAllChilds.top, p->m_rc.top);
			m_rcAllChilds.bottom	= max(m_rcAllChilds.bottom, p->m_rc.bottom);
		//}
		
		/*if(p->m_state & BFS_USE_PARNT_TM)
		{
			ptMPosLoc.x -= m_pCanv->m_TM.x;
			ptMPosLoc.y -= m_pCanv->m_TM.y;
		}*/
		UpdateChild(dx, dy, dwDIStat, ptMPosLoc, dt, p, bFind);
		if(bFind && (idxFnd == -1)) idxFnd = i;
		
		p = 0;	
	}
	// это первое исп VSS!!!
	// это второе
	if(m_bSort && (idxFnd > 0))
	{
		p = m_vChilds[idxFnd];
		m_vChilds[idxFnd] = m_vChilds[0];
		m_vChilds[0] = p; // перемещ. в начало массива
		SET_BIT(m_state, BFS_REDRAW);
	}	

	//RichElmt::Update(dx, dy, ptMousePos, dt);	
	idxFnd = 0; // сбрасываем иначе никогда не получит фокус
	
}
void ElmtContnr::UpdateCanv()
{
	BaseElmt* p = 0;

	RichElmt::UpdateCanv();	
	for(int i = m_numChild; i != 0; --i)
	{
		p = m_vChilds[i-1];
		p->UpdateCanv(); 
		if(m_state & BFS_REDRAW || p->m_state & BFS_REDRAW)
		{
			if(p->m_state & BFS_VISIBLE)
			{
				p->Draw();				
			}
			SET_BIT(m_state,BFS_DRAWTOPRNT);
		}
		p = 0;
	}
	if(/*(m_state & BFS_VISIBLE) && */(m_state & BFS_DRAWTOPRNT)) Draw();
}
int  ElmtContnr::AddElement(BaseElmt *el)
{
	if(m_numChild < m_maxChild)
	{
		el->m_pParent = this;
		el->m_tabs = m_tabs + 3; //!!!
		m_vChilds[m_numChild] = el;	
		m_numChild++;

		MyVScroll* ptr = dynamic_cast<MyVScroll*>(el);
		if(ptr) 
		{
			m_pCanv->m_offsR += ptr->m_w;
			Resize(m_rc); // чтобы обновился размер клиентской области
		}

		return 1;
	}
	else return 0;
}
ElmtContnr::~ElmtContnr(void)
{
	BaseElmt* p = 0;
	for(int i = 0; i < m_numChild; ++i) { p = m_vChilds[i];	delete p;	p = 0; }
}
//=========================================================================
MyWindow::MyWindow(void) {}
MyWindow::MyWindow(int x, int y, int w, int h, bool bFixPos, bool bFixSize, int bFixW, int bFixH, D3DCOLOR clr)
			: ElmtContnr(x, y, w, h, bFixPos, bFixSize, bFixW, bFixH, clr) 
{	
	//m_bSort = 1;
	m_pHdr = 0;
	/*m_pVScroll = 0;
	m_pHScroll = 0;	*/
}
int  MyWindow::AddElement(BaseElmt *el)
{
	//MyHeader* pHdr = dynamic_cast<MyHeader*>(el);
	//if(pHdr) m_pHdr = pHdr;

	return ElmtContnr::AddElement(el);
}
MyWindow::~MyWindow(void)
{
	//BaseElmt* p = 0;
	//for(int i = 0; i < m_numChild; ++i) { p = m_vChilds[i];	delete p;	p = 0; }
}
//=========================================================================
MyLabel::MyLabel(void){}
// RichElmt(int x, int y, int w, int h, bool bFixPos, bool bFixSize, int bFixW, int bFixH, D3DCOLOR clr)
MyLabel::MyLabel(int x, int y, int w, int h, D3DCOLOR cBkg, D3DCOLOR cFont) 
					//: BaseElmt(x, y, w, h, 1, 1, 1, cBkg)
					: RichElmt(x, y, w, h, 1, 1, 1, 1, cBkg) //!!! bFixPos
{
	m_cFont = cFont;
	m_pFont = g_UIPanel->GetFontPtr(0);	
	AlignString(MAKEDWORD(ALGN_LEFT, ALGN_BOTTOM));

	strcpy(m_name, "MyLabel");
}
void MyLabel::AlignString(DWORD flgAlign)
{
	RECT rc;
	m_dTxtAlign = flgAlign;	
	
	rc.left = rc.top = 0;
	rc.bottom = m_pFont->m_h;
	int len = m_pFont->m_w * strlen(str.c_str());
	rc.right = len;
	
	AlignRC(&m_pCanv->m_rcClnt, &rc, flgAlign);
	m_pCanv->m_TM.x = rc.left;
	m_pCanv->m_TM.y = rc.top;
	SET_BIT(m_state, BFS_REDRAW);
}
void MyLabel::Update(int dx, int dy, POINT ptMousePos, float dt) 
{
	if(m_pfUserUpd) m_pfUserUpd(this, dx, dy, ptMousePos, dt);
}
void MyLabel::UpdateCanv(void)
{
	/*BaseElmt*/ RichElmt::UpdateCanv();
	if(m_state & BFS_REDRAW)
	{
		m_pCanv->SetClipRC(1); // если не рамки, то будет браться текущ прям-к.
		m_pFont->DrawString(m_pCanv, 0, &m_pCanv->m_rcCurr, 0, 0, m_cFont, m_clr, str.c_str());
		//PrintName("UpdateCanv->BFS_REDRAW");
	}
}
void MyLabel::SetTextColor(D3DCOLOR c) {	m_clr = c; }
void MyLabel::SetText(const char *txt)
{	
	//if(str != txt) // !!!
	//{
		str = txt;
		AlignString(m_dTxtAlign);
		SET_BIT(m_state, BFS_REDRAW);
	//}
}
MyLabel::~MyLabel(void){}
//====================================================================
TextButton::TextButton(void){}
TextButton::TextButton(int x, int y, int w, int h, 
					   bool bFixPos, bool bFixSize, int bFixW, int bFixH, 
					   D3DCOLOR clr) 
					   : RichElmt(x, y, w, h, bFixPos, bFixSize, bFixW, bFixH, clr) 
{
	Init();
}
TextButton::TextButton(int x, int y, int w, int h, bool bFixPos, D3DCOLOR clr)
						: RichElmt(x, y, w, h, bFixPos, 1, 1, 1, clr)
{
	Init();
}
void TextButton::Init()
{
	m_bttnState = 0;
	m_idxBrdUp = 0;
	m_idxBrdDwn = m_idxBrdUp + 8;

	m_pCaption = new MyLabel(0, 0, m_w, m_h, m_clr, 0xff000000);
	m_pCaption->m_pParent = this;
	m_pCaption->m_dTxtAlign = MAKEDWORD(ALGN_MH, ALGN_MV);

	//SET_BIT(m_state, BFS_DISABLED);

	strcpy(m_name, "TextButton");
}
void TextButton::SetText(const char *txt)
{
	m_pCaption->SetText(txt);
	//SET_BIT(m_state, BFS_REDRAW);
}
void TextButton::Update(int dx, int dy, POINT ptMousePos, float dt)
{
	RESET_BIT(m_bttnState, BTN_UP);
	DWORD dwDIStat = gDInput->m_dwDIState;

	if(m_state & BFS_FOCUS)
	{	
		if((m_state & BFS_HIT) && (dwDIStat & DI_LMBCLK)) 
		{
			//SET_BIT(m_bttnState, BTN_DWN);
			m_bttnState = BTN_DWN;
			//m_idxBrdUp = m_idxBrd;
			m_idxBrd = m_idxBrdDwn;
		}

		if((m_state & BFS_HIT) && (m_bttnState & BTN_DWN)/*m_bBtnDwn*/ && !(dwDIStat & DI_LMBDWN)) 
		{
			if(m_pfUserUpd) m_pfUserUpd(this, dx, dy, ptMousePos, dt);
			//SET_BIT(m_bttnState, BTN_UP);
			//RESET_BIT(m_bttnState, BTN_DWN);
			//m_bBtnDwn = 0;
			m_idxBrd = m_idxBrdUp;
			m_bttnState = BTN_UP;
		}
		if((m_state & BFS_HIT) && (m_bttnState & BTN_DWN)) m_idxBrd = m_idxBrdDwn;
		else { m_idxBrd = m_idxBrdUp;/* m_bBtnDwn = 0;*/ }

		if(m_oldIdx != m_idxBrd) 
		{
			SET_BIT(m_state, BFS_REDRAW);
			m_oldIdx = m_idxBrd;
		}
	}
	RichElmt::Update(dx, dy, ptMousePos, dt);
	
}
void TextButton::UpdateCanv(void)
{
	RichElmt::UpdateCanv();			
	m_pCaption->UpdateCanv();
	
	if(m_state & BFS_REDRAW)
	{
	m_pCanv->SetClipRC(1);
	m_pCaption->Draw();
	//D3DXSaveTextureToFile("Canv.bmp", D3DXIFF_BMP, m_pCanv->m_pTex, 0);
	//PrintName("UpdateCanv->BFS_REDRAW");
	}
}
TextButton::~TextButton(void) {}
//=====================================================================
ImageButton::ImageButton(void){}
ImageButton::ImageButton(int x, int y, int w, int h, 
					   bool bFixPos, bool bFixSize, int bFixW, int bFixH, 
					   D3DCOLOR clr) 
						: TextButton(x, y, w, h, bFixPos, bFixSize, bFixW, bFixH, clr)
{
	Init();
}
ImageButton::ImageButton(int x, int y, int w, int h, bool bFixPos, D3DCOLOR clr) 
						: TextButton(x, y, w, h, bFixPos, 1, 1, 1, clr)
{
	Init();
}
void ImageButton::Init()
{
	m_idxImgOld = 0;
	m_idxImgUp  = 0;
	m_idxImgDwn = 0;
	m_idxImgOut = 0;
	m_pImgs		= 0;

	strcpy(m_name, "ImageButton");
}
void ImageButton::Update(int dx, int dy, POINT ptMousePos, float dt)
{
	TextButton::Update(dx, dy, ptMousePos, dt);

	if((m_state & BFS_HIT) && (m_bttnState & BTN_DWN)) 
		m_idxImgOut = m_idxImgDwn;
	else m_idxImgOut = m_idxImgUp;

	if(m_idxImgOut != m_idxImgOld) 
	{
		SET_BIT(m_state, BFS_REDRAW);
		m_idxImgOld = m_idxImgOut;
	}
}
void ImageButton::UpdateCanv(void)
{
	TextButton::UpdateCanv();
	if(m_state & BFS_REDRAW)
	{
	if(m_pImgs) m_pImgs->Draw(m_pCanv, m_idxImgOut);
	//PrintName("UpdateCanv->BFS_REDRAW");
	}
	
}
ImageButton::~ImageButton(void) {}
void ImageButton::SetImages(ComImgRes* pImgBtn, int strtIdx)
{
	m_pImgs = pImgBtn;
	m_idxImgUp = strtIdx;
	m_idxImgOut = m_idxImgUp;
	m_idxImgDwn = m_idxImgUp + 1; // ????

	RECT rc = m_pImgs->m_vRcImgs[0];
	AlignRC(&m_pCanv->m_rcClnt, &rc, MAKEDWORD(ALGN_MH, ALGN_MV));
	m_pCanv->m_TM.x = rc.left;
	m_pCanv->m_TM.y = rc.top;
}
//=====================================================================
//MyEditCtrl::MyEditCtrl(void) {}
//MyEditCtrl::MyEditCtrl(int x, int y, int w, int h, D3DCOLOR cBkg, D3DCOLOR cFont)
//									: MyLabel(x, y, w, h, cBkg, cFont)
//{		
//	m_bEditable		= 1;
//	m_bSel			= 0;
//	m_bDblClik		= 0;
//	m_iPosInStr		= 0;
//	m_bCtrlDwn		= 0;
//	m_iEndPosSel	= 0;
//	m_idxStartChDraw = 0;
//	m_idxStartSelChDraw = 0;
//	
//	//m_bButtUp		= 0;
//	//m_idxRow		= 0;
//	m_idxCol		= 0;
//	m_carretPosX	= 0.0f;
//	//m_carretPosY	= 0.0f;
//	//m_iNumRows = (m_rcHit.bottom - m_rcHit.top) / (m_pFont->symH + m_pFont->m_interStr);
//	m_iNumCols = (m_rc.right - m_rc.left) / m_pFont->m_w;
//	//SetTxtAlign(TXTALGN_LEFT | TXTALGN_TOP);
//	//m_pSelFont = g_UIPanel->GetFontPtr("CourierNew7x10_fill.bmp");
//}
//void MyEditCtrl::MoveNext()
//{
//	//int size = str.size();	
//	/*m_idxCol++;
//	m_iPosInStr++;
//
//	if(m_idxCol > m_iNumCols)
//	{				
//		m_idxCol--;
//		m_pCanv->m_TM.x -= m_pFont->m_w;
//	}*/
//	int offs1 = m_iNumCols * 0.5f;
//	int size = str.size();	
//	m_iPosInStr++;
//	m_idxCol++;
//	
//	if(m_idxCol > m_iNumCols)
//	{
//		if(m_iPosInStr <= size) m_pCanv->m_TM.x -= m_pFont->m_w;
//		m_idxCol--;
//		/*int offs2 = size - (m_idxStartChDraw + m_iNumCols);
//		if( offs2 < offs1) offs1 = offs2;
//		m_idxCol -= offs1;
//		m_idxStartChDraw += offs1;
//		return;*/
//	}
//
//	if(m_iPosInStr > size)
//	{
//		//m_iPosInStr = size;
//		m_iPosInStr--;	
//		//m_idxCol--;		
//		//return;
//	}
//
//	
//	SET_BIT(m_state, BFS_REDRAW);
//}
//int MyEditCtrl::MovePrev()
//{	
//	int offs1 = m_iNumCols * 0.5f;	
//	m_iPosInStr--;
//	m_idxCol--;
//	int res = m_iPosInStr;
//
//	if(m_idxCol < 0)
//	{
//		m_idxCol++;
//		if(m_iPosInStr >= 0) m_pCanv->m_TM.x += m_pFont->m_w;
//		//if( m_iPosInStr < offs1) offs1 = m_iPosInStr;
//		//m_idxCol += offs1;
//		//m_idxStartChDraw -= offs1;
//		//return 1;
//	}
//
//	if(m_iPosInStr <= 0)
//	{
//		m_iPosInStr = m_idxCol = m_idxStartChDraw = 0;
//		//return 0;
//	}
//	
//	SET_BIT(m_state, BFS_REDRAW);
//	return res;
//}
//////void MyEditCtrl::SetCarret(POINT ptMPos)
//////{
//////	int col = ((float)(ptMPos.x-m_rcHit.left)/m_pFont->symW)+0.5f;
//////	MoveTo(m_idxStartChDraw + col);	
//////}
//////void MyEditCtrl::SelWord(void)
//////{
//////	int d2 = str.size();
//////	for(int i = m_iPosInStr; i < d2; ++i)
//////	{
//////		if(str[i] == 0x20) // пробел
//////		{
//////			d2 = i/*-1*/;
//////			break;
//////		}
//////	}
//////	m_iEndPosSel = d2;		
//////
//////	int d1 = 0;
//////	for(int j = m_iPosInStr; j >= d1; --j) // after^^^!!
//////	{
//////		if(str[j] == 0x20) // пробел
//////		{
//////			d1 = j+1;
//////			break;
//////		}
//////	}
//////
//////	MoveTo(d1);
//////	//m_sSelStr = str.substr(d1, m_iEndPosSel - d1);	
//////}
//////int MyEditCtrl::MoveTo(int pos)
//////{	
//////	int res = 1;
//////	int size = str.size();
//////	int dir = pos - m_iPosInStr;
//////	int cnt = abs(dir);
//////	for(; cnt != 0; --cnt)
//////	{
//////		if(dir > 0) MoveNext();
//////		if(dir < 0) res = MovePrev();
//////	}
//////	return res;
//////}
//////void MyEditCtrl::UpdadeSelView(void)
//////{	
//////	if(m_iPosInStr == m_iEndPosSel)
//////	{
//////		m_bSel = 0;
//////		return;
//////	}
//////	else m_bSel = 1;
//////
//////	int idxLeftSel = min(m_iPosInStr, m_iEndPosSel);	
//////	if(m_idxStartChDraw > idxLeftSel)
//////	{
//////		m_rcSelTxt.left = m_rcHit.left;
//////		m_idxStartSelChDraw = m_idxStartChDraw - idxLeftSel;
//////		m_xTxtSel = m_xTxt;
//////	}
//////	else 
//////	{
//////		m_rcSelTxt.left = (m_rcHit.left + ((idxLeftSel - m_idxStartChDraw) * m_pFont->symW));
//////		m_idxStartSelChDraw = 0;
//////		m_xTxtSel = m_rcSelTxt.left;
//////	}
//////
//////	int idxRightSel = max(m_iPosInStr, m_iEndPosSel);
//////	if(idxRightSel >= m_idxStartChDraw + m_iNumCols) m_rcSelTxt.right = m_rcHit.right;
//////	else m_rcSelTxt.right = (m_rcHit.left + ((idxRightSel - m_idxStartChDraw) * m_pFont->symW));
//////
//////	m_rcSelTxt.top = m_yTxt;
//////	m_rcSelTxt.bottom = m_rcSelTxt.top + m_pFont->symH;
//////
//////	m_sSelStr = str.substr(idxLeftSel, idxRightSel - idxLeftSel);
//////}
//////void MyEditCtrl::DelRange(int pos, int range)
//////{
//////	for(; range != 0; --range)
//////	{					
//////		str.erase(pos, 1);
//////		if(str.size() < m_idxStartChDraw + m_iNumCols)
//////		{
//////			if(m_idxStartChDraw - 1 >= 0)
//////			{
//////				m_idxStartChDraw--;
//////				m_idxCol++;
//////			}
//////		}
//////	}
//////}
//////void MyEditCtrl::OnGetFocus(POINT ptMousePos)
//////{
//////	SetCarret(ptMousePos);
//////	m_iEndPosSel = m_iPosInStr;
//////	m_bDblClik = 0;
//////}
//void MyEditCtrl::Update(int dx, int dy, POINT ptMousePos, float dt)
//{
//	static bool bLMBDown = false;	
//	//m_bVisible = m_pParent->m_bVisible; // ??? Update() в ElmtContnr только когда она top
//	//bLMBDown = gDInput->mouseButtonDown(0);
//	//m_bHit = PtInRect(&m_rcHit, ptMousePos);
//
//	//if(m_bHit)
//	//{
//	//	/*Write_Log_String(hLog, "if(m_bHit)\n");
//	//	Write_Log_String(hLog, "m_idxCol		 %d\n", m_idxCol);
//	//		Write_Log_String(hLog, "m_iPosInStr		 %d\n", m_iPosInStr);*/
//	//	m_curPosX = (float)(ptMousePos.x - (m_pFont->symW >> 1));
//	//	m_curPosY = (float)(ptMousePos.y - (m_pFont->symH >> 1));
//	//}	
//	/*if(!bLMBDown) 
//	{
//		m_bDrag = 0;
//		m_bDblClik = 0;
//	}*/
//	
//	// Фокус должна устанавливать родительская форма??
//	//if(m_bFocus)
//	//{
//		//if(!m_bDrag)
//		//{
//		//	SetCarret(ptMousePos);
//		//	m_iEndPosSel = m_iPosInStr;
//		//	//m_bDrag = 1;	
//		//}		
//	//}
//	//if(bLMBDown && m_bHit)
//	//{	
//	//	//Write_Log_String(hLog, "if(bLMBDown && m_bHit)\n");
//	//	if(!m_bDrag)
//	//	{
//	//		SetCarret(ptMousePos);
//	//		m_iEndPosSel = m_iPosInStr;
//	//		m_bDrag = 1;
//	//		/*Write_Log_String(hLog, "if(bLMBDown && m_bHit) if(!m_bDrag) SetCarret(ptMousePos)\n");
//	//		Write_Log_String(hLog, "m_idxCol		 %d\n", m_idxCol);
//	//		Write_Log_String(hLog, "m_iPosInStr		 %d\n", m_iPosInStr);*/			
//	//	}		
//	//	m_bFocus = 1;
//	//}
//	
//	if(m_state & BFS_FOCUS)
//	{
//		//int delPos = min(m_iPosInStr, m_iEndPosSel); 
//		//int delRange = abs(m_iEndPosSel - m_iPosInStr);		
//		//if(delRange == 0) delRange = 1;	
//		
//		//if(m_bDrag)
//		//{
//		//	if(gDInput->m_bLMBDblClk)
//		//	{
//		//		SelWord();				
//		//		m_bDblClik = 1;
//		//	}
//
//		//	//if(!m_bDblClik)
//		//	//{
//		//	//	int col = ((float)(ptMousePos.x-m_rcHit.left)/m_pFont->symW)+0.5f;
//
//		//	//	//static float t = 0.0f;
//		//	//	//float accel = abs(col - m_idxCol) * 0.05f;				
//
//		//	//	//if(t > 3.0f)
//		//	//	//{
//		//	//		//t = 0.0f;
//		//	//		if(col > m_idxCol) MoveNext();
//		//	//		if(col < m_idxCol) MovePrev();	
//		//	//	//}
//		//	//	//else t += accel;
//		//	//}
//		//}
//		
//		static float accum2 = 0.0f;
//		if(accum2 >= 0.5f)
//		{
//			accum2 = 0.0f;
//			m_bShowCarret = !m_bShowCarret; 	
//		}
//		else accum2 += dt;
//
//		unsigned char ch = gDInput->GetChar();
//		// After!!!^^^
//		//m_bCtrlDwn = gDInput->keyDown(DIK_LCONTROL | DIK_RCONTROL);
//		/*bool bA = gDInput->keyDown(DIK_A);
//		if(m_bCtrlDwn && bA)
//		{
//			m_iEndPosSel = str.size();
//			MoveTo(0);
//			ch = 0;
//		}*/
//
//		switch(ch)
//        {
//			case 0 :				
//			break;
//
//			case 133 :	// Home
//				//MoveTo(0);
//			break;
//
//			case 134 :	// End	
//				//MoveTo(str.size());
//			break;
//
//			case 132 :	// Delete					
//				//MoveTo(delPos);	
//				//DelRange(delPos, delRange);
//				//ch = 0;
//			break;
//
//			case 10 :	// Enter			
//			break;
//
//			//case 135 :	// Ctrl(135)			
//			//break;
//
//			
//
//			case 128 :	// стрелка вправо
//				MoveNext();
//			break;
//
//			case 131 :	// backspace				
//				/*MoveTo(max(m_iPosInStr, m_iEndPosSel));
//				for(; delRange != 0; --delRange)
//				{	
//					if(MovePrev() >= 0) 
//					{
//						str.erase(m_iPosInStr, 1);
//						if(m_idxStartChDraw - 1 >= 0)
//						{
//							m_idxCol++;
//							m_idxStartChDraw--;
//						}
//					}
//				}*/
//					
//				/*Write_Log_String(hLog, "m_idxCol		 %d\n", m_idxCol);
//				Write_Log_String(hLog, "m_iPosInStr		 %d\n", m_iPosInStr);
//				Write_Log_String(hLog, "m_idxStartChDraw %d\n", m_idxStartChDraw);
//				Write_Log_String(hLog, "m_iNumCols		 %d\n", m_iNumCols);
//				Write_Log_String(hLog, "str.size()		 %d\n\n", str.size());*/
//			break;	
//
//			case 127 :	// стрелка влево				
//				MovePrev();
//			break;		
//
//			default :
//				/*if(m_iEndPosSel - m_iPosInStr != 0)
//				{
//					MoveTo(delPos);
//					DelRange(delPos, delRange);
//				}*/
//				str.insert(m_iPosInStr, 1, ch);
//				MoveNext();
//				//SET_BIT(m_state, BFS_REDRAW);
//			break;
//        }
//
//		//if(ch != 0) m_iEndPosSel = m_iPosInStr;
//		//m_carretPosX = (float)(m_rcHit.left + (m_idxCol * m_pFont->symW));
//		//m_carretPosY = (float)(m_rcHit.top + (m_idxRow * (m_pFont->symH + m_pFont->m_interStr)));		
//		// DrawCarret
//		//if(m_bShowCarret) 
//		//{
//			//m_pCanv->DrawRC(m_pFont->m_pTex, m_pFont->m_vCharsTexCoord[0], 10, 0, 0xffffffff);
//			//SET_BIT(m_state, BFS_REDRAW);
//		//}
//	}
//	else m_bShowCarret = 0;
//	
//	/*Write_Log_String(hLog, "m_idxCol		 %d\n", m_idxCol);
//			Write_Log_String(hLog, "m_iPosInStr		 %d\n", m_iPosInStr);
//	Write_Log_String(hLog, "end\n\n\n");*/
//	//Write_Log_String(hLog, "MyLabel::Update\n");
//	//UpdadeSelView();
//	
//	//SET_BIT(m_state, BFS_REDRAW);
//
//	MyLabel::Update(dx, dy, ptMousePos, dt);
//
//	//m_pCanv->DrawRC(m_pFont->m_pTex, m_pFont->m_vCharsTexCoord[0], 10, 0, 0xffffffff);
//	
//	
//	
//}
////////void MyEditCtrl::SetText(const char *txt)
////////{
////////	MyLabel::SetText(txt);
////////}
//void MyEditCtrl::UpdateCanv(void)
//{
//	MyLabel::UpdateCanv();
//
//	m_pCanv->DrawLine(10, 0, 2, 14, 0xffffffff);
//	//m_pFont->DrawString(0, &m_rcHit, (float)m_xTxt, (float)m_yTxt, z, m_clrFnt, &str[m_idxStartChDraw]);	
//	// Draw sel text
//	/*if(m_bSel)
//	{
//		m_pSprite->Draw(m_pTex, &m_rcSelTxt, 0, 
//						&D3DXVECTOR3((float)m_rcSelTxt.left, (float)m_yTxt, z), 
//						D3DCOLOR_XRGB(0, 0, 255));
//		
//		m_pFont->DrawString(0, &m_rcSelTxt, (float)m_xTxtSel, (float)m_yTxt, z, 
//							D3DCOLOR_XRGB(255, 255, 255), &m_sSelStr[m_idxStartSelChDraw]);
//	}*/
//	//if(m_state & BFS_HIT) 
//	//{
//	//	g_UIPanel->ShowUICursor(0);
//	//	//m_pFont->DrawCursor(0, m_curPosX, m_curPosY, z, D3DCOLOR_XRGB(0, 0, 0));		
//	//}
//	//else g_UIPanel->ShowUICursor(1);
//
//	
//}
//MyEditCtrl::~MyEditCtrl(void){}
////====================================================================
////MyImage::MyImage(void){}
////MyImage::MyImage(char *fileSymbTbl, int x, int y, int w, int h) 
////				: RichElmt(x, y, w, h, 1, D3DCOLOR_XRGB(255, 255, 255), D3DCOLOR_XRGB(255, 255, 255))
////{	
////	indCurrImg = 0;		
////	m_pSrcTex = g_pTexMngr->InsertItem(fileSymbTbl, true);
////	LoadSymbolTbl(/*fileSymbTbl, */m_w, m_h, 0, 2, m_pSrcTex, vRcImages);	
////	m_rcSrcTex = vRcImages[indCurrImg];
////}
//////void MyImage::Update(POINT ptMousePos, float dt)
//////{
//////}
//////void MyImage::Draw(float z)
//////{
//////	m_pParent->m_pCanv->Draw(m_pSrcTex, m_rcSrcTex, m_rcW.left, m_rcW.top, m_clMask, m_clBkg);
//////}
////MyImage::~MyImage(void)
////{
////}
//===========================================================================
MySlider::MySlider(void){}
MySlider::~MySlider(void){}
MySlider::MySlider(int x, int y, int w, int h, D3DCOLOR clr) /*не прозрачн!*/
					:ElmtContnr(x, y, w, h, 1/*bFixPos*/, 1/*bFixSize*/, 1, 1, clr)
{
	Init();	
}
MySlider::MySlider(int x, int y, int w, int h, 
				   bool bFixSize, D3DCOLOR clr) /*не прозрачн!*/
					:ElmtContnr(x, y, w, h, 1/*bFixPos*/, bFixSize, w <= h, h < w, clr)
{	
	Init();
}
void MySlider::Init()
{
	// предполагается что горизонтальный
	m_bSort = 0;
	m_pDial = 0;
	m_value = 0;
	m_w < m_h ? m_bVertical = 1 : m_bVertical = 0;
	
	int min_size = min(m_w, m_h);						
	m_pBttn = new ImageButton(0, 0, m_w, m_h, 
							0/*bFixPos*/, m_state & BFS_FIXSIZE/*bFixSize*/, m_w <= m_h, m_h < m_w, 
							m_clr);
	
	SET_BIT(m_state, BFS_FIXSIZE);			// сбрасываем чтобы не было попыток 
	SET_BIT(m_pBttn->m_state, BFS_FIXSIZE);	// изменить размер по тесту на рамку											
	
	SetLimits(0, 100.0f);
	SetRunerSize(min_size, min_size);	

	m_pBttn->SetBoard(g_UIPanel->pBrd8x8x2, 0);
	m_pBttn->m_idxBrdDwn = m_pBttn->m_idxBrdUp;

	m_pBttn->m_align = MAKEDWORD(ALGN_NO, ALGN_MV);
	if(m_bVertical) m_pBttn->m_align = MAKEDWORD(ALGN_MH, ALGN_NO);

	SetRunerPos(0);	
	AddElement(m_pBttn);
}
void MySlider::Resize(RECT &rc)
{
	BaseElmt/*ElmtContnr*/::Resize(rc);

	// предполагается что горизонтальный
	int l = m_pBttn->m_w;
	m_minRunerPos = 0;
	m_maxRunerPos = m_pCanv->m_rcClnt.right;	

	if(m_bVertical) 
	{
		l = m_pBttn->m_h;
		m_minRunerPos = m_pCanv->m_rcClnt.bottom;
		m_maxRunerPos = 0;	
	}
	m_lenDial = max(m_w, m_h) - l;
	m_k = (abs(m_min) + abs(m_max)) / m_lenDial;

	//m_pBttn->AlignElmt(&m_pCanv->m_rcClnt);
	SetRunerPos(m_value);		// т.к. изменился m_lenDial

	//if(m_pDial) m_pDial->AlignElmt(&m_pCanv->m_rcClnt);	
}
void MySlider::SetRunerPos(float val)
{	// предполагается что горизонтальный
	if( val >= m_min && val <= m_max )
	{
		m_value = val;
		int pos = ((float)m_lenDial * val) / m_max;

		int x = m_minRunerPos + pos;
		int y = m_pBttn->m_rc.top;
		if(m_bVertical)
		{ 
			pos = m_lenDial - pos;
			x = m_pBttn->m_rc.left;	
			y = m_maxRunerPos + pos;
		}		
		m_pBttn->SetPos(x, y);		 
		//SET_BIT(m_state, BFS_REDRAW);
	}
}
void MySlider::SetRunerSize(int w, int h)
{
	// предполагается что горизонтальный
	RECT rc;
	rc.left = rc.top = 0;
	rc.right = rc.left + w;
	rc.bottom = rc.top + h;
	m_pBttn->Resize(rc);
	
	Resize(m_rc);
	//m_pBttn->AlignElmt(&m_Canv.m_rcClnt);
	//SetRunerPos(m_value);		// т.к. изменился m_lenDial
}
void MySlider::Update(int dx, int dy, POINT ptMousePos, float dt)
{
	float offset;
	//bool trans = 0;
	//float tmpVal = m_value;
	if(m_bVertical) { dx = 0; offset = dy; }
	else { dy = 0; offset = dx; }
	
	ElmtContnr::Update(dx, dy, ptMousePos, dt);	

	if(m_pBttn->m_state & BFS_DRAG) 
	{
		offset *= m_k;
		if(m_bVertical) m_value -= offset;
		else			m_value += offset;
		
		if( m_value >= m_max) 
		{
			m_value = m_max;
			RESET_BIT(m_pBttn->m_state, BFS_DRAG);
		}
		if( m_value <= m_min) 
		{
			m_value = m_min;
			RESET_BIT(m_pBttn->m_state, BFS_DRAG);
		}
		
		SetRunerPos(m_value);
		if(m_pfUserUpd) m_pfUserUpd(this, dx, dy, ptMousePos, dt);
	}

	//if(m_state & /*BFS_REDRAW*/BFS_RESIZE )  Write_Log_String(hLog, "MySlider::BFS_REDRAW\n");
}
void MySlider::SetDial(MyBorder* pBrd, int idx)
{
	// предполагается что горизонтальный
	int wDial = m_lenDial;
	int hDial = pBrd->m_brdW << 1;

	if(m_bVertical)
	{
		wDial = pBrd->m_brdW << 1;
		hDial = m_lenDial;
	}	
	
	m_pDial = new RichElmt(0, 0, wDial, hDial, 1, 1, 1, 1, 0xffffffff);
	AddElement(m_pDial);
	m_pDial->m_align = MAKEDWORD(ALGN_MH, ALGN_MV);
	m_pDial->AlignElmt(/*&m_pCanv->m_rcClnt*/);
	m_pDial->SetBoard(pBrd, idx);
	
}
void MySlider::SetLimits(float min, float max)
{ 
	m_min = min; m_max = max; 
	m_k = (abs(min) + abs(max)) / m_lenDial;
}
//===========================================================================
MyVScroll::MyVScroll(void){}	
MyVScroll::MyVScroll(int w, D3DCOLOR clr)
					//bFixPos, bool bFixSize, int bFixW, int bFixH
					//: ElmtContnr(0, 0, w, g_UIPanel->m_hScr, 1, 0, 1, 0, clr)
					: ElmtContnr(0, 0, w, g_UIPanel->m_hScr, 1, 1, 1, 1, clr)
{
	m_align = MAKEDWORD(ALGN_RIGHT, ALGN_FITV);
	m_pCanv->m_offsU += w;
	m_pCanv->m_offsD += w;

	//SET_BIT(m_state, BFS_FIXSIZE);	// сбрасываем чтобы не было попыток 
									// изменить размер по тесту на рамку
	//RESET_BIT(m_state, BFS_USE_PARNT_TM);
	RESET_BIT(m_state, BFS_CLIENT_CLIP);

	m_pSlidr = new MySlider(0, 0, w, m_h, 0, m_clr);
	m_pSlidr->m_align = MAKEDWORD(ALGN_LEFT, ALGN_FITV);
	AddElement(m_pSlidr);

	ImageButton* m_pBttnUL = new ImageButton(0, 0, w, w, 1, m_clr /*0xffc8c8c8*/);
	RESET_BIT(m_pBttnUL->m_state, BFS_ALIGN_TO_CLIENT);
	RESET_BIT(m_pBttnUL->m_state, BFS_CLIENT_CLIP);
	m_pBttnUL->SetBoard(g_UIPanel->pBrd8x8x2, 0);
	AddElement(m_pBttnUL);

	ImageButton* m_pBttnRB = new ImageButton(0, 0, w, w, 1, m_clr /*0xffc8c8c8*/);
	RESET_BIT(m_pBttnRB->m_state, BFS_ALIGN_TO_CLIENT);
	RESET_BIT(m_pBttnRB->m_state, BFS_CLIENT_CLIP);
	m_pBttnRB->m_align = MAKEDWORD(ALGN_LEFT, ALGN_BOTTOM);
	m_pBttnRB->SetBoard(g_UIPanel->pBrd8x8x2, 0);
	AddElement(m_pBttnRB);

	strcpy(m_name, "MyVScroll");
}	
//void MyVScroll::Resize(RECT &rc) // убрать если не изменится
//{
//	//Write_Log_String(hLog, "MyVScroll::Resize\n");
//
//	ElmtContnr::Resize(rc);
//	// предполагается что горизонтальный
//	//int offset = min(m_w, m_h);
//	//int l = m_pBttn->m_w;
//	//m_minRunerPos = offset;
//	//m_maxRunerPos = m_pCanv->m_rcClnt.right - offset;	
//
//	//if(m_bVertical) 
//	//{
//	//	l = m_pBttn->m_h;
//	//	m_minRunerPos = m_pCanv->m_rcClnt.bottom - offset;
//	//	m_maxRunerPos = offset;	
//	//}
//	//
//	//m_lenDial = max(m_w, m_h) - offset - offset - l;
//	//m_k = (abs(m_min) + abs(m_max)) / m_lenDial; // обновляем коэф.	
//
//	//if(m_pDial) m_pDial->AlignElmt(&m_pCanv->m_rcClnt);
//	//m_pBttnRB->AlignElmt(&m_pCanv->m_rcClnt);	
//	
//}
void MyVScroll::Update(int dx, int dy, POINT ptMousePos, float dt)
{
	ElmtContnr* p = (ElmtContnr*)(m_anyData);

	if(m_pParent->m_state & BFS_REDRAW)
	{
		m_rc.left += m_w;
		m_rc.right += m_w;
		
		
		RECT rc = p->m_rcAllChilds;
		float k = (float)m_h / (rc.bottom - rc.top);
		if(k < 1.0f) 
		{
			m_pSlidr->SetRunerSize(m_w, k * m_pSlidr->m_h);
			m_pSlidr->SetVisible(1);
		}
		else 
			m_pSlidr->SetVisible(0);
	
		//Write_Log_String(hLog, "m_pParent->m_state & BFS_REDRAW\n");
	}

	/*if(m_pSlidr->m_state & BFS_VISIBLE)
	{
		Write_Log_String(hLog, "m_pSlidr->m_state & BFS_VISIBLE\n");
	}*/
	

	ElmtContnr::Update(dx, dy, ptMousePos, dt);

	

	if(m_pSlidr->m_pBttn->m_state & BFS_DRAG) 
	{
		p->m_pCanv->m_TM.y += dy;
		SET_BIT(p->m_state, BFS_REDRAW);
	}
}
MyVScroll::~MyVScroll(void){}
//===========================================================================