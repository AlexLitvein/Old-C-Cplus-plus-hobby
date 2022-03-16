//=============================================================================
// Camera.h by Frank Luna (C) 2004 All Rights Reserved.
//=============================================================================

#ifndef CAMERA_H
#define CAMERA_H

#include "Debug_first.h"
#include "d3dUtil.h"
#include <d3dx9.h>
#include "mytypes.h"
#include "Debug_last.h"

// Forward declaration.

class Terrain;
class QueueMngr;
struct MSG_OUT;

class Camera
{
public:
	Camera();

	const D3DXMATRIX& view() const;
	const D3DXMATRIX& proj() const;
	const D3DXMATRIX& viewProj() const;

	const D3DXVECTOR3& right() const;
	const D3DXVECTOR3& up() const;
	const D3DXVECTOR3& look() const;

	D3DXVECTOR3& pos();
	//float distFromOrig;

	void lookAt(D3DXVECTOR3& pos, D3DXVECTOR3& target, D3DXVECTOR3& up);
	void setLens(float fov, float aspect, float nearZ, float farZ);
	void setSpeed(float s);
	void setType(bool bFly);

	// Box coordinates should be relative to world space.
	bool isVisible(const AABB& box)const;
	void update(float dt, float offsetHeight);

	std::vector<MY_MSG> vMsg;

	/*int indSB;	
	DWORD indC;
	DWORD indA;
	float C;
	float D;
	float A;
	float B;*/
	//float infoY;

protected:
	void buildView();
	void buildWorldFrustumPlanes();

//protected:
public:
	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mViewProj;

	// Relative to world space.
	D3DXVECTOR3 mPosW;
	D3DXVECTOR3 mRightW;
	D3DXVECTOR3 mUpW;
	D3DXVECTOR3 mLookW;

	bool  mFly;
	float mSpeed;
	//TerrainSubSector *subSect;

	// Frustum Planes
	D3DXPLANE mFrustumPlanes[6]; // [0] = near
	                             // [1] = far
	                             // [2] = left
	                             // [3] = right
	                             // [4] = top
	                             // [5] = bottom

	
};

#endif // CAMERA_H