#pragma once

#include "Debug_first.h"
#include "d3dUtil.h"
#include <string>
#include <vector>
#include "TerrainSector.h"
#include "QueueMngr.h"
#include "Debug_last.h"

class MyTerrain
{
public:
	MyTerrain(void);	
	~MyTerrain(void);

	void Update(float dt);
	void Draw();
	int  Load(char *saveFileName); // если 0 то новая игра
	int  PreLoad(char *newSector); // в имени папки хранятся координаты сектора в формате NSWEUD 000000000000
	void SetDirToSunW(const D3DXVECTOR3& d);
	//float GetHeight(float x, float z);

	std::vector<TerrainSector *> vTrnSecPtrs;

private:

	ID3DXEffect								*mFX;	
	/*D3DXHANDLE								mhTech;
	D3DXHANDLE								mhWVP;		
	D3DXHANDLE								mhTex;	
	D3DXHANDLE								mhDirToSunW;
	D3DXHANDLE								mhEyePosW;*/
	//std::string sCurrDir;
	D3DXHANDLE   mhTech;
	D3DXHANDLE   mhViewProj;
	D3DXHANDLE   mhDirToSunW;
	D3DXHANDLE   mhTex0;
	D3DXHANDLE   mhTex1;
	D3DXHANDLE   mhTex2;
	D3DXHANDLE   mhBlendMap;

	DWORD		indTex0;
	DWORD		indTex1;
	DWORD		indTex2;
	DWORD		indTexBlend;
	DWORD									indTS_CamIn;
	
	void LoadShader();	
};
