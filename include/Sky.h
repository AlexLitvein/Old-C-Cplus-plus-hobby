//=============================================================================
// Sky.h by Frank Luna (C) 2004 All Rights Reserved.
//=============================================================================

#ifndef SKY_H
#define SKY_H

#include "Debug_first.h"
#include <d3dx9.h>
#include <string>
#include "mytypes.h"
#include "Camera.h"
#include "Debug_last.h"

class Sky
{
public:	
	Sky();
	~Sky();
	int InitSky(char* envmapFilename, float skyRadius);
	void Update(float dt);
	void draw();
	void SetPosLight(D3DXVECTOR3 &pos);

	IDirect3DCubeTexture9* getEnvMap();
	float getRadius();
	DWORD getNumTriangles();
	DWORD getNumVertices();

	void onLostDevice();
	void onResetDevice();	

	D3DXMATRIX				mLightVP;
	//Camera					fromLCam;
	SpotLight				mLight;	

private:
	
	std::string				envMapFn;
	void CalcLightVP();
	ID3DXMesh*				mSphere;	
	float					mRadius;
	IDirect3DCubeTexture9*	mEnvMap;

	ID3DXEffect*			mFX;
	D3DXHANDLE				mhTech;	
	D3DXHANDLE				mhEnvMap;
	D3DXHANDLE				mhSpotLight;
	D3DXHANDLE				mhLightVP;	
	D3DXHANDLE				mhWVP; // дл€ трансл€ции небесной сферы
};

#endif // SKY_H