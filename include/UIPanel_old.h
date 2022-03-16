#pragma once

//#include "Debug_first.h"
#include "d3dutil.h"
#include "UIElement.h"
#include <vector>

//#include "Debug_last.h"

#define IDC_MYARROW		0
#define IDC_SIZE_ULRD	1
#define IDC_SIZE_BLRU	2
#define IDC_SIZE_LR		3
#define IDC_SIZE_UD		4

const float UI_MINZ = 0.0001f;
const float UI_MAXZ = 0.01f;
const float UI_DZPOS = 0.0001f;

enum SYS_COLOR : DWORD
{	
	SYSCLR_BKGND	= 0xffc8c8c8, //0xffc8c8c8 
	SYSCLR_DISABLED = 0xffe1e1e1
};

class UIPanel
{
public:
	UIPanel(void);	
	~UIPanel(void);

	void DrawUI();
	void InitPanel(IDirect3DDevice9 *p_d3dDevice, int w, int h);
	void AddElement(RichElmt *pFrm);
	void Update(float dt);
	void UpdFrms(int dx, int dy, POINT ptMousePos, float dt);
	void AddFont(char *fntName, int symbW, int symbH, int interStr);
	//void AddComImgRes(COMRESID id, char *fileSymbTbl, int strtIdx, int count, int w, int h);
	void ShowUICursor(bool b);
	MyFont*		GetFontPtr(char* fntName);
	//ComImgRes*	GetComResPtrByID(int id);

	ComImgRes* pSlider;
	ComImgRes* pComImgBttn;
	MyBorder* pBrd8x8x2;
	ID3DXSprite*	m_pSprite;
	
	bool		m_bShowCursor;
	int			m_wScr;
	int			m_hScr;	

	std::vector<MyFont>		m_vFonts;
	//std::vector<ComImgRes*>	m_vpComRes;
	std::list<BaseElmt*>	m_vFrmPtrs;		
};
