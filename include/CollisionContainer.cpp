#include "CollisionContainer.h"
#include "MyObject.h"
#include "camera.h"

CollisionContainer *gCollContainer = 0;

DWORD WINAPI DoMessages(LPVOID lpParameter)
{	
	int a = -1;
	MY_MSG msgIn;
	//MyObject *obj;
	DWORD msgType = 0;

	while(1)
	{		
		if(gCollContainer->bStop == 0 && gQueueMngr->GetMsgIN(msgIn))
		{			
			msgType = HIWORD(msgIn.type);
			switch(msgType)
			{
			case MSG_END_UPDATE :
				gCollContainer->CC_DoCollision();
				break;

			case CAMERA_OBJ :				
				a = gCollContainer->GetIndCB(msgIn.pt);					
				break;	

			case STATIC_OBJ :
			case DYNAMIC_OBJ :
				a = gCollContainer->GetIndCB(msgIn.pt);				
				break;			

			case TERRAIN_OBJ :					
				a = gCollContainer->GetIndCB(msgIn.pt);				
				break;				
				
				default :
				//чЄт-то по умолчанию;
				break;
			}
			if( a >= 0) 
			{
				gCollContainer->vCBs[a]->InsertMsg(msgIn);
				//if(msg.type == DYNAMIC_OBJ) gCollContainer->vCBs[a]->active = true;
				a = -1;
			}
		}
		else Sleep(1);		
	}
}

int CollisionContainer::GetIndCB(D3DXVECTOR3 &pt)
{
	int res = -1;
	DWORD i = vCBs.size();
	for(; i != 0; --i)
	{
		/*res = TestPtInAABB(pt, vCBs[i-1]->bb);
		if(res) break;*/
		if(TestPtInAABB(pt, vCBs[i-1]->bb))
		{
			res = i-1;	
			break;
		}
	}
	return res;
}

void  CollisionContainer::CC_DoCollision()
{
	MY_MSG msgOut;	

	DWORD i = vCBs.size();	
	for( ; i > 0; --i)
	{
		if(vCBs[i-1]->mActive) vCBs[i-1]->CB_DoCollision();		
	}
	// сообщени€ MSG_END_UPDATE будут накапливатьс€ в vMsgOut т.к. CC_DoCollision вызываетс€ в другом потоке  
	// который быстрее чем основной где ObjMngr::Update(float dt)-> while(gQueueMngr->GetMsgOUT(msgOut))
	msgOut.type = MAKEDWORD(MSG_END_UPDATE, 0);
	gQueueMngr->SendMsgOUT(msgOut); 
}

CollisionContainer::CollisionContainer(void)
{
	bStop = 1;
	LPDWORD hThreadID = 0;
	hT = CreateThread(NULL, 0, DoMessages, 0 , 0/*CREATE_SUSPENDED*/, hThreadID);	
	//Write_Log_String(hLog, " онструктор CollisionContainer\n");
}

void CollisionContainer::Stop()
{
	bStop = 1;
}

//void CollisionContainer::SendMsg(MSG_IN &msg)
//{
//}


void CollisionContainer::Start()
{
	bStop = 0;
}
void CollisionContainer::CreateCB(AABB bb, int colX, int colY, int colZ, int msgCnt, DWORD &outInd)
{
	CollisionBox *cb = new CollisionBox(bb, colX, colY, colZ/*, msgCnt*/);
	outInd = Insert(cb);
}

DWORD CollisionContainer::Insert(CollisionBox *cb)
{
	if(vFreeInd.size() == 0) 
	{
		vCBs.push_back(cb);
		return vCBs.size()-1;
	}
	else 
	{
		DWORD ind = vFreeInd.back();
		vCBs[ind] = cb;
		vFreeInd.pop_back();
		return ind;
	}
}

void  CollisionContainer::Remove(DWORD ind)
{	
	vFreeInd.push_back(ind);
}

CollisionContainer::~CollisionContainer(void)
{
	DWORD i = vCBs.size();	
	for( ; i > 0; --i)
	{
		delete vCBs[i-1];
	}
}
