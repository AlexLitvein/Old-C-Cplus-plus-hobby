#pragma once

#include "Debug_first.h"
#include "d3dutil.h"
#include <windows.h>
//#include "afxwin.h"
#include <vector>
#include <bitset>
//#include <deque>
#include <list>
#include "loadexpgeom.h"
#include "QueueMngr.h"
#include "Debug_last.h"

class MyObject;

class CollisionBox
{
public:
	void InsertMsg(MY_MSG msg);	
	void SendMsg(MY_MSG msg);
	void  CB_DoCollision();
	CollisionBox(void);		
	CollisionBox(AABB inBB, int colX, int colY, int colZ/*, int msgCnt*/);
	~CollisionBox(void);

	bool			mActive;
	bool			visibly;
	AABB			bb;

private:
	SubBox			newSubBox;	
	D3DXVECTOR3		toPosXYZ;

	float	divW;
	float	divH;
	float	divL;
	
	int		mColX;
	int		mHgtY;
	int		mRowZ;

	int		sizeCB;
	//int		msgSize;
	//int		dynMsgSize;
	
	int		vSubBoxsSize;

	std::vector<SubBox>		vSubBoxs;
	//std::vector<SubBox *>	vActiveSubBoxPtrs;
	
	int CalcInd(D3DXVECTOR3 &pos);
};
