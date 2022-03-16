#pragma once

#include "MyHelper.h"
#include "MError.h"
namespace Papo {

#define TESTFREE 0x80000000
#define GETIDX   0x7FFFFFFF

struct Metrics
{
	size_t	szItem;			// размер одного элемента
	size_t	count;			// текущ кол-во элементов
	size_t	capacity;		// макс ёмкость
	size_t	szArrFreeIdx;	// размер массива свободных индексов
	size_t	szArrItems;		// размер массива элементов		
};

class Array // 32 byte
{		
public:	

	Array(void);
	~Array(void);
	
	// 1 - OK, 0 - fail
	size_t Init(size_t capacity, size_t szItem, void* pMem);
	// ret idx or M_FAIL
	size_t Push(void* p);
	//TODO: Resize()
	void Remove(size_t idx);
	size_t* ItemsPtr() { return m_pItems; }
	size_t Count() { return m_metrics.count; }
	size_t Capacity(){ return m_metrics.capacity; }
	size_t TotalSize(){ return m_metrics.szArrFreeIdx + m_metrics.szArrItems; }
	//size_t SizeItem(){ return m_metrics.szItem; }

	// Возвращает размер всех данных(массив указателей на элементы + массив элементов)
	static size_t CalcMetrics(size_t capacity, size_t szItem, Metrics &m);

	// Если элемент существует, то возвр указатель на него иначе 0.
	void* GetAt(size_t idx);
	bool IsInit() { return m_pItems != 0; }
	void Clear()  { initArrFreeIdx(); }
	//inline	size_t	OrderNumToArrIdx(size_t orderNum);

protected:
	Metrics		m_metrics; // 16 byte
	size_t		m_idxFree;	
	size_t*		m_pArrFreeIdx;	// | bFree(1 bit) | idxFree (31 bit) |
	size_t*		m_pItems;
	size_t		m_reserve;

	inline void		pushFreeIdx(size_t idx);

	// Возвр свободный индекс в массиве или 0(нет своб индксов)
	inline size_t	popFreeIdx(void);

	//void			calcMetrics(size_t capacity, size_t szItem, const void* pMem);
	void			initArrFreeIdx();
};

void Array::pushFreeIdx(size_t idx)
{
	size_t bFree;

	if( --m_idxFree <= 0 )
		m_idxFree = 1;
	else 
	{
		bFree = m_pArrFreeIdx[m_idxFree - 1] & TESTFREE;
		m_pArrFreeIdx[m_idxFree - 1] =  ++idx | bFree;
	}
}

size_t Array::popFreeIdx(void)
{
	size_t idx = 0;
	if(m_idxFree != 0)
	{
		idx = m_pArrFreeIdx[m_idxFree-1] & GETIDX;
		if( ++m_idxFree > m_metrics.capacity) m_idxFree = 0;
	}
	return idx;
}

//size_t Array::OrderNumToArrIdx(size_t orderNum)
//{
//	size_t res = M_FAIL;
//	size_t n = 0;
//	for( size_t i = 0; i < m_metrics.capacity; ++i)
//	{
//		if((m_pArrFreeIdx[i] & TESTFREE) != 0) n++;		
//		if(orderNum == n-1) 
//		{			
//			res = i;
//			break;
//		}
//	}
//	return res;
//}

//=====================================================================================

class HeapArray : public Array
{
public:
	HeapArray(void);
	HeapArray(size_t capacity, size_t szItem);
	~HeapArray(void);	
	
	size_t		Init(size_t capacity, size_t szItem);
	void		Free();

protected:

	HANDLE		m_hHeap;

	void		calcMetrics(size_t capacity, size_t szItem, void* pMem);
	
};

} // end namespace Papo
