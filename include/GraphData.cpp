#include "GraphData.h"
#include "MyObject.h"
#include "ShaderMngr.h"
#include "MyRender.h"
#include "Sky.h"
#include "ObjMngr.h"
#include "Vertex.h"

float g_ShadEPSIL = 0.00015f;

BaseRenderObj::BaseRenderObj(void)
{
	//Write_Log_String(hLog, "конструктор по умолч BaseRenderObj\n");	
}
BaseRenderObj::~BaseRenderObj(void)
{	
	//Write_Log_String(hLog, "вызов ~BaseRenderObj(void)\n");
	optMesh->Release();
	//pPMesh->Release();
}

BaseRenderObj::BaseRenderObj(const char *fileObj, const char *fileFX/*, char *name*/)
{
	//Write_Log_String(hLog, "конструктор BaseRenderObj(char *fileObj, char *fileFX)\n");
	optMesh = 0;
	//char texName[128];
	std::vector<std::string> vTexStr;
	fname = fileObj;
	fFX = fileFX;
	std::string file = g_pObjMngr->m_PathObj + fileObj;
	
	int res = CreateMyMesh(file.c_str(), 0, D3DXMESH_MANAGED | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE, &optMesh);
	
	if(res == 0) MessageBox(0, fileObj, "Failed create object!", 0);

	if(optMesh != 0)
	{
		DWORD* adjacencyInfo = new DWORD[optMesh->GetNumFaces() * 3];
		HR(optMesh->GenerateAdjacency(0.0f, adjacencyInfo));
		//HR(D3DXGeneratePMesh(optMesh, adjacencyInfo, NULL, 0, 100, D3DXMESHSIMP_VERTEX, &pPMesh));
		delete [] adjacencyInfo;
		
		LoadBoundBox(file.c_str(), bb);
		// ≈сли дл€ объекта не определен материал, а следовательно  
		// и текстура,то возвращаетс€ строка "Whitetex.bmp"
		LoadMtrlAndTexStr(file.c_str(), m, vTexStr);

		for(int i = 0; i < vTexStr.size(); ++i)
		{
			int fnd = vTexStr[i].find_last_of("\\");
			vTexStr[i].erase(0, fnd + 1);
			pTex[i] = g_pTexMngr->InsertTex(vTexStr[i].c_str(), true);
		}
		//std::string texOut(texName);
		//int fnd = texOut.find_last_of("\\");
		//texOut.erase(0, fnd + 1);
		////texOut.insert(0, "Media\\Tex\\");
		//pTex = g_pTexMngr->InsertTex(texOut, true);
		LoadShader(fileFX);
	}
}

// ¬озвращает индекс указател€ на объект MyObject, используетс€ дл€
// последующего удалени€ указател€ по его индексу
DWORD BaseRenderObj::AddObject(MyObject *pObj)
{
	if(vFreeInd.size() != 0)
	{
		DWORD ind = vFreeInd.back();
		vObjPtrs[ind] = pObj;
		vFreeInd.pop_back();
		return ind;
	}
	else 
	{
		vObjPtrs.push_back(pObj);
		return vObjPtrs.size()-1;
	}
}
void BaseRenderObj::DeleteObject(DWORD ind)
{
	vObjPtrs[ind] = 0;
	vFreeInd.push_back(ind);
}

void BaseRenderObj::Draw()
{
}
bool BaseRenderObj::LoadShader(const char *fileFX) 
{
	mFX = gShdrMngr->InsertItem(fileFX);
	return 1;
}

//=======================================================================
PhongRenderObj::PhongRenderObj(void)
{
	//Write_Log_String(hLog, "конструктор PhongRenderObj(void)\n");
}
PhongRenderObj::~PhongRenderObj(void)
{
	//Write_Log_String(hLog, "деструктор ~PhongRenderObj(void)\n");
}
PhongRenderObj::PhongRenderObj(const char *fileObj, const char *fx) : BaseRenderObj(fileObj, fx)
{
	//Write_Log_String(hLog, "вызов конструктор PhongRenderObj(char *fileObj, char *fx)\n");	
	LoadShader(fx);
}

void PhongRenderObj::Draw()
{
	DWORD i = vObjPtrs.size();
	
	HR(mFX->SetTechnique(mhTech));		
	HR(mFX->SetValue(mhSpecPower, &m.specPower, sizeof(float))); 
	HR(mFX->SetValue(mhReflectivity, &m.reflectivity, sizeof(float))); 
	HR(mFX->SetTexture(mhTex, pTex[0])); 

	UINT numPasses = 0;
	HR(mFX->Begin(&numPasses, 0));
	HR(mFX->BeginPass(0));	
	
	for(; i !=0; --i)
	{
		if(vObjPtrs[i - 1]->visible)
		{			
			HR(mFX->SetMatrix(mhPosW, &vObjPtrs[i - 1]->worldTM));
			HR(mFX->CommitChanges());
			optMesh->DrawSubset(0);
			//pPMesh->SetNumVertices(vObjPtrs[i - 1]->setNumV);			
			//pPMesh->DrawSubset(0);
		}
	}
	HR(mFX->EndPass());
	HR(mFX->End());
}

bool PhongRenderObj::LoadShader(const char *fileFX)
{	
	BaseRenderObj::LoadShader(fileFX);

	mhTech			= mFX->GetTechniqueByName("Textured_Phong");
	mhSpecPower		= mFX->GetParameterByName(0, "specPower");
	mhReflectivity	= mFX->GetParameterByName(0, "reflectivity");	
	mhPosW			= mFX->GetParameterByName(0, "gWorld");
	mhTex			= mFX->GetParameterByName(0, "gTexBaseColor");
	
	return 1;
}
//========================================================================
PhongShadowObj::PhongShadowObj(void)
{	
}
PhongShadowObj::~PhongShadowObj(void)
{	
}
PhongShadowObj::PhongShadowObj(const char *fileObj, const char *fx) : BaseRenderObj(fileObj, fx)
{	
	LoadShader(fx);
}

void PhongShadowObj::Draw()
{	
	//if(mFX != p_inFX)
	//{		
		HR(mFX->SetTechnique(mhTech));		
	//}

	//HR(mFX->SetMatrix(mhVP, &gCamera->viewProj()));	
	//HR(mFX->SetValue(mhMtrl, &m, sizeof(Mtrl)));
	//HR(mFX->SetValue(mhEyePosW, &gCamera->pos(), sizeof(D3DXVECTOR3))); 
	HR(mFX->SetValue(mhShadEPSIL, &g_ShadEPSIL, sizeof(float)));	
	HR(mFX->SetTexture(mhTex, pTex[0]));	

	UINT numPasses = 0;
	HR(mFX->Begin(&numPasses, 0));
	HR(mFX->BeginPass(0));
	
	DWORD i = vObjPtrs.size();
	for(; i !=0; --i)
	{
		if(vObjPtrs[i - 1]->visible)
		{			
			HR(mFX->SetMatrix(mhWorld, &vObjPtrs[i - 1]->worldTM));				
			HR(mFX->CommitChanges());
			optMesh->DrawSubset(0);

			//pPMesh->SetNumVertices(vObjPtrs[i - 1]->setNumV);			
			//pPMesh->DrawSubset(0);
		}
	}
	HR(mFX->EndPass());
	HR(mFX->End());	
}

bool PhongShadowObj::LoadShader(const char *fileFX)
{	
	BaseRenderObj::LoadShader(fileFX);
	
	mhTech               = mFX->GetTechniqueByName("LightShadowTech");	 // 
	//mhVP                = mFX->GetParameterByName(0, "matVP");
	mhWorld              = mFX->GetParameterByName(0, "gWorld");
	//mhMtrl               = mFX->GetParameterByName(0, "gMtrl");
	mhTex                = mFX->GetParameterByName(0, "gTex");
	mhShadEPSIL          = mFX->GetParameterByName(0, "SHADOW_EPSILON");

	return 1;
}
//========================================================================
TexPhongEnvObj::TexPhongEnvObj(void)
{	
}
TexPhongEnvObj::~TexPhongEnvObj(void)
{	
}
TexPhongEnvObj::TexPhongEnvObj(const char *fileObj, const char *fx) : BaseRenderObj(fileObj, fx)
{	
	LoadShader(fx);
}

void TexPhongEnvObj::Draw()
{	
	//if(mFX != p_inFX)
	//{		
		HR(mFX->SetTechnique(mhTech));		
	//}
	static LPDIRECT3DTEXTURE9 pOldTex;

	//HR(mFX->SetMatrix(mhVP, &gCamera->viewProj()));	
	//HR(mFX->SetValue(mhMtrl, &m, sizeof(Mtrl)));
	//HR(mFX->SetValue(mhEyePosW, &gCamera->pos(), sizeof(D3DXVECTOR3))); 
	//HR(mFX->SetValue(mhLight, &g_pRender->mLight, sizeof(SpotLight)));	
	if(pTex[0] != pOldTex)
	{
		HR(mFX->SetTexture(mhTexBaseColor, pTex[0]));	
		pOldTex = pTex[0];
	}
	//HR(mFX->SetTexture(mhTexEnvColor, g_pSky->getEnvMap()));
	//mhTexEnvColor = g_pSky->mhEnvMap;

	UINT numPasses = 0;
	HR(mFX->Begin(&numPasses, 0));
	HR(mFX->BeginPass(0));
	
	DWORD i = vObjPtrs.size();
	for(; i !=0; --i)
	{
		if(vObjPtrs[i - 1]->visible)
		{			
			HR(mFX->SetMatrix(mhWorld, &vObjPtrs[i - 1]->worldTM));				
			HR(mFX->CommitChanges());
			optMesh->DrawSubset(0);
			//pPMesh->SetNumVertices(vObjPtrs[i - 1]->setNumV);			
			//pPMesh->DrawSubset(0);
		}
	}
	HR(mFX->EndPass());
	HR(mFX->End());	
}

bool TexPhongEnvObj::LoadShader(const char *fileFX)
{	
	BaseRenderObj::LoadShader(fileFX);
	
	mhTech               = mFX->GetTechniqueByName("Tex_Phong_Env");	
	mhVP                = mFX->GetParameterByName(0, "gVP");
	mhWorld              = mFX->GetParameterByName(0, "gWorld");
	//!!!mhMtrl               = mFX->GetParameterByName(0, "gMtrl");
	// fSpecularPower;
	// float gReflectivity;
	mhLight              = mFX->GetParameterByName(0, "gLight");
	mhEyePosW            = mFX->GetParameterByName(0, "gEyePosW");
	mhTexBaseColor       = mFX->GetParameterByName(0, "gTexBaseColor");
	mhTexEnvColor        = mFX->GetParameterByName(0, "gTexEnvColor");

	return 1;
}
//========================================================================
GlassRenderObj::GlassRenderObj(void)
{	
}
GlassRenderObj::~GlassRenderObj(void)
{	
}
GlassRenderObj::GlassRenderObj(const char *fileObj, const char *fx) : BaseRenderObj(fileObj, fx)
{	
	LoadShader(fx);
}

void GlassRenderObj::Draw()
{	
	// Enable alpha blending.
	HR(g_pD3Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true));
	HR(g_pD3Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO)); // D3DBLEND_SRCALPHA
	HR(g_pD3Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR)); // D3DBLEND_INVSRCALPHA D3DBLEND_ONE

	//if(mFX != p_inFX)
	//{		
		HR(mFX->SetTechnique(mhTech));		
	//}
	static LPDIRECT3DTEXTURE9 pOldTex;

	//HR(mFX->SetMatrix(mhVP, &gCamera->viewProj()));		
	//HR(mFX->SetValue(mhEyePosW, &gCamera->pos(), sizeof(D3DXVECTOR3))); 
	//HR(mFX->SetValue(mhLight, &g_pRender->mLight, sizeof(SpotLight)));	
	if(pTex[0] != pOldTex)
	{
		HR(mFX->SetTexture(mhTexBaseColor, pTex[0]));	
		pOldTex = pTex[0];
	}
	//HR(mFX->SetTexture(mhTexEnvColor, g_pSky->getEnvMap()));	

	UINT numPasses = 0;
	HR(mFX->Begin(&numPasses, 0));
	HR(mFX->BeginPass(0));
	
	DWORD i = vObjPtrs.size();
	for(; i !=0; --i)
	{
		if(vObjPtrs[i - 1]->visible)
		{			
			HR(mFX->SetMatrix(mhWorld, &vObjPtrs[i - 1]->worldTM));				
			HR(mFX->CommitChanges());
			optMesh->DrawSubset(0);
			//pPMesh->SetNumVertices(vObjPtrs[i - 1]->setNumV);			
			//pPMesh->DrawSubset(0);
		}
	}
	HR(mFX->EndPass());
	HR(mFX->End());	

	HR(g_pD3Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false));
}

bool GlassRenderObj::LoadShader(const char *fileFX)
{	
	BaseRenderObj::LoadShader(fileFX);
	
	mhTech               = mFX->GetTechniqueByName("Glass_tech");	
	//mhVP                = mFX->GetParameterByName(0, "gVP");
	mhWorld              = mFX->GetParameterByName(0, "gWorld");	
	//mhLight              = mFX->GetParameterByName(0, "gLight");
	//mhEyePosW            = mFX->GetParameterByName(0, "gEyePosW");
	mhTexBaseColor       = mFX->GetParameterByName(0, "gTexBaseColor");
	//mhTexEnvColor        = mFX->GetParameterByName(0, "gTexEnvColor");

	return 1;
}
//========================================================================
TexPhongNMEnvObj::TexPhongNMEnvObj(void)
{	
	//Write_Log_String(hLog, "конструктор по умолч TexPhongNMEnvObj");
}
TexPhongNMEnvObj::~TexPhongNMEnvObj(void)
{	
}
TexPhongNMEnvObj::TexPhongNMEnvObj(const char *fileObj, const char *fx) : BaseRenderObj(fileObj, fx)
{	
	//Write_Log_String(hLog, "конструктор с параметрами TexPhongNMEnvObj\n");
	// Get the vertex declaration for the NMapVertex.
	D3DVERTEXELEMENT9 elems[MAX_FVF_DECL_SIZE];
	UINT numElems = 0;
	HR(NMapVertex::Decl->GetDeclaration(elems, &numElems));

	
	/*int numV =  optMesh->GetNumVertices();
	int numF = optMesh->GetNumFaces();
	HR(D3DXCreateMesh(numF, numV, D3DXMESH_MANAGED, elems, g_pD3Device, &tangMesh));

	VertexPNT *v = 0;
	NMapVertex *u = 0;
	HR(optMesh->LockVertexBuffer(0,(void**)&v));		
	HR(tangMesh->LockVertexBuffer(0,(void**)&u));

	for(int i = 0; i < numV; ++i)
	{
		u[i].pos	= v[i].pos;
		u[i].normal = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		u[i].tangent = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		u[i].binormal = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		u[i].tex0	= v[i].tex0;
	}

	HR(tangMesh->UnlockVertexBuffer());
	HR(optMesh->UnlockVertexBuffer());

	WORD *k = 0;
	WORD *r = 0;
	HR(optMesh->LockIndexBuffer(0,(void**)&k));		
	HR(tangMesh->LockIndexBuffer(0,(void**)&r));
	for(int x = 0; x < numF * 3; x +=3 )
	{		
		r[x] = k[x];
		r[x+1] = k[x+1];			
		r[x+2] = k[x+2];
	}
	HR(tangMesh->UnlockIndexBuffer());
	HR(optMesh->UnlockIndexBuffer());*/


	// Clone the mesh to the NMapVertex format.
	ID3DXMesh* clonedTempMesh = 0;
	HR(optMesh->CloneMesh(D3DXMESH_MANAGED, // | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE
							elems, g_pD3Device, &clonedTempMesh));	

	/*DWORD *v = 0;		
	HR(optMesh->LockVertexBuffer(0,(void**)&v));		
	HR(optMesh->UnlockVertexBuffer());*/

	ReleaseCOM(optMesh);
	// Now use D3DXComputeTangentFrameEx to build the TNB-basis for each vertex in the mesh.  
	/*DWORD* adjacencyInfo = new DWORD[clonedTempMesh->GetNumFaces() * 3];
	HR(clonedTempMesh->GenerateAdjacency(0.0f, adjacencyInfo));*/
		
	
	HR(D3DXComputeTangentFrameEx(
	  clonedTempMesh, // Input mesh
	  D3DDECLUSAGE_TEXCOORD, 0, // Vertex element of input tex-coords.  
      D3DDECLUSAGE_BINORMAL, 0, // Vertex element to output binormal.
	  D3DDECLUSAGE_TANGENT, 0,  // Vertex element to output tangent.
      D3DDECLUSAGE_NORMAL, 0,   // Vertex element to output normal.
      0  ,///*| D3DXTANGENT_WIND_CW*/ | D3DXTANGENT_ORTHOGONALIZE_FROM_U   , // Options
      0, // Adjacency
	  0.01f, 0.25f, 0.01f, // Thresholds for handling errors
	  //-1.01f, -0.01f, -1.01f,
	  //1, 1, 1, // Thresholds for handling errors
	  &optMesh, // Output mesh
	  0));         // Vertex Remapping

	//delete [] adjacencyInfo;	

	// Done with temps.	
	ReleaseCOM(clonedTempMesh);

	LoadShader(fx);
}

void TexPhongNMEnvObj::Draw()
{	
	//if(mFX != p_inFX)
	//{		
		HR(mFX->SetTechnique(mhTech));		
	//}
	static LPDIRECT3DTEXTURE9 pOldTex;

	//HR(mFX->SetMatrix(mhVP, &gCamera->viewProj()));		
	//HR(mFX->SetValue(mhEyePosW, &gCamera->pos(), sizeof(D3DXVECTOR3))); 
	//HR(mFX->SetValue(mhLight, &g_pRender->mLight, sizeof(SpotLight)));	
	//if(pTex[0] != pOldTex)
	//{
		HR(mFX->SetTexture(mhTexBaseColor, pTex[0]));
		HR(mFX->SetTexture(mhTexNM, pTex[1]));
		pOldTex = pTex[0];
	//}
	//HR(mFX->SetTexture(mhTexEnvColor, g_pSky->getEnvMap()));
	//mhTexEnvColor = g_pSky->mhEnvMap;

	UINT numPasses = 0;
	HR(mFX->Begin(&numPasses, 0));
	HR(mFX->BeginPass(0));
	
	DWORD i = vObjPtrs.size();
	for(; i !=0; --i)
	{
		if(vObjPtrs[i - 1]->visible)
		{			
			HR(mFX->SetMatrix(mhWorld, &vObjPtrs[i - 1]->worldTM));				
			HR(mFX->CommitChanges());

			/*DWORD *x = 0;		
			HR(optMesh->LockVertexBuffer(0,(void**)&x));		
			HR(optMesh->UnlockVertexBuffer());*/


			optMesh->DrawSubset(0);
			//tangMesh->DrawSubset(0);
			//pPMesh->SetNumVertices(vObjPtrs[i - 1]->setNumV);			
			//pPMesh->DrawSubset(0);
		}
	}
	HR(mFX->EndPass());
	HR(mFX->End());	
}

bool TexPhongNMEnvObj::LoadShader(const char *fileFX)
{	
	BaseRenderObj::LoadShader(fileFX);
	
	mhTech               = mFX->GetTechniqueByName("NM_Env_Tech");	
	//mhVP                = mFX->GetParameterByName(0, "matViewProjection");
	mhWorld              = mFX->GetParameterByName(0, "matWorld");		
	//mhEyePosW            = mFX->GetParameterByName(0, "fvEyePosition");
	mhTexBaseColor       = mFX->GetParameterByName(0, "baseMap");
	mhTexNM			     = mFX->GetParameterByName(0, "bumpMap");
	//mhTexEnvColor        = mFX->GetParameterByName(0, "cubeTex");
	//mhLight              = mFX->GetParameterByName(0, "gLight");	

	return 1;
}