#pragma once

#include "MyHelper.h"
#include "MTable.h"

namespace Papo {

class MResFile
{
public:
	MResFile(void) : m_h(0) {}
	MResFile(char* name, bool bNew);
	~MResFile(void);

	//DWORD AddRes(const char* path, char* name, ResFileItem* out_pItem);

	// ����� ������ � �����, ����� �������������� ��� ������ ���������� ��� ��������
	//DWORD AddRes(char* name, ResFileItem* out_pItem);

	// ���������� ������ ������������ ��� M_FAIL
	DWORD AddRes(char* name, Papo::MyTable* pTbl);
	// M_FAIL - fail, M_OK - success (����� ������ � �����, �� ������������ �� ���)
	DWORD AddResByHandle(HANDLE h, ResFileItem* io_pItem);
	
	//DWORD AddResByHandle(HANDLE h, const char* name,  Papo::MyTable* pTbl);
	//DWORD WriteRes(HANDLE h, DWORD offs, DWORD size);
	//DWORD UpdRes(ResFileItem* in_pItem, Papo::MyTable* pTbl, bool bAddEnd);

	// M_FAIL - fail, M_OK - success, ordNum - ���������� �����(!�� ������ � �������)
	DWORD UpdRes(ResFileItem* pItem, bool bAddEnd);
	//DWORD UpdRes(DWORD ordNum, Papo::MyTable* pTbl, bool bAddEnd);
	void UpdResAll(Papo::MyTable* pTbl, bool bNew, bool bSilent);

	// ����� ������ ���������� ��� M_FAIL
	DWORD RemoveRes(const char* name, Papo::MyTable* pTbl);
	
	//static const DWORD m_mult = 32;
	//static const DWORD m_reserve = 20; // % ��������������
protected:
	HANDLE m_h;

	// M_FAIL - fail, M_OK - success
	DWORD writeRes(HANDLE &h, int offs, DWORD size);
	char* buildName(const char* fName);

	// ���� ������ �� 0
	DWORD createFile(const char* name);
	
};

}