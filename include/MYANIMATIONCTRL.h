#pragma once

#include "Debug_first.h"
#include "d3dutil.h"
#include <windows.h>
//#include "afxwin.h"
#include <vector>
#include <bitset>
#include "camera.h"
#include "vertex.h"
#include "MyHelper.h"
#include "loadexpgeom.h"
#include "D3DX9Mesh.h"
#include "Debug_last.h"

class MYANIMATIONCTRL
{
public:
	MYANIMATIONCTRL(void);
	MYANIMATIONCTRL(char *fileName);
	~MYANIMATIONCTRL(void);

	void AddObject(MY_ANIMMESH &obj);
	void DrawObjects();
	DWORD GetTotalVert() const;
	DWORD GetTotalFace() const;
	void AdvanceTime(float dt);	
	
	std::vector<MY_ANIMMESH>				vObj;	
	AABB									mainBBox;

private:
	
	char									nameObj[16];		
	DWORD									rootBoneIndx;
	float									tickPerFrame;		
	DWORD									numBones;
	
	std::vector<MYBONE>						vBoneHierarchy;
	std::vector<MY_ANIMSET>					vAnimSets;	
	//std::vector<MYFACE>						vFace;
	//template<class T>
	//std::vector<VertexPNTB>					vVert; // 
	
	Mtrl									mtrl;
	IDirect3DTexture9						*tex;

	DWORD									nVerts;
	DWORD									nFaces;
	//DWORD									dwNumFacesStrip;

	//IDirect3DVertexBuffer9					*buffV;
	//IDirect3DVertexBuffer9					*buffN;
	
	IDirect3DVertexBuffer9					*buffMainBBox;
	IDirect3DVertexBuffer9					*bonesBBoxVertBuff;
	//IDirect3DIndexBuffer9					*buffInd;
	//LPDIRECT3DINDEXBUFFER9					g_pIndexBufferOptList;
	//LPDIRECT3DINDEXBUFFER9					g_pIndexBufferOptStrip;
	
	ID3DXMesh								*optMesh;
	
	ID3DXEffect								*mFX; 
	D3DXHANDLE								mhTech;
	D3DXHANDLE								mhVP;		
	D3DXHANDLE								mhPosW;
	D3DXHANDLE								mhTex;	
	D3DXHANDLE								mhFinalTMs;	

	ID3DXEffect								*mBBoxFX;
	D3DXHANDLE								mhTechBBox;
	D3DXHANDLE								mhBBoxWVP;
	D3DXHANDLE								mhBBoxPosW;

	ID3DXEffect								*mBoneBBoxFX;
	D3DXHANDLE								mhBoneBBoxTech;
	D3DXHANDLE								mhBoneBBoxPosW;
	D3DXHANDLE								mhBoneBBoxWVP;
	D3DXHANDLE								mhBoneBBoxFinalTMs;	

private:	
	bool LoadBoneHierarchy(char *fileName);	
	bool LoadAnim(char *fileName);
	
	bool LoadShader(char *fileName);
	bool LoadShaderBBox(char *fileName);
	bool LoadShaderBoneBBox(char *fileName);

	void UpdateTransformMatrix(DWORD animSetInd, float tick);
	void UpdateToRoots(DWORD boneIndx);
	void UpdateFinalTMs(DWORD indObj);
	void MatrixLerp(DWORD indBone, float s, const D3DXMATRIX &m1, const D3DXMATRIX &m2);

	void BuildMainBBoxVertBuffer();	
	void BuildBonesBBoxVertBuffer();

	void DrawMainBBox();
	void DrawBoneBBox();
};

