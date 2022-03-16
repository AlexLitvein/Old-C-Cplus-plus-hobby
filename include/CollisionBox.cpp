#include "CollisionBox.h"
#include "MyObject.h"

CollisionBox::CollisionBox(void)
{
}
CollisionBox::CollisionBox(AABB inBB, int nX, int nY, int nZ/*, int msgCnt*/)
{
	mActive = false;
	bb = inBB;
	//msgSize = msgCnt;	
	
	divW = 1 / (bb.maxPt.x - bb.minPt.x);
	divH = 1 / (bb.maxPt.y - bb.minPt.y);
	divL = 1 / (bb.maxPt.z - bb.minPt.z);

	toPosXYZ = D3DXVECTOR3(-bb.minPt.x, -bb.minPt.y, -bb.minPt.z);	

	mColX = nX;
	mHgtY = nY;
	mRowZ = nZ;	
	
	sizeCB = mColX * mHgtY * mRowZ; // 16*1*16
	
	newSubBox.bActive  = 0;
	newSubBox.bVisible = 0;
	//newSubBox.vMsg.resize(msgSize);	

	for(int i = 0; i < sizeCB; ++i)
	{
		vSubBoxs.push_back(newSubBox);
	}
	vSubBoxsSize = vSubBoxs.size();
	
}
int CollisionBox::CalcInd(D3DXVECTOR3	&pos)
{	
	D3DXVECTOR3 loc = pos + toPosXYZ;	

	int clX = floorf((loc.x * mColX) * divW);		
	int clY = floorf((loc.y * mHgtY) * divH);
	int clZ = floorf((loc.z * mRowZ) * divL);

	return  clX + (clZ * mColX);	
}

void CollisionBox::InsertMsg(MY_MSG msg)
{
	bool				bActive = false;
	bool				bSBactive = false;
	DWORD				ind;
	DWORD				msgType = HIWORD(msg.type);

	switch(msgType)
    {		
		case DYNAMIC_OBJ :
		case STATIC_OBJ :			
			ind = CalcInd(msg.pt);
			vSubBoxs[ind].vMsg.push_back(msg);
			bActive = true;
			bSBactive = true;
			break;

		case CAMERA_OBJ :				
			ind = CalcInd(msg.pt);	

			//Write_Log_String(hLog, "ind %d  ", ind);
			//gCamera->indSB = ind; // индекс SB где нах. камера для инфы!!!!??????????

			vSubBoxs[ind].vMsg.push_back(msg);
			bActive = true;
			bSBactive = true;
			break;

		case TERRAIN_OBJ :			
			ind = CalcInd(msg.pt);
			vSubBoxs[ind].subSec = (TerrainSubSector *)msg.pObj;
			vSubBoxs[ind].bVisible = msg.param1;
			break;

			default :
			//чёт-то по умолчанию;
			break;
    }
	mActive = bActive;
	vSubBoxs[ind].bActive = bSBactive;	
}

void CollisionBox::CB_DoCollision()
{	
	DWORD i = vSubBoxsSize;
	for(; i != 0; --i)
	{
		if(vSubBoxs[i-1].bActive) vSubBoxs[i-1].SB_DoCollision();
	}
}

//void CollisionBox::SendMsg(MSG_IN msg)
//{
//}
CollisionBox::~CollisionBox(void)
{
}
