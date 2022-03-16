#include "MResFile.h"

extern char g_buff[];
namespace Papo {

MResFile::MResFile(char* name, bool bNew) : m_h(0)
{
	if(bNew) DeleteFile(buildName(name));
	createFile(name);
}

DWORD MResFile::createFile(const char* name)
{
	m_h = CreateFile(buildName(name), GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);
	if(m_h == INVALID_HANDLE_VALUE) m_h = 0;	
	return (DWORD)m_h;
}

MResFile::~MResFile(void)
{
	CloseHandle(m_h);
	m_h = 0;
}

DWORD MResFile::writeRes(HANDLE &h, int offs, DWORD size)
{
	DWORD res = M_FAIL;
	HANDLE hMap = 0;
	unsigned char* pData = 0;
	
	if(MapFile(h, hMap, (void** )&pData, size) != 0)
	{
		SetFilePointer(m_h, offs, 0, FILE_BEGIN);
		WriteFile(m_h, pData, size, &res, 0);
		UnMapFile(hMap, pData);
		res = M_OK;
	}

	return res;
}

//DWORD MResFile::AddRes(char* name, ResFileItem* out_pItem)
//{
//	DWORD szByte = 0, res = M_FAIL;
//
//	if(strnlen_s(name, Papo::MAX_CHAR) <= Papo::MAX_CHAR - 1) 	
//	{
//		// GENERIC_WRITE обязательно иначе MapFile fail
//		HANDLE h = CreateFile(name, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);
//		if(AddResByHandle(h, out_pItem) == M_OK)
//		{
//			strncpy_s(out_pItem->m_name, Papo::MAX_CHAR, name, Papo::MAX_CHAR-1);
//			res = M_OK;
//		}
//		CloseHandle(h);		
//	}
//	else g_lastError = M_BIGSTRING;
//	return res;
//}

DWORD MResFile::AddResByHandle(HANDLE h, ResFileItem* io_pItem)
{
	DWORD szByte = 0, res = M_FAIL;
	if(m_h != 0 && h != INVALID_HANDLE_VALUE)
	{
		szByte = GetFileSize(h, 0);
		res = SetFilePointer(m_h, 0, 0, FILE_END);
		
		if( writeRes(h, res, szByte) != M_FAIL)
		{
			io_pItem->m_offs = res;
			io_pItem->m_size = szByte;
			io_pItem->m_state = 0;
			res = M_OK;
		}			
	}
	else g_lastError = M_FILENOFOUND;
	return res;
}
DWORD MResFile::AddRes(char* name, Papo::MyTable* pTbl)
{
	HANDLE h = 0;
	ResFileItem rf;
	DWORD szByte = 0, res = M_FAIL;	
	TblField* pFld = 0;

	if(strnlen_s(name, Papo::MAX_CHAR) <= Papo::MAX_CHAR - 1) 	
	{
		pFld = pTbl->GetFldPtr(pTbl->GetKeyFldIdx());
		res = pTbl->FindCellValue(pFld, name);

		if(res == M_FAIL) // не нашел...
		{
			// GENERIC_WRITE обязательно иначе MapFile fail
			h = CreateFile(name, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);

			res = AddResByHandle(h, &rf);
			if(res != M_FAIL)
			{
				strncpy_s(rf.m_name, Papo::MAX_CHAR, name, Papo::MAX_CHAR-1);
				res = pTbl->Push(&rf);
			}
		}
		else g_lastError = M_ITEM_EXISTING;

		CloseHandle(h);		
	}
	else g_lastError = M_BIGSTRING;
	return res;
}

//DWORD MResFile::AddResByHandle(HANDLE h, const char* name,  Papo::MyTable* pTbl)
//{
//	ResFileItem rf;
//	DWORD szByte = 0, res = M_FAIL;
//	if(m_h != 0 && h != INVALID_HANDLE_VALUE)
//	{
//		//res = pTbl->FindCellValue(0, name);
//		//if(res == M_FAIL) // не нашел...
//		//{
//			szByte = GetFileSize(h, 0);
//			res = SetFilePointer(m_h, 0, 0, FILE_END);
//
//			if(writeRes(h, res, szByte) == M_OK)
//			{
//				strncpy_s(rf.m_name, Papo::MAX_CHAR, name, Papo::MAX_CHAR-1);
//				rf.m_offs = res;
//				rf.m_size = szByte;
//				rf.m_state = 0;				
//
//				res = pTbl->Add(&rf);				
//			}
//		//}
//	}	
//	return res;
//}

//DWORD MResFile::UpdRes(ResFileItem* in_pItem, Papo::MyTable* pTbl, bool bAddEnd)
//{
//	DWORD szByte, res = M_FAIL;
//
//	// Обязательно GENERIC_READ | GENERIC_WRITE, иначе MapFile fail
//	HANDLE h = CreateFile(in_pItem->m_name, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//	if(m_h != 0 && h != INVALID_HANDLE_VALUE)
//	{
//		szByte = GetFileSize(h, 0);
//		if( !bAddEnd && szByte <= in_pItem->m_size) res = writeRes(h, in_pItem->m_offs, szByte);
//		else res = AddResByHandle(h, in_pItem);
//		CloseHandle(h);		
//	}
//	return res;
//}

//DWORD MResFile::UpdRes(DWORD ordNum, Papo::MyTable* pTbl, bool bAddEnd)
//{
//	ResFileItem* pItem = 0;
//	DWORD szByte, res = M_FAIL;
//	DWORD arrIdx = pTbl->OrderNumToArrIdx(ordNum);
//
//	pItem = (ResFileItem* )pTbl->GetElmDataPtr(arrIdx);
//	if(pItem != 0)
//	{
//		// Обязательно GENERIC_READ | GENERIC_WRITE, иначе MapFile fail
//		HANDLE h = CreateFile(pItem->m_name, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//		if(h != INVALID_HANDLE_VALUE)
//		{
//			szByte = GetFileSize(h, 0);
//			if( !bAddEnd && szByte <= pItem->m_size) 
//			{
//				res = writeRes(h, pItem->m_offs, szByte);
//				pItem->m_size = szByte;
//			}
//			else res = AddResByHandle(h, pItem);
//			
//			CloseHandle(h);		
//		}
//		else 
//		{
//			pTbl->Remove(arrIdx); 			
//			g_lastError = M_FILENOFOUND;
//		}
//	}
//	return res;
//}

DWORD MResFile::UpdRes(ResFileItem* pItem, bool bAddEnd)
{
	//ResFileItem* pItem = 0;
	DWORD szByte, res = M_FAIL;
	//DWORD arrIdx = pTbl->OrderNumToArrIdx(ordNum);

	//pItem = (ResFileItem* )pTbl->GetElmDataPtr(arrIdx);
	if(pItem != 0)
	{
		// Обязательно GENERIC_READ | GENERIC_WRITE, иначе MapFile fail
		HANDLE h = CreateFile(pItem->m_name, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(h != INVALID_HANDLE_VALUE)
		{
			szByte = GetFileSize(h, 0);
			if( !bAddEnd && szByte <= pItem->m_size) 
			{
				res = writeRes(h, pItem->m_offs, szByte);
				pItem->m_size = szByte;
			}
			else res = AddResByHandle(h, pItem);
			
			CloseHandle(h);		
		}
		else 
		{
			//pTbl->Remove(arrIdx); 			
			g_lastError = M_FILENOFOUND;
		}
	}
	return res;
}

char*  MResFile::buildName(const char* name)
{
	strncpy_s(g_buff, GBUFF_SIZE, name, GBUFF_SIZE-1);
	strcat_s(g_buff, GBUFF_SIZE, ".rsc");
	return g_buff;
}
void MResFile::UpdResAll(Papo::MyTable* pTbl, bool bNew, bool bSilent)
{
	DWORD res = M_OK;	
	ResFileItem* pItem = 0;	
	
	for (DWORD i = 0; i < pTbl->Capacity(); i++)
	{		
		pItem = (ResFileItem* )pTbl->GetAt(i);
		if(pItem != 0)
		{
			res = UpdRes(pItem, bNew);
			if(!bSilent) HR(res);
			if(res == M_FAIL) pTbl->Remove(i);
		}
	}
	//return res;
}

DWORD MResFile::RemoveRes(const char* name, Papo::MyTable* pTbl)
{
	DWORD idx = pTbl->FindCellValue(0, name);
	if(idx != M_FAIL) pTbl->Remove(idx);
	return idx;
}

}