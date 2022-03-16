#pragma once

#include "Debug_first.h"
#include "d3dUtil.h"
#include <string>
#include <windows.h>
//#include "afxwin.h"
#include <d3dx9math.h>
#include <vector>
#include "vertex.h"
#include "NvTriStrip.h"
#include "MyTypes.h"
#include "MyHelper.h"
#include "Debug_last.h"

void MyCreateBMP(const char *fName, int w, int h, std::vector<DWORD> &vPixels);
bool MyLoadHeightMap(const char *fName, std::vector<DWORD> &vec);
void HeightMap(std::string &nmObj, int row, int col, AABB &bb, std::vector<VertexPNT> &vVertOut);
std::string GetString(HANDLE hf);
std::string GetStringFromMapFile(char **mapFileData); 
// после поиска уст-ет указ-ль файла на следующий за найденным GUID байт
int CreateMyMesh(const char *fileName, char *outObjName, DWORD flags, ID3DXMesh **meshOut);
bool FindHeader(HANDLE f, GUID guid, DWORD counter); 
char* ReadMapFile(char *buff, void *buffOut, DWORD byteToRead, DWORD *byteReaded, int null);
bool FindSubSectHeader(char **src, GUID guid, DWORD end);
void LoadMtrl(char *fileName, Mtrl &m, LPDIRECT3DTEXTURE9 &t);
void LoadMtrlAndTexStr(const char *fileName, Mtrl &m, std::vector<std::string> &vTexStr);
void BuildVertexBuffer(void *pVBuffSrc, LPDIRECT3DVERTEXBUFFER9 &buffVDest, DWORD nVerts, 
					   DWORD bVertBytes, DWORD usage, D3DPOOL pool);
void BuildIndexBuffer(void *pIndBuffSrc, LPDIRECT3DINDEXBUFFER9 &buffIndDest, DWORD nFaces, 
					  /*DWORD bytePerFace, */DWORD usage, D3DPOOL pool);
void LoadBoundBox(const char *fileName, AABB &bb);
void FillBoundBoxVertBuffer(IDirect3DVertexBuffer9	*buff, AABB &bb);
void CreateOptTriStrip(IDirect3DIndexBuffer9 *buffInd,  DWORD inFaces, 
					   LPDIRECT3DINDEXBUFFER9 &outIndexBufferOptStrip, DWORD &dwNumFacesStrip);
void CreateOptTriList(IDirect3DIndexBuffer9	*buffInd, DWORD inFaces, LPDIRECT3DINDEXBUFFER9 &outIndexBufferOptList);

// {C766451C-9036-465c-B191-A3BA332F3634}
static const GUID HEADER_OBJECT = 
{ 0xc766451c, 0x9036, 0x465c, { 0xb1, 0x91, 0xa3, 0xba, 0x33, 0x2f, 0x36, 0x34 } };

// {C5B298E0-4DF5-436b-A387-B33DE7BF690F}
static const GUID HEADER_VERTEX = 
{ 0xc5b298e0, 0x4df5, 0x436b, { 0xa3, 0x87, 0xb3, 0x3d, 0xe7, 0xbf, 0x69, 0xf } };

// {B95197B8-F5D7-40a8-B0CD-CE19C77475B7}
static const GUID HEADER_INDICES = 
{ 0xb95197b8, 0xf5d7, 0x40a8, { 0xb0, 0xcd, 0xce, 0x19, 0xc7, 0x74, 0x75, 0xb7 } };

// {173B8034-7DD9-4e6d-9021-575641ACDF15}
static const GUID HEADER_ANIMATIONS = 
{ 0x173b8034, 0x7dd9, 0x4e6d, { 0x90, 0x21, 0x57, 0x56, 0x41, 0xac, 0xdf, 0x15 } };

// {36520475-7FBD-42f8-849C-9CE7AB6A5F54}
static const GUID HEADER_TERRASUBSEC = 
{ 0x36520475, 0x7fbd, 0x42f8, { 0x84, 0x9c, 0x9c, 0xe7, 0xab, 0x6a, 0x5f, 0x54 } };

// {4E57C7EA-30CC-4732-B4A6-0F1DBCF00093}
static const GUID HEADER_ANIMSET = 
{ 0x4e57c7ea, 0x30cc, 0x4732, { 0xb4, 0xa6, 0xf, 0x1d, 0xbc, 0xf0, 0x0, 0x93 } };

// {6F1909F2-C6E8-4cfd-962D-5DB1BCDD7D0B}
static const GUID HEADER_TEXTURE = 
{ 0x6f1909f2, 0xc6e8, 0x4cfd, { 0x96, 0x2d, 0x5d, 0xb1, 0xbc, 0xdd, 0x7d, 0xb } };

// {C1B37684-6524-4db9-84FD-C8128BEB8D88}
static const GUID HEADER_MATERIAL = 
{ 0xc1b37684, 0x6524, 0x4db9, { 0x84, 0xfd, 0xc8, 0x12, 0x8b, 0xeb, 0x8d, 0x88 } };

// {8EFFE6E7-C925-4ef3-B470-E2E0A5BB3CFD}
static const GUID HEADER_BONEHIERARCHY = 
{ 0x8effe6e7, 0xc925, 0x4ef3, { 0xb4, 0x70, 0xe2, 0xe0, 0xa5, 0xbb, 0x3c, 0xfd } };

// {CDD5FF26-710B-41f4-81CF-000A3258D74F}
static const GUID HEADER_MAINBOUNDBOX = 
{ 0xcdd5ff26, 0x710b, 0x41f4, { 0x81, 0xcf, 0x0, 0xa, 0x32, 0x58, 0xd7, 0x4f } };

template <class T>
int LoadVertexAndIndexList(char *fileName, char *outObjName, std::vector<T> &vVert,  std::vector<MYFACE> &vFace)
{	
	DWORD bytePerVert = 0;
	DWORD dwRead;
	DWORD nVerts = 0;
	DWORD nFaces = 0;
	D3DVERTEXELEMENT9	vElm[MAX_FVF_DECL_SIZE];
	UINT nElm	= 0;

	HANDLE hObjFile = CreateFile( fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		
	if(FindHeader(hObjFile, HEADER_OBJECT, 1))
	{	
		// считываем им€ объекта
		strcpy(outObjName, GetString(hObjFile).c_str());						 
	}
	// инициализируем массив вершин
	if(FindHeader(hObjFile, HEADER_VERTEX, 1))
	{
		// число байт на вершину
		ReadFile( hObjFile, &bytePerVert, sizeof(DWORD), &dwRead, NULL );
		// считываем кол-во элементов в объ€влении вершины
		ReadFile( hObjFile, &nElm, sizeof(UINT), &dwRead, NULL );
		// считываем элементы вершин
		for(int i = 0; i < nElm; ++i)
		{
			ReadFile( hObjFile, &vElm[i], sizeof(D3DVERTEXELEMENT9), &dwRead, NULL );
		}

		ReadFile( hObjFile, &nVerts, sizeof(DWORD), &dwRead, NULL );
		Write_Log_String(hLog,"вершин: %d\n", nVerts);

		T tmp;		
		DWORD i = 0;
		for( ; i < nVerts; ++i)
		{	 
			memset(&tmp, 0, sizeof(T));
			ReadFile( hObjFile, &tmp, sizeof(T), &dwRead, NULL );			
			vVert.push_back(tmp);
		}	
	}
	// инициализируем массив индексов
	if(FindHeader(hObjFile, HEADER_INDICES, 1))
	{		
		ReadFile( hObjFile, &nFaces, sizeof(DWORD), &dwRead, NULL );		

		MYFACE f;
		DWORD y = 0;
		for(; y < nFaces ; ++y)
		{	
			memset(&f, 0, sizeof(MYFACE));
			ReadFile( hObjFile, &f, sizeof(MYFACE), &dwRead, NULL );			
			vFace.push_back(f);
		}			
	}
	
	CloseHandle(hObjFile);

	return true;
}

template <class T>
void CreateMeshFromVertIndList(DWORD flags, std::vector<T> &vVert,  std::vector<MYFACE> &vFace, ID3DXMesh **meshOut)
{
	DWORD nVerts  = vVert.size();
	DWORD nFaces = vFace.size();
	ID3DXMesh *tmpMesh;
	D3DVERTEXELEMENT9	velm[MAX_FVF_DECL_SIZE];
	UINT				nElm;
	//VertexPNTB::Decl->GetDeclaration(velm, &nElm);
	T::Decl->GetDeclaration(velm, &nElm);
	HR(D3DXCreateMesh(nFaces, nVerts, D3DXMESH_MANAGED, velm, gd3dDevice, &tmpMesh));
	
	// заполн€ем буфер вершин
	BYTE *v = 0;
	tmpMesh->LockVertexBuffer(0, (void**)&v);
	memcpy(v, (void*)&vVert[0], nVerts * sizeof(T));
	tmpMesh->UnlockVertexBuffer();

	// заполн€ем буфер индексов
	WORD *k = 0;
	DWORD *src = (DWORD*)&vFace[0];
	tmpMesh->LockIndexBuffer(0, (void**)&k);	
	DWORD i = 0;	
	for(; i < nFaces * 3; i+=3)
	{
		k[i]   = src[i];
		k[i+1] = src[i+1];
		k[i+2] = src[i+2];		
	}
	tmpMesh->UnlockIndexBuffer();	

	// ќптимизируем сетку
	// Get the adjacency info of the non-optimized mesh.
	DWORD* adjacencyInfo = new DWORD[tmpMesh->GetNumFaces() * 3]; //???
	tmpMesh->GenerateAdjacency(0.0f, adjacencyInfo);

	// Array to hold optimized adjacency info.
	DWORD* optimizedAdjacencyInfo = new DWORD[tmpMesh->GetNumFaces() * 3]; //???
	//D3DXMESH_MANAGED | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE
	HR(tmpMesh->Optimize(flags, adjacencyInfo, optimizedAdjacencyInfo, 0, 0, meshOut));

	delete [] adjacencyInfo;
	delete [] optimizedAdjacencyInfo; 
	tmpMesh->Release();
}