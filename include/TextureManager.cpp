#include "TextureManager.h"

//namespace MTex
//{

TexMngr* g_pTexMngr = 0;

TexMngr::TexMngr(void) : m_typeTbl(), m_dataTbl(), m_pMem(0)
{
}

TexMngr::TexMngr(size_t capacity)
{
	size_t res = 0;
	size_t szTypeTbl = 0;
	size_t szDataTbl = 0;
	Papo::TblField tf;
	Papo::Metrics m;
	unsigned char* pTblData = 0;
	TMItem ti;
	//size_t offs = 0;

	szTypeTbl = Papo::Array::CalcSizeData(10 /*кол-во полей в типе TMItem*/, sizeof(Papo::TblField), m);
	szDataTbl = Papo::Array::CalcSizeData(capacity, sizeof(TMItem), m);

	m_pMem = new unsigned char [szTypeTbl + szDataTbl];
	pTblData = (unsigned char* )m_pMem + szTypeTbl;

	if(m_pMem)
	{
		res = m_typeTbl.Init("Type", 10, 0, Papo::MyTable::TF_TYPETBL, sizeof(Papo::TblField), 0, m_pMem);		
//--------------------------------------------------
		tf = Papo::TblField("_name", 0, sizeof(ti._name), 0, Papo::TblField::FOPT_KEYFLD, Papo::TblField::FT_S);
		m_typeTbl.AddRow(&tf);

		tf = Papo::TblField("_usage_pool", 0, sizeof(ti._usage_pool), tf.offsDataCurr + tf.szData, 0, Papo::TblField::FT_U);
		m_typeTbl.AddRow(&tf);

		/*tf = Papo::TblField("_lock", 0, sizeof(ti._lock), tf.offsDataCurr + tf.szData, 0, Papo::TblField::FT_U);
		m_typeTbl.AddRow(&tf);*/

		tf = Papo::TblField("_flags", 0, sizeof(ti._flags), tf.offsDataCurr + tf.szData, 0, Papo::TblField::FT_U);
		m_typeTbl.AddRow(&tf);

		tf = Papo::TblField("_size", 0, sizeof(ti._size), tf.offsDataCurr + tf.szData, 0, Papo::TblField::FT_U);
		m_typeTbl.AddRow(&tf);

		//tf = Papo::TblField("_color", Papo::TblField::FT_U);
		//m_typeTbl.AddRow(&tf);		

		tf = Papo::TblField("_pTex", 0, sizeof(ti._pTex), tf.offsDataCurr + tf.szData, 0, Papo::TblField::FT_U);
		m_typeTbl.AddRow(&tf);

		//tf = Papo::TblField("_res0", Papo::TblField::FT_U);
		//m_typeTbl.AddRow(&tf);

		//tf = Papo::TblField("_res1", Papo::TblField::FT_U);
		//m_typeTbl.AddRow(&tf);

//--------------------------------------------------	
		res = m_dataTbl.Init("Table", capacity, &m_typeTbl, 0, m_typeTbl.GetSzType(), 0, pTblData);
	}
}

size_t TexMngr::AddTexture(char* name, size_t usage, size_t pool,/* size_t lock, size_t color,*/ size_t w, size_t h)
{
	size_t idx = M_FAIL;
	TMItem ti = TMItem(name, usage, pool, w, h);	
	
	/*TMItem* pti = 0;
	idx = m_dataTbl.AddRow(&ti);
	if(idx != M_FAIL)
	{
		pti = (TMItem* )m_dataTbl.GetAt(idx);
		if(D3D_OK != createTex(pti)) m_dataTbl.Remove(idx);
	}*/

	if(m_dataTbl.Count() < m_dataTbl.Capacity())
	{
		if(D3D_OK == createTex(&ti)) idx = m_dataTbl.AddRow(&ti);
	}
	
	return idx;
}

void TexMngr::OnLostDevice(void)
{
	size_t n = 0;
	TMItem* pItem = 0;
	
	for(size_t i = 0; i < m_dataTbl.Capacity(); ++i)
	{
		if(n == m_dataTbl.Count()) break;

		pItem = (TMItem* )m_dataTbl.GetAt(i);
		if(pItem != 0)
		{
			if((pItem->_flags & TEX_RECREATE) != 0)
			{
				pItem->_pTex->Release();
				pItem->_pTex = 0;
			}
			n++;
		}
	}
}

void TexMngr::OnResetDevice(void)
{
	size_t n = 0;
	TMItem* pItem = 0;
	
	for(size_t i = 0; i < m_dataTbl.Capacity(); ++i)
	{
		if(n == m_dataTbl.Count()) break;

		pItem = (TMItem* )m_dataTbl.GetAt(i);
		if(pItem != 0)
		{
			if((pItem->_flags & TEX_RECREATE) != 0)
			{
				if(D3D_OK != createTex(pItem))
				{
					m_dataTbl.Remove(i);
				}
			}
		}
	}
}

HRESULT TexMngr::createTex(TMItem* pItem)
{
	HRESULT res = D3D_OK;
	D3DXIMAGE_INFO inf;

	if(strrchr(pItem->_name, '.') == NULL) // если не найден
	{
		res = D3DXCreateTexture(gd3dDevice, HIWORD(pItem->_size), LOWORD(pItem->_size), 0, 
			HIWORD(pItem->_usage_pool), D3DFMT_A8R8G8B8, 
			(D3DPOOL)LOWORD(pItem->_usage_pool), &pItem->_pTex);			
	}
	else
	{
		res = D3DXCreateTextureFromFileEx(gd3dDevice, pItem->_name, HIWORD(pItem->_size), LOWORD(pItem->_size),
			D3DX_DEFAULT, HIWORD(pItem->_usage_pool), D3DFMT_A8R8G8B8, (D3DPOOL)LOWORD(pItem->_usage_pool),
			D3DX_DEFAULT, D3DX_DEFAULT, 0xffff0000 /*0xffffffff*/, &inf , NULL, &pItem->_pTex);
		
		pItem->_size = MAKEDWORD(inf.Width, inf.Height);
	}	

	return res;
}

TexMngr::~TexMngr(void)
{
	//Destroy();
}

void TexMngr::Destroy(void)
{
	OnLostDevice();
	delete [] m_pMem;
}

void TexMngr::Clear(void)
{
	OnLostDevice();
	m_dataTbl.Clear();
}

//const TMItem* TexMngr::Lock(size_t idx, const RECT* pSubRect, D3DLOCKED_RECT &rcLock_out)
//{
//	const TMItem *pItem = 0;
//	
//	pItem = GetTexData(idx);
//	if(pItem != 0)
//	{
//		HR3D(pItem->_pTex->LockRect(0, &rcLock_out, pSubRect, pItem->_lock));
//	}
//
//	return pItem;
//}

//void TexMngr::FillTextureRect(size_t idxTex, RECT *pRcDst, size_t color, size_t lockFlag)
//{
//	//size_t nTexlInRow = 0;
//	size_t* pTexel = 0;
//	size_t nRow = 0;
//	size_t nCol = 0;
//	D3DLOCKED_RECT rcLock;
//	
//	const TMItem *pItem = GetTexData(idxTex);
//
//	if(pItem != 0) // ???
//	{
//		if(pRcDst == 0) 
//		{
//			nRow = LOWORD(pItem->_size);
//			nCol = HIWORD(pItem->_size);
//		}
//		else 
//		{
//			nRow = pRcDst->bottom - pRcDst->top;
//			nCol = pRcDst->right - pRcDst->left;
//		}
//
//		// при выходе за границу текстуры в (pRcDst) LockRect->failed
//		HR3D(pItem->_pTex->LockRect(0, &rcLock, pRcDst, lockFlag));
//		pTexel = (size_t* )rcLock.pBits;
//		if(pTexel != 0)
//		{
//			//nTexlInRow = rcLock.Pitch >> 2;
//			for(size_t row = 0; row < nRow; ++row )
//			{		
//				Mem_Set_QUAD(pTexel, color, nCol);		
//				pTexel += (rcLock.Pitch >> 2);
//			}
//			pItem->_pTex->UnlockRect(0);
//		}
//	}
//}

void TexMngr::FillTexture2(const TMItem *pItem, RECT *pRect, size_t color, size_t lockFlag)
{
	size_t* pTexel = 0;
	size_t nRow = 0;
	size_t nCol = 0;
	D3DLOCKED_RECT rcLock;

	if(pRect == 0) 
	{
		nRow = LOWORD(pItem->_size);
		nCol = HIWORD(pItem->_size);
	}
	else 
	{
		nRow = pRect->bottom - pRect->top;
		nCol = pRect->right - pRect->left;
	}

	// при выходе за границу текстуры в (pRcDst) LockRect->failed
	HR3D(pItem->_pTex->LockRect(0, &rcLock, pRect, lockFlag));
	pTexel = (size_t* )rcLock.pBits;
	if(pTexel != 0)
	{
		for(size_t row = 0; row < nRow; ++row )
		{		
			Mem_Set_QUAD(pTexel, color, nCol);		
			pTexel += (rcLock.Pitch >> 2);
		}
		pItem->_pTex->UnlockRect(0);
	}
}

//} // end namespace