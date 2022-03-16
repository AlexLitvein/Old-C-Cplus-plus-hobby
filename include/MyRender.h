#pragma once

#include "Debug_first.h"
#include "d3dutil.h"
#include <vector>
#include "MYANIMATIONCTRL.h"
#include "GraphData.h"
#include "mytypes.h"
#include "DrawableTex2D.h"
#include "Debug_last.h"

class MyObject;

class Render
{
public:
	void Draw();
	// вызывать после того как объект обработается менеджером объектов(нужен индекс объекта в массиве)	
	void AddRenderTarget(MyObject &obj, const char *fileObj, const char *fileFX, bool bNewMesh);
	void RemoveObject(MyObject &obj);
	void DrawShadowMap();	
	void InitRender();
	void Update(float dt);
	Render(void);
	~Render(void);
	
private:

	DrawableTex2D	*mShadowMap;	
	ID3DXEffect		*mShareFX;	
	D3DXHANDLE		mhTech;
	D3DXHANDLE		mhVP;	
	D3DXHANDLE		mhShadowMap;	
	D3DXHANDLE		mhEyePos;
	
	ID3DXEffect		*mFX;
	D3DXHANDLE		mhBuildShadowMapTech;	
	D3DXHANDLE		mhWorld;		

	std::vector<BaseRenderObj *>		vRenderData;
	//std::vector<DWORD>				freeIndIn_vGData;
	std::vector<MYANIMATIONCTRL *>	vACtrl;

	int FindDublicate(const char *fileObj, const char *fileFX);
	void buildShadowFX();	
};
