#pragma once

#include "MyHelper.h"
#include "MError.h"
//#include "Container.h"

extern char g_buff[];

namespace Papo {

class MFHeap;

class MFBitArray
{
	friend MFHeap;
public:

	MFBitArray();
	~MFBitArray();

	DWORD Resize(DWORD* pNewData, DWORD nBit);
	// ����� ������ ������� ���������� �� ������ 0-�� ��� ��� M_FAIL
	DWORD GetNBits(DWORD cntBit );	
	DWORD GetZBit();
	// idxStrt ������� �� 0
	DWORD SetNBits(DWORD nBits, DWORD idxStrt, bool bit); // 0 - ��, ��� ������� �������� ����������
	DWORD GetCountZBits();
	DWORD CheckBit(DWORD idx); // ����� ���� ���� � �������
	
	// ����� ������ ������� 4 ������ � ������!
	static inline DWORD CalcDataSize(DWORD cntBit) { return NearBigMult(cntBit, 32) >> 3; }
	inline DWORD Count() { return m_cntBit; }
	inline DWORD* GetDataPtr() { return m_pData; } // ???
	inline DWORD GetSizeData() { return m_szData; }
	inline bool IsInit() { return m_bInit == 1; }
	inline void UpdatePtr(DWORD* ptr) { m_pData = ptr; }
	
protected:
	DWORD	m_bInit;
	DWORD	m_cntBit; // ��� � ������� �����
	DWORD	m_szData; // !byte
	DWORD*	m_pData;

	// ����� ������� �������� ����������
	static inline size_t setNBitsDW(size_t nBits, size_t idxStrt, bool bit, size_t* pData);
	// ����� ������� ����� 0�� � ������ ������� ��������� ����
	static inline size_t getNBitsDW(size_t nBits, size_t data, size_t &outIdxFirst);
	static inline DWORD getCountZBitsDW(DWORD data);		
	static inline bool checkBitDW(DWORD idx, DWORD dw); 
	// ���� ����� ����� 0 � ������ � idx, ���� �� ����� ����� 1, � idx - �� ���������
	static inline bool getZBitDW(DWORD dw, DWORD &idx);	
}; // end MFBitArray

//=========================================================================================

class MFHeap
{
public:
	MFHeap();
	~MFHeap();
	
	// M_FAIL, M_OK
	DWORD	HeapCreate(const char* name, DWORD flSize, DWORD szBlockMem);	
	DWORD	HeapCreateFromMem(void* pMem, DWORD size);
	// M_FAIL, M_OK
	DWORD	HeapOpen(const char* name, DWORD newSize);
	void	HeapClear();
	void	HeapFree();	
	
	// ���� p = 0 ������ ����������� ������, 
	// ���� p != 0, �� ��� � �������� ������, ������� ���� � ������ ��� M_FAIL
	DWORD	AllocData(void* p, DWORD size);		
	//void*	AllocDataPtr(void* p, DWORD size);
	DWORD	GetFreeSize(void);
	
	inline size_t FreeMemByOffs(size_t offs, size_t size); // 0 - ��, ������ - ����� �� �������
	inline size_t FreeMemByPtr(void* p, size_t size); // 0 - ��, ������ - ����� �� �������
	// ����������� ��������(��� ��������) � ��������.
	inline void* GetPtr(DWORD offs);
	inline void SetMainTbl(DWORD offs) { if(offs != M_FAIL) m_offsDB = offs; }
	inline void* GetMainTbl() { return GetPtr(m_offsDB); }	
	inline DWORD CntElm() { return m_BitArr.m_cntBit; }
	inline DWORD Size() { return m_size; }
	//inline DWORD GetBitArrSzData() { return m_BitArr.GetSizeData(); }
	inline DWORD GetBitArrSzData() { return m_BitArr.m_szData; }
	//inline DWORD GetBitArrDataOffs() { return m_BitArr.GetDataOffs(); }
	//inline DWORD GetBitArrDataOffs() { return m_BitArr.m_offsData; }
	inline DWORD GetMainTblOffs() { return m_offsDB; }
	inline DWORD GetBasePtr() { return ( DWORD)m_pMem; }
	//inline int GetOffset() { return (size_t)m_pMem - (size_t)m_pMemOld; }
	//inline DWORD GetOffset(void* p) { return (DWORD)p - (DWORD)m_pMem; }
	/*static*/ inline DWORD CalcCntElmt(DWORD size);	

private:	
	MFHeap*				m_pMem;			// ������ ���� ����� ���� ����� ���� �����
	//MFHeap*				m_pMemOld;
	HANDLE				m_hMap;
	HANDLE				m_h;	
	DWORD				m_szElm2;		// ������� 2-�� ��� ��� ������
	DWORD				m_szElm10;		// 1024 - ��� ���� ����������� �������� ������
	DWORD				m_size;			// ������ ����� ����� �����������
	DWORD				m_offsDB;
	MFBitArray			m_BitArr;

	
	void	CalcHeapMetrics(DWORD fileSize, DWORD szBlockMem); // ������ �� ������ � ���� ��������
	//DWORD	CalcBitArrDataOffs(void);
	DWORD*	CalcBitArrDataPtr();
};

size_t MFHeap::FreeMemByOffs(size_t offs, size_t size) // 0 - ��, ������ - ����� �� �������
{	
	/*DWORD r = M_FAIL;
	if(offs != 0 && offs != M_FAIL) r = m_BitArr.SetNBits(CalcCntElmt(size), offs >> m_szElm2, 0);
	return r;*/

	return m_BitArr.SetNBits(CalcCntElmt(size), offs >> m_szElm2, 0);
}

size_t MFHeap::FreeMemByPtr(void* p, size_t size)
{
	//size_t r = M_FAIL;
	//r = FreeMemByOffs((DWORD)p - (DWORD)m_pMem, size);	
	//return r;
	return FreeMemByOffs((DWORD)p - (DWORD)m_pMem, size);	
}

inline void* MFHeap::GetPtr(DWORD offs) 
{ 
	/*void* p = 0;
	if(offs != M_FAIL) p = (void* )((DWORD)g_pHeap + offs);
	return p;*/ 
	return (void* )((DWORD)m_pMem + offs);
}

DWORD MFHeap::CalcCntElmt(DWORD size)
{
	DWORD nBlock = 0;
	nBlock = size >> m_szElm2;
	if(size % m_szElm10) nBlock += 1;
	return nBlock;
}

//=========================================================================================

inline size_t MFBitArray::setNBitsDW(size_t nBits, size_t idxStrt, bool bit, size_t* pData) 
{
	size_t data = *pData;
	__asm
	{		 
		mov  ecx, 32
		sub  ecx, idxStrt			
	cycl:		
		mov	 esi, 32
		sub  esi, ecx			; op1 = op1 - op2 (32 - ecx (��������� ������� ��� ��� ����))
		test bit, 1
		jnz  bit1				; bit != 0, zf = 0
		btr  data, esi			; cf - ���� ���� �� ������	
		jmp  next
	bit1:
		bts  data, esi			; cf - ���� ���� �� ������	
	next:
		dec  nBits	
		jz   home				; nBits == 0, zf = 1
		loop cycl
	home:
	}
	*pData = data;	

	return nBits; // � nBits ������� �������� ����������
}

inline DWORD MFBitArray::getCountZBitsDW(DWORD data) 
{
	//DWORD n = 0;
	__asm
	{
		;mov  eax,dword ptr [pData] ; ��� ������ ������
		;mov  ebx,dword ptr [eax]   ; �� ���������	
		
		mov  ebx, data
		xor  eax, eax		; cntFnd
		mov  ecx, 32				
	cycl:		
		bt   ebx, ecx		; cf - ���� ����
		jc	 next
		inc  eax
	next:
		loop cycl		
	}

	//return n;
}

inline bool MFBitArray::getZBitDW(DWORD dw, DWORD &idx)
{
	__asm
	{
		xor  eax, eax		
		mov  ebx, [idx]
		mov  edx, dw
		not  edx  
		bsf	 ecx, edx				; bsf out_reg_idx1bit, reg/ptr_src // ���� ��� ��� ��� �� zf = 1	
		setz al	
		mov  [ebx], ecx			
	}	
}

inline bool MFBitArray::checkBitDW(DWORD idx, DWORD dw) 
{
	__asm
	{
		mov  ebx, dw
		xor  eax, eax			
		mov  ecx, idx		
		bt   ebx, ecx		; cf - ���� ����
		setc al		
	}
}

inline size_t MFBitArray::getNBitsDW(size_t nBits, size_t data, size_t &outIdxFirst) 
{
	__asm
	{		 
		xor  eax, eax		
		mov  ebx, [outIdxFirst]	
		mov  [ebx], 0xff
		xor  ecx, ecx
		;mov  edx, data
	cycl:		
		bt   data, ecx			; cf - ���� ����		
		jc   m1					; bit = 1
		cmp  [ebx], 0xff		
		jne	 m3
		mov  [ebx], ecx
	m3:	
		inc  eax
		cmp  eax, nBits
		jne  m2
		jmp  m4
	m1:
		mov  [ebx], 0xff
		xor  eax, eax
	m2:
		inc  ecx
		cmp  ecx, 32
		jb   cycl		; jmp below (op1 < op2)	
	m4:
	}	
}

//=========================================================================================



} // end namespace Papo