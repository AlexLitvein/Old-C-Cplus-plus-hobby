#pragma once

//#include "Debug_first.h"
#include "d3dutil.h"
//#include <vector>
//#include "DrawableTex2D.h"
#include "TextureMngr.h"
#include "Canvas.h"
#include <list>
#include <string>


//#include "Debug_last.h"

struct ComImgRes;
class MyBorder;
//class BaseElmt;
//class ElmtContnr;
class ImageButton;
class MyHeader;
class MyVScroll;

//#include "MyHeader.h"

enum COMRESID : DWORD
{	
	CRID_SYSBRD		= 1,
	CRID_SYSSLDR,
	CRID_COMIMGBTN
};

enum BFRMSTATE : DWORD
{	
	BFS_DRAG		= 0x00000001,
	BFS_HIT			= 2,
	BFS_FOCUS		= 4,
	BFS_DRAWTOPRNT	= 8,
	BFS_RESIZE		= 0x00000010, // 16
	BFS_FIXPOS		= 0x00000020, // 32,
	BFS_FIXSIZE		= 0x00000040, // 64,
	BFS_REDRAW		= 0x00000080, // 128,	// 8-ой бит начиная с 1
	BFS_HITLBRD		= 0x00000100, // 256,
	BFS_HITRBRD		= 512,
	BFS_HITTBRD		= 1024,
	BFS_HITBBRD		= 2048,
	BFS_ALIGN_TO_CLIENT	= 4096,
	BFS_CLIENT_CLIP		= 0x00002000, // 8192,
	BFS_DISABLED		= 0x00004000, //16384
	BFS_VISIBLE			= 0x00008000
};


ImageButton* CreateImgButton(int x, int y, int w, int h, bool bFixPos, DWORD c, 
							 MyBorder* pBrd, int idxBrd, 
							 ComImgRes* pImg, int idxImg,
							 char* text);
//============================================================
struct ComImgRes
{
	ComImgRes(void){}
	ComImgRes(DWORD id, char *fileSymbTbl, int strtIdx, int count, int w, int h)
	{
		m_id = id;
		m_symW = w; m_symH = h;
		m_pTex = g_pTexMngr->InsertTex(fileSymbTbl, true);
		LoadSymbolTbl(m_symW, m_symH, strtIdx, count, m_pTex, m_vRcImgs);		
	}
	virtual void ComImgRes::Draw(Canvas* pCanv, int offset)
	{
		pCanv->DrawRC(m_pTex, m_vRcImgs[offset], 0, 0, 0xffffffff);		
	}

	int					m_id;
	int					m_symW;
	int					m_symH;
	LPDIRECT3DTEXTURE9  m_pTex;
	std::vector<RECT>	m_vRcImgs;	
};

//============================================================
class MyBorder : public ComImgRes
{
public:
	MyBorder(void);	
	MyBorder(DWORD id, char *fileSymbTbl, int strtIdx, int count, int wSym, int hSym, int brdW);

	//virtual void Update(POINT ptMousePos, float dt);
	virtual void Draw(Canvas* pCanv, int offset, DWORD clr);
	virtual ~MyBorder(void);

	int		m_brdW;
	//int		m_offset;
};
//============================================================
class MyFont
{
public:
	MyFont(void); 
	~MyFont(void);
	MyFont(char *fnSymTbl, int symbW, int symbH, int interStr);	
	//void SetFont(char *fnSymTbl, int symbW, int symbH);	
	//void SetFontColor(D3DCOLOR cIn);
	//void DrawCursor(int idCur, float x, float y, float z, D3DCOLOR	color);
	//void AlignString(RECT &rcDst, DWORD fAlign, const char* str, RECT &rcOut);
	void DrawString(Canvas* pCanv, bool bWrap, RECT *rcBnd, int x, int y, 
					D3DCOLOR cFont, D3DCOLOR cBkg, const char* str);	

	char				m_name[32];
	int					m_w;
	int					m_h;
	int					m_interStr;

//private:
	//ID3DXSprite*		m_pSprite;
	LPDIRECT3DTEXTURE9	m_pTex;		
	std::vector<RECT>	m_vCharsTexCoord;
};
//============================================================
class BaseElmt
{
public:
	BaseElmt(void);
	BaseElmt(int x, int y, int w, int h, bool bFixSize, int bFixW, int bFixH, D3DCOLOR clr);	
	virtual ~BaseElmt(void);
	virtual void Update(int dx, int dy, POINT ptMousePos, float dt);
	virtual void UpdateCanv(void);
	virtual void Resize(RECT &rc);

	void Draw(void);
	//void UpdateHit(DWORD distate, POINT ptMousePos);
	void AlignElmt(/*RECT* pDstRC*//*, DWORD fAlign*/);
	void SetPos(int x, int y);

	void PrintName(char* str);
	void SetVisible(bool bVisible);
	
	DWORD				m_anyData;
	DWORD				m_align;
	DWORD				m_state;
	//int					m_offsetX;
	//int					m_offsetY;
	int					m_tabs;
	char				m_name[32];
	int					m_w;
	int					m_h;	
	RECT				m_rc;	
	D3DCOLOR			m_clr;
	BaseElmt*			m_pParent;
	Canvas*				m_pCanv;
	static ID3DXSprite*	m_pSprite;
};
//============================================================
class RichElmt :public BaseElmt
{
public:
	RichElmt(void);
	RichElmt(int x, int y, int w, int h, bool bFixPos, bool bFixSize, int bFixW, int bFixH, D3DCOLOR clr);	
	virtual ~RichElmt(void);
	//virtual void OnGetFocus(POINT ptMousePos);
	virtual void Update(int dx, int dy, POINT ptMousePos, float dt);
	virtual void UpdateCanv(void);
	
	void SetBoard(MyBorder* pBrd, int idx); // или применить какие-либо IDxxx_xxx
	void UpdateSize(int dx, int dy, POINT ptMousePos, float dt);
	//virtual void Resize(RECT &rc);
		
	int					m_idxBrd;
	MyBorder*			m_pBrd;	
	void (*m_pfUserUpd)(BaseElmt *bf, int dx, int dy, POINT ptMousePos, float dt);
};
//============================================================
//class ElmtContnr : public RichElmt
//{
//public:
//	ElmtContnr(void);	
//	ElmtContnr(int x, int y, int w, int h, bool bFixPos, bool bFixSize, D3DCOLOR clr);
//	void AddElement(BaseElmt *el); // virtual???
//
//	virtual ~ElmtContnr(void);
//	virtual void Update(int dx, int dy, POINT ptMousePos, float dt);
//	virtual void UpdateCanv(void);
//	
//	bool					m_bClear;
//	std::list<BaseElmt*>	m_vChilds;
//};
//============================================================
class ElmtContnr : public RichElmt
{
public:
	ElmtContnr(void);	
	ElmtContnr(int x, int y, int w, int h, bool bFixPos, bool bFixSize, int bFixW, int bFixH, D3DCOLOR clr);
	virtual int AddElement(BaseElmt *el);

	virtual ~ElmtContnr(void);
	virtual void Update(int dx, int dy, POINT ptMousePos, float dt);
	virtual void UpdateCanv(void);
	
	void AddScroll(BaseElmt* pScroll);
	//void UpdateChildCanvas(BaseElmt* p);
	void UpdateChild(int dx, int dy,		// ???
					DWORD dwDIStat, POINT ptMousePos, 
					float dt, BaseElmt* p, int &idxFnd);
	
	RECT		m_rcAllChilds;
	//bool					m_bClear;
	bool					m_bSort;
	int						m_maxChild;
	int						m_numChild;
	BaseElmt*				m_vChilds[32];

	//MyVScroll*	m_pVScroll;
	//BaseElmt*	m_pHScroll;	
};
//============================================================
class MyWindow : public ElmtContnr
{
public:
	MyWindow(void);	
	MyWindow(int x, int y, int w, int h, bool bFixPos, bool bFixSize, int bFixW, int bFixH, D3DCOLOR clr);
	virtual int AddElement(BaseElmt *el);

	virtual ~MyWindow(void);
	//virtual void Update(int dx, int dy, POINT ptMousePos, float dt);
	//virtual void UpdateCanv(void);

	//void AddScroll(BaseElmt* pScroll);
	
	BaseElmt*	m_pHdr;
	/*MyVScroll*	m_pVScroll;
	BaseElmt*	m_pHScroll;	*/
};
//============================================================
class MyLabel : public RichElmt/*BaseElmt*/
{
public:
	MyLabel(void);
	MyLabel(int x, int y, int w, int h, D3DCOLOR cBkg, D3DCOLOR cFont);
	virtual ~MyLabel(void);
	virtual void UpdateCanv(void);
	virtual void Update(int dx, int dy, POINT ptMousePos, float dt);		

	void AlignString(DWORD flgAlign);
	void SetText(const char *txt);
	void SetTextColor(D3DCOLOR c);

	MyFont*		m_pFont;
	D3DCOLOR	m_cFont;
	DWORD		m_dTxtAlign;
	std::string str;
};
//============================================================
class TextButton : public RichElmt
{
public:
	TextButton(void);
	~TextButton(void);
	TextButton(int x, int y, int w, int h, 
				bool bFixPos, bool bFixSize, int bFixW, int bFixH, 
				D3DCOLOR clr);
	TextButton(int x, int y, int w, int h, bool bFixPos, D3DCOLOR clr);	
	
	virtual void Update(int dx, int dy, POINT ptMousePos, float dt);
	virtual void UpdateCanv(void);
	//virtual void Draw(void);
	//virtual void SetBoard(MyBorder* pBrd, int idx);
	void SetText(const char *txt);
	void Init();

	enum BTNSTATE : DWORD
	{	
		BTN_DWN		= 1,
		BTN_UP		= 2
	};
	DWORD		m_bttnState;
	int			m_idxBrdUp;
	int			m_idxBrdDwn;
	int			m_oldIdx;
	MyLabel*	m_pCaption;
};
//============================================================
class ImageButton : public TextButton
{
public:
	ImageButton(void);
	~ImageButton(void);
	ImageButton(int x, int y, int w, int h, bool bFixPos, D3DCOLOR clr);
	ImageButton(int x, int y, int w, int h, 
				bool bFixPos, bool bFixSize, int bFixW, int bFixH, 
				D3DCOLOR clr);
	
	virtual void Update(int dx, int dy, POINT ptMousePos, float dt);
	virtual void UpdateCanv(void);

	void Init();
	void SetImages(ComImgRes* pImgBtn, int strtIdx);

	int			m_idxImgOld;
	int			m_idxImgUp;
	int			m_idxImgDwn;
	int			m_idxImgOut;
	ComImgRes*  m_pImgs;
};
//============================================================
class MySlider : public ElmtContnr
{
public:
	MySlider(void);
	MySlider(int x, int y, int w, int h, D3DCOLOR clr);
	MySlider(int x, int y, int w, int h, bool bFixSize, D3DCOLOR clr);

	virtual ~MySlider(void);
	virtual void Update(int dx, int dy, POINT ptMousePos, float dt);
	virtual void Resize(RECT &rc);

	void Init();
	void SetLimits(float min, float max);
	void SetDial(MyBorder* pBrd, int idx);
	void SetRunerSize(int w, int h);
	void SetRunerPos(float val);

	bool		m_bVertical;
	int			m_lenDial;
	int			m_minRunerPos;
	int			m_maxRunerPos;	
	float		m_max;
	float		m_min;
	float		m_k;
	float		m_value;
	ImageButton* m_pBttn;	
	RichElmt*	m_pDial;
};
//============================================================
class MyVScroll : public ElmtContnr
{
public:
	MyVScroll(void);	
	MyVScroll(int w, D3DCOLOR clr);	
	
	virtual void Update(int dx, int dy, POINT ptMousePos, float dt);
	virtual ~MyVScroll(void);
	//virtual void Resize(RECT &rc);

	//int				m_offset; // отсуп сверху(слева)/ снизу(справа) для размеров ползунка
	MySlider*		m_pSlidr;
	//ImageButton* m_pBttnUL;
	//ImageButton* m_pBttnRB;
};
//============================================================
//class MyEditCtrl : public MyLabel
//{
//public:
//	MyEditCtrl(void);	
//	MyEditCtrl(int x, int y, int w, int h, D3DCOLOR cBkg, D3DCOLOR cFont);
//	//void SetText(const char *str);	
//	//void SetCarret(POINT ptMPos);
//	//void UpdadeSelView(void);
//	//void SelWord(void);
//	//void DelRange(int pos, int range);
//	//int MoveTo(int pos);
//	void MoveNext();
//	int MovePrev();
//
//	virtual ~MyEditCtrl(void);
//	//virtual void OnGetFocus(POINT ptMousePos);
//	virtual void Update(int dx, int dy, POINT ptMousePos, float dt);
//	virtual void UpdateCanv(void);	
//	
//	//bool m_bButtDown;
//	//bool m_bButtUp;	
//private:
//	int		m_iNumCols;
//	int		m_iPosInStr;
//	int		m_idxCol;
//
//	RECT	m_rcSelTxt;
//	//MyFont*	m_pSelFont;
//	int		m_idxStartChDraw;
//	bool	m_bEditable;
//	bool	m_bCtrlDwn;
//	bool	m_bShowCarret;
//	//int		m_iNumRows;	
//	bool	m_bSel; //??
//	int		m_iEndPosSel;
//	bool	m_bDblClik;
//	//int		m_idxRow;	
//	float	m_curPosX;
//	float	m_curPosY;
//	float	m_carretPosX;
//	//float	m_carretPosY;
//	int		m_xTxtSel;
//	int		m_idxStartSelChDraw;
//	std::string m_sSelStr;
//};

//============================================================