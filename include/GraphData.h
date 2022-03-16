#ifndef GRAPHDATA_H
#define GRAPHDATA_H

#include "Debug_first.h"
#include "d3dUtil.h"
#include <vector>
#include "camera.h"
#include "loadexpgeom.h"
#include "vertex.h"
#include "TextureMngr.h"
#include "Debug_last.h"

class MyObject;

class BaseRenderObj
{
public:
	BaseRenderObj(void);
	BaseRenderObj(const char *fileObj, const char *fileFX/*, char *name*/);
	virtual ~BaseRenderObj(void);		
	
	DWORD AddObject(MyObject *pObj);
	void DeleteObject(DWORD ind);
	virtual void Draw();
	
	AABB				bb;
	ID3DXPMesh			*pPMesh;
	ID3DXMesh			*optMesh;

	std::string			fFX;
	std::string			fname;		
	Mtrl				m;
	LPDIRECT3DTEXTURE9	pTex[4];	
	ID3DXEffect			*mFX;	
	
	std::vector<MyObject *>	vObjPtrs;
	std::vector<DWORD>		vFreeInd;

	virtual bool LoadShader(const char *fileFX);		
};

class PhongRenderObj : public BaseRenderObj
{
public:
	PhongRenderObj(void);
	PhongRenderObj(const char *fileObj, const char *fx);	
	virtual ~PhongRenderObj(void);		
	virtual void Draw();
private:		
	D3DXHANDLE   mhTech;
	D3DXHANDLE   mhPosW;
	D3DXHANDLE   mhTex;	
	D3DXHANDLE   mhSpecPower;
	D3DXHANDLE   mhReflectivity;

	virtual bool LoadShader(const char *fileFX);		
};

class PhongShadowObj : public BaseRenderObj
{
public:
	PhongShadowObj(void);
	PhongShadowObj(const char *fileObj, const char *fx);
	virtual ~PhongShadowObj(void);		
	virtual void Draw();
private:	
	D3DXHANDLE   mhTech;
	//D3DXHANDLE   mhVP;
	//D3DXHANDLE   mhWorldInvTrans;
	//D3DXHANDLE   mhEyePosW;
	D3DXHANDLE   mhWorld;	// obj mat
	D3DXHANDLE   mhTex;
	//D3DXHANDLE   mhShadowMap;
	//D3DXHANDLE   mhSpecPower;
	//D3DXHANDLE   mhReflectivity;
	D3DXHANDLE   mhShadEPSIL;
	//D3DXHANDLE   mhLightVP;

	virtual bool LoadShader(const char *fileFX);		
};

class TexPhongEnvObj : public BaseRenderObj
{
public:
	TexPhongEnvObj(void);
	TexPhongEnvObj(const char *fileObj, const char *fx);
	virtual ~TexPhongEnvObj(void);		
	virtual void Draw();
private:	
	D3DXHANDLE   mhTech;
	D3DXHANDLE   mhVP;
	D3DXHANDLE   mhWorld;
	//D3DXHANDLE   mhMtrl;
	D3DXHANDLE   mhLight;
	D3DXHANDLE   mhEyePosW;	
	D3DXHANDLE   mhTexBaseColor;
	D3DXHANDLE   mhTexEnvColor;

	virtual bool LoadShader(const char *fileFX);		
};

class GlassRenderObj : public BaseRenderObj
{
public:
	GlassRenderObj(void);
	GlassRenderObj(const char *fileObj, const char *fx);
	virtual ~GlassRenderObj(void);		
	virtual void Draw();
private:	
	D3DXHANDLE   mhTech;
	D3DXHANDLE   mhVP;
	D3DXHANDLE   mhWorld;	
	D3DXHANDLE   mhLight;
	D3DXHANDLE   mhEyePosW;	
	D3DXHANDLE   mhTexBaseColor;
	D3DXHANDLE   mhTexEnvColor;

	virtual bool LoadShader(const char *fileFX);		
};

class TexPhongNMEnvObj : public BaseRenderObj
{
public:
	TexPhongNMEnvObj(void);
	TexPhongNMEnvObj(const char *fileObj, const char *fx);
	virtual ~TexPhongNMEnvObj(void);		
	virtual void Draw();
private:	
	D3DXHANDLE   mhTech;
	D3DXHANDLE   mhVP;
	D3DXHANDLE   mhWorld;
	//D3DXHANDLE   mhMtrl;
	D3DXHANDLE   mhLight;
	D3DXHANDLE   mhEyePosW;	
	D3DXHANDLE   mhTexBaseColor;
	D3DXHANDLE   mhTexEnvColor;
	D3DXHANDLE   mhTexNM;

	virtual bool LoadShader(const char *fileFX);		
};

#endif