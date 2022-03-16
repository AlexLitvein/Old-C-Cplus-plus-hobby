//=============================================================================
// d3dUtil.h by Frank Luna (C) 2005 All Rights Reserved.
//
// Contains various utility code for DirectX applications, such as, clean up
// and debugging code.
//=============================================================================

#ifndef D3DUTIL_H
#define D3DUTIL_H

// Enable extra D3D debugging in debug builds if using the debug DirectX runtime.  
// This makes D3D objects work well in the debugger watch window, but slows down 
// performance slightly.
#if defined(DEBUG) | defined(_DEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

//#include <d3d9.h>
//#include <D3DX10Math.h>
#include "MyHelper.h"
//#include <dxerr.h>
//#include <d3dx9math.h>

#define DEG_TO_RAD(ang) ((ang)*D3DX_PI/180.0f) // ????/
#define RAD_TO_DEG(rads) ((rads)*180.0f/D3DX_PI) // ????/

//===============================================================
// Debug
#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR3D
	#define HR3D(x)                                      \
	{                                                  \
		HRESULT hr = x;                                \
		if(FAILED(hr))                                 \
		{                                              \
			DXTrace(__FILE__, __LINE__, hr, #x, TRUE); \
		}                                              \
	}
	#endif

#else
	#ifndef HR3D
	#define HR3D(x) x;
	#endif
#endif 



#endif // D3DUTIL_H