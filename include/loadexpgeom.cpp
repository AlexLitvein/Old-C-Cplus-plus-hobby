#include "loadexpgeom.h"
//#include "d3dUtil.h"

void MyCreateBMP(const char *fName, int w, int h, std::vector<DWORD> &vPixels)
{
	DWORD byteWriten;	
	BITMAPFILEHEADER bitmapfileheader;
	BITMAPINFOHEADER bitmapinfoheader; 	

	bitmapfileheader.bfType = 0x4D42;	
	bitmapfileheader.bfReserved1 = 0;
	bitmapfileheader.bfReserved2 = 0;	
	
	bitmapinfoheader.biSize = sizeof(BITMAPINFOHEADER); 
	bitmapinfoheader.biWidth = w; 
	bitmapinfoheader.biHeight = h; 
	bitmapinfoheader.biPlanes = 1; 
	bitmapinfoheader.biBitCount = 32; 
	bitmapinfoheader.biCompression = BI_RGB; 
	bitmapinfoheader.biSizeImage = 0; 
	bitmapinfoheader.biXPelsPerMeter = 0; 
	bitmapinfoheader.biYPelsPerMeter = 0; 
	bitmapinfoheader.biClrUsed = 0; 
	bitmapinfoheader.biClrImportant = 0; 

	bitmapfileheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bitmapfileheader.bfSize = bitmapfileheader.bfOffBits + bitmapinfoheader.biSizeImage;

	HANDLE file = CreateFile(fName, GENERIC_WRITE,0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		
	WriteFile(file, &bitmapfileheader, sizeof(BITMAPFILEHEADER), &byteWriten, 0);
	WriteFile(file, &bitmapinfoheader, sizeof(BITMAPINFOHEADER), &byteWriten, 0);
	
	DWORD size = vPixels.size();
	for(DWORD i = 0; i < size; ++i)
	{
		WriteFile(file, &vPixels[i], sizeof(DWORD), &byteWriten, 0);
	}
	CloseHandle(file);
}

void HeightMap(std::string &nmObj, int row, int col, AABB &bb, std::vector<VertexPNT> &vVertOut)
{
	std::vector<DWORD> vHeights;
	vHeights.resize((row+1) * (col+1), 0x00ff);
	
	float kx = row / (bb.maxPt.x - bb.minPt.x);
	float kz = col / (bb.maxPt.z - bb.minPt.z);

	float toPosX = -bb.minPt.x;
	float toPosZ = -bb.minPt.z;
	
	for (DWORD i = 0; i < vVertOut.size(); i++)
	{
		DWORD m = floor(((vVertOut[i].pos.x + toPosX) * kx) + 0.5f);
		DWORD n = floor(((vVertOut[i].pos.z + toPosZ) * kz) + 0.5f);
		DWORD ind = m + (n * (row + 1));
		unsigned char h = (unsigned char)floor(((vVertOut[i].pos.y * 255) / bb.maxPt.y) + 0.5f);

		unsigned char minY = vHeights[ind] & 0x000000ff;
		unsigned char maxY = (vHeights[ind] & 0x0000ffff) >> 8;
		minY = min(minY, h);
		maxY = max(maxY, h);
		DWORD res = maxY; 
		res <<= 8; 
		res |= minY;
		res |= 0xff000000;
		vHeights[ind] = res;
		//Write_Log_String(ghLog, "maxY = %u minY = %u\n", maxY, minY);			
	}	
	
	nmObj += ".bmp";
	MyCreateBMP(nmObj.c_str(), row+1, col+1, vHeights);
}

bool MyLoadHeightMap(const char *fName, std::vector<DWORD> &vec)
{
	DWORD dwRead;	
	BITMAPFILEHEADER bitmapfileheader;
	BITMAPINFOHEADER bitmapinfoheader; 
	unsigned char byte;	
	DWORD pix = 0;
	
	HANDLE file = CreateFile(fName, GENERIC_READ,0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);	

	ReadFile( file, &bitmapfileheader, sizeof(BITMAPFILEHEADER), &dwRead, NULL );
	ReadFile( file, &bitmapinfoheader, sizeof(BITMAPINFOHEADER), &dwRead, NULL );

	if(bitmapfileheader.bfType != 0x4D42 || bitmapinfoheader.biCompression != BI_RGB)
	{
		CloseHandle(file);
		return 0;
	}
	
	DWORD byteInPixel = bitmapinfoheader.biBitCount / 8;
	DWORD numPixels = bitmapinfoheader.biWidth * bitmapinfoheader.biHeight;

	DWORD tmp;
	for(DWORD i = 0; i < numPixels; ++i)
	{
		pix = 0;
		for(DWORD j = 0; j < byteInPixel; ++j)
		{	
			tmp = 0;			
			ReadFile( file, &byte, sizeof(unsigned char), &dwRead, NULL );
			tmp = byte;
			tmp = tmp << (j*8);
			pix = pix | tmp;			
		}
		vec.push_back(pix);
	}	
	CloseHandle(file);
	return 1;
}
std::string GetString(HANDLE hf)
{
	DWORD dwRead = 0;
	std::string tmp;
	char ch = 'a';

	while(1)
	{
		ReadFile ( hf, &ch, 1, &dwRead, NULL );
		if(ch != 0) tmp.push_back(ch);
		else break;
	}
	return tmp;
}

std::string GetStringFromMapFile(char **mapFileData)
{
	DWORD dwRead = 0;
	std::string tmp;
	char ch = 'a';

	while(1)
	{
		*mapFileData = ReadMapFile ( *mapFileData, &ch, 1, &dwRead, NULL );
		if(ch != 0) tmp.push_back(ch);
		else break;
	}
	return tmp;
}

bool FindHeader(HANDLE f, GUID guid, DWORD counter)
{
	if(!f) return 0;
	SetFilePointer(f, 0, 0, FILE_BEGIN);
	bool bFind = false;
	DWORD cx = 0;
	DWORD dwRead;
	GUID tmpGuid;
	DWORD fSize = GetFileSize(f, 0);	 
	for(DWORD i = 1; i <= fSize; i++)
	{		
		ReadFile( f, &tmpGuid, sizeof(GUID), &dwRead, NULL );
		if(guid == tmpGuid)
		{
			cx++;
			if(cx == counter)
			{
				bFind = true;
				break;
			}
		}
		else SetFilePointer(f, i + 1, 0, FILE_BEGIN);		
	}
	return bFind;
}

char* ReadMapFile(char *buff, void *buffOut, DWORD byteToRead, DWORD *byteReaded, int null)
{
	memcpy(buffOut, buff, byteToRead);
	return buff + byteToRead;
}

bool FindSubSectHeader(char **src, GUID guid, DWORD end)
{	
	bool bFind = false;	
	DWORD dwRead;
	GUID tmpGuid;	 
	//for(DWORD i = 0; i + sizeof(GUID) < fSize; i+=sizeof(GUID))
	while((DWORD)*src < (end - sizeof(GUID)))
	{		
		//ReadFile( f, &tmpGuid, sizeof(GUID), &dwRead, NULL );
		*src = ReadMapFile( *src, &tmpGuid, sizeof(GUID), &dwRead, NULL );
		if(guid == tmpGuid)
		{
			bFind = true;
			break;
		}
	}
	return bFind;
}
void BuildVertexBuffer(void *pVBuffSrc, LPDIRECT3DVERTEXBUFFER9 &buffVDest, DWORD nVerts, 
					   DWORD bytePerVert, DWORD usage, D3DPOOL pool)
{	
	
	HR(g_pD3Device->CreateVertexBuffer( nVerts * bytePerVert, usage, 0, pool, &buffVDest, 0));

	//if(0 == buffV) MessageBox(0,"buffV","",0);

	BYTE *v = 0;		
	HR(buffVDest->Lock(0, 0,  (void**)&v, 0));
	memcpy(v, /*(BYTE*)*/pVBuffSrc, nVerts * bytePerVert);	
	HR(buffVDest->Unlock());
}

void BuildIndexBuffer(void *pIndBuffSrc, LPDIRECT3DINDEXBUFFER9 &buffIndDest, DWORD nFaces, 
					  /*DWORD bytePerFace, */DWORD usage, D3DPOOL pool)
{
	g_pD3Device->CreateIndexBuffer(nFaces * 3 * sizeof( WORD ), usage /*D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY*/, 
										D3DFMT_INDEX16, pool , &buffIndDest, 0);	//	D3DPOOL_DEFAULT
	
	WORD *k = 0;
	DWORD *src = (DWORD*)pIndBuffSrc;

	HR(buffIndDest->Lock(0, 0,  (void**)&k, 0));
	DWORD i = 0;	
	for(; i < nFaces * 3; i+=3)
	{
		k[i]   = src[i];
		k[i+1] = src[i+1];
		k[i+2] = src[i+2];		
	}
	HR(buffIndDest->Unlock());	
}
int CreateMyMesh(const char *fileName, char *outObjName, DWORD flags, ID3DXMesh **meshOut)
{	
	DWORD bytePerVert = 0;
	DWORD dwRead;
	DWORD nVerts = 0;
	DWORD nFaces = 0;	
	D3DVERTEXELEMENT9	vElm[MAX_FVF_DECL_SIZE];
	UINT nElm	= 0;
	HANDLE hObjFile = 0;
	//LPDIRECT3DVERTEXBUFFER9 p_vVert;
	//LPDIRECT3DINDEXBUFFER9  p_vFace;
	ID3DXMesh *tmpMesh;
	std::string objName;

	hObjFile = CreateFile( fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );	
	if(hObjFile == INVALID_HANDLE_VALUE) return 0; // 

	if(FindHeader(hObjFile, HEADER_OBJECT, 1))
	{	
		// считываем имя объекта
		objName = GetString(hObjFile);
		if(outObjName != 0) strcpy(outObjName, objName.c_str());
		//Write_Log_String(hLog, "%s\n", outObjName);
	}
	// инициализируем массив вершин
	if(FindHeader(hObjFile, HEADER_VERTEX, 1))
	{
		//Write_Log_String(hLog,"HEADER_VERTEX\n");
		// число байт на вершину
		ReadFile( hObjFile, &bytePerVert, sizeof(DWORD), &dwRead, NULL );
		// считываем кол-во элементов в объявлении вершины
		ReadFile( hObjFile, &nElm, sizeof(UINT), &dwRead, NULL );		
		// считываем элементы вершин
		UINT e = 0;
		for(; e < nElm; ++e)
		{
			ReadFile( hObjFile, &vElm[e], sizeof(D3DVERTEXELEMENT9), &dwRead, NULL );
			/*Write_Log_String(hLog, "%d %d %d %d %d %d\n", 	vElm[i].Stream, vElm[i].Offset, vElm[i].Type,
															vElm[i].Method, vElm[i].Usage,  vElm[i].UsageIndex);*/
		}
		// Обязательно! D3DDECL_END();
		vElm[e].Stream = 0xFF;  	
		vElm[e].Offset = 0;
		vElm[e].Type = D3DDECLTYPE_UNUSED;
		vElm[e].Method = 0;
		vElm[e].Usage = 0;
		vElm[e].UsageIndex = 0;

		ReadFile( hObjFile, &nVerts, sizeof(DWORD), &dwRead, NULL );
		ReadFile( hObjFile, &nFaces, sizeof(DWORD), &dwRead, NULL );
		//Write_Log_String(hLog,"вершин: %d\n", nVerts);
		HR(D3DXCreateMesh(nFaces, nVerts, D3DXMESH_MANAGED, vElm, g_pD3Device, &tmpMesh));

		DWORD total = nVerts * (bytePerVert / 4);
		DWORD *v = 0;		
		HR(tmpMesh->LockVertexBuffer(0,(void**)&v));
		DWORD x = 0;
		for( ; x < total; ++x)
		{				
			ReadFile( hObjFile, &v[x], sizeof(DWORD), &dwRead, NULL );	
			//Write_Log_String(hLog, "%f\n", (float)*v[x]);
		}	
		HR(tmpMesh->UnlockVertexBuffer());
	}

	// инициализируем массив индексов
	if(FindHeader(hObjFile, HEADER_INDICES, 1))
	{	
		ReadFile( hObjFile, &nFaces, sizeof(DWORD), &dwRead, NULL );
		
		WORD *k = 0;		
		HR(tmpMesh->LockIndexBuffer(0, (void**)&k));
		DWORD i = 0;
		DWORD n = 0;
		for(; i < nFaces * 3; i+=3)
		{
			ReadFile( hObjFile, &n, sizeof(DWORD), &dwRead, NULL );
			k[i] = n;
			ReadFile( hObjFile, &n, sizeof(DWORD), &dwRead, NULL );
			k[i+1] = n;
			ReadFile( hObjFile, &n, sizeof(DWORD), &dwRead, NULL );
			k[i+2] = n;
			//Write_Log_String(hLog, "индексы: %d %d %d\n", k[i], k[i+1], k[i+2]);
		}
		HR(tmpMesh->UnlockIndexBuffer());	
	}	
	CloseHandle(hObjFile);	

	// Оптимизируем сетку
	// Get the adjacency info of the non-optimized mesh.
	DWORD* adjacencyInfo = new DWORD[tmpMesh->GetNumFaces() * 3]; //???
	tmpMesh->GenerateAdjacency(0.0f, adjacencyInfo);

	// Array to hold optimized adjacency info.
	DWORD* optimizedAdjacencyInfo = new DWORD[tmpMesh->GetNumFaces() * 3]; //???
	//D3DXMESH_MANAGED | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE
	HR(tmpMesh->Optimize(flags, adjacencyInfo, optimizedAdjacencyInfo, 0, 0, meshOut));
	

	delete [] optimizedAdjacencyInfo; 	
	delete [] adjacencyInfo;	
	tmpMesh->Release();	

	return 1;
}
void LoadMtrl(char *fileName, Mtrl &m, LPDIRECT3DTEXTURE9 &t)
{
	DWORD dwRead;	
	char fileNameTex[128];

	HANDLE hObjFile = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if(FindHeader(hObjFile, HEADER_MATERIAL, 1))
	{
		ReadFile( hObjFile, &m, sizeof(Mtrl), &dwRead, NULL );
		strcpy(fileNameTex, GetString(hObjFile).c_str());		
		HR(D3DXCreateTextureFromFile(g_pD3Device, fileNameTex, &t));		
	}
	else 
	{ 
		/* конструктор материала без параметров инициализирует белый материал */
		HR(D3DXCreateTextureFromFile(g_pD3Device, "RedTex.tga", &t)); // whitetex.dds
	}	
	CloseHandle(hObjFile);	
}


void LoadMtrlAndTexStr(const char *fileName, Mtrl &m, std::vector<std::string> &vTexStr)
{
	// Если для объекта не определен материал,а следовательно и текстура, 
	// то возвращается строка "Whitetex.bmp"
	int numTex = 0;
	DWORD dwRead;	
	HANDLE hObjFile = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if(FindHeader(hObjFile, HEADER_MATERIAL, 1))
	{
		ReadFile( hObjFile, &m, sizeof(Mtrl), &dwRead, NULL );
		ReadFile( hObjFile, &numTex, sizeof(int), &dwRead, NULL );
		for(int i = 0; i < numTex; ++i)
		{
			//strcpy(texStr, GetString(hObjFile).c_str());
			vTexStr.push_back(GetString(hObjFile).c_str());
		}
	}
	else 
	{
		vTexStr.push_back("Whitetex.bmp");
		//strcpy(texStr, "Whitetex.bmp");
		m.ambient = m.diffuse = m.spec = 0xffffffff;
		m.specPower = 0.0f;		
	}
	
	CloseHandle(hObjFile);	
}

void FillBoundBoxVertBuffer(IDirect3DVertexBuffer9	*buff, AABB &bb)
{
	D3DXVECTOR3 v1 = bb.minPt;
	D3DXVECTOR3 v2 = D3DXVECTOR3(bb.minPt.x, bb.maxPt.y, bb.minPt.z);
	D3DXVECTOR3 v3 = D3DXVECTOR3(bb.maxPt.x, bb.maxPt.y, bb.minPt.z);
	D3DXVECTOR3 v4 = D3DXVECTOR3(bb.maxPt.x, bb.minPt.y, bb.minPt.z);
	D3DXVECTOR3 v5 = D3DXVECTOR3(bb.minPt.x, bb.minPt.y, bb.maxPt.z);
	D3DXVECTOR3 v6 = D3DXVECTOR3(bb.minPt.x, bb.maxPt.y, bb.maxPt.z);
	D3DXVECTOR3 v7 = bb.maxPt;
	D3DXVECTOR3 v8 = D3DXVECTOR3(bb.maxPt.x, bb.minPt.y, bb.maxPt.z);

	VertexPos *v = 0;		
	HR(buff->Lock(0, 0,  (void**)&v, 0));

	v[0] = v1;
	v[1] = v2;
	v[2] = v2;
	v[3] = v3;
	v[4] = v3;
	v[5] = v4;
	v[6] = v4;
	v[7] = v1;

	v[8] = v5;
	v[9] = v6;
	v[10] = v6;
	v[11] = v7;
	v[12] = v7;
	v[13] = v8;
	v[14] = v8;
	v[15] = v5;

	v[16] = v1;
	v[17] = v5;
	v[18] = v2;
	v[19] = v6;
	v[20] = v3;
	v[21] = v7;
	v[22] = v4;
	v[23] = v8;	
	HR(buff->Unlock());
}

void LoadBoundBox(const char *fileName, AABB &bb)
{
	DWORD dwRead;
	HANDLE hObjFile = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if(FindHeader(hObjFile, HEADER_MAINBOUNDBOX, 1))
	{	
		ReadFile( hObjFile, &bb, sizeof(AABB), &dwRead, NULL );
	}
	CloseHandle(hObjFile);
}

void CreateOptTriList(IDirect3DIndexBuffer9	*buffInd, DWORD inFaces, LPDIRECT3DINDEXBUFFER9 &outIndexBufferOptList)
{
	//primitive groups from NvTriStrip
	PrimitiveGroup *pPrimitiveGroupsList;
	//PrimitiveGroup *g_pPrimitiveGroupsStrip;
	// NVTriStrip setup code
	SetCacheSize(CACHESIZE_GEFORCE3);
	SetStitchStrips(true);
	SetMinStripSize(0);

	//First, we create the optimized list indices
	SetListsOnly(true);

	unsigned short* pIndices = NULL;
	HR(buffInd->Lock(0, 0, (void**)&pIndices, 0)); //D3DLOCK_DISCARD D3DLOCK_READONLY			

	unsigned short numSections;	
	GenerateStrips(pIndices, inFaces * 3, &pPrimitiveGroupsList, &numSections);
	buffInd->Unlock();	

	//copy optimized data to index buffer
	g_pD3Device->CreateIndexBuffer(pPrimitiveGroupsList[0].numIndices * sizeof(unsigned short), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
		D3DPOOL_DEFAULT, &outIndexBufferOptList, NULL);

	unsigned short* pDest;
	outIndexBufferOptList->Lock(0, pPrimitiveGroupsList[0].numIndices * sizeof(unsigned short), (void**)&pDest, 0);
	memcpy(pDest, pPrimitiveGroupsList[0].indices, pPrimitiveGroupsList[0].numIndices * sizeof(unsigned short));
	outIndexBufferOptList->Unlock();
}
void CreateOptTriStrip(IDirect3DIndexBuffer9 *buffInd,  DWORD inFaces, 
					   LPDIRECT3DINDEXBUFFER9 &outIndexBufferOptStrip,
					   DWORD &dwNumFacesStrip)
{
	//primitive groups from NvTriStrip	
	PrimitiveGroup *pPrimitiveGroupsStrip;
	// NVTriStrip setup code
	SetCacheSize(CACHESIZE_GEFORCE3);
	SetStitchStrips(true);
	SetMinStripSize(0);
	//Next, we create the optimized strip indices
	SetListsOnly(false);

	unsigned short* pIndices = NULL;
	HR(buffInd->Lock(0, 0, (void**)&pIndices, 0));

	unsigned short numSections;	
	GenerateStrips(pIndices, inFaces * 3, &pPrimitiveGroupsStrip, &numSections);
	buffInd->Unlock();

	//copy optimized data to index buffer
	g_pD3Device->CreateIndexBuffer(pPrimitiveGroupsStrip[0].numIndices * sizeof(unsigned short), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
		D3DPOOL_DEFAULT, &outIndexBufferOptStrip, NULL);

	unsigned short* pDest;
	outIndexBufferOptStrip->Lock(0, pPrimitiveGroupsStrip[0].numIndices * sizeof(unsigned short), (void**)&pDest, 0);
	memcpy(pDest, pPrimitiveGroupsStrip[0].indices, pPrimitiveGroupsStrip[0].numIndices * sizeof(unsigned short));
	outIndexBufferOptStrip->Unlock();

	dwNumFacesStrip = pPrimitiveGroupsStrip[0].numIndices - 2;
}