#pragma once

#include "Debug_first.h"
#include "d3dutil.h"
#include <vector>
#include "CollisionBox.h"
#include "QueueMngr.h"
#include "MyHelper.h"
#include "Debug_last.h"

class MyObject;
class  Camera;

class CollisionContainer
{
public:
	void  CreateCB(AABB bb, int colX, int colY, int colZ, int msgCnt, DWORD &outInd);
	void  Remove(DWORD ind);	
	void  Stop();
	void  Start();
	void  CC_DoCollision();
	int   GetIndCB(D3DXVECTOR3 &pt);
	CollisionContainer(void);
	~CollisionContainer(void);

	DWORD bStop;
	std::vector<CollisionBox *> vCBs;

private:	
	
	DWORD Insert(CollisionBox *cb);
	HANDLE hT;
	
	
	std::vector<DWORD>			vFreeInd; 

};
