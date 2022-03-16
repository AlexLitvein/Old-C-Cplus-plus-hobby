#include "MVector.h"

namespace MHeap {

DWORD MVector::Resize(DWORD resElm/*, DWORD szElm*/)
{
	DWORD offs = M_FAIL;
	DWORD vecDataSzNew = 0;
	//if(szElm < 0 || szElm > MAXWORD) szElm = 0;

	if(resElm > m_capacity || !IsInit())
	{
		if(!IsInit()) resElm = m_capacity;
		vecDataSzNew = resElm << m_ptrSize;
		offs = m_pHeap->AllocData(0, vecDataSzNew + MFBitArray::CalcDataSize(resElm));
		//HR(offs);
		if(offs != M_FAIL) 
		{
			if(m_offsArrPtr != M_FAIL) // если в массив уже были элементы,  ќѕ»–”≈ћ их в нов массив
			{				
				Mem_Mov_QUAD(m_pHeap->GetPtr(m_offsArrPtr), m_pHeap->GetPtr(offs), m_szArrPtr >> 2);			
				// !после освоб пам€ть котор заним старые указатели на элементы
				m_pHeap->FreeMem(m_offsArrPtr, GetSizeArrPrtAndBitArr());	
			}
			// !после
			//if(m_BitArr.IsInit()) m_BitArr.Resize(offs + vecDataSzNew, resElm);
			//else m_BitArr.InitArray(offs + vecDataSzNew, resElm);

			m_BitArr.Resize((DWORD* )(m_pHeap->GetBasePtrDWORD() + offs + vecDataSzNew), resElm);

			// !после 	
			m_capacity = resElm;	
			m_offsArrPtr = offs;
			m_szArrPtr = vecDataSzNew;
		}
	}
	if(offs == M_FAIL) g_lastError = M_RESIZEFAIL;

	return offs;
}

DWORD MVector::Add(void* p)
{
	DWORD idx = M_FAIL;

	//if(m_idxFree == 0)
	//{
		idx = m_BitArr.GetZBit();
		if(idx == M_FAIL) 
		{
			Resize(m_capacity + m_capacity);
			idx = m_BitArr.GetZBit();
		}
	//}
	//else idx = m_idxFree;


	if(idx != M_FAIL) 
	{
		if(AddByIdx(idx, p) == M_FAIL) 
		{
			idx = M_FAIL;
		}
	}
	return idx;
}

//DWORD MVector::Add(void* p)
//{
//	DWORD idx = m_BitArr.GetNBits(1);
//	if(idx == M_FAIL) Resize(m_capacity + (m_capacity >> 1));
//
//	idx = m_BitArr.GetNBits(1);
//	if(idx != M_FAIL) 
//	{
//		if(AddByIdx(idx, p) == M_FAIL) idx = M_FAIL;
//	}
//
//	return idx;
//}

void  MVector::Clear()
{ 
	for(DWORD i = 0; i < m_capacity; ++i)
	{		
		Remove(i);
	}
}

MVector::~MVector(void) { }

void MVector::Destroy()
{
	Clear();
	// after!!
	m_pHeap->FreeMem(m_offsArrPtr,  GetSizeArrPrtAndBitArr());
}

} // end namespace