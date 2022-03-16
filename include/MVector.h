#pragma once

#include "MHeap.h"

namespace Papo {

class MVector
{
public:	
	MVector() : m_idxFree(0), m_capacity(0), m_count(0), 
			m_elmDataSize(0), m_szArrPtr(0), m_offsArrPtr(M_FAIL), m_BitArr() {}

	MVector(DWORD count, DWORD szElm, MFHeap* pHeap) : m_pHeap(pHeap), m_idxFree(0), m_capacity(count), m_count(0), 
			m_elmDataSize(0), m_szArrPtr(0), m_offsArrPtr(M_FAIL), m_BitArr() 
	{
		m_elmDataSize = NearBigMilt(szElm, sizeof(int));
		Resize(count);
	}

	~MVector(void);	

	// ������� ���� ������ ������� ���������� ��� M_FAIL
	DWORD	Resize(DWORD resElm/*, DWORD szElm*/);	// ���������� MVector
	void	Destroy(); // WARNING: ������ �������� ��� �������� �������� MVector �� MFHeap
	// ���� � = 0, ����������� ���� �������, ����� ������ ������������ ��� M_FAIL
	DWORD	Add(void* p); 	
	// ����� �� idx �� ������ ����� ������ ��������
	// �������� ������ � ���������� ������ ������
	inline DWORD AddByIdx(DWORD idx, void* p); // ������� ���� ��� M_FAIL

	// ������� ������������ ������� � ����������� ���������� �� ������.
	inline void Remove(DWORD idx);
	// ������� ��� �������� � ����������� ���������� ��� ������.
	void Clear();
	// ����� ��������� �� ������������ ������� ��� 0ty

	// ���������� ��������� �� ������������ �������(� ���������) ��� 0.
	inline void* GetElmDataPtr(DWORD idx);
	inline	DWORD Count() { return m_count; }
	inline	DWORD Capacity() { return m_capacity; }
	inline	void UpdateFreeIdx(DWORD currIdx);
	
	inline	bool IsInit() { return m_offsArrPtr != M_FAIL; }
	//inline	void SetSzElm(DWORD sz) { m_elmDataSize = sz; }	
	
	inline	DWORD GetSizeArrPtr() { return m_szArrPtr; }
	inline	DWORD GetSizeArrPrtAndBitArr() { return m_szArrPtr + m_BitArr.GetSizeData(); }
	inline	DWORD GetOffsArrPrtAndBitArr() { return m_offsArrPtr; }
	inline	DWORD GetSizeElm() { return m_elmDataSize; }	
	inline	DWORD OrderNumToArrIdx(DWORD orderNum);
	//inline	void ResetNextPos() { m_idxFree = 0; }	
	
	// ������� 2-�� ��� ��� ���
	static const DWORD m_ptrSize = 2; // ������ ��������� �� ������ ��������� (4 byte)
		
protected:
	MFHeap* m_pHeap;
	DWORD	m_idxFree;
	DWORD	m_capacity;
	DWORD	m_count;	
	DWORD	m_elmDataSize;	// ������ ������� �������� � ������
	DWORD	m_szArrPtr;		// ������ ������ ��� ��������� �� �������� ������� � ������	
public:	DWORD	m_offsArrPtr;	// ���� ������� ��������� �� ������ ������� �������
public:	
	// TODO: ��������� ������ m_BitArr ����� �������� ���������� m_offsArrPtr ��� ��������� ��������� � ���
	MFBitArray m_BitArr;	


//protected:
	inline	bool checkIdx(DWORD idx) { return idx >= 0 && idx <= m_capacity - 1; }
	// ���������� ��������� �� ������� ���������� ����������� �������� ���������
	// � ��������� ��������� ��� 0.
	inline	DWORD* getElmOffsPtr(DWORD idx);
	//inline  DWORD AddByIdx(DWORD realIdx, void* p); // ������� ���� ��� M_FAIL
};

DWORD  MVector::OrderNumToArrIdx(DWORD orderNum)
{
	DWORD res = M_FAIL;
	DWORD n = 0;
	for( DWORD i = 0; i < m_capacity; ++i)
	{	
		n += m_BitArr.CheckBit(i);
		if(orderNum == n-1) 
		{			
			res = i;
			break;
		}
		//if(n == m_count) break; // �����
	}
	return res;
}

inline void MVector::UpdateFreeIdx(DWORD currIdx)
{
	m_idxFree = 0;
	//if(m_idxFree == 0)
	//{
		if(checkIdx(currIdx + 1))
		{
			if(m_BitArr.CheckBit(currIdx + 1) == 0)
			{
				m_idxFree = currIdx + 1;
			}
		}
	//}
}

inline DWORD MVector::AddByIdx(DWORD idx, void* p)
{
	DWORD offs = M_FAIL;
	offs = m_pHeap->AllocData(p, m_elmDataSize);
	if(offs != M_FAIL) // ���� ���� ����� � ������
	{
		DWORD* pElmOffs = getElmOffsPtr(idx);
		*pElmOffs = offs;
		m_BitArr.SetNBits(1, idx, 1); // ����� ��� ������	

		//UpdateFreeIdx(idx);

		m_count++;
	}
	return offs;
}

inline	DWORD* MVector::getElmOffsPtr(DWORD idx)
{
	DWORD* p = 0;
	if(checkIdx(idx)) p = (DWORD* )m_pHeap->GetPtr(m_offsArrPtr + (idx << m_ptrSize));
	return p;

	//DWORD* pElmOffs = (DWORD* )g_pHeap->GetPtr(m_offsArrPtr + (idx << m_ptrSize));
	//return pElmOffs;
}

/*inline*/ void* MVector::GetElmDataPtr(DWORD idx)
{	
	void* p = 0;
	if(m_BitArr.CheckBit(idx)) p = m_pHeap->GetPtr(*getElmOffsPtr(idx));
	return p;
}

inline void MVector::Remove(DWORD idx) 
{ 	
	if(checkIdx(idx))
	{	
		//DWORD i = IdxConvert(idx);
		if(m_BitArr.CheckBit(idx) == 1)
		{
			m_pHeap->FreeMem(*getElmOffsPtr(idx), m_elmDataSize); // 0 - ok
			m_BitArr.SetNBits(1, idx, 0);
			m_idxFree = idx;
			m_count--;
		}
	}
}

} // end namespace