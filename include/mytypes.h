#pragma once

#include "Debug_first.h"
#include "d3dUtil.h"
#include <bitset>
#include "Debug_last.h"

class  Camera;

#pragma warning(disable : 4996)

struct SpotLight
{
	D3DXCOLOR	ambient;
	D3DXCOLOR	diffuse;
	D3DXCOLOR	spec;
	D3DXVECTOR3 posW;
	D3DXVECTOR3 dirW;  
	float		spotPower;
};

// У каждого типа объекта свой набор действий
// Для каждого набора действий свой скрипт
enum DYN_OBJ_STATE : DWORD
{	
	ACT_STAY		= 1,
	ACT_WALK		= 2,
	ACT_RUN			= 4,
	ACT_JMPFLY		= 8,
	ACT_SLEEP		= 16,
	ACT_SCAN		= 32,
	ACT_SIT			= 64,
	ACT_LIE			= 128,
	ACT_CREEP		= 256,
	bNEW_TAREGT		= 512
};

enum MSG_FLAGS : DWORD
{	
	MFLG_VISIBLY	= 1,
	MFLG_HIT		= 2
	/*MFLG_RUN			= 4,
	MFLG_JMPFLY		= 8,
	MFLG_SLEEP		= 16,
	MFLG_SCAN		= 32,
	MFLG_SIT			= 64,
	MFLG_LIE			= 128,
	MFLG_CREEP		= 256*/
};

enum TYPE_OBJ : WORD // порядок не менять!!!
{	
	CONST_OBJ,
	STATIC_OBJ,
	DYNAMIC_OBJ,
	TERRAIN_OBJ,	
	CAMERA_OBJ,
	MSG_END_UPDATE	
};

//enum MSG_SUBTYPE : WORD
//{
//	//OBJ_HEIGHT,
//	//OBJ_HIT,
//	//CAM_HEIGHT,
//	//TER_SSVIS,
//	TER_REG
//};

struct MY_MSG // 44 bytes
{	
	DWORD					type;	
	DWORD					*pObj;
	DWORD					DmgType;		//		| энерг | радио | огон | физ |	
	DWORD					flags;
	int						param1;
	D3DXVECTOR3				pt;
	D3DXVECTOR3				dir;		
};

struct TerrainSubSector
{		
	int					nRow;
	int					nCol;
	bool				visible;
	//DWORD				nVerts;
	//DWORD				nFaces;	
	//float				distToCam;
	//DWORD				setNumV;
	AABB				m_bb;
	DWORD				dTexInd;
	Mtrl				mtrl;

	ID3DXPMesh			*pPMesh;
	ID3DXMesh			*optMesh;

	float				heightHit;
	bool				hit;

	//float				cx; // = row / (bb.maxPt.x - bb.minPt.x);
	//float				cz; //= col / (bb.maxPt.z - bb.minPt.z);
	float				toPosX; // = -bb.minPt.x;
	float				toPosZ; // = -bb.minPt.z;

	std::vector<DWORD>	heightMap;	

	TerrainSubSector(AABB &bb);
	float GetHeight(float x, float z);
};

struct SubBox
{	
	DWORD				bActive;
	DWORD				bVisible;	
	TerrainSubSector	*subSec;
	std::vector<MY_MSG>	vMsg;	

	SubBox();	
	void SB_DoCollision();
};

struct TEXMNGR_ITEM
{
	TEXMNGR_ITEM() : numRef(0), pTex(0){}
	DWORD				numRef;
	//Mtrl				mtrl;
	char				cTexName[32];
	LPDIRECT3DTEXTURE9	pTex;
};

struct SHDRMNGR_ITEM
{
	SHDRMNGR_ITEM() : fx(0){}	
	std::string			fnFX;
	ID3DXEffect			*fx;	
	//D3DXHANDLE			hFX;
};

struct SHORTSTR12
{
	SHORTSTR12(){};
	SHORTSTR12(char *str)
	{
		strcpy((char *)this, str);
	}
	bool operator ==(SHORTSTR12 &str)
	{
		if((str1 != str.str1) || (str2 != str.str2)  || (str3 != str.str3)) return 0;
		else return 1;
	}
	DWORD str1;
	DWORD str2;
	DWORD str3;
};



struct MYFACE
{
	UINT a;
	UINT b;
	UINT c;
};

//struct SCENE_ITEM
//{
//	SCENE_ITEM()
//	{
//		D3DXMatrixIdentity(&posW);
//	}
//	DWORD			type;
//	D3DXMATRIX		posW;
//	char			fileMesh[16];
//	char			script[16];
//	char			fileFX[16];
//		
//};

struct MY_ANIMSET
{
	char  name[32];
	float tickAnimCount;
	float numSamples;
	float deltaTickSamples;
	std::vector<D3DXMATRIX> vAnimMatrices;
	//std::vector<D3DXQUATERNION> vAnimMatrices;
};

struct MY_ANIMSET_OPTIONS
{
	char  name[32];
	float startFrame;
	float endFrame;		
};

struct REMAPVERTEXIND
{
	UINT newInd;
	UINT oldInd;
};
struct MYBONE
{
    char            name[32];
	D3DXMATRIX		toRoot;
    D3DXMATRIX      TransformationMatrix;
	D3DXMATRIX      boneOffsetTransform;
	AABB			boundBox;
	DWORD			numChild;
	DWORD			arrayChildInd[5]; // 8
};

//struct EffectBlock
//{
//	SHORTSTR12								nameFx;
//	ID3DXEffect								*mFX;
//	D3DXHANDLE								mhPosW;	
//	D3DXHANDLE								mhTex0;	
//	D3DXHANDLE								mhTex1;	
//	D3DXHANDLE								mhTex2;	
//	D3DXHANDLE								mhFinalTMs;	
//	D3DXHANDLE								mhParameters;
//	D3DXHANDLE								mhVP;		
//	//D3DXHANDLE								mhPosW;
//};

enum MYANCTRL_OBJ_STATE : DWORD
{	
	OBS_ANIMATE		= 0,
	OBS_VISIBLE		= 2,
	OBS_SELECTED	= 4
};

struct MY_ANIMMESH // оптимизировать структуры по cache line!
{
	MY_ANIMMESH()
	{
		strcpy(name, "Default");
		//state |= OBS_ANIMATE | OBS_VISIBLE;	// ACS_NOANIMATE | 
		D3DXMatrixIdentity(&this->worldTM);
		strcpy(nameAnimSet, "Default");
		indxAnimSet = DWORD_MAX;
		cyclAnimation = false;
		elapsedTick = 0.0f;
	}

	char					name[32];	
	std::bitset<32>			state;	
	D3DXMATRIX				worldTM;
	char					nameAnimSet[32];
	DWORD					indxAnimSet;
	bool					cyclAnimation;
	float					elapsedTick;
	std::vector<D3DXMATRIX>	vFinalTMs;
	AABB					mainAABB;
	std::vector<AABB>		vBonesAABB;
};