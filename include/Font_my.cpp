//#include "Font_my.h"
#include "UISystemDef.h"

//namespace TextMy
//{

MyFont::MyFont(void) : m_name(), m_h(0), m_interStr(0)
{
}

MyFont::MyFont(char* name, int h, int interStr) : m_name(), m_h(0), m_interStr(interStr)
{
	ScanFont(name, h);
	//LoadFont(name, m_h);
}

int MyFont::LoadFont(const char* name, int h)
{
	int res = 0;
	IDirect3DTexture9*	pTex = 0;
	D3DXIMAGE_INFO inf;
	size_t *pFirstTexel = 0, *pCurrTexel = 0, *pEndTexel = 0;	
	size_t total = 0, texlInX = 0, currW = 0, cntX = 0, cntY = 0, prevX = 0;
	D3DLOCKED_RECT rcLock;

	strncpy(m_name, name, MAXCHAR_FONTNAME);

	HR3D(D3DXCreateTextureFromFileEx(gd3dDevice, m_name, 0, 0, D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, 
		D3DPOOL_SYSTEMMEM, D3DX_DEFAULT, D3DX_DEFAULT, 0xFF000000, &inf , 0, &pTex));
	
	if(pTex != 0)
	{		
		HR3D(pTex->LockRect(0, &rcLock, 0, 0));
		pFirstTexel = (size_t* )rcLock.pBits;
		if(pFirstTexel != 0)
		{
			texlInX = rcLock.Pitch >> 2;
			pCurrTexel = pFirstTexel;
			pEndTexel = pFirstTexel + (inf.Height * inf.Width);
			while(pCurrTexel < pEndTexel)
			{
				if(cntX == texlInX) 
				{
					cntY++;
					pCurrTexel = pFirstTexel + ((m_h + 1) * texlInX * cntY);
					cntX = 0;
					prevX = 0;
				}

				if( *pCurrTexel == 0)
				{
					currW = cntX - prevX;
					defineSymbol(&m_arrSymbDefs[total], currW, texlInX, pCurrTexel + texlInX - currW);

					prevX = cntX;
					total++;
				}
				pCurrTexel++;				
				cntX++;				
			}
			pTex->UnlockRect(0);
			res = 1;
		}
		pTex->Release();		
	}
	return res;
}

int MyFont::ScanFont(const char* name, int hInPix)
{
	int res = 0;
	IDirect3DTexture9*	pTex = 0;
	IDirect3DSurface9* pSurf = 0;
	//D3DXIMAGE_INFO inf;
	//size_t *pFirstTexel = 0, *pCurrTexel = 0, *pEndTexel = 0;	
	//size_t total = 0, texlInX = 0, currW = 0, cntX = 0, cntY = 0, prevX = 0;
	D3DLOCKED_RECT rcLock;
	SIZE size;
	//TEXTMETRIC tm;
	//D3DLOCKED_RECT rcSurf;
	HDC hDC = 0;
	LOGFONT m_lf;
	HFONT hFont;
	//int hFont = 0;
	
	strncpy(m_name, name, MAXCHAR_FONTNAME);
	HR3D(D3DXCreateTexture(gd3dDevice, 256, 256, 0, 0, D3DFMT_A8R8G8B8,  D3DPOOL_SYSTEMMEM, &pTex));
	if(pTex == 0) return res;
	HR3D(pTex->GetSurfaceLevel(0, &pSurf))	
	//HR3D(pSurf->GetDC(&hDC));
	//m_h = MulDiv(hInPix, GetDeviceCaps(hDC, LOGPIXELSY), 72);	
	//m_h = hInPix;
	
	m_lf.lfHeight         = -hInPix;//-MulDiv(hInPix, GetDeviceCaps(hDC, LOGPIXELSY), 72); //
	m_lf.lfWidth          = 0;
	m_lf.lfEscapement     = 0;
	m_lf.lfOrientation    = 0;
	m_lf.lfWeight         = FW_NORMAL; //; //  FW_BOLD
	m_lf.lfItalic         = 0;
	m_lf.lfUnderline      = FALSE;
	m_lf.lfStrikeOut      = FALSE;
	m_lf.lfCharSet        = DEFAULT_CHARSET ; // ANSI_CHARSET
	m_lf.lfOutPrecision   = OUT_DEFAULT_PRECIS; // OUT_TT_PRECIS OUT_RASTER_PRECIS  OUT_OUTLINE_PRECIS OUT_CHARACTER_PRECIS;  ;
	m_lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS; // 
	m_lf.lfQuality        = NONANTIALIASED_QUALITY; //DEFAULT_QUALITY  ;PROOF_QUALITY ; 
	m_lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;	
	strncpy(m_lf.lfFaceName, m_name, LF_FACESIZE-1);
	
	hFont = CreateFontIndirect(&m_lf);
	//hFont = CreateFont(-hF, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, "Courier New");
	if(hFont == NULL) return res;
	
	//SelectObject(hDC, hFont);	
	//SetTextAlign(hDC, TA_TOP | TA_LEFT);
	//SetBkColor(hDC,   RGB(0xFF, 0xFF, 0xFF)); 
	//GetTextMetrics(hDC, &tm);
	//m_h = tm.tmHeight;
	//SetTextColor(hDC, RGB(0, 0, 0));	
	//SetBkMode(hDC, TRANSPARENT);
	//pSurf->ReleaseDC(hDC);

	for(unsigned int i = 0; i < 256; ++i)
	{
		HR3D(pSurf->GetDC(&hDC));
		SelectObject(hDC, hFont);
		GetTextExtentPoint32(hDC, (LPCSTR)&i, 1, &size);
		if(size.cx >= SYMB_COL || size.cy > SYMB_ROWS) return res;
		m_h = size.cy;
		TextOut(hDC, 0, 0, (LPCSTR)&i, 1);
		pSurf->ReleaseDC(hDC);

		pSurf->LockRect(&rcLock, NULL , D3DLOCK_READONLY);
		scanSymbol(&m_arrSymbDefs[i], size.cx, rcLock.Pitch >> 2, (size_t* )rcLock.pBits);
		pSurf->UnlockRect();

		//HR3D(D3DXSaveTextureToFile("tex.bmp", D3DXIFF_BMP, pTex, 0));
	}
	pSurf->Release();	
	pTex->Release();
	DeleteObject(hFont);
	res = 1;

	return res;
}

void MyFont::defineSymbol(SymbolDef* pSymbDef, size_t w, size_t pitch, size_t* pTexl)
{
	size_t cntX = 0; //, cntY = 0;
	pSymbDef->width = w;

	for(size_t y = 0 ; y != m_h; ++y)
	{
		for(size_t x = 0 ; x != w; ++x)
		{
			if(*(pTexl + x) == 0)
			{
				pSymbDef->lines[y].data[cntX] = (unsigned char)x;
				cntX++;
			}
		}
		pTexl += pitch;
		pSymbDef->lines[y].cnt = cntX;
		cntX = 0; // после^^^
	}
}

void MyFont::scanSymbol(SymbolDef* pSymbDef, size_t w, size_t pitchInPix, size_t* pTexl)
{
	size_t cntX = 0;
	pSymbDef->width = w;

	for(size_t y = 0 ; y != m_h; ++y)
	{
		for(size_t x = 0 ; x != w; ++x)
		{
			if(*(pTexl + x) == 0)
			{
				pSymbDef->lines[y].data[cntX] = (unsigned char)x;
				cntX++;
			}
		}
		pTexl += pitchInPix;
		pSymbDef->lines[y].cnt = cntX;
		cntX = 0; // после^^^
	}
}

void MyFont::drawChar(unsigned char ch, size_t w, size_t color, size_t clrBckgnd, size_t* pDst, size_t pitchDst)
{
	SymbolDef* pSymbDef = &m_arrSymbDefs[ch];
	for(size_t y = 0 ; y != m_h; ++y)
	{
		if(clrBckgnd != 0) Mem_Set_QUAD(pDst, clrBckgnd, w); // надо ли?!
		for(size_t x = 0 ; x != pSymbDef->lines[y].cnt; ++x)
		{
			*(pDst + pSymbDef->lines[y].data[x]) = color;	 // argb		
		}
		pDst += pitchDst;
	}
}

void MyFont::DrawStringM2(int y, TEXTROW &textrow, RECT &rcClip, size_t align, 
					const TMItem *pTexItemDst, size_t clrFont, size_t clrBkg)
{
	const char* ch = textrow.text;
	size_t* pDstTexel = 0;	
	int currX = 0, symW = 0;
	D3DLOCKED_RECT rcLock;
	
	if(y < rcClip.top || y > rcClip.bottom) return;
//----------------------------------------------------------	
	currX = rcClip.left;
	if(((align >> 16) & ALIGN_RIGHT) != 0) currX = rcClip.right - textrow.width;
	if(((align >> 16) & ALIGN_MIDDLE_X) != 0) currX = ((rcClip.right - rcClip.left) - textrow.width) >> 1;
//-----------------------------------------------------------
	
	HR3D(pTexItemDst->_pTex->LockRect(0, &rcLock, 0, 0));
	pDstTexel = (size_t* )rcLock.pBits + (y * (rcLock.Pitch >> 2));

	for(int i = 0 ; i < textrow.count; ++i)
	{
		currX += symW;
		symW = m_arrSymbDefs[(unsigned char)*ch].width;

		if(currX < rcClip.left) continue;		
		drawChar((unsigned char)*ch, symW, clrFont, clrBkg, pDstTexel + currX, rcLock.Pitch >> 2);	
		ch++;
	}	
	pTexItemDst->_pTex->UnlockRect(0);
}

void MyFont::DrawTextM2(/*int x, int y,*/ TEXTBLOCK* pTextBlock, RECT &rcClip, size_t align, 
					const TMItem *pTexItemDst, size_t clrFont, size_t clrBkg, bool bWrap)
{	
	TEXTROW tr;
	int currY = 0;
//-----------------------------------------------------------
	currY = rcClip.top;
	if(((align >> 16) & ALIGN_BOTTOM) != 0) currY = rcClip.bottom - (m_h * pTextBlock->cntRow);
	if(((align >> 16) & ALIGN_MIDDLE_Y) != 0) currY = ((rcClip.bottom - rcClip.top) - (m_h * pTextBlock->cntRow)) >> 1;
//-----------------------------------------------------------
	
	for(int i = 0; i < pTextBlock->cntRow; ++i)
	{
		tr = pTextBlock->rows[i];		
		DrawStringM2(currY, tr, rcClip, align, pTexItemDst, clrFont, clrBkg);	
		currY += m_h;
		if(bWrap == 0) break;
	}
}

const char* MyFont::ProcessString(const char* str, const RECT &rcClip, TEXTROW &textrow)
{
	const char* prev = 0;
	size_t sz = 0;
	int razn = 0;
	int cnt = 0, symW = 0;

	textrow.count = textrow.width = 0;
	textrow.text = str;

	for( ; *str != 0; ++str)
	{
			
		
		symW = m_arrSymbDefs[(unsigned char)*str].width;
		if(*str == ' ') 
		{
			prev = str + 1;
			cnt = textrow.count;
			sz = textrow.width;
		}
		razn = rcClip.right - textrow.width;
		if(razn < symW) 
		{
			if(prev != 0) // если был пробел
			{
				textrow.count = cnt;
				textrow.width = sz;
				str = prev;
			}
			break;
		}		

		textrow.width += symW;
		textrow.count++;

		if(*str == '\n') { str++; break; }	
	}
	return str;
}

void MyFont::ProcessText(const char* str, const RECT &rcClip, TEXTBLOCK* pTextBlock, bool bWrap)
{
	int idx = 0;
	bool next = 1;
	TEXTROW tr;
	
	while(next != 0)
	{
		str = ProcessString(str, rcClip, tr);
		if(tr.count != 0) 
		{
			pTextBlock->rows[idx] = tr;
			idx++;
		}
		if(*str == 0 || bWrap == 0 || idx == MAX_ROWS) next = 0;		
	}
	pTextBlock->cntRow = idx;
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

//int MyFont::CharOut(/*HDC hDC, */int x, int y, int ch, KFontHeader20* pH, int sx, int sy)
//{
//	GLYPHINFO_20 * pGlyph = (GLYPHINFO_20 *) ( (BYTE *) & pH->BitsOffset + 5);
//	
//	if ( (ch<pH->FirstChar) || (ch>pH->LastChar) )
//		ch = pH->DefaultChar;
//	else
//		ch -= pH->FirstChar;
//
//	int width  = pGlyph[ch].GIwidth;
//	int height = pH->PixHeight;
//
//	struct { BITMAPINFOHEADER bmiHeader; RGBQUAD bmiColors[2]; } dib = 
//	{ 
//		{ sizeof(BITMAPINFOHEADER), width, -height, 1, 1, BI_RGB }, 
//		{ { 0xFF, 0xFF, 0xFF, 0 }, { 0, 0, 0, 0 } }
//	};
//    
//	int bpl = ( width + 31 ) / 32 * 4;
//	BYTE data[64/8*64]; // enough for 64x64
//
//	const BYTE * pPixel = (const BYTE *) pH + pGlyph[ch].GIoffset;
//	
//	for (int i=0; i<(width+7)/8; i++)
//	for (int j=0; j<height; j++)
//		data[bpl * j + i] = * pPixel ++;
//
//	/*StretchDIBits(hDC, x, y, width * sx, height * sy, 0, 0, width, height, 
//					   data, (BITMAPINFO *) & dib, DIB_RGB_COLORS, SRCCOPY);*/
//
//	return width * sx;
//}

//void MyFont::OutSymbols()
//{
//	HANDLE hFile = 0;
//	HANDLE hMap = 0;
//	void* pData = 0;
//
//	hFile = CreateFile(m_name, GENERIC_READ /*| GENERIC_WRITE*/, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//
//	if ( hFile != INVALID_HANDLE_VALUE )
//	{
//		hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
//		pData = MapViewOfFile(hMap, FILE_MAP_READ , 0, 0, 0);
//
//		CharOut(0, 0, '1', (KFontHeader20* )pData);
//
//		UnMapFile(hMap, pData);
//		CloseHandle(hFile);
//	}
//}

MyFont::~MyFont(void)
{
	//Destroy();
}

void MyFont::Destroy(void)
{
}

//} // end namespace