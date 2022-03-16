#pragma once

#include "MyHelper.h"
#include "Array.h"
#include "MHeap.h"
#include "MError.h"
//#include "MVector.h"
//#include "MResFile.h"

#pragma warning(disable : 4351) // WARNING: pragma warning(disable : 4351)

extern char g_buff[];
// ???
#define TYPE_MAIN_TBL "TypeMainTbl"
#define TYPE_TYPE_TBL "TypeTypeTbl"
#define TYPE_RES_TBL  "TypeResTbl"

extern Papo::MFHeap* g_pHeap;
//class /*Papo::*/MyTable;
//extern /*Papo::*/MyTable* g_pMainTbl;

namespace Papo {

class MyTable;
extern MyTable* g_pMainTbl;

const int	MAX_CHAR_MY		= 32;
const int	TBL_NAME_MAX_CHAR	= 16;
const int	MAX_FIELDS			= 32;
const int	FLD_NAME_MAX_CHAR	= 32;

union MValue // = MAX_CHAR байт 
{
	char sVal[MAX_CHAR_MY];
	int iVal;
	DWORD uVal;
	float fVal;	
};

struct ResFileItem // 32byte
{
	enum RES_STATE : DWORD
	{
		RES_UPDATE = 1
	};

	ResFileItem() : m_name(), m_offs(0), m_size(0), m_state(0), m_res(0) {}
	/*ResFileItem(char* name) : m_name(), m_offs(0), m_size(0), m_reserve(0), m_state(0) 
	{
		strncpy_s(m_name, MAX_CHAR, name, MAX_CHAR-1);
	}*/

	// пор€док мен€ть осторожно
	char	m_name[Papo::MAX_CHAR_MY];	
	DWORD	m_size;		// тек размер	
	DWORD	m_offs;
	DWORD	m_state;
	DWORD	m_res;		// дополн до 32 байт
};

class MBase
{
public:
	// ¬озвр M_FAIL или другое
	static DWORD Create(const char* name, DWORD size, DWORD cntTbl, size_t szBlockMem); 
	static DWORD Open(const char* name, DWORD size = 0);
	static void Close();
	
	// ¬озвращ индекс добавленной таблицы или M_FAIL. Ќе провер€ет размер элемента
	// по установленной таблице типа (какой забили в параметре ф-ции такой и будет).
	static DWORD AddTable(const char* name, int nRec, DWORD tblFlags, DWORD szData, MyTable* pTypeTbl);
	static DWORD AddTypeTable(const char* name, int maxFlds);
	static DWORD DeleteTableByName(const char* name);
	static DWORD DeleteTableByIdx(size_t idx);
	//static DWORD GetSizeType(MyTable* pTypeTbl);
	static const char* MBase::GetTblNameByIdx(DWORD idx);
	
	//static void CreateResTbl(MyTable* pTbl);
	static void CheckBase();
	static DWORD CopyBase(const char* newName);	

	// возвр индекс или M_FAIL
	static DWORD GetTblIdxByName(const char* name); 
	static MyTable* GetTblPtrByName(const char* name); 
	// ¬озвратит указатель на существующую таблицу или 0.
	static inline MyTable* GetTblPtr(DWORD idx);
	static inline DWORD Capacity();
	static inline DWORD Count();
	static inline const char* Name();
	static inline bool IsInit();
	static void WriteTblElmData(HANDLE h, MyTable* pTbl, DWORD* pNewArrPtr);
	// ƒобавл и инициирует табл в g_pMainTbl, возвр индекс или M_FAIL
	//static DWORD InitTable(MyTable &tbl);

protected:

	MBase(void);
	~MBase(void);

	static DWORD AddTypeMainTbl();
	static DWORD AddTypeTypeTbl();
	static DWORD AddTypeResTbl();
};

struct TblField // 64 byte
{
	enum FIELD_TYPE : unsigned int //  надо ли вообще enum?????
	{
		// Types for printf()
		FT_U = 'u',	// Unsigned
		FT_D = 'd',	// int
		FT_F = 'f',	// double
		FT_S = 's',	// строка байт
		
		// Column types in DataGridView
		CT_TXTB,	// textBox
		CT_CHKB,	// checkBox
		CT_COMB,	// comboBox
		CT_LINK,	// HREF
		CT_BTN		// button
	};

	enum FIELD_OPT : DWORD // не больше 16-ти
	{
		FOPT_HIDEN = 1,
		FOPT_READONLY = 2,
		FOPT_KEYFLD = 4
	};
	
	TblField(): m_name(),  defValue(0), szData(sizeof(int)), offsDataCurr(-1), options(MAKEDWORD(100, 0)),
		 type(MAKEDWORD(FT_D, CT_TXTB)), srcData(0),  rHelp(0), offsDataOld(-1) {}

	// TODO: »зменил пор€док параметров проследить в др вызовах - исправить
	TblField(const char* name, int value, DWORD szDt, int offsData, DWORD opt = 0, 
			DWORD typeFld = FT_D, DWORD typeCol = CT_TXTB, DWORD srcTbl = 0, DWORD srcFld = 0, 
			DWORD w = 100, DWORD hlp = 0, int oOld = -1) : 

		m_name(), defValue(value), szData(szDt), options(MAKEDWORD(w, opt)),
			type(MAKEDWORD(typeFld, typeCol)), srcData(MAKEDWORD(srcTbl, srcFld)),  
			rHelp(hlp), offsDataCurr(offsData), offsDataOld(oOld)
	{
		StringCchCopy(m_name, FLD_NAME_MAX_CHAR - 1, name);
		//strncpy_s(m_name, FLD_NAME_MAX_CHAR, name, FLD_NAME_MAX_CHAR-1);		
		options = MAKEDWORD(w, opt);
		if(typeCol == CT_COMB) typeFld = FT_U;
		type = MAKEDWORD(typeFld, typeCol);			
	}
	// !!!ќсторожно с перестановкой полей(изменить в MBase::AddTypeTypeTbl())
	char	m_name[FLD_NAME_MAX_CHAR];	
	int		defValue;		// значение по умолчанию
	DWORD	szData;	
	DWORD	options;		// упаковать width | options
	DWORD	type;			// FT_ | CT_	
	DWORD	srcData;		// idxTbl | idxFld (индекс таблицы и столбца данных отображаемых в combobox)	
	DWORD	rHelp;			// может просто строка? “екстовое описание	(idxTbl | idxFld)
	int		offsDataCurr;	// TODO: упаковать? (смещ. данных от начала записи)
	int		offsDataOld;	// нов смещ(после перестановки/добавлени€/удалени€ полей)	
};

// TODO: ¬ынести в отдельный файл
class MyTable : public Array // 64 byte
{
	
public:
	enum TBL_FLAG : DWORD
	{
		TF_RESTBL		= 1,
		//TF_EDIT			= 2,		
		TF_REBUILD		= 4,		
		//TF_ALLOYDOUBL	= 8, // если в типе таблице определено ключ поле, то повторени€ не допускаютс€
		TF_TYPETBL		= 16,
		TF_HIDETBL		= 32
	};

	MyTable();
	//MyTable(const char* name, size_t capacity, size_t state, size_t szData);
	~MyTable(){};	

	// —лужит дл€ создани€ таблицы на основе ранее выделенной пам€ти. ret 1 - ok, 0 - fail
	size_t Init(const char* name, size_t capacity, MyTable* pTypeTbl, size_t flags, size_t szRow, 
				const void* pBasePtr, void* pData);

	inline	void UpdatePtrs(void* pBasePtr);
	// ¬озвр индекс найденного или M_FAIL
	size_t	FindCellValue(TblField* pFld, const void* pVal);

	// ƒобавл строку с проверкой на дублик(g_lastError = M_ITEM_EXISTING ), 
	// если pRow = 0, добавл€етс€ пуст запись, возвр индекс добавленного или дубликата
	// или M_FAIL(не смог добавить)
	size_t	AddRow(void* pRow); 
	inline	void	ClearRow(size_t idx);
	//DWORD	RebuildRow(DWORD idxSrc, MyTable* pTblSrc); // M_OK, M_FAIL(M_COPYTBL_FAIL)
	
	//void*	RemoveRow(size_t row) { m_pArray->Remove(row); }
	//void*	NeedRow(size_t row) { return m_pArray->GetAt(row); }
	//DWORD	PushRow(void* pData) { return m_pArray->Push(pData); }
	// запис €ч в существующую строку ret M_OK or idx if doubl cell
	size_t	PushCell(size_t row, TblField* pFld, void* pCell); 
	void	NeedCell(size_t row, TblField* pFld, void** pCell);
	MyTable* Type(void) { return m_pTypeTbl; }
	
	// «аполнит таблицу строками из табл источника с приведением размера данных в табл назначени€.
	// “аблиц назнач дб инициирована пол€ми
	//DWORD	RebuildTable(MyTable* pTblOld);
	inline TblField* GetFldPtr(size_t idxFld) { return (TblField* )(m_pTypeTbl->GetAt(idxFld)); }	
	inline TblField* GetKeyFldPtr();	
	//inline MyTable* GetTypeTblPtr() { return m_pTypeTbl; }	
	size_t	GetFldIdxByName(const char* name);
	inline	size_t CntFlds();
	void SetType(MyTable* pTypeTbl);		
	inline	size_t Flags() { return m_flags; }
	inline	char* Name() { return m_name; }	
	inline	size_t GetKeyFldIdx();
	inline	void SetKeyFldIdx(size_t idx);
	inline	size_t GetFldType(size_t idxFld) { return HIWORD(GetFldPtr(idxFld)->type); }
	inline  void SetFlag(TBL_FLAG flag, bool bit);
	inline  bool IsFlag(TBL_FLAG flag);
	inline  size_t GetSzType(void);

	protected:	const void*	m_pBasePtr;			// указатель базы MFHeap
	protected:	char		m_name[TBL_NAME_MAX_CHAR];
	protected:	size_t		m_flags;
	protected:	size_t		m_keyFldIdx;
	protected:	MyTable*	m_pTypeTbl;			// таблица типа
	//protected:	size_t		m_offsArrData;
	//protected:	size_t		m_offsTypeTbl;
};

void MyTable::ClearRow(size_t idx)
{
	void* p = GetAt(idx);
	if(p != 0) Mem_Set_QUAD(p, 0, m_metrics.szItem >> 2); 
}
size_t MyTable::GetSzType(void)
{
	size_t sz = 0;
	TblField* pFld = 0;
	
	if(IsFlag(TF_TYPETBL))
	{
		for(size_t i = 0; i < Capacity(); i++)
		{
			pFld = (TblField* )GetAt(i);
			if(pFld != 0) sz += pFld->szData;
		}
	}
	return sz;
}
void MyTable::UpdatePtrs(void* pBasePtr) // param void* pBasePtr
{
	size_t offs;
	if(m_pBasePtr != 0)
	{
		if(m_pTypeTbl != 0)
		{
			offs = (size_t)m_pTypeTbl - (size_t)m_pBasePtr;
			m_pTypeTbl = (MyTable* )((size_t)pBasePtr + offs);
		}

		offs = (size_t)m_pArrFreeIdx - (size_t)m_pBasePtr;			
		m_pArrFreeIdx = (size_t* )((size_t)pBasePtr + offs);
		m_pItems = (size_t* )((size_t)m_pArrFreeIdx + m_metrics.szArrFreeIdx);
	
		m_pBasePtr = pBasePtr;	
	}
}

void MyTable::SetKeyFldIdx(size_t idx)
{ 
	//if(m_type == M_FAIL) m_keyFldIdx = idx;
	//else GetTypeTblPtr()->SetKeyFldIdx(idx);

	m_keyFldIdx = idx;
}
size_t MyTable::GetKeyFldIdx() 
{
	/*DWORD idx = 0;
	if(m_type == M_FAIL) idx = m_keyFldIdx;
	else idx = GetTypeTblPtr()->GetKeyFldIdx();

	return idx;*/

	return m_keyFldIdx;
}
//MyTable* MyTable::GetTypeTblPtr() 
//{ 
//	MyTable* p = 0;
//	if(m_type != M_FAIL) p = MBase::GetTblPtr(m_type);
//	return p;
//}

TblField* MyTable::GetKeyFldPtr()
{ 
	//MyTable* pTypeTbl = 0;
	TblField* p = 0;

	//pTypeTbl = GetTypeTblPtr();
	if(m_pTypeTbl != 0)
	{		
		p = (TblField* )m_pTypeTbl->GetAt(m_pTypeTbl->GetKeyFldIdx());
	}
	return p;
}
size_t MyTable::CntFlds() 
{ 
	size_t n = 0;
	if(m_pTypeTbl != 0) n = m_pTypeTbl->Count();
	else 
	{
		n = sizeof(TblField) - FLD_NAME_MAX_CHAR;
		n = (n >> 2) + 1;
	}
	return n; 
}
inline void MyTable::SetFlag(TBL_FLAG flag, bool bit)
{
	if(bit) SET_BIT(m_flags, flag);
	else RESET_BIT(m_flags, flag);
}

inline  bool MyTable::IsFlag(TBL_FLAG flag)
{
	return 0 != (m_flags & flag);
}
//================================================================================================

inline MyTable* MBase::GetTblPtr(DWORD idx) { return (MyTable* )g_pMainTbl->GetAt(idx); }
inline DWORD MBase::Capacity() { return g_pMainTbl->Capacity(); }
inline DWORD MBase::Count()  { return g_pMainTbl->Count(); }
inline const char* MBase::Name()  {	return g_pMainTbl->Name();	}
inline bool MBase::IsInit()  {	return g_pMainTbl != 0;	}

} // end namespace Papo
