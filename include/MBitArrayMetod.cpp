#include "MHeap.h"


namespace Papo {

MFBitArray::MFBitArray() : m_bInit(0), m_cntBit(0), m_szData(0), m_pData(0) {}
MFBitArray::~MFBitArray() {}

//DWORD MFBitArray::InitArray(DWORD offsData, DWORD nBit)
//{	
//	DWORD res = M_FAIL;
//	if(m_bInit == 0)
//	{
//		m_cntBit = nBit;
//		m_offsData = offsData;	
//		m_szData = CalcDataSize(nBit);
//		// TODO: временно закоментировать
//		Mem_Set_QUAD(g_pHeap->GetPtr(m_offsData), 0, m_szData >> 2); 
//		m_bInit = 1;
//		res = M_OK;		
//	}
//	return 0;
//}

//DWORD MFBitArray::Resize(DWORD offsData, DWORD nBit)
//{
//	DWORD szData = 0;
//	DWORD res = M_FAIL;
//	MFBitArray ba;
//	
//	if(m_bInit == 1)
//	{		
//		szData = CalcDataSize(nBit);
//		// проверка перекрытия
//		if(offsData >= m_offsData + szData || offsData <= m_offsData - szData)
//		{
//			ba.InitArray(offsData, nBit);
//			Mem_Mov_QUAD(g_pHeap->GetPtr(m_offsData), 
//						 g_pHeap->GetPtr(ba.m_offsData), szData >> 2);
//			*this = ba;	
//			res = M_OK;
//		}
//	}
//	return res;	
//}

DWORD MFBitArray::Resize(DWORD* pNewData, DWORD nBit)
{
	DWORD szData, res = M_FAIL;
	
	szData = CalcDataSize(nBit);
	if(m_bInit == 1)
	{
		if(pNewData >= m_pData + szData || pNewData <= m_pData - szData)
		{
			Mem_Set_QUAD(pNewData, 0, szData >> 2);
			Mem_Mov_QUAD(m_pData, pNewData, m_szData >> 2);	 // старый размер!!!
			res = M_OK;
		}
	}
	else Mem_Set_QUAD(pNewData, 0, szData >> 2); 

	m_bInit = 1;
	m_cntBit = nBit;
	m_szData = szData;
	m_pData = pNewData;
	
	return res;	
}

DWORD MFBitArray::GetNBits(DWORD cntBit)
{
	DWORD cntTmp, cntFnd = 0;	
	DWORD idxTmp, idxFrst = M_FAIL;	

	//DWORD* p = (DWORD* )g_pHeap->GetPtr(m_offsData);

	for(DWORD i = 0; i < m_szData >> 2; ++i)
	{			
		cntTmp = getNBitsDW(cntBit - cntFnd, *(m_pData + i), (size_t &)idxTmp); // возвр сколько нашел	

		if(cntTmp != 0 && idxFrst == M_FAIL)
		{
			idxFrst = (idxTmp + (i << 5));
			cntFnd = cntTmp;
		}
		else
		{
			if(idxTmp == 0) cntFnd += cntTmp;
			else 
			{
				idxFrst = M_FAIL;
				cntFnd = 0;
			}
		}
		if(cntFnd == cntBit) break;	
	}	
	if(idxFrst + cntBit > m_cntBit) idxFrst = M_FAIL;

	return idxFrst;
}

DWORD MFBitArray::GetZBit()
{	
	DWORD idx = M_FAIL;

	//DWORD* p = (DWORD* )g_pHeap->GetPtr(m_offsData);
	for(DWORD i = 0; i < m_szData >> 2; ++i)
	{
		if( getZBitDW(*(m_pData + i), idx) == 0 ) 
		{
			idx = idx + (i << 5);
			if(idx >= m_cntBit) idx = M_FAIL;
			//else res = idx;
			break;
		}
		else idx = M_FAIL;
	}

	return idx;
}
DWORD MFBitArray::SetNBits(DWORD nBits, DWORD idxStrt, bool bit)
{
	DWORD i = idxStrt >> 5; // перед!
	idxStrt = idxStrt % 32;
	//DWORD* p = (DWORD* )g_pHeap->GetPtr(m_offsData);

	if(idxStrt + nBits <= m_cntBit)
	{
		for( ; i < m_szData >> 2; ++i)
		{
			nBits = setNBitsDW(nBits, idxStrt, bit,  (size_t* )(m_pData + i)); // Возвр сколько осталось установить			
			idxStrt = 0;
			if(nBits == 0) break;
		}
	}
	return nBits; // 0 - ок, или сколько осталось установить
}

DWORD MFBitArray::CheckBit(DWORD idx)
{
	DWORD i = idx >> 5; // перед!
	idx = idx % 32;		
	return checkBitDW(idx, *(m_pData + i));	
}

DWORD MFBitArray::GetCountZBits()
{
	DWORD nBits = 0;
	//DWORD szDataInDW = GetSizeDataInDW(m_cntBit);

	for(DWORD i = 0; i < m_szData >> 2; ++i) { nBits += getCountZBitsDW(m_pData[i]); }
	// (m_szDataInDW << 5 - m_cntBit) вычитаем неиспользуемые биты(котор всегда = 0) 
	// дополняющие m_cntBit до DWORD
	return nBits  - ((m_szData << 3) - m_cntBit); 
}

} // end namespace