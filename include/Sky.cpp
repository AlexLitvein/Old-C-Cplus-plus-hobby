//=============================================================================
// Sky.cpp by Frank Luna (C) 2004 All Rights Reserved.
//=============================================================================

#include "Sky.h"
#include "Camera.h"
#include "d3dUtil.h"
#include "loadexpgeom.h"
#include "ShaderMngr.h"
#include "TextureMngr.h"

Sky *g_pSky = 0;
D3DXCOLOR g_LightColor = D3DXCOLOR(0.9f, 0.9f, 0.9f, 1.0f);

Sky::Sky() {}
int Sky::InitSky(char* envmapFilename, float skyRadius)
{
	mRadius = skyRadius;
	envMapFn = envmapFilename;
	// This function creates a mesh with the D3DXMESH_MANAGED creation 
	// option and D3DFVF_XYZ | D3DFVF_NORMAL flexible vertex format (FVF).
	HR(D3DXCreateSphere(g_pD3Device, skyRadius, 30, 30, &mSphere, 0));	
	//CHECKVAL(gShdrMngr, "failed -> gShdrMngr! ");
	HR(D3DXCreateCubeTextureFromFileEx(g_pD3Device, (g_pTexMngr->m_PathTex + envMapFn).c_str(), D3DX_DEFAULT, 
										D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, 
										D3DX_DEFAULT, 0, NULL, NULL, &mEnvMap));

	mFX = gShdrMngr->InsertItem("sky.fx");
	
	mhTech		= mFX->GetTechniqueByName("SkyTech");
	mhWVP		= mFX->GetParameterByName(0, "gWVP");
	mhEnvMap	= mFX->GetParameterByName(0, "texSkyEnvMap");
	mhSpotLight	= mFX->GetParameterByName(0, "spotLight");
	mhLightVP	= mFX->GetParameterByName(0, "matLightVP");	

	mLight.ambient   = D3DXCOLOR(0.9f, 0.9f, 0.9f, 1.0f);
	mLight.diffuse   = D3DXCOLOR(0.9f, 0.9f, 0.9f, 1.0f);
	mLight.spec      = D3DXCOLOR(0.9f, 0.9f, 0.9f, 1.0f);
	mLight.spotPower = 20.1f;
	mLight.posW = D3DXVECTOR3(10.0f, 10.0f, -10.0f);
	
	//fromLCam.setLens(D3DX_PI * 0.25f, 800/600, 1.0f, 1000.0f);

	// Set effect parameters that do not vary.
	HR(mFX->SetTechnique(mhTech));
	HR(mFX->SetTexture(mhEnvMap, mEnvMap));	
	
	return 1;
}
void Sky::CalcLightVP()
{
	//-----------------------------------------------------
	//float dist = 10.0f;
	//D3DXVECTOR3 projTrgt;// = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//D3DXVECTOR3 camDir = gCamera->mLookW;
	//D3DXVECTOR3 camPos = gCamera->mPosW;
	//if(camDir.x < 0) projTrgt.x = camPos.x - dist;
	//else projTrgt.x = camPos.x + dist;
	//if(camDir.y < 0) projTrgt.y = camPos.y - dist;
	//else projTrgt.y = camPos.y + dist;
	//if(camDir.z < 0) projTrgt.z = camPos.z - dist;
	//else projTrgt.z = camPos.z + dist;


	//D3DXMATRIX projTrgtTM;	
	//D3DXMatrixIdentity(&projTrgtTM);
	//projTrgtTM._41 = projTrgt.x;
	//projTrgtTM._42 = projTrgt.y;
	//projTrgtTM._43 = projTrgt.z;

	//D3DXMATRIX mToProjTrgt;
	//D3DXMatrixInverse(&mToProjTrgt, 0, &projTrgtTM);	

	//
	//// переносим камеру в пространство цели
	//D3DXVECTOR3 camPosTrgt;
	//D3DXVec3TransformCoord(&camPosTrgt, &camPos, &mToProjTrgt);
	//
	//
	//
	//D3DXVECTOR3 orto, res;	
	//D3DXVec3Normalize(&mLight.dirW, &(-mLight.dirW));
	//float angle = D3DXVec3Dot(&camDir, &mLight.dirW);
	//D3DXVec3Cross(&orto, &camDir, &mLight.dirW);
	//
	//D3DXMATRIX m;
	//float grad = RAD_TO_DEG(acosf(angle));
	//D3DXMatrixRotationAxis(&m, &orto, DEG_TO_RAD(grad));
	//// вращаем камеру вокруг цели (выравниваем по напр. света)
	//D3DXVec3TransformCoord(&res, &camPosTrgt, &m);
	//// переносим обратно в мировое пр.
	//D3DXVec3TransformCoord(&res, &res, &projTrgtTM);

	//mLightVP._41 = res.x;
	//mLightVP._42 = res.y;
	//mLightVP._43 = res.z;

	//--------------------------------------------------------

	D3DXMATRIX lightView;	
	D3DXVECTOR3 lightTargetW(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 lightUpW(0.0f, 1.0f, 0.0f);	
	mLight.dirW = - mLight.posW;		

	D3DXMatrixLookAtLH(&lightView, &/*res*/mLight.posW, &lightTargetW, &lightUpW);	
	D3DXMATRIX lightLens;
	float lightFOV = D3DX_PI*0.25f;
	D3DXMatrixPerspectiveFovLH(&lightLens, lightFOV, 1.0f, 1.0f, 200.0f); 
	mLightVP = lightView * lightLens;	
}
void Sky::SetPosLight(D3DXVECTOR3 &pos)
{
	mLight.posW = pos;
	//CalcLightVP();
}
void Sky::Update(float dt)
{
	/*static float time = 0.0f;
	time += dt;

	mLight.posW.x =  3.0f*cosf(time);
	mLight.posW.y =  -1.0f;
	mLight.posW.z =  3.0f*sinf(time);*/
	mLight.ambient = mLight.diffuse = mLight.spec = g_LightColor;

	CalcLightVP(); // после операций с pos
	// Sky always centered about camera's position.
	D3DXMATRIX W;
	D3DXVECTOR3 p = gCamera->pos();	
	D3DXMatrixTranslation(&W, p.x, p.y, p.z);

	HR(mFX->SetMatrix(mhWVP, &(W * gCamera->viewProj())));
	HR(mFX->SetMatrix(mhLightVP, &mLightVP));
	HR(mFX->SetValue(mhSpotLight, &mLight, sizeof(SpotLight))); 
}
Sky::~Sky()
{
	ReleaseCOM(mSphere);
	ReleaseCOM(mEnvMap);	
}

DWORD Sky::getNumTriangles()
{
	return mSphere->GetNumFaces();
}

DWORD Sky::getNumVertices()
{
	return mSphere->GetNumVertices();
}

IDirect3DCubeTexture9* Sky::getEnvMap()
{
	return mEnvMap;
}

float Sky::getRadius()
{
	return mRadius;
}

void Sky::onLostDevice()
{
	mEnvMap->Release();
	HR(mFX->OnLostDevice());
}

void Sky::onResetDevice()
{
	HR(mFX->OnResetDevice());
	//HR(D3DXCreateCubeTextureFromFile(g_pD3Device, (g_pTexMngr->m_PathTex + envmapFilename).c_str(), &mEnvMap));
	/*HR(D3DXCreateCubeTextureFromFileEx(g_pD3Device, (g_pTexMngr->m_PathTex + envMapFn).c_str(), D3DX_DEFAULT, 
										D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, 
										D3DX_DEFAULT, 0, NULL, NULL, &mEnvMap));	*/
}

void Sky::draw()
{	
	UINT numPasses = 0;
	HR(mFX->Begin(&numPasses, 0));
	HR(mFX->BeginPass(0));
	HR(mSphere->DrawSubset(0));
	HR(mFX->EndPass());
	HR(mFX->End());
}