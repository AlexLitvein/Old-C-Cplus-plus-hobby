#include "ObjMngr.h"
#include "MyObject.h"

ObjMngr *g_pObjMngr = 0;

ObjMngr::ObjMngr(void)
{
	m_PathObj = "Media\\Geom\\";
	vStatObj.reserve(100);
}

ObjMngr::~ObjMngr(void)
{
}
int	ObjMngr::IsFileExist(std::string &file)
{
	WIN32_FIND_DATA fd;

	HANDLE h = FindFirstFile((m_PathObj + file).c_str(), &fd);	
	if(h == INVALID_HANDLE_VALUE) return 0;
	else return 1;
}

void ObjMngr::Update(float dt)
{	
	DWORD i = vStatObj.size();
	for(; i != 0; --i)
	{
		vStatObj[i-1].DoLogic(dt);		
	}
}

void ObjMngr::AddObj(MyObject obj, const char *fileObj, const char *fileFX)
{	
	DWORD ind;
	//MY_MSG msg;
	//_heapchk();

	//if(obj.type == TYPE_STATIC)
	//{		
		if(vFreeIndInVStatObj.size() != 0)
		{
			ind = vFreeIndInVStatObj.back();
			obj.ptrInLstObj = (DWORD *)&vStatObj[ind];
			vStatObj[ind] = obj;
			vFreeIndInVStatObj.pop_back();			
		}
		else 
		{
			vStatObj.push_back(obj);
			ind = vStatObj.size()-1;
			vStatObj.back().ptrInLstObj = (DWORD *)&vStatObj[ind]; 
		}
		g_pRender->AddRenderTarget(vStatObj[ind], fileObj, fileFX, false);		
}

void ObjMngr::RemoveObj(DWORD type, DWORD ind)
{
	//if(type == TYPE_STATIC)
	//{
		BaseRenderObj *gd = vStatObj[ind].pRendData;
		gd->DeleteObject(vStatObj[ind].indInRenderLst);
		vFreeIndInVStatObj.push_back(ind); //(GraphData *)
	//}
}