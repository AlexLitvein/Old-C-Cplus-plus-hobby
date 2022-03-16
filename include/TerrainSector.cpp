#include "TerrainSector.h"
#include "DirectInput.h"

TerrainSector::TerrainSector(void)
{
}
TerrainSector::TerrainSector(char *file)
{
	HANDLE hIn, hMap;
	char *pData;
	DWORD end, dwRead;
	DWORD nVerts = 0;
	DWORD nFaces = 0;	
	AABB  tmpBB;	
	std::string path = "Data\\";
	std::string objName; 
	std::string texName;
	std::vector<VertexPNT>	vVerts;
	std::vector<MYFACE>		vFaces;
	VertexPNT tmp;
	TerrainSubSector *subSec = 0;
	DWORD t = timeGetTime();	

	//vSubSec.resize(0);

	hIn = CreateFile( file, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );		
	hMap = CreateFileMapping(hIn, NULL, PAGE_READONLY, 0,0, NULL);
	pData = (LPSTR)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
	DWORD size = GetFileSize(hIn, NULL);
	end = (DWORD)pData + size;

	DWORD cnt = 0;	
	while(FindSubSectHeader(&pData, HEADER_TERRASUBSEC, end)) //  FindHeader(pData, HEADER_TERRASUBSEC, cnt)
	{
		vVerts.clear();
		vFaces.clear();		
		objName = path + GetStringFromMapFile(&pData);
		pData = ReadMapFile( pData, &tmpBB, sizeof(AABB), &dwRead, NULL );

		subSec = new TerrainSubSector(tmpBB);
		// вычисляем minPt и maxPt всего TerrainSector
		D3DXVec3Minimize(&bbTerrSect.minPt, &bbTerrSect.minPt, &tmpBB.minPt);
		D3DXVec3Maximize(&bbTerrSect.maxPt, &bbTerrSect.maxPt, &tmpBB.maxPt);
		
		// инициализируем массив вершин
		pData = ReadMapFile( pData, &nVerts, sizeof(DWORD), &dwRead, NULL );
		//subSec->nVerts = nVerts;	
			
		DWORD i = 0;
		for( ; i < nVerts; ++i)
		{	 
			//memset(&tmp, 0, sizeof(VertexPNT));
			pData = ReadMapFile( pData, &tmp, sizeof(VertexPNT), &dwRead, NULL );			
			vVerts.push_back(tmp);
		}		
		// инициализируем массив индексов				
		pData = ReadMapFile( pData, &nFaces, sizeof(DWORD), &dwRead, NULL );	
		//subSec->nFaces = nFaces;

		MYFACE f;
		DWORD y = 0;
		for(; y < nFaces ; ++y)
		{	
			//memset(&f, 0, sizeof(MYFACE));
			pData = ReadMapFile( pData, &f, sizeof(MYFACE), &dwRead, NULL );			
			vFaces.push_back(f);
		}
		Mtrl m;	
		DWORD nTex;
		pData = ReadMapFile( pData, &m, sizeof(Mtrl), &dwRead, NULL );
		subSec->mtrl = m;
		pData = ReadMapFile( pData, &nTex, sizeof(DWORD), &dwRead, NULL );
		
		if(nTex != 0) 
		{
			texName = GetStringFromMapFile(&pData);
			subSec->dTexInd = gTexMngr->InsertItem(texName, true);
		}
		else subSec->dTexInd = 0;
		
		/*BuildVertexBuffer(&subSec->vVerts[0], *(&subSec->buffV), subSec->nVerts, 
							sizeof(subSec->vVerts[0]), D3DUSAGE_WRITEONLY, D3DPOOL_SYSTEMMEM);
		BuildIndexBuffer(&subSec->vFaces[0], *(&subSec->buffInd), subSec->nFaces, 
							D3DUSAGE_WRITEONLY, D3DPOOL_SYSTEMMEM);*/
		CreateMeshFromVertIndList(D3DXMESH_MANAGED | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE,
								vVerts, vFaces, &subSec->optMesh);		

		/*DWORD* adjacencyInfo = new DWORD[subSec->optMesh->GetNumFaces() * 3];
		subSec->optMesh->GenerateAdjacency(0.0f, adjacencyInfo);
		D3DXGeneratePMesh(subSec->optMesh, adjacencyInfo, NULL, 0, 100, D3DXMESHSIMP_VERTEX, &subSec->pPMesh);
		delete [] adjacencyInfo;*/
		objName += ".bmp";
		MyLoadHeightMap(objName.c_str(), subSec->heightMap);
		objName.clear();

		vSubSec.push_back(subSec);
		cnt++;
		
		Write_Log_String(hLog,"cnt:%d %d\n", cnt, timeGetTime() - t);
		t = timeGetTime();
		subSec = 0;
	}	
	UnmapViewOfFile(pData);
	CloseHandle(hMap);
	CloseHandle(hIn);

	gCollContainer->CreateCB(bbTerrSect, 16, 1, 16, 512, indCBInCollContainer);
	
	RegisterSubSectors();
	gCollContainer->Start();	// ?????
}

//void TerrainSector::Release()
//{
//	int size = vSubSec.size();
//	for(int i=0; i < size; ++i) delete vSubSec[i];
//}

void TerrainSector::Update(float dt)
{	
	visSSecs = 0;
	TerrainSubSector *pSSec = 0;
	AABB box;
	MY_MSG msg;
	//bool mouseDown = false;
	//D3DXVECTOR3 camPos = gCamera->pos();
	//D3DXVECTOR3 centSSec;

	/*D3DXVECTOR3 originW(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 dirW(0.0f, 0.0f, 0.0f);
	if(gDInput->mouseButtonDown(0)) 
	{
		getWorldPickingRay(originW, dirW);
		mouseDown = true;
	}*/

	DWORD i = vSubSec.size();
    for( ; i != 0; --i)
    {
		pSSec = vSubSec[i-1];
		box = pSSec->m_bb;
		bool vis = gCamera->isVisible(box);
		pSSec->visible = vis;
		if(vis) visSSecs++;

		//msg.type = MAKEDWORD(MSG_TERRAIN, TER_SSVIS);
		//msg.pObj = (DWORD *)pSSec;
		//msg.param1 = vis;
		//msg.pt = pSSec->m_bb.center();

		//gQueueMngr->SendMsgIN(msg); // !!! уменьшает производительность

		//centSSec = pSSec->bb.center();
		//pSSec->distToCam = Fast_Distance_3D(centSSec.x + (-camPos.x), centSSec.y + (-camPos.y), centSSec.z + (-camPos.z));	
		//pSSec->setNumV = 512 - pSSec->distToCam;	

		//if(mouseDown)
		//{
		//	if(D3DXBoxBoundProbe(&box.minPt, &box.maxPt, &originW, &dirW))
		//	{
		//		pSSec->hit = true;
		//		//pSSec->GetHeight()
		//	}
		//	else pSSec->hit = false;
		//}
		//else pSSec->hit = false;
		pSSec = 0;
	}
}

void TerrainSector::RegisterSubSectors()
{
	MY_MSG msg;
	msg.type = MAKEDWORD(MSG_TERRAIN, 0);
	
	DWORD i = vSubSec.size();	
	for( ; i != 0; --i)
	{
		// если MSG_TERRAIN, то msg.pObj указатель на элемент в vSubSec[] 
		msg.pObj = (DWORD *)vSubSec[i-1];	
		msg.pt  = vSubSec[i-1]->m_bb.center();
		gQueueMngr->SendMsgIN(msg);
	}		
}

void TerrainSector::Draw()
{
	DWORD i = vSubSec.size();
    for( ; i != 0; --i)
    {
		TerrainSubSector *sub = vSubSec[i-1];		
		if(sub->visible)
		{
			//!!!!HR(mFX1->SetTexture(mhTex1, gTexMngr->GetTexturePtr(sub->dTexInd)));
			//mFX1->CommitChanges();
			//sub->pPMesh->SetNumVertices(sub->setNumV);
			//sub->pPMesh->DrawSubset(0);
			
			sub->optMesh->DrawSubset(0);
		}		
	}	
}
void TerrainSector::LoadShader()
{
	//ID3DXBuffer* errors = 0;
	//HR(D3DXCreateEffectFromFile(gd3dDevice, "Terrain.fx", 0, 0, D3DXSHADER_DEBUG, 0, &mFX, &errors));
	////if( errors ) MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);

	//mhTech      = mFX->GetTechniqueByName("TerrainTech");	
	//mhTex		= mFX->GetParameterByName(0, "gTex");	
}
TerrainSector::~TerrainSector(void)
{
	//mFX1 = 0;
	int size = vSubSec.size();
	for(int i=0; i < size; ++i)
	{
		vSubSec[i]->optMesh->Release();
		delete vSubSec[i];
	}
	//delete txt;
	//Release();
}
