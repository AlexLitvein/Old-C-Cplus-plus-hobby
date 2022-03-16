#include "MyTerrain.h"

MyTerrain *g_pMyTerrain = 0;

MyTerrain::MyTerrain(void)
{	
}

//float MyTerrain::GetHeight(float x, float z)
//{
//	TerrainSector *trn = vTrnSecPtrs[indTS_CamIn];
//	DWORD i = trn->vSubSec.size();
//	for(; i != 0; --i)
//	{
//		if(TestPtInAABB(D3DXVECTOR3(x, 0.0f, z), trn->vSubSec[i-1]->m_bb)) return trn->vSubSec[i-1]->GetHeight(x, z);
//	}
//}

void MyTerrain::Draw()
{
	HR(mFX->SetMatrix(mhViewProj, &gCamera->viewProj()));
	HR(mFX->SetTexture(mhTex0, gTexMngr->GetTexturePtr(indTex0)));
	HR(mFX->SetTexture(mhTex1, gTexMngr->GetTexturePtr(indTex1)));
	HR(mFX->SetTexture(mhTex2, gTexMngr->GetTexturePtr(indTex2)));
	HR(mFX->SetTexture(mhBlendMap, gTexMngr->GetTexturePtr(indTexBlend)));
	//HR(mFX->SetMatrix(mhWVP, &gCamera->viewProj()));	
	//HR(mFX->SetValue(mhEyePosW, &gCamera->pos(), sizeof(D3DXVECTOR3)));

	HR(mFX->SetTechnique(mhTech));
	UINT numPasses = 0;
	HR(mFX->Begin(&numPasses, 0));
	HR(mFX->BeginPass(0));

	DWORD i = vTrnSecPtrs.size();
	for(; i != 0; --i)
	{
		if(vTrnSecPtrs[i-1]->active)
		{
			//vTrnSecPtrs[i-1]->mFX1 = mFX;
			//vTrnSecPtrs[i-1]->mhTex1 = mhTex;
			vTrnSecPtrs[i-1]->Draw();
		}
	}

	HR(mFX->EndPass());
	HR(mFX->End());
	
}
void MyTerrain::SetDirToSunW(const D3DXVECTOR3& d)
{
	HR(mFX->SetValue(mhDirToSunW, &d, sizeof(D3DXVECTOR3)));
}

void MyTerrain::Update(float dt)
{
	bool vis = 0;
	DWORD i = vTrnSecPtrs.size();
	for(; i != 0; --i)
	{
		//if(TestPtInAABB(gCamera->pos(), vTrnSecPtrs[i-1]->bbTerrSect)) indTS_CamIn = i-1; // ????
		
		vis = gCamera->isVisible(vTrnSecPtrs[i-1]->bbTerrSect);
		vTrnSecPtrs[i-1]->active = vis;
		//if(vis)
		//{
			vTrnSecPtrs[i-1]->Update(dt);
		//}		
	}
}

void MyTerrain::LoadShader()
{
	ID3DXBuffer* errors = 0;
	HR(D3DXCreateEffectFromFile(gd3dDevice, "Terrain.fx", 0, 0, D3DXSHADER_DEBUG, 0, &mFX, &errors));
	//if( errors ) MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);
	ReleaseCOM(errors);

	/*mhTech      = mFX->GetTechniqueByName("TerrainTech");
	mhWVP		= mFX->GetParameterByName(0, "gWVP");
	mhDirToSunW = mFX->GetParameterByName(0, "gDirToSunW");
	mhTex		= mFX->GetParameterByName(0, "gTex");
	mhEyePosW	= mFX->GetParameterByName(0, "gEyePosW");*/

	mhTech      = mFX->GetTechniqueByName("TerrainTech");
	mhViewProj  = mFX->GetParameterByName(0, "gViewProj");
	mhDirToSunW = mFX->GetParameterByName(0, "gDirToSunW");
	mhTex0      = mFX->GetParameterByName(0, "gTex0");
	mhTex1      = mFX->GetParameterByName(0, "gTex1");
	mhTex2      = mFX->GetParameterByName(0, "gTex2");
	mhBlendMap  = mFX->GetParameterByName(0, "gBlendMap");	

	indTex0		= gTexMngr->InsertItem("grass.dds", true);
	indTex1		= gTexMngr->InsertItem("dirt.dds", true);
	indTex2		= gTexMngr->InsertItem("stone.dds", true);
	indTexBlend = gTexMngr->InsertItem("blend_hm17.dds", true);	
	
}

int MyTerrain::Load(char *saveFileName)
{
	if(saveFileName == 0)
	{
		TerrainSector *trn = new TerrainSector("Data\\000000000000.TRN" );
		vTrnSecPtrs.push_back(trn);
	}

	LoadShader();
	// направление на солнце по умолчанию
	D3DXVECTOR3 toSun(-1.0f, 3.0f, 1.0f);
	D3DXVec3Normalize(&toSun, &toSun);
	SetDirToSunW(toSun); 


	return 0;
}

MyTerrain::~MyTerrain(void)
{
	mFX->Release();

	DWORD i = vTrnSecPtrs.size();	
    for( ; i != 0; --i)
    {
		delete vTrnSecPtrs[i-1];
    }	
}
