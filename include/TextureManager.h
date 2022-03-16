#pragma once

#include "d3dUtil.h"
#include "MTable.h"

extern IDirect3DDevice9* gd3dDevice;
//namespace MTex
//{

#define NAME_MAX_CHAR 16

enum TEX_FLAG : size_t
{
	TEX_NONE		= 0,
	TEX_RECREATE	= 1,
	TEX_LOCKABLE	= 2
	//TEX_SUBRECT  = 4,
};

struct Material
{

	char				_name[NAME_MAX_CHAR];	// если в имени есть ".", то из файла, иначе из памяти
	size_t				_usage_pool;			// | usage(16) | pool(16) |
	size_t				_flags;					// bLock|bRecreate
	size_t				_size;					// w | h

	IDirect3DTexture9*	_pTex;	

	size_t				_ambient;
	size_t				_diffuse;
	size_t				_specular;
};

struct TMItem // 32 byte // Material???
{
	// !!!Осторожно с добавлением\удал членов, не забывай про иниц-ию таблицы типов в др месте
	char				_name[NAME_MAX_CHAR];	// если в имени есть ".", то из файла, иначе из памяти
	size_t				_usage_pool;			// | usage(16) | pool(16) |
	//size_t				_lock;	
	size_t				_flags;					// bLock|bRecreate
	size_t				_size;					// w | h
	//size_t				_color;
	IDirect3DTexture9*	_pTex;	
	//size_t				_res0;
	//size_t				_res1;

	TMItem() : _name(), _usage_pool(MAKEDWORD(NULL, D3DPOOL_MANAGED)), /*_lock(NULL),*/
			   _flags(TEX_NONE), _size(0),/* _color(0),*/ _pTex(0)  {}

	TMItem(char* name, size_t usage, size_t pool, /*size_t lock,*/ size_t w, size_t h) : _name(),
				_usage_pool(MAKEDWORD(usage, pool)), /*_lock(lock),*/ _flags(TEX_NONE), 
				_size(MAKEDWORD(w, h)), _pTex(0)
	{
		strncpy(_name, name, NAME_MAX_CHAR);
		if(usage == D3DUSAGE_DYNAMIC || pool == D3DPOOL_DEFAULT /*нельзя указ subRect в Lock()*/)
		{
			_flags |= TEX_RECREATE;
		}
	}
};

class TexMngr
{
public:
	TexMngr(void);
	// TODO: В конструкторе добавл текстуру-заглушку по индексу 0
	TexMngr(size_t capacity);
	virtual ~TexMngr(void);
	void	Destroy(void);
	//static	void CreateTex(LONG w, LONG h, size_t color, size_t usage, D3DPOOL pool, IDirect3DTexture9* &out_pTex);
	//void FillTextureRect(size_t idxTex, RECT *pRcDst, size_t color, size_t lockFlag);
	void FillTexture2(const TMItem *pItem, RECT *pRcDst, size_t color, size_t lockFlag);

	// возвр индекс добавленного или дубликата или M_FAIL(не смог добавить)
	size_t  AddTexture(char* name, size_t usage, size_t pool, /*size_t lock, size_t color,*/ size_t w, size_t h);
	void	OnLostDevice(void);
	void	OnResetDevice(void);
	//const TMItem*	Lock(size_t idx, const RECT* pSubRect, D3DLOCKED_RECT &rcLock_out);
	//void	Unlock(size_t idx);
	void	Clear(void);
	//IDirect3DTexture9* GetTexture(size_t idx);
	const TMItem *GetTexData(size_t idx) { return (TMItem* )m_dataTbl.GetAt(idx); }

private:

	Papo::MyTable	m_typeTbl;	
	Papo::MyTable	m_dataTbl;
	void*			m_pMem;

	// Если в имени есть ".", создает текстуру из файла иначе созд белую текстуру установленного размера.
	HRESULT createTex(TMItem* pItem);

};



//} // end namespace

extern TexMngr* g_pTexMngr;

