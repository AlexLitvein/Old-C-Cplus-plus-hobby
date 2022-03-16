#include "Array.h"

namespace Papo {

Array::Array(void) : /*m_this(0),*/ /*m_size(0), m_szItem(0), m_count(0), 
					m_capacity(0),*/ m_metrics(), m_idxFree(1), m_pArrFreeIdx(0), m_pItems(0)
{	
}

Array::~Array(void)
{
}

size_t Array::Init(size_t capacity, size_t szItem, void* pMem)
{
	if(pMem != 0)
	{
		CalcMetrics(capacity, szItem, m_metrics);
		m_pArrFreeIdx = (size_t* )pMem;			
		m_pItems =  (size_t* )((size_t)m_pArrFreeIdx + m_metrics.szArrFreeIdx);		

		initArrFreeIdx();
		return 1;
	}	
	return 0;
}

void* Array::GetAt(size_t idx)
{
	void* p = 0;

	if((m_pArrFreeIdx[idx] & TESTFREE) != 0) // надо ли?
	{
		//p = m_pItems + idx;
		p = &m_pItems[idx * (m_metrics.szItem >> 2)];
	}

	return p;
}

void Array::initArrFreeIdx()
{
	if(IsInit() != 0)
	{
		for(size_t i = 1; i <= m_metrics.capacity; ++i) // idx from 1!!!
		{
			m_pArrFreeIdx[i-1] = i;
		}
		m_idxFree = 1;
		m_metrics.count = 0;
	}
}

//void Array::calcMetrics(size_t capacity, size_t szItem, const void* pMem)
//{
//	size_t szArrFreeIdx;
//	
//	CalcSizeData(capacity, szItem, m_metrics);	
//	szArrFreeIdx = m_metrics.capacity * sizeof(size_t);
//	
//	m_pItems =  (size_t* )pMem;
//	m_pArrFreeIdx = (size_t* )((size_t)m_pItems + m_metrics.szData - szArrFreeIdx);		
//}

size_t Array::CalcMetrics(size_t capacity, size_t szItem, Metrics &m)
{
	m.count = 0;
	m.capacity = capacity;
	m.szItem = NearBigMult(szItem, sizeof(size_t));
	m.szArrFreeIdx = m.capacity * sizeof(size_t);
	m.szArrItems = m.szItem * m.capacity;	
	
	return m.szArrFreeIdx + m.szArrItems;
}

size_t Array::Push(void* p)
{
	size_t idx;

	idx = popFreeIdx();

	if(idx != 0) 
	{
		--idx;
		if(p != 0) Mem_Mov_QUAD(p, &m_pItems[idx * (m_metrics.szItem >> 2)], m_metrics.szItem >> 2);
		m_pArrFreeIdx[idx] |= TESTFREE; // помеч что занято
		++m_metrics.count;
	}
	else 
	{
		g_lastError = M_FULL; 
		idx = M_FAIL;
	}

	return idx;
}

void Array::Remove(size_t idx)
{
	if((m_pArrFreeIdx[idx] & TESTFREE) != 0)
	{
		m_pArrFreeIdx[idx] &= GETIDX;
		pushFreeIdx(idx);
		--m_metrics.count;
	}
}
//=====================================================================================

HeapArray::HeapArray(void) : m_hHeap(0)
{	
}

HeapArray::HeapArray(size_t capacity, size_t szItem)
{
	
}

size_t HeapArray::Init(size_t capacity, size_t szItem)
{
	size_t size;
	Metrics m;

	m_hHeap = GetProcessHeap();	
	if(m_hHeap != 0)
	{	
		size =  CalcMetrics(capacity, szItem, m);
		void* p = HeapAlloc(m_hHeap, HEAP_NO_SERIALIZE, size);

		if(p != 0)
		{	
			calcMetrics(capacity, szItem, p);
			initArrFreeIdx();
			return 1;
		}	
	}
	return 0;
}
HeapArray::~HeapArray(void)
{
}

void HeapArray::calcMetrics(size_t capacity, size_t szItem, void* pMem)
{
	/*size_t szArrFreeIdx, szItems;
	SYSTEM_INFO si;
	GetSystemInfo(&si);		

	m_this = pMem;
	m_szItem = NearBigMilt(szItem, sizeof(size_t));
	szItems = NearBigMilt(m_szItem * capacity, si.dwPageSize);
	m_capacity = szItems / m_szItem;
	szArrFreeIdx = m_capacity * sizeof(size_t);

	m_size = NearBigMilt(szItems + szArrFreeIdx + sizeof(HeapArray), si.dwPageSize);	
	m_pItems =  (size_t* )((size_t)m_this + sizeof(HeapArray));
	m_pArrFreeIdx = (size_t* )((size_t)m_this + m_size - szArrFreeIdx);	*/
}

void HeapArray::Free()
{
	//HeapFree(m_hHeap, HEAP_NO_SERIALIZE, m_this);
}

} // end namespace Papo