#include "UISystemDef.h"

//namespace UIElement
//{

Base_Elm::Base_Elm(void) : m_idx(-1), m_name(), m_pParent(0), m_state(0), m_property(0),
	m_messages(0), m_bckColor(0), m_align(ALIGN_DEFAULT), /*m_anchor(),*/ m_x(0), m_y(0),  
	m_w(0), m_h(0), m_maxW(0), m_maxH(0), m_minSize(0), m_idxTex(M_FAIL), 
	OnMBDown(0), OnMBUp(0), OnMMove(0), OnKeyDown(0), OnResize(0)
{
}

//Base_Elm::Base_Elm(char* name, SIZE pos, SIZE size, /*RECT &rc,*/ SIZE szMax, size_t color, 
//				   /*size_t align,*/ /*bool bCanvas,*/ Base_Elm* pParent)

Base_Elm::Base_Elm(Base_Elm* pParent, int x, int y, int w, int h, int maxW, int maxH, char* name)

  : m_idx(-1), m_name(), m_pParent(pParent), m_state(0), m_property(0), m_messages(0),
	m_bckColor(0), m_align(ALIGN_DEFAULT), /*m_anchor(),*/ m_x(x), m_y(y),  
	m_w(w), m_h(h), m_maxW(maxW), m_maxH(maxH), m_minSize(10), m_idxTex(M_FAIL), 
	OnMBDown(0), OnMBUp(0), OnMMove(0), OnKeyDown(0), OnResize(0)
{
	//SetSize(w, h);
	strncpy(m_name, name, ELEMENT_NAME_MAXCHAR);	
	if(pParent == 0) 
	{
		m_idxTex = g_pTexMngr->AddTexture(name, 0, D3DPOOL_MANAGED, m_w, m_h);
		if(m_idxTex == M_FAIL) m_idxTex = 0; 		
	}
	else m_idxTex = pParent->IdxTex();
}

Base_Elm::~Base_Elm(void)
{
	//Destroy();
}

void Base_Elm::Destroy()
{
}

void Base_Elm::Update(float dt)
{
	RECT rc;

	CurrRect(rc, 0);
	if(PtInRect(&rc, gDInput->MousePos()) != 0)
	{
		SET_BIT(m_state, STATE_MOUSEHIT);
		if(gDInput->IsLMB_CLK_DWN() != 0) SetFocus();			
	}
	else RESET_BIT(m_state, STATE_MOUSEHIT);

//-----------------------------------------------------------

	if(m_messages != 0)
	{
		//if((m_messages & MSG_RESIZE) != 0) Resize();

		if((m_messages & MSG_CHANGE_BCKGND_COLOR) != 0) m_bckColor = g_pUIPanel->GetColor(CLR_BCKGND);
	}

	UpdateState(dt);
	UpdateView();

	m_messages = 0;
}

//void Base_Elm::DefaultProc(float dt)
//{
//}

void Base_Elm::UpdateState(float dt)
{
	bool bDo = 0;
	//POINT pt;
	
	static size_t fps = 0;
	static float cnt = 0.0f;
	cnt += dt;
	fps++;
	if(cnt > 1.0f)
	{
		tmpFPS = fps;
		fps = 0;

		m_bckColor = ColorARGB(0xff, GetRandomInt(0, 0xff), GetRandomInt(0, 0xff), GetRandomInt(0, 0xff));
		cnt = 0.0f;
	}	

//--------------------------------------------------

	if(gDInput->IsLMB_DWN() != 0)
	{
		if((m_state & STATE_RESIZE) != 0)
		{
			Resize(m_w + gDInput->MouseDX(), m_h + gDInput->MouseDY());
			RESET_BIT(m_state, STATE_MOVE);
		}

		if((m_state & STATE_MOVE) != 0) // !!!after
		{
			m_x += gDInput->MouseDX();
			m_y += gDInput->MouseDY();
		}
	}
	else 
	{
		RESET_BIT(m_state, STATE_MOVE);
		RESET_BIT(m_state, STATE_RESIZE);
	}

	
	/*if((m_state & STATE_MOVING) != 0 && gDInput->IsLMB_DWN() != 0)
	{
		if((m_state & STATE_RESIZING) != 0)
		{
			 Resize();
			 RESET_BIT(m_state, STATE_MOVING);
		}
		else
		{
			m_x += gDInput->MouseDX();
			m_y += gDInput->MouseDY();
		}
	}
	else RESET_BIT(m_state, STATE_MOVING);*/
	
	
//--------------------------------------------------
}

void Base_Elm::UpdateView()
{
	const TMItem* pItemTex = 0;
	SIZE tm = { 0, 0 };
	//RECT rcIn;
	RECT rcClip;
	TEXTBLOCK tb;
	TEXTROW tr;
	char* but = 0;
	//static int x = 0; //gd3dApp->GetBackBuffWidth() >> 1;

	Mem_Set_QUAD(&tb, 0, sizeof(TEXTBLOCK) >> 2);

	CurrRect(rcClip, 1);
	pItemTex = g_pTexMngr->GetTexData(m_idxTex);

	if(gDInput->IsLMB_DWN()) but = "Left";
	if(gDInput->IsRMB_DWN()) but = "Right";
	if(gDInput->IsMMB_DWN()) but = "Midle";
	//x += gDInput->MouseX();

	//if(x < 0) x = 0;
	//if(x > gd3dApp->GetBackBuffWidth()) x = gd3dApp->GetBackBuffWidth();

	sprintf_s(g_buff, GBUFF_SIZE, "FPS: %d\nX = %d\nY = %d\nbutton - %s", 
		tmpFPS, gDInput->MouseX(), gDInput->MouseY(), but);
	
	
	
	//SetAlign(ALIGN_LEFT | ALIGN_BOTTOM);
	//SetAlign(ALIGN_RIGHT | ALIGN_BOTTOM);
	//SetAlign(ALIGN_RIGHT | ALIGN_TOP);
	//SetAlign(ALIGN_MIDDLE_X | ALIGN_MIDDLE_Y);

	//g_pUIPanel->m_font.ProcessString("Для этоrо на зажимы А, В, С подается трехфазное питание.", rcClip, tr);
	//g_pUIPanel->m_font.DrawStringM2(tr, rcClip, m_align, pItemTex, 0xffffffff, m_bckColor);
	g_pTexMngr->FillTexture2(pItemTex, &rcClip, 0xff7BCDF3, 0);
	g_pUIPanel->m_font.ProcessText(g_buff, rcClip, &tb, 1);
	//g_pUIPanel->m_font.ProcessText("\nДля этоrо на\n\n зажимы А, В, С подается трехфазное питание.\n", rcClip, &tb, 1);
	g_pUIPanel->m_font.DrawTextM2(&tb, rcClip, m_align, pItemTex, 0xffffffff, m_bckColor, 1);


	//IntersectRect(&rcOut, &rcIn, &rcClip);
	//g_pTexMngr->FillTexture2(pItemTex, &rcOut, m_bckColor, 0);
	//g_pUIPanel->m_font.MeasureText(g_buff, rcClip, 1, rcIn);
	//g_pUIPanel->m_font.MeasureString("Для этоrо па зажимы А, В, С подается трехфазное питание.", rcClip, rcIn);
	//g_pUIPanel->m_font.MeasureString("Для", rcClip, rcIn);
	//AlignRect(&rcClip, &rcIn);
	//g_pUIPanel->m_font.DrawStringM(g_buff, rcClip, m_align, pItemTex, 0xffffffff, m_bckColor);
	

	//D3DXSaveTextureToFile("tex.bmp", D3DXIFF_BMP, pItemTex->_pTex, 0);
	
}
//void Base_Elm::Resize()
//{
//	if(gDInput->MouseX() >= m_x + m_w - 5) 
//	{
//		m_w += gDInput->MouseDX();
//		if(m_w > m_maxW) m_w = m_maxW;
//		if(m_w < m_minSize) m_w = m_minSize;
//	}
//}
void  Base_Elm::AlignRect(RECT* pDstRC, RECT* pSrcRC)
{
	int wSrc = pSrcRC->right - pSrcRC->left;
	int hSrc = pSrcRC->bottom - pSrcRC->top;
	int wDst = pDstRC->right - pDstRC->left;
	int hDst = pDstRC->bottom - pDstRC->top;

	if(isAlign(ALIGN_LEFT) != 0) pSrcRC->left = pDstRC->left;
	if(isAlign(ALIGN_TOP) != 0) pSrcRC->top = pDstRC->top;
	if(isAlign(ALIGN_RIGHT) != 0) pSrcRC->left = pDstRC->right - wSrc;
	if(isAlign(ALIGN_BOTTOM) != 0) pSrcRC->top = pDstRC->bottom - hSrc;
	if(isAlign(ALIGN_MIDDLE_X) != 0) pSrcRC->left = pDstRC->left + ((wDst - wSrc) >> 1);
	if(isAlign(ALIGN_MIDDLE_Y) != 0) pSrcRC->top = pDstRC->top + ((hDst - hSrc) >> 1);	

	pSrcRC->right = pSrcRC->left + wSrc;
	pSrcRC->bottom = pSrcRC->top + hSrc;
}


void Base_Elm::Resize(size_t w, size_t h)
{

	//doAnchor();
	//doDock();
	//if(OnResize == 0)
	//{
		//if(gDInput->MouseX() >= m_x + m_w - 5) 
		//{
			/*m_w += gDInput->MouseDX();
			if(m_w > m_maxW) m_w = m_maxW;
			if(m_w < m_minSize) m_w = m_minSize;
			RESET_BIT(m_state, STATE_MOVING);*/
		//m_w += gDInput->MouseDX();
		//	if(m_w > m_maxW) m_w = m_maxW;
		//	if(m_w < m_minSize) m_w = m_minSize;
		//	RESET_BIT(m_state, STATE_MOVING);
		//}
	m_w = w;
	m_h = h;
	if(m_w < m_minSize) m_w = m_minSize;
	if(m_h < m_minSize) m_h = m_minSize;
	if(m_w > m_maxW) m_w = m_maxW;
	if(m_h > m_maxH) m_h = m_maxH;
	//}
	//else OnResize(this);

}

void Base_Elm::doAnchor()
{
	if(m_pParent == 0) return; // ???

	if(isAnchor(ANCHOR_LEFT_TOP))
	{
		m_x = m_pParent->X();
		m_y = m_pParent->Y();		
		return;
	}

	if(isAnchor(ANCHOR_LEFT_BOTTOM))
	{
		m_x = m_pParent->X();
		m_y = m_pParent->H() - m_w;		
		return;
	}
}

void Base_Elm::doDock()
{	
	if(m_pParent == 0) return; // ???

	if(isDock(DOCK_FILL))
	{
		m_x = m_pParent->X();
		m_y = m_pParent->Y();
		m_w = m_pParent->W();
		m_h = m_pParent->H();
		return;
	}

	if(isDock(DOCK_LEFT))
	{
		m_x = m_pParent->X();
		m_y = m_pParent->Y();			
		m_h = m_pParent->H();
		return;
	}

	if(isDock(DOCK_RIGHT))
	{
		m_x = m_pParent->W() - m_w;
		m_y = m_pParent->Y();		
		m_h = m_pParent->H();
		return;
	}

	if(isDock(DOCK_TOP))
	{
		m_x = m_pParent->X();
		m_y = m_pParent->Y();		
		m_w = m_pParent->W();
		return;
	}

	if(isDock(DOCK_BOTTOM))
	{
		m_x = m_pParent->X();
		m_y = m_pParent->H() - m_h;		
		m_w = m_pParent->W();
		return;
	}

}

//=====================LABEL=================================

//LabelMy::LabelMy(char* name, char* text, SIZE pos, SIZE size, 
//				 size_t align, size_t dock, size_t anchor, Base_Elm* pParent)	
//{
//	SIZE szMax = {0, 0};
//	Base_Elm(name, pos, size, szMax, 0, pParent);
//	SetText(text);
//	//SET_ALIGN(, m_align);
//}


//} // end namespace UIElement