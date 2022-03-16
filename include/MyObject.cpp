#include "MyObject.h"
#include "QueueMngr.h"
#include "mytypes.h"

MyObject::MyObject(void) : type(CONST_OBJ), visible(0)
{
	//vVelosity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//velosity = 0.0f;
	bb.maxPt = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	bb.minPt = bb.maxPt;

	D3DXMatrixIdentity(&worldTM);
	RESET_BIT(state, 0xffffffff);	
}

MyObject::~MyObject(void){}

//void MyObject::ProcessState(float dt)
//{
//	/*D3DXMATRIX m;
//	D3DXMatrixIdentity(&m);
//	D3DXVECTOR3 posW;
//	float newAngl;
//	if(state & ACT_HIT) 
//	{
//		newAngl = GetRandomFloat(0.0f, D3DX_PI);
//		D3DXMatrixRotationY(&m, newAngl);
//	}
//	posW.x = worldTM._41; posW.y = worldTM._42; posW.z = worldTM._43;
//	worldTM *= m;
//	worldTM._41 = posW.x; worldTM._42 = posW.y; worldTM._43 = posW.z;*/ 
//
//	if(type == DYNAMIC_OBJ)
//	{
//		worldTM._41 += worldTM._13 * dt;//(dt * xyzVelocity.x);
//	
//		worldTM._42 += (dt * xyzVelocity.y);
//		xyzVelocity.y -= gravity;
//		if(xyzVelocity.y <= 0.0f) xyzVelocity.y = 0.0f;
//		
//		worldTM._43 += -(worldTM._33 * dt); //-(dt * xyzVelocity.z);
//	}	
//}

void MyObject::DoLogic(float dt)
{
	/*D3DXVECTOR3 camPos = gCamera->pos();
	distToCam = Fast_Distance_3D(worldTM._41 + (-camPos.x), worldTM._42 + (-camPos.y), worldTM._43 + (-camPos.z));	
	if(distToCam >= 0.0f ) setNumV = 10000;
	if(distToCam >= 8.0f ) setNumV = 1000;
	if(distToCam >= 20.0f ) setNumV = 500;
	if(distToCam >= 50.0f ) setNumV = 0;*/
	MY_MSG	msg; 
		
	pRendData->bb.xform(worldTM, bb);
	funcIQ(this, dt);	

//------------------------------End Logic-------------------------------------
	if(type == DYNAMIC_OBJ)
	{
		msg.type	= MAKEDWORD(DYNAMIC_OBJ, 0); // указать тип!!!!
		msg.pObj	= (DWORD *)this;
		msg.pt		= bb.center(); // D3DXVECTOR3(worldTM._41, worldTM._42, worldTM._43);

		gQueueMngr->SendMsgIN(msg);
	}	
}