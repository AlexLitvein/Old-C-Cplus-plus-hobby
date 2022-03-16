#include "QueueMngr.h"
#include "CollisionContainer.h"

QueueMngr *gQueueMngr = 0;

void QueueMngr::SendMsgIN(MY_MSG &msgIn)
{
	EnterCriticalSection(&csIn);
	vMsgIn.push_back(msgIn);
	LeaveCriticalSection(&csIn);
}
DWORD QueueMngr::GetSizeMsgIN()
{
	return vMsgIn.size();
}
DWORD QueueMngr::GetSizeMsgOUT()
{
	return vMsgOut.size();
}
int QueueMngr::GetMsgIN(MY_MSG &msgIn)
{
	int res = 0;
	EnterCriticalSection(&csIn);
	if(vMsgIn.size() != 0)
	{
		msgIn = vMsgIn.front();	
		vMsgIn.pop_front();
		res = 1;
	}
	else res = 0;
	LeaveCriticalSection(&csIn);
	return res;
}

int QueueMngr::GetMsgOUT(MY_MSG &msgOut)
{
	int res = 0;
	EnterCriticalSection(&csOut);
	if(vMsgOut.size() != 0)
	{
		msgOut = vMsgOut.front();	
		vMsgOut.pop_front();
		res = 1;
	}
	else res = 0;
	LeaveCriticalSection(&csOut);
	return res;
}

void QueueMngr::SendMsgOUT(MY_MSG &msgOut)
{
	UINT res = 1;
	EnterCriticalSection(&csOut);

	/*if(vMsgOut.size() != 0)
	{
		res &= vMsgOut.back().type == MSG_END_UPDATE;
		res &= msgOut.type == MSG_END_UPDATE;		
	}
	else res = 0;
	if(!res) vMsgOut.push_back(msgOut);*/
	vMsgOut.push_back(msgOut);

	LeaveCriticalSection(&csOut);
}

QueueMngr::QueueMngr(void)
{
	InitializeCriticalSection(&csIn);
	InitializeCriticalSection(&csOut);
}

QueueMngr::~QueueMngr(void)
{
	// останавливаем обработку входных сообщений, иначе gCollContainer пытается
	// получить сообщение тогда как QueueMngr уже уничтожен
	gCollContainer->Stop(); 
	DeleteCriticalSection(&csIn);
	DeleteCriticalSection(&csOut);
}
