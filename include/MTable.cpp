#include "MTable.h"

//#include <iostream>

#pragma warning(disable : 4996)

namespace Papo {

MyTable* g_pMainTbl = 0;

MyTable::MyTable() : m_pBasePtr(0), m_name(), m_flags(0), m_keyFldIdx(0), m_pTypeTbl(0)
{
};

//MyTable::MyTable(const char* name, size_t capacity, size_t state, size_t szData) : 
//		 m_name(), m_state(state), m_keyFldIdx(0), m_type(M_FAIL), m_pArray(0)
//{
//	// TODO: провер имя на не читабельн символы	
//	strncpy_s(m_name, MAX_CHAR, name, MAX_CHAR - 1);
//
//	// Здесь никаких полей не добавлять! Функция AddField изменит размер элемента,
//	// переданного при вызове конструктора.	
//}

size_t MyTable::Init(const char* name, size_t capacity, MyTable* pTypeTbl, size_t flags, 
					 size_t szRow, const void* pBasePtr, void* pData)
{
	Array::Init(capacity, szRow, pData);
	m_pBasePtr = pBasePtr;
	// TODO: провер имя на не читабельн символы	
	strncpy_s(m_name, TBL_NAME_MAX_CHAR, name, TBL_NAME_MAX_CHAR - 1);
	m_pTypeTbl = pTypeTbl;
	m_flags = flags;	
	return 1;
}

size_t MyTable::FindCellValue(TblField* pFld, const void* pVal)
{
	MValue* p = 0;
	size_t n = 0;
	size_t idx = M_FAIL;

	for(DWORD i = 0; i < m_metrics.capacity; ++i)
	{		
		if(n == m_metrics.count) break;

		NeedCell(i, pFld, (void** )&p);
		if(p != 0)
		{
			if(HIWORD(pFld->type) == TblField::FT_S)
			{				
				if(strncmp(p->sVal, (char* )pVal, Papo::MAX_CHAR_MY) == 0) { idx = i;  break; }
			}
			else
			{
				if(p->uVal == *(DWORD* )pVal) 
				{
					idx = i;
					break;
				}		
			}
			n++;
		}		
	}
	return idx;
}

size_t MyTable::GetFldIdxByName(const char* name)
{
	size_t res = M_FAIL;
	TblField* pFld = 0;

	if(m_pTypeTbl != 0)
	{
		pFld = (TblField* )m_pTypeTbl->GetKeyFldPtr();
		res = m_pTypeTbl->FindCellValue(pFld, name);
	}	
	return res;
}

// offsDataOld должно хранить смещ данных в pTblSrc, его нужно иниц заранее
//DWORD MyTable::RebuildRow(DWORD idxSrc, MyTable* pTblSrc)
//{
//	DWORD idxDst = M_OK;
//	unsigned char* pSrc = 0;
//	unsigned char* pDst = 0;
//
//	//if(pTblSrc->m_BitArr.CheckBit(idxSrc))
//	//{
//	//	idxDst = AddRow(0);
//	//	if(idxDst != M_FAIL)
//	//	{
//	//		pDst = (unsigned char* )GetElmDataPtr(idxDst);
//	//		pSrc = (unsigned char* )pTblSrc->GetElmDataPtr(idxSrc);
//
//	//		for(DWORD i = 0; i < m_nFlds; ++i) // поля всегда друг за другом, пробелов не может быть
//	//		{
//	//			Mem_Mov_QUAD(pSrc + m_fldsDef[i].offsDataOld, pDst + m_fldsDef[i].offsDataCurr, 
//	//							m_fldsDef[i].szData >> 2);
//	//		}
//	//	}
//	//}
//	return idxDst;
//}


//DWORD MyTable::RebuildTable(MyTable* pTblOld)
//{
//	DWORD res = M_OK;
//
//	for(DWORD i = 0; i < pTblOld->Capacity(); ++i)
//	{
//		res = RebuildRow(i, pTblOld);
//		if(res == M_FAIL) 
//		{
//			g_lastError = M_COPYTBL_FAIL;
//			break;
//		}
//	}
//	return res;
//}

//void MyTable::ClearFields()
//{
//	for(DWORD i = 0; i < MAX_FIELDS; ++i) 
//	{
//		m_fldsDef[i].m_name[0] = 0;
//	}
//	m_nFlds = 0;
//	m_elmDataSize = 0;
//}
void MyTable::SetType(MyTable* pTypeTbl) 
{
	m_pTypeTbl = pTypeTbl;
	//if(!IsState(TF_TYPETBL))
	//if(idxTypeTbl != M_FAIL)
	//{
		//MyTable* pTypeTbl = MBase::GetTblPtr(idxTypeTbl);
		//if(pTypeTbl != 0 && pTypeTbl->IsState(MyTable::TF_TYPETBL))
		//{
		//	m_type =  idxTypeTbl; 
		//	m_elmDataSize = MBase::GetSizeType(pTypeTbl); // после
		//}
	//}
	//else m_type = M_FAIL;
}
// TODO: в tag столбцов занести pFld
void MyTable::NeedCell(size_t row, TblField* pFld, void** pCell)
{
	*pCell = 0;
	unsigned char* p =  (unsigned char* )GetAt(row);
	if(p != 0) *pCell = p + pFld->offsDataCurr; 
}

size_t MyTable::PushCell(size_t row, TblField* pFld, void* pCell)
{
	// WARNING: выделение столбцов текста alt + sel	
	size_t res = M_FAIL;
	
	if((pFld->options & TblField::FIELD_OPT::FOPT_KEYFLD)/* != 0 && (IsState(TF_ALLOYDUBL) == 0)*/)
	{
		res = FindCellValue(pFld, pCell);
		if(res != M_FAIL) g_lastError = M_ITEM_EXISTING;
	}

	if(res == M_FAIL) // если дубл разрешены или не найдены 
	{
		unsigned char* p = (unsigned char* )GetAt(row);
		if(p != 0) // если строка сущ-ет
		{
			Mem_Mov_QUAD(pCell, p + pFld->offsDataCurr, pFld->szData >> 2);
			res = M_OK;
		}
	}	

	return res;
}

//DWORD MyTable::PushRow(int row, void* pData)
//{
//	DWORD res = M_FAIL;
//	unsigned char* pCellDataIn = (unsigned char*)pData + m_fldsDef[m_idxKeyFld].offsDataCurr;
//
//	if(!(m_state & TF_ALLOYDUBL))
//	{
//		res = FindCellValue2(m_idxKeyFld, (MValue* )pCellDataIn); // если не нашел, то M_FAIL		
//	}
//	
//	if(res == M_FAIL) //  && !m_BitArr.CheckBit(row)
//	{	
//		Mem_Mov_QUAD(pData, GetElmPtr(row), m_szData >> 2);
//		m_BitArr.SetNBits(1, row, 1);
//		m_count++;
//		res = M_OK;
//	}
//	else
//	{
//		g_lastError = M_ITEM_EXISTING;
//		res = M_FAIL;
//	}
//
//	return res;
//}

//DWORD MyTable::AddRow(void* pRow)
//{
//	DWORD res = M_FAIL;
//	DWORD idx = M_FAIL;
//	MyTable* pTypeTbl = GetTypeTblPtr();
//	TblField* pFld = 0;
//	unsigned char* pCellSrc = 0;
//
//	idx = Add(0);
//	if(idx != M_FAIL)
//	{
//		// строки в TypeTbl должны следовать плотно друг за другом
//		for(DWORD i = 0; i < pTypeTbl->Count(); ++i)
//		{
//			pFld = (TblField* )pTypeTbl->GetElmDataPtr(i);
//			pCellSrc = (unsigned char* )pRow + pFld->offsDataCurr;
//			res = PushCell(idx, pFld, pCellSrc);
//			if(res != M_OK) 
//			{
//				Remove(idx);
//				idx = M_FAIL; // после
//				g_lastError = M_ITEM_EXISTING;
//				break;
//			}
//		}
//	}	
//	return idx;	
//}

// не применять для заполнения таблиц базовых типов(ResType, TypeTypeTbl, TypeMainTbl)
size_t MyTable::AddRow(void* pRow)
{
	size_t idxFnd = M_FAIL;
	size_t idx = M_FAIL;
	TblField* pFld = GetKeyFldPtr();

	if(pFld != 0 && (pFld->options & TblField::FIELD_OPT::FOPT_KEYFLD) != 0)
	{
		unsigned char* pCell = (unsigned char* )pRow + pFld->offsDataCurr;
		idxFnd = FindCellValue(pFld, pCell);
	}
	if(idxFnd == M_FAIL) 	idx = Push(pRow);
	else 
	{
		idx = idxFnd;
		g_lastError = M_ITEM_EXISTING; 
	}

	return idx;	
}

//===========================================================================

MBase::MBase(void)  /*: pMainTbl(0)*/
{
}

MBase::~MBase(void)
{
	// g_pHeap еще не = 0!
	Close();
}

void MBase::Close()
{
	g_pHeap->HeapFree();
	g_pMainTbl = 0;
}

DWORD MBase::CopyBase(const char* newName)
{
	HANDLE h = 0;
	DWORD res = M_FAIL;
	//Papo::MFHeap hp;
	//Papo::MyTable* pTbl = 0;
	//Papo::MyTable tmpTblCurr;
	//Papo::MyTable tmpTblMain;
	//DWORD offsElm = 0;
	//DWORD* pOffsElmMainTbl = 0;
	//DWORD* pOffsElmCurrTbl = 0;
	//DWORD offsNextTblCurr = 0;
	////DWORD offsArrPtrAndArrBitCurrTbl = 0;
	////bool b = 0;
	////void* pTblElm = 0;

	//if(g_pHeap != 0)
	//{
	//	h = CreateFile(newName, GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);
	//	if(h != INVALID_HANDLE_VALUE)
	//	{	
	//		hp = *g_pHeap;
	//		tmpTblMain = *(Papo::MyTable* )g_pHeap->GetMainTbl();

	//		// Отступить на MFHeap + MainTbl = m_firstElmOffs
	//		tmpTblMain.m_firstElmOffs = SetFilePointer(h, sizeof(Papo::MFHeap) + sizeof(Papo::MyTable), 0, FILE_BEGIN);
	//		// Отступить на arrPtr = offs arrBit data MainTbl
	//		tmpTblMain.m_BitArr.m_offsData = SetFilePointer(h, g_pMainTbl->GetSizeArrPtr(), 0, FILE_CURRENT);
	//		// Отступить на arrBit MainTbl
	//		offsNextTblCurr = SetFilePointer(h, g_pMainTbl->m_BitArr.GetSizeData(), 0, FILE_CURRENT);			
	//		
	//		pOffsElmMainTbl = new DWORD[g_pMainTbl->Capacity()];

	//		// проходим по всем таблицам в MainTbl
	//		for (DWORD i = 0; i < g_pMainTbl->Capacity(); i++)
	//		{
	//			pTbl = GetTblPtr(i);
	//			if(pTbl != 0)
	//			{
	//				tmpTblCurr = *pTbl;
	//				*(pOffsElmMainTbl + i) = SetFilePointer(h, offsNextTblCurr, 0, FILE_BEGIN);

	//				// Отступить на CurrTbl = m_firstElmOffs
	//				tmpTblCurr.m_firstElmOffs = SetFilePointer(h, sizeof(Papo::MyTable), 0, FILE_CURRENT);

	//				// Отступить на arrPtr = offs arrBit data CurrTbl
	//				tmpTblCurr.m_BitArr.m_offsData =  SetFilePointer(h, pTbl->GetSizeArrPtr(), 0, FILE_CURRENT);
	//				// Отступить на arrBit MainTbl
	//				SetFilePointer(h, pTbl->m_BitArr.GetSizeData(), 0, FILE_CURRENT);									

	//				pOffsElmCurrTbl = new DWORD[pTbl->Capacity()];
	//				// запис елементы CurrTbl
	//				WriteTblElmData(h, pTbl, pOffsElmCurrTbl);
	//				// сохр смещ для след табл
	//				offsNextTblCurr = SetFilePointer(h, 0, 0, FILE_CURRENT);

	//				// запис CurrTbl	
	//				SetFilePointer(h, *(pOffsElmMainTbl + i), 0, FILE_BEGIN);
	//				WriteFile(h, &tmpTblCurr, sizeof(Papo::MyTable), &res, 0);	

	//				// запис масс указат CurrTbl
	//				SetFilePointer(h, tmpTblCurr.m_firstElmOffs, 0, FILE_BEGIN);
	//				WriteFile(h, pOffsElmCurrTbl, pTbl->Capacity() * sizeof(DWORD), &res, 0);

	//				// запис масс бит CurrTbl за масс указат
	//				WriteFile(h, (void* )((DWORD)g_pHeap + pTbl->GetOffsArrPrtAndBitArr() + 
	//					pTbl->GetSizeArrPrtAndBitArr() - pTbl->m_BitArr.GetSizeData()), 
	//					pTbl->m_BitArr.GetSizeData(), &res, 0);	

	//				delete [] pOffsElmCurrTbl;					
	//			}
	//		}
	//		
	//		// запис MFHeap
	//		SetFilePointer(h, 0, 0, FILE_BEGIN);
	//		WriteFile(h, &hp, sizeof(Papo::MFHeap), &res, 0);

	//		// запис MainTbl
	//		hp.SetMainTbl(SetFilePointer(h, sizeof(Papo::MFHeap), 0, FILE_BEGIN));
	//		WriteFile(h, &tmpTblMain, sizeof(Papo::MyTable), &res, 0);	
	//		
	//		// запис масс указат MainTbl в конце
	//		WriteFile(h, pOffsElmMainTbl, g_pMainTbl->Capacity() * sizeof(DWORD), &res, 0);

	//		// запис масс бит MainTbl за масс указат
	//		WriteFile(h, (void* )((DWORD)g_pHeap + g_pMainTbl->GetOffsArrPrtAndBitArr() + 
	//					g_pMainTbl->GetSizeArrPrtAndBitArr() - g_pMainTbl->m_BitArr.GetSizeData()), 
	//					g_pMainTbl->m_BitArr.GetSizeData(), &res, 0);	
	//		

	//		delete [] pOffsElmMainTbl;			

	//		// запис bitArr MFHeap !  надо ли? при проверке в CheckBase восстановится актуальный	
	//		SetFilePointer(h, g_pHeap->GetBitArrDataOffs(), 0, FILE_BEGIN);	
	//		WriteFile(h,  (unsigned char*)((DWORD)g_pHeap + g_pHeap->GetBitArrDataOffs()), g_pHeap->GetBitArrSzData(), &res, 0);	
	//		
	//		CloseHandle(h);
	//		res = M_OK;
	//	}	
	//}
	//else {}

	return res;
}

void MBase::WriteTblElmData(HANDLE h, Papo::MyTable* pTbl, DWORD* pNewArrPtr)
{
	//DWORD dw;
	//void* pTblElm = 0;
	//for (DWORD y = 0; y < pTbl->Capacity(); y++)
	//{
	//	pTblElm = pTbl->GetAt(y);
	//	if(pTblElm != 0)
	//	{
	//		// потом после перебора всех элементов таблицы
	//		*(pNewArrPtr + y) = SetFilePointer(h, 0, 0, FILE_CURRENT);
	//		WriteFile(h, pTblElm, pTbl->GetSizeElm(), &dw, 0);
	//	}
	//}
}

DWORD MBase::Create(const char* name, DWORD size, DWORD cntTbl, size_t szBlockMem)
{
	DWORD res = M_FAIL;
	MFHeap hp;
	DWORD offsMainTbl = 0;
	Metrics m;

	// размер таблицы описания данных главной таблицы
	DWORD szTypeMainTbl = Array::CalcMetrics(6 /* 6 полей*/, sizeof(TblField), m); 
	// размер таблицы описания типа-тип
	DWORD szTypeTypeTbl = Array::CalcMetrics(8 /* 8 полей*/, sizeof(TblField), m); 
	// размер таблицы описания типа-ресурс
	DWORD szTypeResTbl  = Array::CalcMetrics(5 /* 5 полей*/, sizeof(ResFileItem), m); 

	res = hp.HeapCreate(name, size + szTypeMainTbl + szTypeTypeTbl + szTypeResTbl, szBlockMem);
	if(res != M_FAIL)
	{
		offsMainTbl = g_pHeap->AllocData(&MyTable(), sizeof(MyTable));
		if(offsMainTbl != M_FAIL)
		{
			g_pMainTbl = (MyTable* )g_pHeap->GetPtr(offsMainTbl);
			g_pHeap->SetMainTbl(offsMainTbl);			

			res = g_pHeap->AllocData(0, Array::CalcMetrics(cntTbl, sizeof(MyTable), m));
			if(res != M_FAIL)
			{
				g_pMainTbl->Init("MainTable", cntTbl, 0, 0, sizeof(MyTable), g_pHeap, g_pHeap->GetPtr(res));
								
				res = AddTypeMainTbl(); //после!( возвр индекс )
				if(res != M_FAIL)
				{
					g_pMainTbl->SetType(GetTblPtr(res));  //после!
					res = AddTypeTypeTbl();
					res = AddTypeResTbl();
				}				
			}
		}
	}
	return res;
}

//DWORD MBase::GetSizeType(MyTable* pTypeTbl)
//{
//	DWORD sz = 0;
//	TblField* pFld = 0;
//	
//	if(pTypeTbl->IsState(MyTable::TF_TYPETBL))
//	{
//		for (DWORD i = 0; i < pTypeTbl->Count(); i++)
//		{
//			pFld = (TblField* )pTypeTbl->GetAt(i);
//			sz += pFld->szData;
//		}
//	}
//	return sz;
//}

DWORD MBase::Open(const char* name, DWORD size)
{
	DWORD res = M_FAIL;
	MFHeap hp;
	size_t cap, fnd = 0, cnt = 0;
	MyTable* pTbl = 0;

	res = hp.HeapOpen(name, size); 
	if(res != M_FAIL)
	{	
		g_pMainTbl = (MyTable* )g_pHeap->GetMainTbl();
		g_pMainTbl->UpdatePtrs(g_pHeap);

		//-------обновл указ-ли всем таблицам-----------
		cap = g_pMainTbl->Capacity();
		cnt = g_pMainTbl->Count();

		for(size_t i = 0; i < cap; ++i)
		{
			pTbl = (MyTable* )g_pMainTbl->GetAt(i);
			if(pTbl != 0)
			{
				pTbl->UpdatePtrs(g_pHeap);
				if(++fnd == cnt) break;
			}
		}
		//----------------------------------------------
	}

	return res;
}

DWORD MBase::AddTable(const char* name, int nRec, DWORD tblFlags, DWORD szData, MyTable* pTypeTbl)
{
	DWORD offs, idx = M_FAIL;	
	MyTable tbl;
	Metrics m;	

	offs = g_pHeap->AllocData(0, Array::CalcMetrics(nRec, szData, m));
	if(offs != M_FAIL)
	{
		tbl.Init(name, nRec, pTypeTbl, tblFlags, szData, g_pHeap, g_pHeap->GetPtr(offs));
		idx = g_pMainTbl->AddRow(&tbl);		
		if(idx == M_FAIL) g_pHeap->FreeMemByOffs(offs, tbl.TotalSize());
	}
	return idx;
}

DWORD MBase::AddTypeTable(const char* name, int maxFlds)
{	
	DWORD idx = AddTable(name, maxFlds, MyTable::TF_TYPETBL, sizeof(TblField), 0);
	return idx;
}

DWORD MBase::AddTypeMainTbl()
{
	size_t offs, idx = M_FAIL;
	TblField fld;
	MyTable* pTbl = 0;
	MyTable tbl;
	Metrics m;
	size_t cap = 6;
	//MyTable tbl;
	size_t szItem = sizeof(TblField);	

	offs = g_pHeap->AllocData(0, Array::CalcMetrics(cap, szItem, m));
	if(offs != M_FAIL)
	{
		tbl.Init(TYPE_MAIN_TBL, cap, 0, MyTable::TF_TYPETBL /* | MyTable::TF_HIDETBL*/, szItem, g_pHeap, g_pHeap->GetPtr(offs));

		idx = g_pMainTbl->Push(&tbl); // не исп AddTable, AddRow
		if(idx != M_FAIL)
		{
			pTbl = GetTblPtr(idx);
			pTbl->SetKeyFldIdx(2);

			fld = Papo::TblField("ArrayData", 0, sizeof(Array), 0, 
								TblField::FOPT_READONLY | TblField::FOPT_HIDEN, TblField::FT_S);					
			pTbl->Push(&fld);

			fld = Papo::TblField("BasePtr", 0, sizeof(void*), fld.offsDataCurr + fld.szData, 
				TblField::FOPT_READONLY | TblField::FOPT_HIDEN, TblField::FT_U);					
			pTbl->Push(&fld);

			fld = Papo::TblField("Name", 0, TBL_NAME_MAX_CHAR, fld.offsDataCurr + fld.szData,
				TblField::FOPT_READONLY | TblField::FOPT_KEYFLD, TblField::FT_S);
			pTbl->Push(&fld);			

			fld = Papo::TblField("State", 0, sizeof(size_t), fld.offsDataCurr + fld.szData,
				TblField::FOPT_READONLY, TblField::FT_U);
			pTbl->Push(&fld);

			fld = Papo::TblField("IdxFeyFld", 0, sizeof(size_t), fld.offsDataCurr + fld.szData, 
				TblField::FOPT_READONLY, TblField::FT_U);
			pTbl->Push(&fld);

			fld = Papo::TblField("TypeTblPtr", 0, sizeof(MyTable*), fld.offsDataCurr + fld.szData,
				TblField::FOPT_READONLY, TblField::FT_U);
			pTbl->Push(&fld);
		}
		else g_pHeap->FreeMemByOffs(offs, tbl.TotalSize());		
	}		
	return idx;
}

DWORD MBase::AddTypeTypeTbl()
{
	DWORD offs, idx = M_FAIL;
	TblField fld;
	MyTable* pTbl = 0;
	MyTable tbl;
	Metrics m;
	size_t cap = 8;
	size_t szItem = sizeof(TblField);	

	offs = g_pHeap->AllocData(0, Array::CalcMetrics(cap, szItem, m));
	if(offs != M_FAIL)
	{
		tbl.Init(TYPE_TYPE_TBL, cap, 0, MyTable::TF_TYPETBL/* | MyTable::TF_HIDETBL*/, szItem, g_pHeap, g_pHeap->GetPtr(offs));

		idx = g_pMainTbl->Push(&tbl); // не исп AddTable, AddRow
		if(idx != M_FAIL)
		{
			pTbl = GetTblPtr(idx);		
			pTbl->SetKeyFldIdx(0);
			
			fld = Papo::TblField("m_name", 0, TBL_NAME_MAX_CHAR, 0,
				TblField::FOPT_READONLY | TblField::FOPT_KEYFLD, TblField::FT_S);
			pTbl->Push(&fld);

			fld = Papo::TblField("defValue", 0, sizeof(int), fld.offsDataCurr + fld.szData, 
				TblField::FOPT_READONLY);
			pTbl->Push(&fld);			

			fld = Papo::TblField("szData", 0, sizeof(size_t), fld.offsDataCurr + fld.szData,
				TblField::FOPT_READONLY, TblField::FT_U);
			pTbl->Push(&fld);

			fld = Papo::TblField("options", 0, sizeof(size_t), fld.offsDataCurr + fld.szData,
				TblField::FOPT_READONLY, TblField::FT_U);
			pTbl->Push(&fld);

			fld = Papo::TblField("type", 0, sizeof(size_t), fld.offsDataCurr + fld.szData,
				TblField::FOPT_READONLY, TblField::FT_U);
			pTbl->Push(&fld);

			fld = Papo::TblField("srcData", 0, sizeof(size_t), fld.offsDataCurr + fld.szData,
				TblField::FOPT_READONLY, TblField::FT_U);
			pTbl->Push(&fld);

			fld = Papo::TblField("rHelp", 0, sizeof(size_t), fld.offsDataCurr + fld.szData,
				TblField::FOPT_READONLY, TblField::FT_U);
			pTbl->Push(&fld);

			fld = Papo::TblField("offsDataCurr", 0, sizeof(int), fld.offsDataCurr + fld.szData, 
				TblField::FOPT_READONLY);
			pTbl->Push(&fld);	

			fld = Papo::TblField("offsDataOld", 0, sizeof(int), fld.offsDataCurr + fld.szData, 
				TblField::FOPT_READONLY);
			pTbl->Push(&fld);	
		}
		else g_pHeap->FreeMemByOffs(offs, tbl.TotalSize());	
	}
	
	return idx;
}

DWORD MBase::AddTypeResTbl()
{
	DWORD offs, idx = M_FAIL;
	TblField fld;
	MyTable* pTbl = 0;
	MyTable tbl;
	Metrics m;
	size_t cap = 5;
	size_t szItem = sizeof(ResFileItem);	

	offs = g_pHeap->AllocData(0, Array::CalcMetrics(cap, szItem, m));
	if(offs != M_FAIL)
	{
		tbl.Init(TYPE_RES_TBL, cap, 0, MyTable::TF_TYPETBL/* | MyTable::TF_HIDETBL*/, szItem, g_pHeap, g_pHeap->GetPtr(offs));

		idx = g_pMainTbl->Push(&tbl); // не исп AddTable, AddRow
		if(idx != M_FAIL)
		{
			pTbl = GetTblPtr(idx);
			pTbl->SetKeyFldIdx(0);

			fld = Papo::TblField("m_name", 0, Papo::MAX_CHAR_MY, 0,
				TblField::FOPT_READONLY | TblField::FOPT_KEYFLD, TblField::FT_S);
			pTbl->Push(&fld);			

			fld = Papo::TblField("m_size", 0, sizeof(size_t), fld.offsDataCurr + fld.szData,
				TblField::FOPT_READONLY, TblField::FT_U);
			pTbl->Push(&fld);

			fld = Papo::TblField("m_offs", 0, sizeof(size_t), fld.offsDataCurr + fld.szData,
				TblField::FOPT_READONLY, TblField::FT_U);
			pTbl->Push(&fld);

			fld = Papo::TblField("m_state", 0, sizeof(size_t), fld.offsDataCurr + fld.szData,
				TblField::FOPT_READONLY, TblField::FT_U);
			pTbl->Push(&fld);

			fld = Papo::TblField("m_res", 0, sizeof(size_t), fld.offsDataCurr + fld.szData,
				TblField::FOPT_READONLY, TblField::FT_U);
			pTbl->Push(&fld);
		}	
		else g_pHeap->FreeMemByOffs(offs, tbl.TotalSize());	
	}	
	return idx;
}

DWORD MBase::DeleteTableByName(const char* name)
{
	size_t res = M_OK;
	size_t idx = GetTblIdxByName(name);
	
	res = DeleteTableByIdx(idx);
	return res;
}

DWORD MBase::DeleteTableByIdx(size_t idx)
{
	size_t res = M_OK;
	MyTable* pTbl = GetTblPtr(idx);
	
	if(pTbl != 0 && pTbl->IsInit()) 
	{
		if(pTbl->IsFlag(Papo::MyTable::TF_RESTBL))
		{
			strncpy_s(g_buff, GBUFF_SIZE, pTbl->Name(), GBUFF_SIZE);	
			strcat_s(g_buff, GBUFF_SIZE, ".rsc");
			DeleteFile(g_buff);
		}	

		res = g_pHeap->FreeMemByPtr(pTbl->ItemsPtr(), pTbl->TotalSize());
		g_pMainTbl->Remove(idx);
	}
	else res = M_FAIL;
	return res;
}

DWORD MBase::GetTblIdxByName(const char* name)
{
	DWORD res = M_FAIL;
	MyTable* pTypeTbl = g_pMainTbl->Type();
	if(pTypeTbl != 0)
	{
		TblField* pFld = (TblField* )pTypeTbl->GetAt(pTypeTbl->GetKeyFldIdx());
		res = g_pMainTbl->FindCellValue(pFld, name);
	}	

	return res;
}

MyTable* MBase::GetTblPtrByName(const char* name)
{
	MyTable* p = 0;
	size_t idx = GetTblIdxByName(name);
	if(idx != M_FAIL) p = GetTblPtr(idx);
	return p;
}
const char* MBase::GetTblNameByIdx(DWORD idx)
{
	const char* pName = 0;
	 MyTable* p =  GetTblPtr(idx);
	 if(p != 0) pName = p->Name();
	return pName;
}

void MBase::CheckBase()
{
	//char lcBuff[MAX_CHAR];
	//void* pElmData = 0;
	//DWORD res = 0;
	//DWORD offsTblData = 0;
	//MyTable* pTbl = 0;
	//Papo::MFBitArray barr;	
	//
	//DWORD freeElm = g_pHeap->GetFreeSize() >> MFHeap::m_szElm; // !до выд памяти под данные MFBitArray
	//DWORD cntBitNewBitArr = MFHeap::CalcCntElmt(g_pHeap->Size());
	//DWORD cntByteNewBitArr = MFBitArray::CalcDataSize(cntBitNewBitArr);
	//DWORD offsBitArrData = g_pHeap->AllocData(0, cntByteNewBitArr); // при подсчете разницы отнять у g_pHeap cntByte

	//if(offsBitArrData != M_FAIL)
	//{
	//	//barr.InitArray(offsBitArrData, cntBitNewBitArr);
	//	barr.Resize(offsBitArrData, cntBitNewBitArr);

	//	barr.SetNBits(MFHeap::CalcCntElmt(sizeof(MFHeap)), 0, 1);
	//	barr.SetNBits(MFHeap::CalcCntElmt(barr.GetSizeData()), barr.Count() - MFHeap::CalcCntElmt(barr.GetSizeData()), 1);

	//	// Учесть выделенную память под MainTbl + размер под массив смещ + bitArr
	//	barr.SetNBits( MFHeap::CalcCntElmt(sizeof(MyTable)), g_pHeap->GetMainTblOffs() >> MFHeap::m_szElm, 1);
	//	barr.SetNBits( MFHeap::CalcCntElmt(g_pMainTbl->GetSizeArrPrtAndBitArr()), g_pMainTbl->GetOffsArrPrtAndBitArr() >> 2, 1);
	//	
	//	for(DWORD i = 0; i < Capacity(); ++i)
	//	{			
	//		pTbl = GetTblPtr(i);
	//		if(pTbl != 0)
	//		{
	//			offsTblData = *g_pMainTbl->getElmOffsPtr(i); // тут!
	//			// Заним место данными таблицы в главн таблице
	//			barr.SetNBits(MFHeap::CalcCntElmt(g_pMainTbl->GetSizeElm()), offsTblData >> MFHeap::m_szElm, 1);
	//			// PtrArrbitArr
	//			barr.SetNBits(MFHeap::CalcCntElmt(pTbl->GetSizeArrPrtAndBitArr()), pTbl->GetOffsArrPrtAndBitArr() >> MFHeap::m_szElm, 1);

	//			// Заним место данными таблицы в текущ таблице
	//			for(DWORD y = 0; y < pTbl->Capacity(); ++y)
	//			{
	//				pElmData = pTbl->GetElmDataPtr(y);
	//				if(pElmData != 0)
	//				{
	//					offsTblData = *pTbl->getElmOffsPtr(y); 
	//					barr.SetNBits(MFHeap::CalcCntElmt(pTbl->GetSizeElm()), offsTblData >> MFHeap::m_szElm, 1);
	//				}
	//			}
	//		}
	//	}

	//	// Освобождаем выделенные данные(фактически данные по этому смещению остаются актуальными
	//	// поке не будит записаны поверх другие), делаем для того чтобы привести в соответствие
	//	// BitArr g_pHeap иначе при memcmp будет не соответствие.
	//	g_pHeap->FreeMem(offsBitArrData, cntByteNewBitArr);

	//	// 0 равно
	//	if(0 != memcmp(g_pHeap->GetPtr(barr.GetDataOffs()), g_pHeap->GetPtr(g_pHeap->GetBitArrDataOffs()), 
	//					g_pHeap->GetBitArrSzData()))
	//	{
	//		res  = freeElm - barr.GetCountZBits();

	//		itoa(res << g_pHeap->m_szElm, lcBuff, 10);
	//		strcpy_s(g_buff, GBUFF_SIZE, "Обнаружена ошибка в таблице распределения памяти и\nутечка памяти: ");
	//		strcat_s(g_buff, GBUFF_SIZE, lcBuff);
	//		strcat_s(g_buff, GBUFF_SIZE, " байт.");
	//		strcat_s(g_buff, GBUFF_SIZE, "\nОткорректировать таблицу распределения памяти?");

	//		if(IDOK == MessageBox(NULL, g_buff, "Внимание!", MB_ICONWARNING | MB_OKCANCEL | MB_DEFBUTTON2))
	//		{
	//			Mem_Mov_QUAD(g_pHeap->GetPtr(offsBitArrData), 
	//						g_pHeap->GetPtr(g_pHeap->GetBitArrDataOffs()),
	//						g_pHeap->GetBitArrSzData() >> 2);
	//		}
	//	}		
	//}
	//else  MessageBox(NULL, "Не достаточно свободной\nпамяти для проверки...", "Внимание!", MB_ICONWARNING);
}

//void MBase::CreateResTbl(MyTable* pTbl)
//{
	//Papo::MResFile rf = Papo::MResFile(pTbl->Name());
//}

} // end namespace
