#include "MyRender.h"
#include "MyObject.h"
#include "ShaderMngr.h"
#include "d3dApp.h"
#include "Sky.h"

Render *g_pRender = 0;
ID3DXEffectPool	*g_pEffectPool = 0;

void Render::Update(float dt)
{	
}
void Render::InitRender()
{
	D3DXCreateEffectPool(&g_pEffectPool);

	mShareFX	= gShdrMngr->InsertItem("ShareParam.fx");
	mhTech      = mShareFX->GetTechniqueByName("TechShareParam");
	mhVP		= mShareFX->GetParameterByName(0, "matVP");		
	mhShadowMap	= mShareFX->GetParameterByName(0, "texShadowMap");
	mhEyePos    = mShareFX->GetParameterByName(0, "vEyePosW");

	D3DVIEWPORT9 vp = {0, 0, 1600, 1200, 0.0f, 1.0f};
	mShadowMap = new DrawableTex2D(1600, 1200, 1, D3DFMT_R32F, true, D3DFMT_D24X8, vp, false);
	
	/*D3DVIEWPORT9 vp = {0, 0, 800, 600, 0.0f, 1.0f};
	mShadowMap = new DrawableTex2D(800, 600, 1, D3DFMT_R32F, true, D3DFMT_D24X8, vp, false);
	*/
	mShadowMap->onResetDevice();
	
	buildShadowFX();	
}
void Render::AddRenderTarget(MyObject &obj, const char *fileObj, const char *fileFX, bool bNewMesh)
{
	BaseRenderObj *pRenderObj = 0;

	int indIn_vGData = FindDublicate(fileObj, fileFX);
	if(indIn_vGData >= 0)
	{
		obj.pRendData = vRenderData[indIn_vGData];
		obj.indInRenderLst = vRenderData[indIn_vGData]->AddObject(&obj);		
	}
	else
	{
		if(strcmp(fileFX, "TexPhong.fx") == 0)
		{
			PhongRenderObj *p = new PhongRenderObj(fileObj, fileFX);
			pRenderObj = p;
			p = 0;
		}
		if(strcmp(fileFX, "TexPhongEnv.fx") == 0)
		{
			TexPhongEnvObj *p = new TexPhongEnvObj(fileObj, fileFX);
			pRenderObj = p;
			p = 0;
		}
		if(strcmp(fileFX, "PhongShadow.fx") == 0) //
		{
			PhongShadowObj *p = new PhongShadowObj(fileObj, fileFX); //"NewPhongShadow.fx" 
			pRenderObj = p;
			p = 0;
		}
		if(strcmp(fileFX, "NMTexPhongEnv.fx") == 0)
		{
			TexPhongNMEnvObj *p = new TexPhongNMEnvObj(fileObj, fileFX);
			pRenderObj = p;
			p = 0;
		}
		if(strcmp(fileFX, "Glass.fx") == 0)
		{
			GlassRenderObj *p = new GlassRenderObj(fileObj, fileFX);
			pRenderObj = p;
			p = 0;
		}
		
		// если объект удалось создать то заносим его в список... проверяем optMesh,
		// но может быть и pPMesh!!!
		if(pRenderObj->optMesh != 0)
		{
			vRenderData.push_back(pRenderObj);
			obj.pRendData = vRenderData.back();
			obj.indInRenderLst = vRenderData.back()->AddObject(&obj);
		}
	}
}

void RemoveObject(MyObject &obj)
{
	//if(obj.type == TYPE_STATIC) obj.p_graphData->.ptrInRenderLst = 0;
}

void Render::DrawShadowMap()
{
	mShadowMap->beginScene();
	HR(g_pD3Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff888888, 1.0f, 0));
 
	HR(mFX->SetTechnique(mhBuildShadowMapTech));	
	UINT numPasses = 0;
	HR(mFX->Begin(&numPasses, 0));
	HR(mFX->BeginPass(0));	
	
	BaseRenderObj *obj = 0;
	
	DWORD i = vRenderData.size();
	for(; i != 0; --i)
	{
		obj = vRenderData[i-1];
		if(obj != 0)
		{
			DWORD y = obj->vObjPtrs.size();
			for(; y != 0; --y)
			{
				HR(mFX->SetMatrix(mhWorld, &obj->vObjPtrs[y-1]->worldTM));				
				HR(mFX->CommitChanges());  //!!! Перед каждым вызовом DrawPrimitive 
				obj->optMesh->DrawSubset(0);
			}
		}
		obj = 0;
	}
	HR(mFX->EndPass());
	HR(mFX->End());

	mShadowMap->endScene();
}
void Render::Draw()
{	
	bool bEnableShadows = true; // !!!

	HR(mShareFX->SetMatrix(mhVP, &gCamera->viewProj()));	
	HR(mShareFX->SetValue(mhEyePos, &gCamera->pos(), sizeof(D3DXVECTOR3))); 
	if(bEnableShadows) HR(mShareFX->SetTexture(mhShadowMap, mShadowMap->d3dTex()));
	
	DWORD i = vRenderData.size();
	for(; i != 0; --i)
	{
		if(vRenderData[i-1] != 0)
		{
			vRenderData[i-1]->Draw();
		}
	}
}

int Render::FindDublicate(const char *fileObj, const char *fileFX)
{
	int res = -1;
	DWORD i = vRenderData.size();
	for(; i != 0; --i)
	{
		if(strcmp(fileObj, vRenderData[i-1]->fname.c_str()) == 0)
		{
			if(strcmp(fileFX, vRenderData[i-1]->fFX.c_str()) == 0)
			{
				res = i-1;	
				break;
			}
		}
	}
	return res;
}

Render::Render(void)
{	
}

Render::~Render(void)
{
	// не делаем release т.к. это делает gShdrMngr
	mFX = 0; 
	mShareFX = 0;
	delete mShadowMap;
	//delete mMyShadowMap;

	DWORD i = vRenderData.size();
	for(; i != 0; --i)
	{		
		delete vRenderData[i-1];
	}
}

void Render::buildShadowFX()
{
	mFX = gShdrMngr->InsertItem("PhongShadow.fx");	
	mhBuildShadowMapTech = mFX->GetTechniqueByName("BuildShadowMapTech");	
	mhWorld              = mFX->GetParameterByName(0, "gWorld");	
}