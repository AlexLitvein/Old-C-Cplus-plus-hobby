#include "mytypes.h"
#include "camera.h"
#include "MyObject.h"
#include "QueueMngr.h"

SubBox::SubBox() 
{ 
	bActive = false; 
	subSec = 0;
	//freeInd = 0;
}

//void SubBox::InsertMsg(MY_MSG msg)
//{
//	vMsg[freeInd] = msg;
//	freeInd++;
//	//return (DWORD *)&vMsg[freeInd-1];
//	/*if(vFreeIndIn_vMsg.size() != 0)
//	{
//		DWORD ind = vFreeIndIn_vMsg.back();
//		vMsg[ind] = msg;
//		vFreeIndIn_vMsg.pop_back();
//		return (DWORD *)&vMsg[ind];
//	}
//	else
//	{
//		vMsg.push_back(msg);
//		return (DWORD *)&vMsg[vMsg.size()-1];
//	}*/
//}

void SubBox::SB_DoCollision()
{
	MY_MSG		msg;	
	//DWORD		msgType;
	//DWORD		msgSubType;	
	AABB		tempBB; //???!!!!

	tempBB.minPt.x = 0.0f;
	tempBB.minPt.z = -30.0f;
	tempBB.maxPt.x = 30.0f;
	tempBB.maxPt.z = 0.0f;
	
	DWORD size = vMsg.size();	
	UINT i = 0;
	for(; i < vMsg.size(); ++i)
	{
		msg		= vMsg[i];
		//msgType	= HIWORD(msg.type);
		//msgSubType	= LOWORD(msgIn.type);		
		
		// отфильтровать объекты которым не нужна высота территории
		if(subSec) 
		{
			msg.pt.y = subSec->GetHeight(msg.pt.x, msg.pt.z);

			if(subSec->visible) SET_BIT(msg.flags,	 MFLG_VISIBLY);
			else				RESET_BIT(msg.flags, MFLG_VISIBLY);
		}

		if(!TestPtInAABB(msg.pt, tempBB)) SET_BIT(msg.flags,	 MFLG_HIT);
		else RESET_BIT(msg.flags, MFLG_HIT);				

		// ѕроходим по всем нижележащим сообщени€м объектов на предмет столкновений и т.д.
		for(UINT y = i+1; y < vMsg.size(); ++y)
		{}

		gQueueMngr->SendMsgOUT(msg);		
	}

	vMsg.clear();
}

TerrainSubSector::TerrainSubSector(AABB &bb)
{
	// т.к. €чейки TerrainSubSector всегда должны быть = 1х1 то
	nRow = (int)floorf(fabs(bb.maxPt.z - bb.minPt.z)+0.5f);
	nCol = (int)floorf(fabs(bb.maxPt.x - bb.minPt.x)+0.5f);
	m_bb = bb;

	toPosX = -bb.minPt.x;
	toPosZ = -bb.minPt.z;
	//toPosX = -floorf(bb.minPt.x);
	//toPosZ = -floorf(bb.minPt.z);
}

float TerrainSubSector::GetHeight(float x, float z)
{
	// €чейки TerrainSubSector всегда должны быть = 1х1
	float locX = x + toPosX; // * (nCol))/ (m_bb.maxPt.x - m_bb.minPt.x); // + 1
	float locZ = z + toPosZ; // * (nRow))/ (m_bb.maxPt.z - m_bb.minPt.z);// + 1

	// Get the row and column we are in.
	int row = (int)floorf(locZ); //fabs fabsfabs ???
	int col = (int)floorf(locX);

	// A*--*B   32 -> 33...63
	//  |\ |
	//  | \|
	// C*--*D   0  -> 1....31
	DWORD indC = col + (row * (nCol + 1)); // + 1
	DWORD indA = col + ((row + 1) * (nCol + 1));// + 1	

	/*gCamera->indA = indA;
	gCamera->indC = indC;*/

	/*gCamera->C = (unsigned char)heightMap[indC];
	gCamera->D = (unsigned char)heightMap[indC + 1];
	gCamera->A = (unsigned char)heightMap[indA];
	gCamera->B = (unsigned char)heightMap[indA + 1];	*/

	float C = (m_bb.maxPt.y * (unsigned char)heightMap[indC]) / 255;
	float D = (m_bb.maxPt.y * (unsigned char)heightMap[indC + 1]) / 255;
	float A = (m_bb.maxPt.y * (unsigned char)heightMap[indA]) / 255;
	float B = (m_bb.maxPt.y * (unsigned char)heightMap[indA + 1]) / 255;	

	/*gCamera->C = C;
	gCamera->D = D;
	gCamera->A = A;
	gCamera->B = B;	*/

	// Where we are relative to the cell.
	float s = locX - (float)col;
	float t = locZ - (float)row;	

	// If upper triangle ABC.
	if(t > 1.0f - s)
	{
		float uy = A - B;
		float vy = D - B;		
		return B + (1.0f-s)*uy + (1.0f-t)*vy;
	}
	else // lower triangle DCB.
	{
		float uy = D - C; 
		float vy = A - C;		
		return C + s*uy + t*vy;
	}		
}