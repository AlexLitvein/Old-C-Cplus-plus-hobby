#pragma once

#include "Debug_first.h"
#include "d3dutil.h"
#include <vector>
#include <bitset>
#include "loadexpgeom.h"
#include "GraphData.h"
#include "MyRender.h"
#include "QueueMngr.h"
#include "Debug_last.h"

class MyObject;

class ObjMngr
{
public:
	void AddObj(MyObject obj, const char *fileObj, const char *fileFX);
	void RemoveObj(DWORD type, DWORD ind);
	void Update(float dt);
	int	IsFileExist(std::string &file);
	ObjMngr(void);
	~ObjMngr(void);

	std::vector<MyObject> vStatObj;
	std::string m_PathObj;
private:
	
	//D3DXMATRIX ident;
	//std::vector<SHORTSTR12> vStatObj;
	
	std::vector<DWORD>		vFreeIndInVStatObj;

	//void Move(MY_OBJECT &obj, float dt);
};
