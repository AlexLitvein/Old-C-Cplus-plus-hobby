#include "ShaderMngr.h"

ShaderMngr *gShdrMngr = 0;


ShaderMngr::ShaderMngr(void)
{	
	m_PathFX = "Media\\FX\\";
}

ShaderMngr::~ShaderMngr(void)
{
	int size = vItems.size();
	for(int i=0; i < size; ++i)
	{
		if(vItems[i].fx != 0) vItems[i].fx->Release();
	}
}

int ShaderMngr::FindCopyFX(const char *str)
{
	int res = -1;
	int size = vItems.size();
	for(int i=0; i < size; ++i)
	{		
		if(strcmp(vItems[i].fnFX.c_str(), str) == 0) res = i;
	}
	return res;
}

 // вызывать после иниц-ии g_pD3Device в D3DApp
ID3DXEffect	*ShaderMngr::InsertItem(const char *fn)
{
	int dblInd = -1;
	SHDRMNGR_ITEM item;	
	DWORD dwShaderFlags = 0;

	#if defined( DEBUG ) || defined( _DEBUG )
        dwShaderFlags |= D3DXSHADER_DEBUG;
	#else
		dwShaderFlags |= D3DXSHADER_OPTIMIZATION_LEVEL3;
    #endif

	dblInd = FindCopyFX(fn);	
	if(dblInd < 0)
	{
		item.fnFX = fn;
		ID3DXBuffer* errors = 0;
		HR(D3DXCreateEffectFromFile(g_pD3Device, (m_PathFX + fn).c_str(), 0, 0, 
									dwShaderFlags, g_pEffectPool, &item.fx, &errors)); // D3DXSHADER_DEBUG

		if( errors ) MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);
		ReleaseCOM(errors);
		vItems.push_back(item);
		return item.fx;
	}
	else return vItems[dblInd].fx;	
}

int	ShaderMngr::IsFileExist(std::string &file)
{
	WIN32_FIND_DATA fd;

	HANDLE h = FindFirstFile((m_PathFX + file).c_str(), &fd);	
	if(h == INVALID_HANDLE_VALUE) return 0;
	else return 1;
}