#pragma once

#include "Debug_first.h"
#include "d3dutil.h"
#include <deque>
#include <bitset>
#include "MyTypes.h"
#include "Debug_last.h"

class QueueMngr
{
public:
	int   GetMsgIN(MY_MSG &msgIn);		// ����� �� �������
	void  SendMsgIN(MY_MSG &msgIn);		// � �������
	int   GetMsgOUT(MY_MSG &msgOut);		// ����� �� �������
	void  SendMsgOUT(MY_MSG &msgOut);	// � �������
	DWORD GetSizeMsgIN();
	DWORD GetSizeMsgOUT();

	QueueMngr(void);
	~QueueMngr(void);

private:
	CRITICAL_SECTION	csIn;
	CRITICAL_SECTION	csOut;

	std::deque<MY_MSG>	vMsgIn;
	std::deque<MY_MSG>	vMsgOut;
};
