#pragma once

#include "Debug_first.h"
#include "d3dUtil.h"
#include <vector>
#include "MyTypes.h"
#include "Debug_last.h"

class ShaderMngr
{
public:
	ShaderMngr(void);
	~ShaderMngr(void);	
	ID3DXEffect	*InsertItem(const char *fn);  // вызывать после иниц-ии gd3dDevice в D3DApp
	int	IsFileExist(std::string &file);
	
private:
	std::string m_PathFX;
	std::vector<SHDRMNGR_ITEM> vItems;	
	int FindCopyFX(const char *str);
};
