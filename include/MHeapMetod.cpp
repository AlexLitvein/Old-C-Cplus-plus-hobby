#include "MHeap.h"

Papo::MFHeap* g_pHeap = 0;

namespace Papo {

//MFHeap* g_pHeap = 0;

MFHeap::MFHeap() :  m_pMem(0),/* m_pMemOld(0),*/ m_hMap(0), m_h(0), m_szElm2(0), m_szElm10(0),
					m_size(0), m_offsDB(M_FAIL), m_BitArr(MFBitArray()) {}
	
MFHeap::~MFHeap(){}

DWORD MFHeap::HeapCreateFromMem(void* pMem, DWORD size)
{
	DWORD res = M_FAIL;

	m_pMem = (MFHeap* )pMem;
	m_size = size;

	m_BitArr.Resize(CalcBitArrDataPtr(), CalcCntElmt(m_size));
	DWORD nElm = CalcCntElmt(m_BitArr.m_szData);
	m_BitArr.SetNBits(nElm, m_BitArr.m_cntBit - nElm, 1); // заним место m_BitArr данными

	res = AllocData(this, sizeof(MFHeap));	// занимаем место MFHeap
	if(res != M_FAIL) res = M_OK;

	return res;
}

DWORD MFHeap::HeapCreate(const char* name, DWORD flSize, DWORD szBlockMem)
{
	DWORD res = M_FAIL;

	SplitPath(name, g_buff, 0);
	if(g_buff[0] != 0) CreateDirectory(g_buff, NULL);

	m_h = CreateFile(name, GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);
	if(m_h != INVALID_HANDLE_VALUE)
	{
		CalcHeapMetrics(flSize, szBlockMem);
		if(MapFile(m_h, m_hMap, (void **)&m_pMem, m_size))
		{
			res = HeapCreateFromMem(m_pMem, m_size);
			if(res != M_FAIL) g_pHeap = m_pMem;
		}
	}
	if(res == M_FAIL) g_lastError = M_HEAP_CREATE_FAIL;

	return res;
}

DWORD MFHeap::HeapOpen(const char* name, DWORD newSize = 0)
{
	HANDLE hTmp;
	DWORD dw, res = M_FAIL;
	DWORD /*ofstOld = 0,*/ cntElmOld = 0, cntBitOld = 0;
	DWORD oldArrBitDataOffs = 0;

	hTmp = CreateFile(name, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);
	if(hTmp != INVALID_HANDLE_VALUE)
	{
		// достаем стар MFHeap из файла
		ReadFile(hTmp, this, sizeof(MFHeap), &dw, 0);

		m_h = hTmp; // после, иначе затирает		
		oldArrBitDataOffs = (DWORD)m_BitArr.GetDataPtr() - (DWORD)m_pMem;

		// кол-во блоков котор занимал старый MFBitArray если придется делать ресиз
		cntElmOld = CalcCntElmt(m_BitArr.GetSizeData());
		cntBitOld = CntElm(); // кол-во бит котор хранил старый MFBitArray

		// файл MFHeap может только становиться больше
		if(m_size < newSize) CalcHeapMetrics(newSize, m_szElm10);			
		if(MapFile(m_h, m_hMap, (void **)&m_pMem, m_size))
		{ 
			// Обновл стар указатель на данные m_BitArr				
			m_BitArr.UpdatePtr((DWORD* )((DWORD)m_pMem + oldArrBitDataOffs));				
			// иниц старый MFBitArray				
			int out = m_BitArr.Resize(CalcBitArrDataPtr(), CalcCntElmt(m_size));
			if(out != M_FAIL) // иначе - размер не изм или уменьшился(оставляем как есть)
			{
				// освобожд место котор занимал старый MFBitArray при старом размере MFHeap
				m_BitArr.SetNBits(cntElmOld, cntBitOld - cntElmOld, 0);
				DWORD nElm = CalcCntElmt(m_BitArr.GetSizeData());
				// занимаем место новым MFBitArray
				m_BitArr.SetNBits(nElm, m_BitArr.Count() - nElm, 1);
			}

			*m_pMem = *this;
			g_pHeap = m_pMem;

			res = M_OK;
		}		
	}
	if(res == M_FAIL) g_lastError = M_HEAP_CREATE_FAIL;

	return res;
}

DWORD*	MFHeap::CalcBitArrDataPtr()
{
	DWORD nElm = CalcCntElmt(MFBitArray::CalcDataSize(CalcCntElmt(m_size)));
	return (DWORD* )((DWORD)m_pMem + (m_size - (nElm << m_szElm2)));	
}

//DWORD MFHeap::CalcBitArrDataOffs(void)
//{
//	DWORD nElm = CalcCntElmt(MFBitArray::CalcDataSize(CalcCntElmt(m_size)));
//	return m_size - (nElm << m_szElm);
//}

void MFHeap::CalcHeapMetrics(DWORD fileSize, DWORD szBlockMem)
{
	DWORD szArrBit, szMFHeap;
	m_szElm2 = NearBigMult2(szBlockMem);
	m_szElm10 = power(2, m_szElm2);	
	
	szMFHeap = NearBigMult(sizeof(MFHeap), m_szElm10);	
	fileSize = NearBigMult(fileSize, m_szElm10);
	szArrBit = NearBigMult(MFBitArray::CalcDataSize(CalcCntElmt(fileSize)), m_szElm10);
	m_size = szMFHeap + szArrBit + fileSize;
}

DWORD MFHeap::AllocData(void* p, DWORD size)
{
	DWORD offs = M_FAIL, idx = M_FAIL;

	DWORD nElm = CalcCntElmt(size);
	idx = m_BitArr.GetNBits(nElm);
	
	if(idx != M_FAIL) // if find
	{
		offs = idx << m_szElm2;
		if(p) Mem_Mov_QUAD(p, (DWORD* )GetPtr(offs), size >> 2);
		else Mem_Set_QUAD(GetPtr(offs), 0, size >> 2);
		if(0 != m_BitArr.SetNBits(nElm, idx, 1))  // помеч что заняты
		{
			offs = M_FAIL;
			g_lastError = M_OUTOFRANGES;
		}
	}
	else g_lastError = M_HEAPFULL;
	return offs;
}

//void* MFHeap::AllocDataPtr(void* p, DWORD size)
//{
//	DWORD /*offs = M_FAIL,*/ idx = M_FAIL;
//	void* pOut = 0;
//
//	DWORD nElm = CalcCntElmt(size);
//	idx = m_BitArr.GetNBits(nElm);
//	
//	if(idx != M_FAIL) // if find
//	{
//		//offs = idx << m_szElm2;
//		pOut = GetPtr(idx << m_szElm2);
//		if(p) Mem_Mov_QUAD(p, pOut, size >> 2);
//		else Mem_Set_QUAD(pOut, 0, size >> 2);
//		if(0 != m_BitArr.SetNBits(nElm, idx, 1))  // помеч что заняты
//		{
//			pOut = 0;
//			g_lastError = M_OUTOFRANGES;
//		}
//	}
//	else g_lastError = M_HEAPFULL;
//	return pOut;
//}

DWORD MFHeap::GetFreeSize(void)
{
	return m_BitArr.GetCountZBits() << m_szElm2;
}

void MFHeap::HeapClear()
{
	// !оставить биты MFHeap и данные MFBitArray
	DWORD heapBit = CalcCntElmt(sizeof(MFHeap));
	DWORD arrDataBit = CalcCntElmt(m_BitArr.GetSizeData());	
	m_BitArr.SetNBits(m_BitArr.Count() - heapBit - arrDataBit, heapBit, 0);
}
void MFHeap::HeapFree()
{
	if(m_pMem) 
	{ 
		CloseHandle(m_h); // именно здесь
		UnMapFile(m_hMap, m_pMem);
	}
}


} // end namespace Papo