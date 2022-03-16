//#include "UIPanel.h"
#include "UISystemDef.h"
#include "d3dApp.h"
//namespace UIPanel
//{

UIPanelNew* g_pUIPanel = 0;

UIPanelNew::UIPanelNew(void) : m_w(0), m_h(0), m_cntWnd(0), m_szElm(ELEMENT_MAX_SIZE), m_state(PS_NONE), 
							m_VB(0), m_IB(0), m_typeTbl(), m_dataTbl(), m_pMem(0), m_idxTopWnd(0), m_arrWndPtr()
{
}

UIPanelNew::UIPanelNew(size_t w, size_t h, size_t maxElm) : m_w(w), m_h(h), m_cntWnd(0), 
						m_szElm(ELEMENT_MAX_SIZE), m_state(PS_NONE), m_VB(0), m_IB(0), m_typeTbl(),
						m_dataTbl(), m_pMem(0), m_idxTopWnd(0), m_arrWndPtr()
{
	size_t res = 0;
	size_t szTypeTbl = 0;
	size_t szDataTbl = 0;
	size_t capTypeTbl = 32; // сколько может быть полей в типе Base_Elm с запасом
	Papo::TblField tf;
	Papo::Metrics m;
	unsigned char* pTblData = 0;

	szTypeTbl = Papo::Array::CalcSizeData(capTypeTbl /*кол-во полей в типе Base_Elm*/, sizeof(Papo::TblField), m);
	szDataTbl = Papo::Array::CalcSizeData(maxElm, m_szElm, m);

	m_pMem = new unsigned char [szTypeTbl + szDataTbl];
	pTblData = (unsigned char* )m_pMem + szTypeTbl;

	if(m_pMem)
	{
		res = m_typeTbl.Init("UIElm_Type", capTypeTbl, 0, Papo::MyTable::TF_TYPETBL, sizeof(Papo::TblField), 0, m_pMem);		

		tf = Papo::TblField("elmData", 0, m_szElm, 0, 
			Papo::TblField::FOPT_READONLY | Papo::TblField::FOPT_HIDEN, Papo::TblField::FT_S);
		m_typeTbl.AddRow(&tf);		
		
		res = m_dataTbl.Init("UIElm_Table", maxElm, &m_typeTbl, 0, m_typeTbl.GetSzType(), 0, pTblData);

		createVIBuffers(PANEL_MAX_WND);
		//createVIBuffers_2(MAX_WND);
		//createVIBuffers_fx(); // не использовать OnResetDevice();
		//build_fx();
		//int sz = sizeof(m_font);
		m_font = MyFont("MS Sans Serif", 12, 0); // ;"Times New Roman" // "Courier New"; 

		
		//m_font.OutSymbols();
	}
}

size_t UIPanelNew::addWind(Base_Elm* pWnd)
{
	size_t res = 1;
	if(pWnd != 0 && m_cntWnd < PANEL_MAX_WND)
	{
		m_arrWndPtr[m_cntWnd] = pWnd;
		m_idxTopWnd = m_cntWnd; // тут!
		m_cntWnd++;
		res = 0;
	}
	return res;
}

void UIPanelNew::Update(float dt)
{	
	//bool bit = 0;
	//RECT rc;
	//size_t n = 0;
	Base_Elm* pElm = 0;
	
	//for(size_t x = 0; x < m_dataTbl.Capacity(); ++x)
	// !!!Именно в обратном порядке(связанно с обработкой попадания в топ окно)
	//for(size_t x = m_dataTbl.Capacity() - 1; x >= 0; --x)
	//{
	//	if(n == m_dataTbl.Count()) break;
	//	pElm = (Base_Elm* )m_dataTbl.GetAt(x);
	//	if(pElm != 0)
	//	{
	//		//-----------------------------------------------
	//		//pElm->CurrRect(rc, 0);
	//		//if(PtInRect(&rc, gDInput->MousePos()) != 0)
	//		//{
	//		//	//pElm->SetState();
	//		//}
	//		//-----------------------------------------------
	//		pElm->Update(dt);
	//		n++;
	//	}
	//}

	/*for(size_t x = 0; x < m_cntWnd; ++x)
	{   
		pElm = m_arrWndPtr[x];
		pElm->Update(dt);
	}*/

	//--------------------------------------------------------------------
	// может перенести в Draw()???
	// кажд цикл обновл VB полностью т.к. при блокировании части VB (флаг D3DLOCK_DISCARD)
	// создается новый VB и не обновленные вершины теряют данные.

	// D3DLOCK_NOOVERWRITE - данные ресурса изменяются только если драйвер его не использует
	VertexRHW* v = 0;
	HR3D(m_VB->Lock(0, m_cntWnd * 4 * sizeof(VertexRHW), (void**)&v, D3DLOCK_NOOVERWRITE)); // D3DLOCK_DISCARD
	for(int y = 0; y < m_cntWnd; ++y)
	{   
		pElm = m_arrWndPtr[y];

		//pElm->CurrRect(rc, 0);
		//if(PtInRect(&rc, gDInput->MousePos()) != 0) //bit = 1;
		//{
		//	//pElm->SetState(ES_HIT, 1);
		////pElm->SetState(ES_HIT, PtInRect(&rc, gDInput->MousePos()));
		//	if(gDInput->IsLMB_CLK_DWN() != 0) pElm->SetFocus();
		//}
		
		pElm->Update(dt);
		updateVBuff_2(v + (y << 2), pElm);
	}
	HR3D(m_VB->Unlock());

	//---------------------------------------------------------------------
		
	/*for(size_t y = 0; y < m_cntWnd; ++y)
	{
		pElm = m_arrWndPtr[y];
		updateVBuff(y, pElm);		
	}*/
}

void UIPanelNew::updateVBuff_2(VertexRHW* v, Base_Elm* pElm)
{
	/*	v1--v2
		| /	|
		v0--v3 
	*/

	//RECT rc = { pElm->X(), pElm->Y(), pElm->X() + pElm->W(), pElm->Y() + pElm->H() };
	RECT rc;
	pElm->CurrRect(rc, 0);
	float kx = 1.0f / pElm->MaxW();
	float ky = 1.0f / pElm->MaxH();
	
	v[0] = VertexRHW((float)rc.left, (float)rc.bottom,  1.0f, 1.0f, 0.0f, ky * (float)pElm->H());
	v[1] = VertexRHW((float)rc.left, (float)rc.top,	 1.0f, 1.0f, 0.0f, 0.0f);
	v[2] = VertexRHW((float)rc.right, (float)rc.top,	 1.0f, 1.0f, kx * (float)pElm->W(), 0.0f);
	v[3] = VertexRHW((float)rc.right, (float)rc.bottom, 1.0f, 1.0f, kx * (float)pElm->W(), ky * (float)pElm->H());
	
	/*v[0] = VertexRHW((float)pElm->X(), (float)pElm->Y() + pElm->H(),  1.0f, 1.0f, 0.0f, ky * (float)pElm->H());
	v[1] = VertexRHW((float)pElm->X(), (float)pElm->Y(),	 1.0f, 1.0f, 0.0f, 0.0f);
	v[2] = VertexRHW((float)pElm->X() + pElm->W(), (float)pElm->Y(),	 1.0f, 1.0f, kx * (float)pElm->W(), 0.0f);
	v[3] = VertexRHW((float)pElm->X() + pElm->W(), (float)pElm->Y() + pElm->H(), 1.0f, 1.0f, kx * (float)pElm->W(), ky * (float)pElm->H());
	*/
}

void UIPanelNew::updateVBuff(int idx, Base_Elm* pElm)
{
	/*	v1--v2
		| /	|
		v0--v3 
	*/

	RECT rc = { pElm->X(), pElm->Y(), pElm->X() + pElm->W(), pElm->Y() + pElm->H() };
	float kx = 1.0f / pElm->MaxW();
	float ky = 1.0f / pElm->MaxH();

	VertexRHW* v0 = 0;
	HR3D(m_VB->Lock(idx * 4 * sizeof(VertexRHW), sizeof(VertexRHW) << 2, (void**)&v0, 0));
	
	v0[0] = VertexRHW((float)rc.left, (float)rc.bottom,  1.0f, 1.0f, 0.0f, ky * (float)pElm->H());
	v0[1] = VertexRHW((float)rc.left, (float)rc.top,	 1.0f, 1.0f, 0.0f, 0.0f);
	v0[2] = VertexRHW((float)rc.right, (float)rc.top,	 1.0f, 1.0f, kx * (float)pElm->W(), 0.0f);
	v0[3] = VertexRHW((float)rc.right, (float)rc.bottom, 1.0f, 1.0f, kx * (float)pElm->W(), ky * (float)pElm->H());
	
	HR3D(m_VB->Unlock());
}

void UIPanelNew::Draw()
{
	HR3D(gd3dDevice->SetVertexDeclaration(VertexRHW::Decl));
	HR3D(gd3dDevice->SetIndices(m_IB));
	HR3D(gd3dDevice->SetStreamSource(0, m_VB, 0, sizeof(VertexRHW)));
	// Обязательно отключать свет!!! иначе если текстура без альфа, то вся текстура черная ???
	HR3D(gd3dDevice->SetRenderState(D3DRS_LIGHTING, false));

//---------------------------------------------------------------------------
	Base_Elm *pWnd = 0, *pTmp = 0;
	for(int i = 0; i < m_cntWnd; ++i)
	{
		pWnd = m_arrWndPtr[i];
		HR3D(gd3dDevice->SetTexture(0, pWnd->Texture()));	
		HR3D(gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, i << 2, 0, 4, 0, 2));	
		//--------------------------------
		if(m_idxTopWnd == i) pTmp = pWnd;
		if(i > m_idxTopWnd) m_arrWndPtr[i - 1] = m_arrWndPtr[i];
	}
	m_arrWndPtr[m_cntWnd - 1] = pTmp;
	m_idxTopWnd = m_cntWnd - 1;
}

void UIPanelNew::Draw_fx()
{
//	HR(m_FX->SetTechnique(m_hTech));
//
//	HR(gd3dDevice->SetVertexDeclaration(VertexPos::Decl));
//	HR(gd3dDevice->SetIndices(m_IB));
//	HR(gd3dDevice->SetStreamSource(0, m_VB, 0, sizeof(VertexPos)));
//	// Обязательно отключать свет!!! иначе если текстура без альфа, то вся текстура черная ???
//	HR(gd3dDevice->SetRenderState(D3DRS_LIGHTING, false));
//	//HR(gd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME));
//	//HR(gd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
////---------------------------------------------------------------------------
//
//	UINT numPasses = 0;
//	HR(m_FX->Begin(&numPasses, 0));
//	HR(m_FX->BeginPass(0));
//
//	ELEMENTDATA elmd;
//	UIElement::Base_Elm* pElm = 0;	
//	for(size_t i = 0; i < m_cntWnd; ++i)
//	{
//		pElm = m_arrWndPtr[i];
//
//		elmd.v0_x = -0.5f;
//		elmd.v0_y = -0.5f;
//		elmd.v1_x = -0.5f;
//		elmd.v1_y = 0.5f;
//		elmd.v2_x =  0.5f;
//		elmd.v2_y =  0.5f;
//		elmd.v3_x =  0.5f;
//		elmd.v3_y =  -0.5f;
//		elmd.w	  = 2.0f / m_w;
//		elmd.h	  = 2.0f / m_h;
//
//		/*elmd.v0_x = pElm->X();
//		elmd.v0_y = pElm->Y() + pElm->H();
//		elmd.v1_x = pElm->X();
//		elmd.v1_y = pElm->Y();
//		elmd.v2_x = pElm->X() + pElm->W();
//		elmd.v2_y = pElm->Y();
//		elmd.v3_x = pElm->X() + pElm->W();
//		elmd.v3_y = pElm->Y() + pElm->H();
//		elmd.w	  = 2.0f / m_w;
//		elmd.h	  = 2.0f / m_h;*/
//
//		/*elmd.v0_x = 100.0f;
//		elmd.v0_y = 100.0f;
//		elmd.v1_x = 100.0f;
//		elmd.v1_y = 0.0f;
//		elmd.v2_x = 200.0f;
//		elmd.v2_y = 0.0f;
//		elmd.v3_x = 200.0f;
//		elmd.v3_y = 100.0f;
//		elmd.w	  = 2.0f / m_w;
//		elmd.h	  = 2.0f / m_h;*/
//
//		HR(m_FX->SetTexture(m_hTex0, pElm->Texture()));
//		HR(m_FX->SetValue(m_hElmData, &elmd, sizeof(ELEMENTDATA)));
//		
//		HR(m_FX->CommitChanges());
//		HR(gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2));		
//	}	
//
//	HR(m_FX->EndPass());
//	HR(m_FX->End());
}

void UIPanelNew::build_fx()
{
	//// Create the FX from a .fx file.
	//ID3DXBuffer* errors = 0;
	//HR(D3DXCreateEffectFromFile(gd3dDevice, "UIPanel.fx", 
	//	0, 0, D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION /* | D3DXSHADER_USE_LEGACY_D3DX9_31_DLL*/, 0, &m_FX, &errors));
	//if( errors ) MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);

	//// Obtain handles.
	//m_hTech = m_FX->GetTechniqueByName("UIPanelTech");	
	//m_hTex0 = m_FX->GetParameterByName(0, "gTex0");
	//m_hElmData = m_FX->GetParameterByName(0, "arrElmData");

	
	
}

size_t UIPanelNew::AddElement(Base_Elm* pElm)
{
	size_t res = M_FAIL;

	res = m_dataTbl.AddRow(pElm);
	if(res != M_FAIL && pElm->Parent() == 0)
	{
		addWind((Base_Elm* )m_dataTbl.GetAt(res)); // а может добавлять индексы???
	}

	return res;
}

UIPanelNew::~UIPanelNew(void)
{
}

void UIPanelNew::Destroy(void)
{
	OnLostDevice();
	delete [] m_pMem;
}

void UIPanelNew::OnLostDevice(void)
{
	//m_FX->OnLostDevice();
	ReleaseCOM(m_VB);	
	ReleaseCOM(m_IB);
}

void UIPanelNew::OnResetDevice(void)
{
	m_w = gd3dApp->GetBackBuffWidth();
	m_h = gd3dApp->GetBackBuffHeight();
	createVIBuffers(PANEL_MAX_WND);
	//createVIBuffers_2(MAX_WND);	
	//createVIBuffers_fx();
	//m_FX->OnResetDevice();
}

HRESULT UIPanelNew::createVIBuffers(int cntElm)
{	
	WORD*	k = 0;
	HRESULT res = 0;
	size_t	stepIdx = 0;
	WORD	stepVert = 0;

	res = gd3dDevice->CreateVertexBuffer(cntElm * 4 * sizeof(VertexRHW), 
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &m_VB, 0);
	/*res = gd3dDevice->CreateVertexBuffer(cntElm * 4 * sizeof(VertexRHW), 
		D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_VB, 0);*/
	HR3D(res);

//----------------CreateIndexBuffer-------------------------------------	
	// D3DUSAGE_WRITEONLY обязательно даже если только читаем
	res = gd3dDevice->CreateIndexBuffer(6 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_IB, 0);
	HR3D(res);

	if(res == D3D_OK)
	{	
		HR3D(m_IB->Lock(0, 0, (void**)&k, 0));

		k[0] = 0; k[1] = 1; k[2] = 2;
		k[3] = 0; k[4] = 2; k[5] = 3;

		HR3D(m_IB->Unlock());
	}
	return res;
}

HRESULT UIPanelNew::createVIBuffers_fx()
{	
	WORD*	k = 0;
	HRESULT res = 0;
	size_t	stepIdx = 0;
	WORD	stepVert = 0;
	/*	v1--v2 
		| /	|
		v0--v3 
	*/
	res = gd3dDevice->CreateVertexBuffer(4 * sizeof(VertexRHW), 0, 0, D3DPOOL_DEFAULT, &m_VB, 0);
	HR3D(res);

	VertexRHW* v0 = 0;
	HR3D(m_VB->Lock(0, 4 * sizeof(VertexRHW), (void**)&v0, 0));

	/*v0[0] = VertexPos(100.0f, 100.0f,	1.0f, 1.0f, 0.0f, 1.0f);
	v0[1] = VertexPos(100.0f, 0.0f,		1.0f, 1.0f, 0.0f, 0.0f);
	v0[2] = VertexPos(200.0f, 0.0f,		1.0f, 1.0f, 1.0f, 0.0f);
	v0[3] = VertexPos(200.0f, 100.0f,	1.0f, 1.0f, 1.0f, 1.0f);*/

	/*v0[0] = VertexPos(-0.5f, -0.5f,	1.0f, 0.0f, 1.0f);
	v0[1] = VertexPos(-0.5f,  0.5f,	1.0f, 0.0f, 0.0f);
	v0[2] = VertexPos( 0.5f,  0.5f,	1.0f, 1.0f, 0.0f);
	v0[3] = VertexPos( 0.5f, -0.5f,	1.0f, 1.0f, 1.0f);*/

	/*v0[0] = VertexPos(100.0f, 100.0f,	1.0f, 0.0f, 1.0f);
	v0[1] = VertexPos(100.0f, 0.0f,		1.0f, 0.0f, 0.0f);
	v0[2] = VertexPos(200.0f, 0.0f,		1.0f, 1.0f, 0.0f);
	v0[3] = VertexPos(200.0f, 100.0f,	1.0f, 1.0f, 1.0f);*/

	HR3D(m_VB->Unlock());

//----------------CreateIndexBuffer-------------------------------------	
	res = gd3dDevice->CreateIndexBuffer(6 * sizeof(WORD), 0 /*D3DUSAGE_WRITEONLY*/, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_IB, 0);
	HR3D(res);

	if(res == D3D_OK)
	{	
		HR3D(m_IB->Lock(0, 0, (void**)&k, 0));

		k[0] = 0; k[1] = 1; k[2] = 2;
		k[3] = 0; k[4] = 2; k[5] = 3;

		HR3D(m_IB->Unlock());
	}

	return res;
}

//} // end namespace