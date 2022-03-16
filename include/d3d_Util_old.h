
//=============================================================================
// d3dUtil.h by Frank Luna (C) 2005 All Rights Reserved.
//
// Contains various utility code for DirectX applications, such as, clean up
// and debugging code.
//=============================================================================

#ifndef D3DUTIL_H
#define D3DUTIL_H

// Enable extra D3D debugging in debug builds if using the debug DirectX runtime.  
// This makes D3D objects work well in the debugger watch window, but slows down 
// performance slightly.
#if defined(DEBUG) | defined(_DEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

//#include "Debug_first.h" // первый!

#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>
#include <d3dx9math.h>
#include <string>
#include <sstream>
#include <vector>
#include "MyHelper.h"

//#include "Debug_last.h"

//===============================================================
// Globals for convenient access.
//class  D3DApp;
//extern D3DApp* gd3dApp;
//extern IDirect3DDevice9* g_pD3Device;
//class  Camera;
//extern Camera* gCamera;
class  TextureMngr;
extern TextureMngr *g_pTexMngr;
//
//class  ShaderMngr;
//extern ShaderMngr *gShdrMngr;
//
//class  QueueMngr;
//extern QueueMngr *gQueueMngr;
//class CollisionContainer;
//extern CollisionContainer *gCollContainer;
//extern HANDLE hLog;
//extern ID3DXEffectPool	*g_pEffectPool;
//class Render;
//extern Render *g_pRender;
//class ObjMngr;
//extern ObjMngr *g_pObjMngr;
//class MyTerrain;
//extern MyTerrain *g_pMyTerrain;
//extern float g_time;
////extern D3DXVECTOR3 gLightPos;
//class AudioMngr;
//extern AudioMngr *g_AudioMngr;
//class UIPanel;
//extern UIPanel *g_UIPanel;
//class Sky;
//extern Sky *g_pSky;

// Старшее слово выравнивание по гориз-ли, младшее по вертикали
// MAKEDWORD(ALGN_LEFT, ALGN_BOTTOM)
enum MY_ALIGN : WORD
{
	ALGN_NO = 0,
	ALGN_LEFT,
	ALGN_RIGHT,
	ALGN_MH	,
	ALGN_FITH,
	ALGN_TOP,
	ALGN_BOTTOM,
	ALGN_MV,	
	ALGN_FITV
};

//#define SWAP(a,b,t) {t=a; a=b; b=t;}
//
//#define ReleaseCOM(x) { if(x){ x->Release();x = 0; } }
//#define MAKEDWORD(a, b) ((LONG)(((WORD)(((DWORD_PTR)(b)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(a)) & 0xffff))) << 16))
//
//#define SET_BIT(word,bit_flag)   ((word)=((word) | (bit_flag)))
//#define RESET_BIT(word,bit_flag) ((word)=((word) & (~bit_flag)))
//
#define DEG_TO_RAD(ang) ((ang)*D3DX_PI/180.0f)
#define RAD_TO_DEG(rads) ((rads)*180.0f/D3DX_PI)

//===============================================================
// Geometry generation.

void GenTriGrid(int numVertRows, int numVertCols, 
	float dx, float dz, const D3DXVECTOR3& center, 
	std::vector<D3DXVECTOR3>& verts, std::vector<DWORD>& indices);

//===============================================================
// Colors and Materials

const D3DXCOLOR WHITE(1.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR BLACK(0.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR RED(1.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR GREEN(0.0f, 1.0f, 0.0f, 1.0f);
const D3DXCOLOR BLUE(0.0f, 0.0f, 1.0f, 1.0f);

struct Mtrl
{
	Mtrl()
		:ambient(WHITE), diffuse(WHITE), spec(WHITE), specPower(60.0f), reflectivity(0.0f) {}
	Mtrl(const D3DXCOLOR& a, const D3DXCOLOR& d, 
		 const D3DXCOLOR& s, float power, float reflect)
		:ambient(a), diffuse(d), spec(s), specPower(power), reflectivity(reflect){}

	D3DXCOLOR ambient;
	D3DXCOLOR diffuse;
	D3DXCOLOR spec;
	float specPower;
	float reflectivity;
};

struct DirLight
{
	D3DXCOLOR ambient;
	D3DXCOLOR diffuse;
	D3DXCOLOR spec;
	D3DXVECTOR3 dirW;
};

//===============================================================
// .X Files

void LoadXFile(
	const std::string& filename, 
	ID3DXMesh** meshOut, 
	std::vector<Mtrl>& mtrls, 
	std::vector<IDirect3DTexture9*>& texs);

//===============================================================
// Math Constants

const float INFINITY = FLT_MAX;
const float EPSILON  = 0.001f;
const DWORD DWORD_MAX  = sizeof(DWORD);

//===============================================================
// Randomness

float GetRandomFloat(float a, float b);
void GetRandomVec(D3DXVECTOR3& out);


//===============================================================
// Bounding Volumes

struct AABB 
{
	// Initialize to an infinitely small bounding box.
	AABB()
		: minPt(INFINITY, INFINITY, INFINITY),
		  maxPt(-INFINITY, -INFINITY, -INFINITY){}

    D3DXVECTOR3 center()const
	{
		return (minPt+maxPt)*0.5f;
	}

	D3DXVECTOR3 extent()const
	{
		return (maxPt-minPt)*0.5f;
	}

	void xform(const D3DXMATRIX& M, AABB& out)
	{
		// Convert to center/extent representation.
		D3DXVECTOR3 c = center();
		D3DXVECTOR3 e = extent();

		// Transform center in usual way.
		D3DXVec3TransformCoord(&c, &c, &M);

		// Transform extent.
		D3DXMATRIX absM;
		D3DXMatrixIdentity(&absM);
		absM(0,0) = fabsf(M(0,0)); absM(0,1) = fabsf(M(0,1)); absM(0,2) = fabsf(M(0,2));
		absM(1,0) = fabsf(M(1,0)); absM(1,1) = fabsf(M(1,1)); absM(1,2) = fabsf(M(1,2));
		absM(2,0) = fabsf(M(2,0)); absM(2,1) = fabsf(M(2,1)); absM(2,2) = fabsf(M(2,2));
		D3DXVec3TransformNormal(&e, &e, &absM);

		// Convert back to AABB representation.
		out.minPt = c - e;
		out.maxPt = c + e;
	}

	D3DXVECTOR3 minPt;
	D3DXVECTOR3 maxPt;
};

struct BoundingSphere 
{
	BoundingSphere()
		: pos(0.0f, 0.0f, 0.0f), radius(0.0f){}

	D3DXVECTOR3 pos;
	float radius;
};

//===============================================================
// Debug

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
	#define HR(x)                                      \
	{                                                  \
		HRESULT hr = x;                                \
		if(FAILED(hr))                                 \
		{                                              \
			DXTrace(__FILE__, __LINE__, hr, #x, TRUE); \
		}                                              \
	}
	#endif

#else
	#ifndef HR
	#define HR(x) x;
	#endif
#endif 
//================================================================
#if defined(DEBUG) | defined(_DEBUG)
	#ifndef CHECKVAL
	#define CHECKVAL(val, str){	if( val == 0) { Write_Log_String(hLog, str); Write_Log_String(hLog, __FILE__); Write_Log_String(hLog, " %d\n", __LINE__); } }
	#endif

#else
	#ifndef CHECKVAL
	#define CHECKVAL(str, val) str, val;
	#endif
#endif 
//===============================================================
// Разное


bool  TestPtInAABB(D3DXVECTOR3 &pt, AABB &bb);

void  LoadSymbolTbl(/*char *fnSymTbl, */int symbW, int symbH, int strtIdx, int countSymb, 
				   LPDIRECT3DTEXTURE9 p_inTex, std::vector<RECT> &vCharsTexCoord);

float Fast_Distance_3D(float fx, float fy, float fz);
void  getWorldPickingRay(D3DXVECTOR3& originW, D3DXVECTOR3& dirW);
void  MatrixFromStr(std::string &str, D3DXMATRIX &m);
void  GetRCTexFromID(char *fileSymbTbl, int wSynb, int hSynb, int idButton, LPDIRECT3DTEXTURE9 ptex, RECT &outRC);
//bool  MyDelay(float per, float dt);
void  AlignRC(RECT* pDstRC, RECT* pSrcRC, DWORD fAlign);
//void  AlignOffsetRC(RECT* pDstRC, RECT* pSrcRC, DWORD fAlign, int offsX, int offsY);
bool  GetClipRect(RECT &rcDstIn, RECT &rcSrcInOut);
bool  GetSubSprite(POINT &pt, RECT &rcDst, RECT &rcSrcInOut);
void PrintD3DXMATRIXToLogFile(HANDLE file, D3DXMATRIX &m);
inline void TranslateRC(int x, int y, RECT &rc)
{
	rc.left += x; rc.right += x;
	rc.top += y;  rc.bottom += y;
}

#endif // D3DUTIL_H