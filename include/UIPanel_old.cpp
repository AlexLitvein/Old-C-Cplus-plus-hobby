#include "UIPanel.h"
#include "d3dApp.h"
#include "TextureMngr.h"
#include "DirectInput.h"
#include "MyHelper.h"
#include "Canvas.h"

UIPanel *g_UIPanel = 0;

UIPanel::UIPanel(void){}
// размеры по экрану, сделать!!! и может позицию ху задать?
void UIPanel::InitPanel(IDirect3DDevice9 *p_d3dDevice, int w, int h)
{	
	m_wScr			= w;
	m_hScr			= h;	
	m_bShowCursor	= 1;
	
	HR(D3DXCreateSprite(p_d3dDevice, &m_pSprite));
	BaseElmt::m_pSprite = m_pSprite;
	
	// попробовать сделать = 0, легче сравнивать при тесте, default = 0;
	HR(g_pD3Device->SetRenderState(D3DRS_ALPHAREF, 10)); 
	HR(g_pD3Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER));
	
	AddFont("CourierNew7x10.bmp", 7, 10, 3);	
	pComImgBttn = new ComImgRes(CRID_COMIMGBTN, "Buttons.bmp", 0, 3, 12, 12);
	pBrd8x8x2 = new MyBorder(CRID_SYSBRD, "Border8x8x2.bmp", 0, 16, 8, 8, 2);
	pSlider = new ComImgRes(CRID_SYSSLDR, "Slider.bmp", 0, 4, 16, 16);
}
//ComImgRes* UIPanel::GetComResPtrByID(int id)
//{
//	ComImgRes* p = 0;
//	int i = m_vpComRes.size();
//	for(; i != 0; --i) 
//	{
//		p = m_vpComRes[i-1];
//		if(p->m_id == id) break;
//		p = 0;
//	}
//	return p;
//}
void UIPanel::ShowUICursor(bool b) { m_bShowCursor = b; }
//void UIPanel::SetCursorID(int id)
//{
//	//m_Cursors.indCurrImg = id;
//}
void UIPanel::UpdFrms(int dx, int dy, POINT ptMousePos, float dt)
{
	DWORD dwDIStat = gDInput->m_dwDIState;
	BaseElmt* pFrm = 0;	
	
	int find = 0;
	std::list<BaseElmt*>::iterator pos = m_vFrmPtrs.begin();
	std::list<BaseElmt*>::iterator fndPos = pos;
	while(pos != m_vFrmPtrs.end())
	{
		pFrm = *pos;			
		if(PtInRect(&pFrm->m_rc, ptMousePos))	SET_BIT(pFrm->m_state, BFS_HIT);
		else									RESET_BIT(pFrm->m_state, BFS_HIT);
				
		if((dwDIStat & DI_LMBCLK) && !find && (pFrm->m_state & BFS_HIT))
		{			
			SET_BIT(pFrm->m_state, BFS_FOCUS);	
			fndPos = pos;						
			find = 1;				
		}
		if((dwDIStat & DI_LMBCLK) && !(pFrm->m_state & BFS_HIT)) RESET_BIT(pFrm->m_state, BFS_FOCUS);
		
		if((pFrm->m_state & BFS_FOCUS) && (dwDIStat & DI_LMBDWN) && (dx != 0 || dy != 0)) 
		{
			SET_BIT(pFrm->m_state, BFS_DRAG);
		}
		else RESET_BIT(pFrm->m_state, BFS_DRAG);	
	
		pFrm->Update(dx, dy, ptMousePos, dt);
		pFrm->UpdateCanv();
		
		pFrm = 0;
		++pos;		
	}	

	if(fndPos != m_vFrmPtrs.begin())
	{
		pFrm = *fndPos;
		m_vFrmPtrs.erase(fndPos);
		m_vFrmPtrs.push_front(pFrm);	
	}
}
void UIPanel::AddElement(RichElmt *pFrm)
{	
	//pFrm->SetPos(pFrm->m_rcW.left, pFrm->m_rcW.top);
	//m_vpAllForms.push_back(pFrm);
	//if(pFrm->m_pParent == 0) 
	m_vFrmPtrs.push_back(pFrm);
}
MyFont* UIPanel::GetFontPtr(char* fntName)
{
	int idx = 0;
	if(fntName != 0)
	{
		size_t size = m_vFonts.size();
		for(size_t i = 0; i < size; ++i)
		{
			if(0 == lstrcmp(m_vFonts[i].m_name, fntName))
			{
				idx = i;
				break;
			}
		}
	}
	return &m_vFonts[idx];
}

//void UIPanel::AddComImgRes(COMRESID id, char *fileSymbTbl, int strtIdx, int count, int w, int h)
//{
//	ComImgRes imgRes = ComImgRes(fileSymbTbl, strtIdx, count, w, h);
//	imgRes.id = id;
//	m_vImgRes.push_back(imgRes);
//}
void UIPanel::AddFont(char *fntName, int symbW, int symbH, int interStr)
{
	MyFont fnt(fntName, symbW, symbH, interStr);
	m_vFonts.push_back(fnt);
}

void UIPanel::Update(float dt)
{
	static	POINT ptOldMPos;
	POINT	ptMPos;
	
	GetCursorPos(&ptMPos);	
	ScreenToClient(gd3dApp->getMainWnd(), &ptMPos);		
	
	if(ptMPos.x < 0) ptMPos.x = 0;
	if(ptMPos.y < 0) ptMPos.y = 0;
	if(ptMPos.x > m_wScr) ptMPos.x = m_wScr;
	if(ptMPos.y > m_hScr) ptMPos.y = m_hScr;	

	int	dx = ptMPos.x - ptOldMPos.x; // after^^^!
	int	dy = ptMPos.y - ptOldMPos.y;
	ptOldMPos = ptMPos;
	
	UpdFrms(dx, dy, ptMPos, dt);	
}
void UIPanel::DrawUI()
{	
	BaseElmt* pFrm = 0;
	// Обязательно отключать свет!!! иначе если текстура без альфа, то вся текстура черная ???
	HR(g_pD3Device->SetRenderState(D3DRS_LIGHTING, false)); //???
	HR(g_pD3Device->SetRenderState(D3DRS_ALPHATESTENABLE, true));
	// При вызове m_pSprite->Begin неявно изменяются многие параметры рендера :(.
	// В частности вкл. D3DRS_ALPHABLENDENABLE = TRUE (может и не нужен) и т.д.
	m_pSprite->Begin(/*D3DXSPRITE_ALPHABLEND*/D3DXSPRITE_DONOTMODIFY_RENDERSTATE); // параметр возможно придется убрать ???

	//HR(g_pD3Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE));
	//HR(g_pD3Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1));
	//HR(g_pD3Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE)); 
	//HR(g_pD3Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO));	
	
	std::list<BaseElmt*>::reverse_iterator rPos = m_vFrmPtrs.rbegin();
	while(rPos != m_vFrmPtrs.rend())
	{		
		pFrm = *rPos;
		pFrm->Draw();
		pFrm = 0;
		++rPos;
	}	

	m_pSprite->End();
	HR(g_pD3Device->SetRenderState(D3DRS_ALPHATESTENABLE, false));
	HR(g_pD3Device->SetRenderState(D3DRS_LIGHTING, true));
}

UIPanel::~UIPanel(void)
{
	m_pSprite->Release();
	m_pSprite = 0;
	delete pComImgBttn;
	delete pBrd8x8x2;
	delete pSlider;
	//int i = m_vpComRes.size();
	//for(; i != 0; --i) delete m_vpComRes[i-1];
}
