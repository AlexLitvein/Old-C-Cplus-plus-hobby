#pragma once

#include "Debug_first.h"
#include "d3dUtil.h"
#include "camera.h"
#include "GraphData.h"
#include "Debug_last.h"

class MyObject
{
public:
	MyObject(void);	
	~MyObject(void);

	DWORD		type;
	DWORD		state;
	DWORD		visible;		
	float		distToCam;
	DWORD		setNumV;
	char		name[16];	
	AABB		bb;

	D3DXMATRIX	worldTM;
	D3DXVECTOR3 xyzVelocity;
	D3DXVECTOR3 anglXYZ;
	float		initVel;
	float		gravity;	//???
	float		resistance; //???
	float		t;
	float		distView;
	
	BaseRenderObj *pRendData;
	DWORD *ptrInLstObj;	
	DWORD indInRenderLst;	

	//std::vector<MY_MSG> vMsg;

	void DoLogic(float dt);
	//void ProcessState(float dt);
	void (*funcIQ)(MyObject *obj, float dt);
};
