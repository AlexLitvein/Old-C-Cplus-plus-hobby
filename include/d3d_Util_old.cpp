//=============================================================================
// d3dUtil.cpp by Frank Luna (C) 2005 All Rights Reserved.
//=============================================================================

#include "d3dUtil.h"
#include "Vertex.h"
#include "d3dApp.h"
#include "camera.h"

//bool MyDelay(float per, float dt)
//{
//	static float accum = 0.0f;
//	if(accum >= per)
//	{
//		accum = 0.0f;
//		return 1;
//	}
//	else
//	{
//		accum += dt;
//		return 0;
//	}
//}
void MatrixFromStr(std::string &str, D3DXMATRIX &m)
{
	int row = 0;
	int col = 0;
	std::string tmpStr;
	D3DXMatrixIdentity(&m);	

	int size = str.size();
	int i = 0;
	for(; i < size; ++i)
	{
		if(str[i] != ' ') tmpStr += str[i];
		else 
		{
			//if(str[i] == (char)("")) break;
			sscanf(tmpStr.c_str(), "%f", &m.m[row][col]);

			col++;
			if(col > 2) 
			{
				col = 0;
				row++;
			}			
			if(row > 3) row = 0;
			tmpStr.clear();
		}
	}
	// в конце нет пробела, по этому еще ТАК!
	if(i == size) sscanf(tmpStr.c_str(), "%f", &m.m[row][col]);
	//tmpStr.clear();
}

void getWorldPickingRay(D3DXVECTOR3& originW, D3DXVECTOR3& dirW)
{
	// Get the screen point clicked.
	POINT s;
	GetCursorPos(&s);

	// Make it relative to the client area window.
	ScreenToClient(gd3dApp->getMainWnd(), &s);

	// By the way we've been constructing things, the entire 
	// backbuffer is the viewport.

	float w = (float)gd3dApp->md3dPP.BackBufferWidth;
	float h = (float)gd3dApp->md3dPP.BackBufferHeight;

	D3DXMATRIX proj = gCamera->proj();

	float x = (2.0f*s.x/w - 1.0f) / proj(0,0);
	float y = (-2.0f*s.y/h + 1.0f) / proj(1,1);

	// Build picking ray in view space.
	D3DXVECTOR3 origin(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 dir(x, y, 1.0f);

	// So if the view matrix transforms coordinates from 
	// world space to view space, then the inverse of the
	// view matrix transforms coordinates from view space
	// to world space.
	D3DXMATRIX invView;
	D3DXMatrixInverse(&invView, 0, &gCamera->view());

	// Transform picking ray to world space.
	D3DXVec3TransformCoord(&originW, &origin, &invView);
	D3DXVec3TransformNormal(&dirW, &dir, &invView);
	D3DXVec3Normalize(&dirW, &dirW);
}

float Fast_Distance_3D(float fx, float fy, float fz)
{
	// this function computes the distance from the origin to x,y,z
	int temp;  // used for swaping
	int x,y,z; // used for algorithm

	// make sure values are all positive
	x = fabs(fx) * 1024;
	y = fabs(fy) * 1024;
	z = fabs(fz) * 1024;

	// sort values
	if (y < x) SWAP(x,y,temp)
	if (z < y) SWAP(y,z,temp)
	if (y < x) SWAP(x,y,temp)

	int dist = (z + 11 * (y >> 5) + (x >> 2) );
	// compute distance with 8% error
	return((float)(dist >> 10));
} 

//inline void Mem_Set_QUAD(void *dest, UINT data, int count)
//{
//// this function fills or sets unsigned 32-bit aligned memory
//// count is number of quads
//
//_asm 
//    { 
//    mov edi, dest   ; edi points to destination memory
//    mov ecx, count  ; number of 32-bit words to move
//    mov eax, data   ; 32-bit data
//    rep stosd       ; move data
//    } // end asm
//
//} // end Mem_Set_QUAD

bool TestPtInAABB(D3DXVECTOR3 &pt, AABB &bb)
{
	bool res = 1;
	res &= pt.x >= bb.minPt.x;
	//res &= pt.y >= bb.minPt.y;
	res &= pt.z >= bb.minPt.z;

	res &= pt.x < bb.maxPt.x;
	//res &= pt.y < bb.maxPt.y;
	res &= pt.z < bb.maxPt.z;
	return res;
}

void GenTriGrid(int numVertRows, int numVertCols,
				float dx, float dz, 
				const D3DXVECTOR3& center, 
				std::vector<D3DXVECTOR3>& verts,
				std::vector<DWORD>& indices)
{
	int numVertices = numVertRows*numVertCols;
	int numCellRows = numVertRows-1;
	int numCellCols = numVertCols-1;

	int numTris = numCellRows*numCellCols*2;

	float width = (float)numCellCols * dx;
	float depth = (float)numCellRows * dz;

	//===========================================
	// Build vertices.

	// We first build the grid geometry centered about the origin and on
	// the xz-plane, row-by-row and in a top-down fashion.  We then translate
	// the grid vertices so that they are centered about the specified 
	// parameter 'center'.

	verts.resize( numVertices );

	// Offsets to translate grid from quadrant 4 to center of 
	// coordinate system.
	float xOffset = -width * 0.5f; 
	float zOffset =  depth * 0.5f;

	int k = 0;
	for(float i = 0; i < numVertRows; ++i)
	{
		for(float j = 0; j < numVertCols; ++j)
		{
			// Negate the depth coordinate to put in quadrant four.  
			// Then offset to center about coordinate system.
			verts[k].x =  j * dx + xOffset;
			verts[k].z = -i * dz + zOffset;
			verts[k].y =  0.0f;

			// Translate so that the center of the grid is at the
			// specified 'center' parameter.
			D3DXMATRIX T;
			D3DXMatrixTranslation(&T, center.x, center.y, center.z);
			D3DXVec3TransformCoord(&verts[k], &verts[k], &T);
			
			++k; // Next vertex
		}
	}

	//===========================================
	// Build indices.

	indices.resize(numTris * 3);
	 
	// Generate indices for each quad.
	k = 0;
	for(DWORD i = 0; i < (DWORD)numCellRows; ++i)
	{
		for(DWORD j = 0; j < (DWORD)numCellCols; ++j)
		{
			indices[k]     =   i   * numVertCols + j;
			indices[k + 1] =   i   * numVertCols + j + 1;
			indices[k + 2] = (i+1) * numVertCols + j;
					
			indices[k + 3] = (i+1) * numVertCols + j;
			indices[k + 4] =   i   * numVertCols + j + 1;
			indices[k + 5] = (i+1) * numVertCols + j + 1;

			// next quad
			k += 6;
		}
	}
}

void LoadXFile(
	const std::string& filename, 
	ID3DXMesh** meshOut,
	std::vector<Mtrl>& mtrls, 
	std::vector<IDirect3DTexture9*>& texs)
{
	// Step 1: Load the .x file from file into a system memory mesh.

	ID3DXMesh* meshSys      = 0;
	ID3DXBuffer* adjBuffer  = 0;
	ID3DXBuffer* mtrlBuffer = 0;
	DWORD numMtrls          = 0;

	HR(D3DXLoadMeshFromX(filename.c_str(), D3DXMESH_SYSTEMMEM, g_pD3Device,
		&adjBuffer,	&mtrlBuffer, 0, &numMtrls, &meshSys));


	// Step 2: Find out if the mesh already has normal info?

	D3DVERTEXELEMENT9 elems[MAX_FVF_DECL_SIZE];
	HR(meshSys->GetDeclaration(elems));
	
	bool hasNormals = false;
	D3DVERTEXELEMENT9 term = D3DDECL_END();
	for(int i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	{
		// Did we reach D3DDECL_END() {0xFF,0,D3DDECLTYPE_UNUSED, 0,0,0}?
		if(elems[i].Stream == 0xff )
			break;

		if( elems[i].Type == D3DDECLTYPE_FLOAT3 &&
			elems[i].Usage == D3DDECLUSAGE_NORMAL &&
			elems[i].UsageIndex == 0 )
		{
			hasNormals = true;
			break;
		}
	}


	// Step 3: Change vertex format to VertexPNT.

	D3DVERTEXELEMENT9 elements[64];
	UINT numElements = 0;
	VertexPNT::Decl->GetDeclaration(elements, &numElements);

	ID3DXMesh* temp = 0;
	HR(meshSys->CloneMesh(D3DXMESH_SYSTEMMEM, 
		elements, g_pD3Device, &temp));
	ReleaseCOM(meshSys);
	meshSys = temp;


	// Step 4: If the mesh did not have normals, generate them.

	if( hasNormals == false)
		HR(D3DXComputeNormals(meshSys, 0));


	// Step 5: Optimize the mesh.

	HR(meshSys->Optimize(D3DXMESH_MANAGED | 
		D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, 
		(DWORD*)adjBuffer->GetBufferPointer(), 0, 0, 0, meshOut));
	ReleaseCOM(meshSys); // Done w/ system mesh.
	ReleaseCOM(adjBuffer); // Done with buffer.

	// Step 6: Extract the materials and load the textures.

	if( mtrlBuffer != 0 && numMtrls != 0 )
	{
		D3DXMATERIAL* d3dxmtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();

		for(DWORD i = 0; i < numMtrls; ++i)
		{
			// Save the ith material.  Note that the MatD3D property does not have an ambient
			// value set when its loaded, so just set it to the diffuse value.
			Mtrl m;
			m.ambient   = d3dxmtrls[i].MatD3D.Diffuse;
			m.diffuse   = d3dxmtrls[i].MatD3D.Diffuse;
			m.spec      = d3dxmtrls[i].MatD3D.Specular;
			m.specPower = d3dxmtrls[i].MatD3D.Power;
			mtrls.push_back( m );

			// Check if the ith material has an associative texture
			if( d3dxmtrls[i].pTextureFilename != 0 )
			{
				// Yes, load the texture for the ith subset
				IDirect3DTexture9* tex = 0;
				char* texFN = d3dxmtrls[i].pTextureFilename;
				HR(D3DXCreateTextureFromFile(g_pD3Device, texFN, &tex));

				// Save the loaded texture
				texs.push_back( tex );
			}
			else
			{
				// No texture for the ith subset
				texs.push_back( 0 );
			}
		}
	}
	ReleaseCOM(mtrlBuffer); // done w/ buffer
}

float GetRandomFloat(float a, float b)
{
	if( a >= b ) // bad input
		return a;

	// Get random float in [0, 1] interval.
	float f = (rand()%10001) * 0.0001f;

	return (f*(b-a))+a;
}

void GetRandomVec(D3DXVECTOR3& out)
{
	out.x = GetRandomFloat(-1.0f, 1.0f);
	out.y = GetRandomFloat(-1.0f, 1.0f);
	out.z = GetRandomFloat(-1.0f, 1.0f);

	// Project onto unit sphere.
	D3DXVec3Normalize(&out, &out);
}

void LoadSymbolTbl(/*char *fnSymTbl, */int symbW, int symbH, int strtIdx, int countSymb, 
				   LPDIRECT3DTEXTURE9 p_inTex, std::vector<RECT> &vCharsTexCoord)
{	
	RECT rc;	
	D3DSURFACE_DESC	sd;		
	p_inTex->GetLevelDesc(0, &sd);	

	int col = 0;
	int row = 0;
	int pitch = sd.Width / symbW;	

	row = strtIdx / pitch;
	col = strtIdx % pitch;
	if(col == 0) col = strtIdx;

	DWORD y = row;
	DWORD x = col;
	for( ; y <= sd.Height - symbH; y += symbH)
	{		
		for( ; x <= sd.Width - symbW; x += symbW)
		{
			rc.top  = y;
			rc.left = x;
			rc.right =  x + symbW;
			rc.bottom = y + symbH;

			if(countSymb != 0) --countSymb;
			else return;

			vCharsTexCoord.push_back(rc);
		}
		x = col;
	}
	//========== Генерирует в файл таблицу символов ============================
	//int dh = 0;
	//HANDLE h = Open_Log_File("Table.txt");
	//Write_Log_String(h, "\n");
	//for(DWORD i = 0; i < 256; ++i)
	//{
	//	//if(i%8 != 0) Write_Log_String(h, "| %3d(%2Xh)-%2c- ", i, i, i);
	//	if(dh <= 4) 
	//	{
	//		Write_Log_String(h, "| %3d(%2Xh) %2c ", i, i, i);
	//		dh++;
	//	}
	//	else
	//	{
	//		i--;
	//		Write_Log_String(h, "\n");
	//		dh = 0;
	//	}
	//}
	//Write_Log_String(h, "\n");
	//Close_Log_File(h);
}
void GetRCTexFromID(char *fileSymbTbl, int wSynb, int hSynb, int idButton, LPDIRECT3DTEXTURE9 ptex, RECT &outRC)
{	
	D3DSURFACE_DESC	sd;	
	ptex->GetLevelDesc(0, &sd);	

	int col = 0;
	int row = 0;
	int pitch = sd.Width / wSynb;	

	row = idButton / pitch;
	col = idButton % pitch;
	if(col == 0) col = idButton;

	outRC.top  = row * hSynb;
	outRC.left = col * wSynb;
	outRC.right =  outRC.left + wSynb;
	outRC.bottom = outRC.top + hSynb;		
}
void  AlignRC(RECT* pDstRC, RECT* pSrcRC, DWORD fAlign)
{
	int wSrc = pSrcRC->right - pSrcRC->left;
	int hSrc = pSrcRC->bottom - pSrcRC->top;

	int wDst = pDstRC->right - pDstRC->left;
	int hDst = pDstRC->bottom - pDstRC->top;

	if(HIWORD(fAlign) == ALGN_LEFT) pSrcRC->left = 0;
	if(LOWORD(fAlign) == ALGN_TOP) pSrcRC->top = 0;
	if(HIWORD(fAlign) == ALGN_RIGHT) pSrcRC->left = wDst - wSrc;
	if(LOWORD(fAlign) == ALGN_BOTTOM) pSrcRC->top = hDst - hSrc;
	if(HIWORD(fAlign) == ALGN_MH) pSrcRC->left = pDstRC->left + ((wDst - wSrc) >> 1);
	if(LOWORD(fAlign) == ALGN_MV) pSrcRC->top = pDstRC->top + ((hDst - hSrc) >> 1);

	pSrcRC->right = pSrcRC->left + wSrc;
	pSrcRC->bottom = pSrcRC->top + hSrc;
}

bool  GetClipRect(RECT &rcDstIn, RECT &rcSrcInOut)
{
	if(rcSrcInOut.left > rcDstIn.right) return 0;
	if(rcSrcInOut.right < rcDstIn.left) return 0;
	if(rcSrcInOut.top > rcDstIn.bottom) return 0;
	if(rcSrcInOut.bottom < rcDstIn.top) return 0;

	if(rcSrcInOut.left < rcDstIn.left) rcSrcInOut.left = rcDstIn.left;
	if(rcSrcInOut.right > rcDstIn.right) rcSrcInOut.right = rcDstIn.right;
	if(rcSrcInOut.top < rcDstIn.top) rcSrcInOut.top = rcDstIn.top;
	if(rcSrcInOut.bottom > rcDstIn.bottom) rcSrcInOut.bottom = rcDstIn.bottom;

	return 1;
}

bool  GetSubSprite(POINT &pt, RECT &rcDst, RECT &rcSrcInOut)
{
	//pParent->DrawRC(rcDst.left, rcDst.top, rcSrcInOut);
	RECT rcTrans;	
	rcTrans.left = pt.x; rcTrans.right = rcTrans.left + (rcSrcInOut.right - rcSrcInOut.left);
	rcTrans.top = pt.y; rcTrans.bottom = rcTrans.top + (rcSrcInOut.bottom - rcSrcInOut.top);

	if(rcTrans.left > rcDst.right) return 0;
	if(rcTrans.right < rcDst.left) return 0;
	if(rcTrans.top > rcDst.bottom) return 0;
	if(rcTrans.bottom < rcDst.top) return 0;

	if(rcTrans.left < rcDst.left) 
	{
		rcSrcInOut.left += (rcDst.left - rcTrans.left);
		pt.x += (rcDst.left - rcTrans.left);
	}
	if(rcTrans.right > rcDst.right)
	{
		rcSrcInOut.right -= (rcTrans.right - rcDst.right);
		//pt.x += (rcDst.left - rcTrans.left);
	}
	if(rcTrans.top < rcDst.top) 
	{
		rcSrcInOut.top += (rcDst.top - rcTrans.top);
		pt.y += (rcDst.top - rcTrans.top);
	}
	if(rcTrans.bottom > rcDst.bottom) rcSrcInOut.bottom -= (rcTrans.bottom - rcDst.bottom);

	return 1;
}

void PrintD3DXMATRIXToLogFile(HANDLE file, D3DXMATRIX &m)
{
	Write_Log_String(file, "\n %9f %9f %9f %9f\n", m._11, m._12, m._13, m._14);
	Write_Log_String(file, " %9f %9f %9f %9f\n", m._21, m._22, m._23, m._24);
	Write_Log_String(file, " %9f %9f %9f %9f\n", m._31, m._32, m._33, m._34);
	Write_Log_String(file, " %9f %9f %9f %9f\n", m._41, m._42, m._43, m._44);
}