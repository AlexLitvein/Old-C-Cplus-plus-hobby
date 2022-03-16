#pragma once

#include "Debug_first.h"
#include "d3dUtil.h"
#include <vector>
#include "MYANIMATIONCTRL.h"
#include "vertex.h"
#include "loadexpgeom.h"
#include "TextureMngr.h"
#include "CollisionContainer.h"
#include "QueueMngr.h"
#include "Debug_last.h"

class TerrainSector
{
public:
	TerrainSector(void);
	TerrainSector(char *file);
	~TerrainSector(void);

	void Draw();
	void Update(float dt);	
	//void Release();	

	bool									active;	// значит его видит камера
	AABB									bbTerrSect;
	DWORD									visSSecs;
	//D3DXHANDLE								mhTex1;	
	//ID3DXEffect								*mFX1;	

	std::vector<TerrainSubSector*>			vSubSec;
private:
	
	DWORD									indCBInCollContainer;
	//MyText									*txt; 
	//DWORD									indSubS_CamIn;

	void LoadShader();
	void RegisterSubSectors();
	
};
