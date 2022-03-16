#include "MYANIMATIONCTRL.h"

//#pragma warning(disable : 4996)



MYANIMATIONCTRL::MYANIMATIONCTRL(void)
{	
}

MYANIMATIONCTRL::MYANIMATIONCTRL(char *fileName)
{
	numBones = 0;	

	//std::vector<VertexPNTB> vVert;
	/*char logFileName[64];
	strcpy(logFileName, fileName);
	strcat(logFileName, ".txt");
	hLog = Open_Log_File(logFileName);	*/

	//LoadVertexAndIndexList(fileName, nameObj, vVert, vFace);
	//nVerts = vVert.size();
	//nFaces = vFace.size();
	//BuildVertexBuffer((void*)&vVert[0], buffV, vVert.size(), sizeof(vVert[0]), D3DUSAGE_WRITEONLY, D3DPOOL_SYSTEMMEM);	
	//BuildVertexBuffer((void*)&vVert[0], buffN, vVert.size(), sizeof(VertexPos), D3DUSAGE_WRITEONLY, D3DPOOL_SYSTEMMEM); // для рисования нормалей
	//BuildIndexBuffer((void*)&vFace[0], buffInd, vFace.size(), 0, D3DPOOL_SYSTEMMEM);
	//
	//CreateOptTriList (buffInd, nFaces, g_pIndexBufferOptList);
	//CreateOptTriStrip(buffInd, nFaces, g_pIndexBufferOptStrip, dwNumFacesStrip);
	//CreateMeshFromVertIndList();
	/*CreateMeshFromVertIndList(D3DXMESH_MANAGED | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE, 
																		vVert, vFace, &optMesh);*/

	CreateMyMesh(fileName, nameObj, D3DXMESH_MANAGED | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE, &optMesh); 
	nVerts = optMesh->GetNumVertices();
	nFaces = optMesh->GetNumFaces();

	LoadMtrl(fileName, mtrl, tex);
	LoadBoundBox(fileName, mainBBox);
	
	LoadBoneHierarchy(fileName);	// 1)
	LoadAnim(fileName);				// 2)	

	BuildMainBBoxVertBuffer();
	BuildBonesBBoxVertBuffer();		// 3)	
	
	LoadShader("vblend2.fx"); 
	LoadShaderBBox("Line.fx");
	LoadShaderBoneBBox("boneBBox.fx");
}

void MYANIMATIONCTRL::DrawObjects()
{
	HR(mFX->SetMatrix(mhVP, &gCamera->viewProj()));	
	HR(mFX->SetTexture(mhTex, tex));

	HR(mFX->SetTechnique(mhTech));
	UINT numPasses = 0;
	HR(mFX->Begin(&numPasses, 0));
	HR(mFX->BeginPass(0));	
	//!!!HR(gd3dDevice->SetStreamSource(0, buffV, 0, sizeof(VertexPNTB)));
	//!!!HR(gd3dDevice->SetIndices(buffInd));					// use D3DPT_TRIANGLELIST and nFaces!!!!
	//HR(gd3dDevice->SetIndices(g_pIndexBufferOptList));		// use D3DPT_TRIANGLELIST and nFaces!!!!
	//HR(gd3dDevice->SetIndices(g_pIndexBufferOptStrip));	// use D3DPT_TRIANGLESTRIP and dwNumFacesStrip!!!
	//!!!HR(gd3dDevice->SetVertexDeclaration(VertexPNTB::Decl));
	DWORD i = vObj.size();
    for( ; i != 0; --i)
    {
		HR(mFX->SetMatrixArray(mhFinalTMs, &vObj[i - 1].vFinalTMs[0], numBones));		
		HR(mFX->SetMatrix(mhPosW, &vObj[i - 1].worldTM));		
		//HR(mFX->CommitChanges());													// D3DPT_TRIANGLESTRIP
		//!!!HR(gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, nVerts, 0, nFaces/* dwNumFacesStrip */));
		optMesh->DrawSubset(0);
    }
	HR(mFX->EndPass());
	HR(mFX->End());	

	//DrawMainBBox();
	//DrawBoneBBox();
}
void MYANIMATIONCTRL::DrawMainBBox()
{
	HR(mBBoxFX->SetTechnique(mhTechBBox));
	UINT numPasses = 0;
	HR(mBBoxFX->Begin(&numPasses, 0));
	HR(mBBoxFX->BeginPass(0));

	HR(mBBoxFX->SetMatrix(mhBBoxWVP, &gCamera->viewProj()));	
	HR(gd3dDevice->SetStreamSource(0, buffMainBBox, 0, sizeof(VertexPos)));
	HR(gd3dDevice->SetVertexDeclaration(VertexPos::Decl));

	DWORD i = vObj.size();
    for( ; i != 0; --i)
    {	
		if(vObj[i - 1].state.test(OBS_SELECTED))
		{
			HR(mBBoxFX->SetMatrix(mhBBoxPosW, &vObj[i - 1].worldTM));		
			HR(mBBoxFX->CommitChanges());													
			HR(gd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, 12));
		}
    }
	HR(mBBoxFX->EndPass());
	HR(mBBoxFX->End());	
}
void MYANIMATIONCTRL::DrawBoneBBox()
{
	HR(mBoneBBoxFX->SetTechnique(mhBoneBBoxTech));
	UINT numPasses = 0;
	HR(mBoneBBoxFX->Begin(&numPasses, 0));
	HR(mBoneBBoxFX->BeginPass(0));

	HR(mBoneBBoxFX->SetMatrix(mhBoneBBoxWVP, &gCamera->viewProj()));	
	HR(gd3dDevice->SetStreamSource(0, bonesBBoxVertBuff, 0, sizeof(VertexCol)));
	HR(gd3dDevice->SetVertexDeclaration(VertexCol::Decl));

	DWORD i = vObj.size();
    for( ; i != 0; --i)
    {
		if(vObj[i - 1].state.test(OBS_SELECTED))
		{
			HR(mBoneBBoxFX->SetMatrixArray(mhBoneBBoxFinalTMs, &vObj[i - 1].vFinalTMs[0], numBones)); 	
			HR(mBoneBBoxFX->SetMatrix(mhBoneBBoxPosW, &vObj[i - 1].worldTM));		
			HR(mBoneBBoxFX->CommitChanges());													
			HR(gd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, numBones * 12));
		}
    }
	HR(mBoneBBoxFX->EndPass());
	HR(mBoneBBoxFX->End());	
}

void MYANIMATIONCTRL::AdvanceTime(float dt)
{	
	float tick = 0.0f;
	float tickAnimCnt = 0.0f;
	float dtTick = dt * 4800.0f;
	DWORD iAnimSet;

	DWORD i = vObj.size();
    for( ; i != 0; --i)
    {
		if(vObj[i - 1].state.test(OBS_ANIMATE))
		{
			tick = vObj[i-1].elapsedTick + dtTick;
			iAnimSet = vObj[i - 1].indxAnimSet;
			tickAnimCnt = vAnimSets[iAnimSet].tickAnimCount;

			if(tick <= tickAnimCnt) vObj[i - 1].elapsedTick = tick;
			else
			{
				tick = 0.0f; //tick - tickAnimCnt;
				vObj[i - 1].elapsedTick = tick; 				
				if( !vObj[i - 1].cyclAnimation)
				{
					vObj[i - 1].elapsedTick = 0.0f;
					vObj[i - 1].state.reset(OBS_ANIMATE);
					break;
				}
			}
			// есть tick
			if(vObj[i - 1].state.test(OBS_VISIBLE))
			{
				UpdateTransformMatrix(iAnimSet, tick);	
				vBoneHierarchy[rootBoneIndx].toRoot = vBoneHierarchy[rootBoneIndx].TransformationMatrix;
				UpdateToRoots(rootBoneIndx);
				UpdateFinalTMs(i-1);
			}			
		}
    }
}

void MYANIMATIONCTRL::AddObject(MY_ANIMMESH &obj)
{	
	D3DXMATRIX m;
	D3DXMatrixIdentity(&m);	

	obj.state.set(OBS_VISIBLE); // потом когда будет "проверка видимости" - убрать!!!

	// если костей нет кладем одну единичную финальную ТМ в obj.vFinalTMs
	if(numBones == 0)
	{
		obj.vFinalTMs.push_back(m);	
		obj.state.reset(OBS_ANIMATE);
	}
	else
	{	
		obj.vFinalTMs.resize(numBones);		
		// Находим имя набора анимации используемого в объекте и заносим его индекс в объект
		DWORD i = 0;
		for( ; i < vAnimSets.size(); ++i)
		{
			if(0 == strcmp(obj.nameAnimSet, vAnimSets[i].name))
			{
				obj.state.set(OBS_ANIMATE);
				obj.indxAnimSet = i;
			}
		}	
	}
	vObj.push_back(obj);
}

bool MYANIMATIONCTRL::LoadShader(char *fileName) // "vblend2.fx"
{	
	/*DWORD dwFlags = 0; 
	#if defined( _DEBUG ) || defined( DEBUG )
	dwFlags |= D3DXSHADER_DEBUG;
	#endif*/

	ID3DXBuffer* errors = 0;
	HR(D3DXCreateEffectFromFile(gd3dDevice, fileName, 0, 0, D3DXSHADER_DEBUG, 0, &mFX, &errors)); //g_pEffectPool
	if( errors ) MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);
	ReleaseCOM(errors);

	mhTech				= mFX->GetTechniqueByName("VBlend2Tech");
	mhVP				= mFX->GetParameterByName(0, "gVP");
	mhFinalTMs			= mFX->GetParameterByName(0, "gFinalTMs");
	mhPosW				= mFX->GetParameterByName(0, "gPosW");
	mhTex				= mFX->GetParameterByName(0, "gTex");
	//mhWorldInvTrans   = mFX->GetParameterByName(0, "gWorldInvTrans");	
	//mhMtrl            = mFX->GetParameterByName(0, "gMtrl");
	//mhLight           = mFX->GetParameterByName(0, "gLight");
	//mhEyePos          = mFX->GetParameterByName(0, "gEyePosW");
	return 1;
	
}
bool MYANIMATIONCTRL::LoadShaderBBox(char *fileName) // "Line.fx"
{	
	ID3DXBuffer* errors = 0;
	HR(D3DXCreateEffectFromFile(gd3dDevice, fileName, 0, 0, D3DXSHADER_DEBUG, 0, &mBBoxFX, &errors));
	if( errors ) MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);
	ReleaseCOM(errors);

	mhTechBBox			= mBBoxFX->GetTechniqueByName("LineTech");
	//mhBBoxWVP			= mBBoxFX->GetParameterByName(0, "gWVP");	
	mhBBoxPosW			= mBBoxFX->GetParameterByName(0, "gPosW");
	
	return 1;
}
bool MYANIMATIONCTRL::LoadShaderBoneBBox(char *fileName) // "boneBBox.fx"
{	
	ID3DXBuffer* errors = 0;
	HR(D3DXCreateEffectFromFile(gd3dDevice, fileName, 0, 0, D3DXSHADER_DEBUG, 0, &mBoneBBoxFX, &errors));
	if( errors ) MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);
	ReleaseCOM(errors);

	mhBoneBBoxTech			= mBoneBBoxFX->GetTechniqueByName("BoneBBoxTech");
	//mhBoneBBoxWVP			= mBoneBBoxFX->GetParameterByName(0, "gBoneBBoxWVP");	
	mhBoneBBoxPosW			= mBoneBBoxFX->GetParameterByName(0, "gBoneBBoxPosW");
	mhBoneBBoxFinalTMs		= mBoneBBoxFX->GetParameterByName(0, "gBoneBBoxFinalTMs");
	
	return 1;
}
void MYANIMATIONCTRL::UpdateFinalTMs(DWORD indObj)
{
	MY_ANIMMESH *objPtr = &vObj[indObj];	
	Write_Log_String(hLog, "\n");

	DWORD i = 0;
    for( ; i < numBones; i++)
    {
		objPtr->vFinalTMs[i] =  vBoneHierarchy[i].boneOffsetTransform * vBoneHierarchy[i].toRoot;
		//                      ^^^^ !!!!!!!vBoneHierarchy[i].boneOffsetTransform *
		//PrintD3DXMATRIXToLogFile(hLog, objPtr->vFinalTMs[i]);
		//PrintD3DXMATRIXToLogFile(hLog, vBoneHierarchy[i].toRoot);		
	}	
}

void MYANIMATIONCTRL::UpdateTransformMatrix(DWORD animSetInd, float tick)
{
	// v1 + s(v2 - v1)	
	float s = 0.0f;
	DWORD indStartMatrixForLerp;	
	MY_ANIMSET *anset = &vAnimSets[animSetInd];

	// Находим индекс метрицы чьё время < tick, и используем его при выполнении интерполяции
	// для других костей т.к. кол-во и время выборок одинаково для всех костей в наборе.

	DWORD nSampl = anset->numSamples;  // это все должно быть заранее подсчитано!!! и глобализовано.
	float dtTickSamples = anset->deltaTickSamples;  	
	indStartMatrixForLerp = tick / dtTickSamples;	
	s = (tick / dtTickSamples) - indStartMatrixForLerp;		
	DWORD i = 0;
    for( ; i < numBones ; ++i)
    {
		DWORD k = i * (nSampl + 1); // не учитанная нулевая выборка( дополняет до 11)
		// вычисляем начальные/конечные матрицы других костей по стартовуму индексу и кол-ву костей						
		MatrixLerp(i, s, anset->vAnimMatrices[(indStartMatrixForLerp) + k], 
					anset->vAnimMatrices[(indStartMatrixForLerp + 1) + k]);
	}
}

void MYANIMATIONCTRL::MatrixLerp(DWORD indBone, float s, const D3DXMATRIX &m1, const D3DXMATRIX &m2)
{
	// v1 + s(v2 - v1)
	vBoneHierarchy[indBone].TransformationMatrix._11 = m1._11 + (s * (m2._11 - m1._11));
	vBoneHierarchy[indBone].TransformationMatrix._12 = m1._12 + (s * (m2._12 - m1._12));
	vBoneHierarchy[indBone].TransformationMatrix._13 = m1._13 + (s * (m2._13 - m1._13));
	vBoneHierarchy[indBone].TransformationMatrix._14 = 0.0f;
	vBoneHierarchy[indBone].TransformationMatrix._21 = m1._21 + (s * (m2._21 - m1._21));
	vBoneHierarchy[indBone].TransformationMatrix._22 = m1._22 + (s * (m2._22 - m1._22));
	vBoneHierarchy[indBone].TransformationMatrix._23 = m1._23 + (s * (m2._23 - m1._23));
	vBoneHierarchy[indBone].TransformationMatrix._24 = 0.0f;
	vBoneHierarchy[indBone].TransformationMatrix._31 = m1._31 + (s * (m2._31 - m1._31));
	vBoneHierarchy[indBone].TransformationMatrix._32 = m1._32 + (s * (m2._32 - m1._32));
	vBoneHierarchy[indBone].TransformationMatrix._33 = m1._33 + (s * (m2._33 - m1._33));
	vBoneHierarchy[indBone].TransformationMatrix._34 = 0.0f;
	vBoneHierarchy[indBone].TransformationMatrix._41 = m1._41 + (s * (m2._41 - m1._41));
	vBoneHierarchy[indBone].TransformationMatrix._42 = m1._42 + (s * (m2._42 - m1._42));
	vBoneHierarchy[indBone].TransformationMatrix._43 = m1._43 + (s * (m2._43 - m1._43));
	vBoneHierarchy[indBone].TransformationMatrix._44 = 1.0f;

	/*D3DXVECTOR3		OutScale;
	D3DXQUATERNION	OutRotation1;
	D3DXQUATERNION	OutRotation2;
	D3DXQUATERNION	outRot;
	D3DXVECTOR3		OutTranslation1;	
	D3DXVECTOR3		OutTranslation2;	
	D3DXVECTOR3		outTrans;
	D3DXMATRIX		out;
	
	D3DXMatrixDecompose(&OutScale, &OutRotation1, &OutTranslation1, &m1);
	D3DXMatrixDecompose(&OutScale, &OutRotation2, &OutTranslation2, &m2);

	D3DXQuaternionNormalize(&OutRotation1, &OutRotation1);
	D3DXQuaternionNormalize(&OutRotation2, &OutRotation2);
	D3DXQuaternionSlerp(&outRot, &OutRotation1, &OutRotation2, s);
	D3DXQuaternionNormalize(&outRot, &outRot);
	
	D3DXVec3Lerp(&outTrans, &OutTranslation1, &OutTranslation2, s);
	D3DXMatrixRotationQuaternion(&out, &outRot);
	outTrans = OutTranslation1 + s * (OutTranslation2 - OutTranslation1);
	out._41 = outTrans.x, out._42 = outTrans.y, out._43 = outTrans.z, out._44 = 1.0f;
	vBoneHierarchy[indBone].TransformationMatrix = out;*/

	//PrintD3DXMATRIXToLogFile(hLog, vBoneHierarchy[indBone].TransformationMatrix); // ok
}

bool MYANIMATIONCTRL::LoadBoneHierarchy(char *fileName)
{
	DWORD dwRead;
	HANDLE hObjFile = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if(FindHeader(hObjFile, HEADER_BONEHIERARCHY, 1))
	{	
		ReadFile( hObjFile, &numBones,     sizeof(DWORD), &dwRead, NULL );		
		ReadFile( hObjFile, &rootBoneIndx, sizeof(DWORD), &dwRead, NULL );
		Write_Log_String(hLog, "костей: %d, индекс корневой: %d\n", numBones, rootBoneIndx);

		MYBONE bone;
		DWORD i = 0;
        for( ; i < numBones; ++i)
        {
			memset(&bone, 0, sizeof(MYBONE));
			ReadFile( hObjFile, &bone, sizeof(MYBONE), &dwRead, NULL );
			//Write_Log_String(hLog, "    %s\n", bone.name);
			vBoneHierarchy.push_back(bone);

			//Write_Log_String(hLog, "boneOffset->\n"); 
			//PrintD3DXMATRIXToLogFile(hLog, bone.boneOffsetTransform);
        }						 
	}	
	//SetFilePointer(hObjFile, 0, 0, FILE_BEGIN);
	CloseHandle(hObjFile);	
	return 1;
}

bool MYANIMATIONCTRL::LoadAnim(char *fileName)
{
	DWORD dwRead, numAnimSets;
	// если костей нет, нет и скелетной анимации
	if(numBones == 0) return 0;
	HANDLE hObjFile = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if(FindHeader(hObjFile, HEADER_ANIMATIONS, 1))
	{
		ReadFile( hObjFile, &tickPerFrame, sizeof(float), &dwRead, NULL );
		//Write_Log_String(hLog, "tickPerFrame = %f\n", tickPerFrame);
		ReadFile( hObjFile, &numAnimSets, sizeof(DWORD), &dwRead, NULL );
		//Write_Log_String(hLog, "numAnimSets = %d\n", numAnimSets);
	}
	//else return 0;
	
	MY_ANIMSET anset;
	DWORD x = 1;
	for( ; x <= numAnimSets; ++x)
	{
		if(FindHeader(hObjFile, HEADER_ANIMSET, x))
		{
			memset(&anset, 0, sizeof(MY_ANIMSET));
			ReadFile( hObjFile, &anset.name, sizeof(anset.name), &dwRead, NULL );			
			ReadFile( hObjFile, &anset.tickAnimCount, sizeof(float), &dwRead, NULL );			
			ReadFile( hObjFile, &anset.numSamples, sizeof(float), &dwRead, NULL );	
			/*Write_Log_String(hLog, "\nanset.name = %s, anset.tickAnimCount = %f, anset.numSamples = %f\n", 
										anset.name, anset.tickAnimCount, anset.numSamples);*/

			anset.deltaTickSamples = anset.tickAnimCount / anset.numSamples;

			D3DXMATRIX mat;			
			float i = 0;
			for( ; i < (anset.numSamples + 1) * numBones; ++i) // +1 это не посчитанный нулевая выборка
			{
				memset(&mat, 0, sizeof(D3DXMATRIX));
				ReadFile( hObjFile, &mat, sizeof(D3DXMATRIX), &dwRead, NULL );
				anset.vAnimMatrices.push_back(mat); 

				//Write_Log_String(hLog, "vAnimMatrices->\n"); 
				//PrintD3DXMATRIXToLogFile(hLog, mat);
			}
			vAnimSets.push_back(anset);
		}		
	}		
	//SetFilePointer(hObjFile, 0, 0, FILE_BEGIN);
	CloseHandle(hObjFile);
//====================================================================================
//	D3DXMATRIX root;
//	D3DXMATRIX rootAnim;
//	D3DXMatrixIdentity(&root);
//	D3DXMatrixIdentity(&rootAnim);
//	D3DXMATRIX out;
//	D3DXMATRIX tmp;
//	D3DXMATRIX tmpAnim;
//
//	//D3DXMATRIX rotZ; // 60
//	//rotZ._11 = 0.5f;      rotZ._12 = 0.866f;  rotZ._13 = 0.0f;   rotZ._14 = 0.0f;
//	//rotZ._21 = -0.866f;   rotZ._22 = 0.5f;    rotZ._23 = 0.0f;   rotZ._24 = 0.0f;
//	//rotZ._31 = 0.0f;      rotZ._32 = 0.0f;    rotZ._33 = 1.0f;   rotZ._34 = 0.0f;
//	//rotZ._41 = 0.0f;      rotZ._42 = 0.0f;    rotZ._43 = 0.0f;   rotZ._44 = 1.0f;
//	//D3DXMATRIX rotY; // 30
//	//rotY._11 = 0.866f;    rotY._12 = 0.0f;    rotY._13 = -0.5f;  rotY._14 = 0.0f;
//	//rotY._21 = 0.0f;      rotY._22 = 1.0f;    rotY._23 = 0.0f;   rotY._24 = 0.0f;
//	//rotY._31 = 0.5f;      rotY._32 = 0.0f;    rotY._33 = 0.866f; rotY._34 = 0.0f;
//	//rotY._41 = 0.0f;      rotY._42 = 0.0f;    rotY._43 = 0.0f;   rotY._44 = 1.0f;
//	//D3DXMATRIX rotX;
//	//rotX._11 = 1.0f;    rotX._12 = 0.0f;    rotX._13 = 0.0f;    rotX._14 = 0.0f;
//	//rotX._21 = 0.0f;    rotX._22 = 0.866f;  rotX._23 = -0.5f;   rotX._24 = 0.0f;
//	//rotX._31 = 0.0f;    rotX._32 = 0.5f;    rotX._33 = 0.866f;  rotX._34 = 0.0f;
//	//rotX._41 = 0.0f;    rotX._42 = 0.0f;    rotX._43 = 0.0f;    rotX._44 = 1.0f;
////---------------------------------------------------
//	D3DXMATRIX boneTM1;
//	boneTM1._11 = 0.0f;   boneTM1._12 = -1.0f;  boneTM1._13 = 0.0f;  boneTM1._14 = 0.0f;
//	boneTM1._21 = 1.0f;   boneTM1._22 = 0.0f;  boneTM1._23 = 0.0f;  boneTM1._24 = 0.0f;
//	boneTM1._31 = 0.0f;   boneTM1._32 = 0.0f;  boneTM1._33 = 1.0f;  boneTM1._34 = 0.0f;
//	boneTM1._41 = 10.0f;  boneTM1._42 = -10.0f; boneTM1._43 = 0.0f;  boneTM1._44 = 1.0f;
//
//	D3DXMATRIX m1 = /*rotX **/ boneTM1;
//	D3DXMATRIX m12;
//	m12._11 = 0.0f;   m12._12 = -0.866025f;  m12._13 = -0.5f;      m12._14 = 0.0f;
//	m12._21 = 1.0f;   m12._22 = 0.0f;        m12._23 = 0.0f;       m12._24 = 0.0f;
//	m12._31 = 0.0f;   m12._32 = -0.5f;       m12._33 = 0.866025f;  m12._34 = 0.0f;
//	m12._41 = 10.0f;  m12._42 = -10.0f;      m12._43 = 0.0f;       m12._44 = 1.0f;
//		
//	vAnimSets[0].vAnimMatrices[0] = m1;
//	vAnimSets[0].vAnimMatrices[1] = m12;
//
//	Write_Log_String(hLog, "b1_1\n");
//	PrintD3DXMATRIXToLogFile(hLog, vAnimSets[0].vAnimMatrices[0]);	
//	Write_Log_String(hLog, "b1_2\n");
//	PrintD3DXMATRIXToLogFile(hLog, vAnimSets[0].vAnimMatrices[1]);
//
//	D3DXMatrixInverse(&vBoneHierarchy[0].boneOffsetTransform, NULL, &boneTM1);	
//	root = m1; //boneTM1
//	//rootAnim = vAnimSets[0].vAnimMatrices[1];
////-----------------------------------------------
//	D3DXMATRIX boneTM2;
//	boneTM2._11 = 1.0f;   boneTM2._12 = 0.0f;   boneTM2._13 = 0.0f;  boneTM2._14 = 0.0f;
//	boneTM2._21 = 0.0f;   boneTM2._22 = 1.0f;   boneTM2._23 = 0.0f;  boneTM2._24 = 0.0f;
//	boneTM2._31 = 0.0f;   boneTM2._32 = 0.0f;   boneTM2._33 = 1.0f;  boneTM2._34 = 0.0f;
//	boneTM2._41 = 10.0f;  boneTM2._42 = -30.0f; boneTM2._43 = 0.0f;  boneTM2._44 = 1.0f;
//	
//	D3DXMATRIX m2 = boneTM2;
//	/*m2._11 = 0.0f;    m2._12 = 1.0f;  m2._13 = 0.0f;   m2._14 = 0.0f;
//	m2._21 = -1.0f;   m2._22 = 0.0f;  m2._23 = 0.0f;   m2._24 = 0.0f;
//	m2._31 = 0.0f;    m2._32 = 0.0f;  m2._33 = 1.0f;   m2._34 = 0.0f;
//	m2._41 = 20.0f;   m2._42 = 0.0f;  m2._43 = 0.0f;   m2._44 = 1.0f;*/
//	D3DXMATRIX m21;
//	m21._11 = 1.0f;   m21._12 = 0.0f;      m21._13 = 0.0f;       m21._14 = 0.0f;
//	m21._21 = 0.0f;   m21._22 = 0.866025f; m21._23 = 0.5f;       m21._24 = 0.0f;
//	m21._31 = 0.0f;   m21._32 = -0.5f;     m21._33 = 0.866025f;  m21._34 = 0.0f;
//	m21._41 = 10.0f;  m21._42 = -30.0f;    m21._43 = 0.0f;       m21._44 = 1.0f;
//
//	vAnimSets[0].vAnimMatrices[2] = m2;	// m2
//	vAnimSets[0].vAnimMatrices[3] = m21;
//
//	Write_Log_String(hLog, "b2_1\n");
//	PrintD3DXMATRIXToLogFile(hLog, vAnimSets[0].vAnimMatrices[2]);	
//	Write_Log_String(hLog, "b2_2\n");
//	PrintD3DXMATRIXToLogFile(hLog, vAnimSets[0].vAnimMatrices[3]);
//	
//	D3DXMatrixInverse(&vBoneHierarchy[1].boneOffsetTransform, NULL, &boneTM2);//	m21
	//root = m2; // m21boneTM2
	//rootAnim = vAnimSets[0].vAnimMatrices[3];
//-------------------------------------------------
	/*D3DXMATRIX boneTM3;
	boneTM3._11 = 1.0f;   boneTM3._12 = 0.0f;  boneTM3._13 = 0.0f;  boneTM3._14 = 0.0f;
	boneTM3._21 = 0.0f;   boneTM3._22 = 1.0f;  boneTM3._23 = 0.0f;  boneTM3._24 = 0.0f;
	boneTM3._31 = 0.0f;   boneTM3._32 = 0.0f;  boneTM3._33 = 1.0f;  boneTM3._34 = 0.0f;
	boneTM3._41 = 50.0f;  boneTM3._42 = -20.0f; boneTM3._43 = 0.0f;  boneTM3._44 = 1.0f;

	D3DXMATRIX m3 = rotY * boneTM3;		
	D3DXMatrixInverse(&tmp, NULL, &root);
	D3DXMatrixInverse(&tmpAnim, NULL, &rootAnim);
	vAnimSets[0].vAnimMatrices[4] = boneTM3 * tmp;	
	vAnimSets[0].vAnimMatrices[5] = m3 * tmpAnim; 

	Write_Log_String(hLog, "b3_1\n");
	PrintD3DXMATRIXToLogFile(hLog, vAnimSets[0].vAnimMatrices[4]);	
	Write_Log_String(hLog, "b3_2\n");
	PrintD3DXMATRIXToLogFile(hLog, vAnimSets[0].vAnimMatrices[5]);	

	D3DXMatrixInverse(&vBoneHierarchy[2].boneOffsetTransform, NULL, &boneTM3);	
	root = boneTM3;
	rootAnim = vAnimSets[0].vAnimMatrices[5];*/
//--------------------------------------------------
	/*Write_Log_String(hLog, "boneOffset 0\n");
	PrintD3DXMATRIXToLogFile(hLog, vBoneHierarchy[0].boneOffsetTransform);
	Write_Log_String(hLog, "boneOffset 1\n");
	PrintD3DXMATRIXToLogFile(hLog, vBoneHierarchy[1].boneOffsetTransform);*/
	/*Write_Log_String(hLog, "boneOffset 2\n");
	PrintD3DXMATRIXToLogFile(hLog, vBoneHierarchy[2].boneOffsetTransform);*/
//====================================================================================
//	D3DXMATRIX root;
//	D3DXMATRIX rootAnim;
//	D3DXMatrixIdentity(&root);
//	D3DXMatrixIdentity(&rootAnim);
//	D3DXMATRIX out;
//	D3DXMATRIX tmp;
//	D3DXMATRIX tmpAnim;
//
//	D3DXMATRIX rotZ; // 60
//	rotZ._11 = 0.5f;      rotZ._12 = 0.866f;  rotZ._13 = 0.0f;   rotZ._14 = 0.0f;
//	rotZ._21 = -0.866f;   rotZ._22 = 0.5f;    rotZ._23 = 0.0f;   rotZ._24 = 0.0f;
//	rotZ._31 = 0.0f;      rotZ._32 = 0.0f;    rotZ._33 = 1.0f;   rotZ._34 = 0.0f;
//	rotZ._41 = 0.0f;      rotZ._42 = 0.0f;    rotZ._43 = 0.0f;   rotZ._44 = 1.0f;
//	D3DXMATRIX rotY; // 30
//	rotY._11 = 0.866f;    rotY._12 = 0.0f;    rotY._13 = -0.5f;  rotY._14 = 0.0f;
//	rotY._21 = 0.0f;      rotY._22 = 1.0f;    rotY._23 = 0.0f;   rotY._24 = 0.0f;
//	rotY._31 = 0.5f;      rotY._32 = 0.0f;    rotY._33 = 0.866f; rotY._34 = 0.0f;
//	rotY._41 = 0.0f;      rotY._42 = 0.0f;    rotY._43 = 0.0f;   rotY._44 = 1.0f;
//	D3DXMATRIX rotX;
//	rotX._11 = 1.0f;    rotX._12 = 0.0f;    rotX._13 = 0.0f;    rotX._14 = 0.0f;
//	rotX._21 = 0.0f;    rotX._22 = 0.866f;  rotX._23 = -0.5f;   rotX._24 = 0.0f;
//	rotX._31 = 0.0f;    rotX._32 = 0.5f;    rotX._33 = 0.866f;  rotX._34 = 0.0f;
//	rotX._41 = 0.0f;    rotX._42 = 0.0f;    rotX._43 = 0.0f;    rotX._44 = 1.0f;
////---------------------------------------------------
//	D3DXMATRIX boneTM1;
//	boneTM1._11 = 0.0f;   boneTM1._12 = -1.0f;  boneTM1._13 = 0.0f;  boneTM1._14 = 0.0f;
//	boneTM1._21 = 1.0f;   boneTM1._22 = 0.0f;  boneTM1._23 = 0.0f;  boneTM1._24 = 0.0f;
//	boneTM1._31 = 0.0f;   boneTM1._32 = 0.0f;  boneTM1._33 = 1.0f;  boneTM1._34 = 0.0f;
//	boneTM1._41 = 20.0f;  boneTM1._42 = -10.0f; boneTM1._43 = 0.0f;  boneTM1._44 = 1.0f;
//
//	D3DXMATRIX m1 = /*rotX **/ boneTM1;
//		
//	D3DXMatrixInverse(&tmp, NULL, &root);
//	D3DXMatrixInverse(&tmpAnim, NULL, &rootAnim);
//	
//	vAnimSets[0].vAnimMatrices[0] = boneTM1 * tmp;
//	vAnimSets[0].vAnimMatrices[1] = m1 * tmpAnim;
//
//	Write_Log_String(hLog, "b1_1\n");
//	PrintD3DXMATRIXToLogFile(hLog, vAnimSets[0].vAnimMatrices[0]);	
//	Write_Log_String(hLog, "b1_2\n");
//	PrintD3DXMATRIXToLogFile(hLog, vAnimSets[0].vAnimMatrices[1]);
//
//	D3DXMatrixInverse(&vBoneHierarchy[0].boneOffsetTransform, NULL, &boneTM1);	
//	root = boneTM1;
//	rootAnim = vAnimSets[0].vAnimMatrices[1];
////-----------------------------------------------
//	D3DXMATRIX boneTM2;
//	boneTM2._11 = 1.0f;   boneTM2._12 = 0.0f;   boneTM2._13 = 0.0f;  boneTM2._14 = 0.0f;
//	boneTM2._21 = 0.0f;   boneTM2._22 = 1.0f;   boneTM2._23 = 0.0f;  boneTM2._24 = 0.0f;
//	boneTM2._31 = 0.0f;   boneTM2._32 = 0.0f;   boneTM2._33 = 1.0f;  boneTM2._34 = 0.0f;
//	boneTM2._41 = 20.0f;  boneTM2._42 = -20.0f; boneTM2._43 = 0.0f;  boneTM2._44 = 1.0f;
//	
//	D3DXMATRIX m21;
//	m21._11 = 0.866f;  m21._12 = 0.5;     m21._13 = 0.0f;   m21._14 = 0.0f;
//	m21._21 = -0.5f;   m21._22 = 0.866f;  m21._23 = 0.0f;   m21._24 = 0.0f;
//	m21._31 = 0.0f;    m21._32 = 0.0f;    m21._33 = 1.0f;   m21._34 = 0.0f;
//	m21._41 = 20.0f;   m21._42 = -20.0f;  m21._43 = 0.0f;   m21._44 = 1.0f;
//
//	D3DXMATRIX m2 = rotZ * boneTM2; //m21
//
//	D3DXMatrixInverse(&tmp, NULL, &root);
//	D3DXMatrixInverse(&tmpAnim, NULL, &rootAnim);
//	vAnimSets[0].vAnimMatrices[2] = boneTM2 * tmp;	// m2
//	vAnimSets[0].vAnimMatrices[3] = m2 * tmpAnim;
//
//	Write_Log_String(hLog, "b2_1\n");
//	PrintD3DXMATRIXToLogFile(hLog, vAnimSets[0].vAnimMatrices[2]);	
//	Write_Log_String(hLog, "b2_2\n");
//	PrintD3DXMATRIXToLogFile(hLog, vAnimSets[0].vAnimMatrices[3]);
//	
//	D3DXMatrixInverse(&vBoneHierarchy[1].boneOffsetTransform, NULL, &boneTM2);//	m21
//	root = boneTM2; // m21
//	rootAnim = vAnimSets[0].vAnimMatrices[3];
////-------------------------------------------------
//	D3DXMATRIX boneTM3;
//	boneTM3._11 = 1.0f;   boneTM3._12 = 0.0f;  boneTM3._13 = 0.0f;  boneTM3._14 = 0.0f;
//	boneTM3._21 = 0.0f;   boneTM3._22 = 1.0f;  boneTM3._23 = 0.0f;  boneTM3._24 = 0.0f;
//	boneTM3._31 = 0.0f;   boneTM3._32 = 0.0f;  boneTM3._33 = 1.0f;  boneTM3._34 = 0.0f;
//	boneTM3._41 = 50.0f;  boneTM3._42 = -20.0f; boneTM3._43 = 0.0f;  boneTM3._44 = 1.0f;
//
//	D3DXMATRIX m3 = rotY * boneTM3;
//		
//	D3DXMatrixInverse(&tmp, NULL, &root);
//	D3DXMatrixInverse(&tmpAnim, NULL, &rootAnim);
//	vAnimSets[0].vAnimMatrices[4] = boneTM3 * tmp;	
//	vAnimSets[0].vAnimMatrices[5] = m3 * tmpAnim; 
//
//	Write_Log_String(hLog, "b3_1\n");
//	PrintD3DXMATRIXToLogFile(hLog, vAnimSets[0].vAnimMatrices[4]);	
//	Write_Log_String(hLog, "b3_2\n");
//	PrintD3DXMATRIXToLogFile(hLog, vAnimSets[0].vAnimMatrices[5]);	
//
//	D3DXMatrixInverse(&vBoneHierarchy[2].boneOffsetTransform, NULL, &boneTM3);	
//	root = boneTM3;
//	rootAnim = vAnimSets[0].vAnimMatrices[5];
////--------------------------------------------------
//	Write_Log_String(hLog, "boneOffset 0\n");
//	PrintD3DXMATRIXToLogFile(hLog, vBoneHierarchy[0].boneOffsetTransform);
//	Write_Log_String(hLog, "boneOffset 1\n");
//	PrintD3DXMATRIXToLogFile(hLog, vBoneHierarchy[1].boneOffsetTransform);
//	Write_Log_String(hLog, "boneOffset 2\n");
//	PrintD3DXMATRIXToLogFile(hLog, vBoneHierarchy[2].boneOffsetTransform);
//====================================================================================
	return 1;
}

MYANIMATIONCTRL::~MYANIMATIONCTRL(void)
{		
	ReleaseCOM(optMesh);
	ReleaseCOM(tex);
	ReleaseCOM(mFX);
	ReleaseCOM(mBBoxFX);
	ReleaseCOM(mBoneBBoxFX);	
	ReleaseCOM(buffMainBBox);
	ReleaseCOM(bonesBBoxVertBuff);	
}

void MYANIMATIONCTRL::UpdateToRoots(DWORD boneIndx)
{
	DWORD nChild = vBoneHierarchy[boneIndx].numChild;
	if(nChild !=0)
	{
		MYBONE *parentBone = &vBoneHierarchy[boneIndx];
		DWORD i = 0;
		for( ; i < nChild; ++i)
		{			
			vBoneHierarchy[parentBone->arrayChildInd[i]].toRoot = vBoneHierarchy[parentBone->arrayChildInd[i]].TransformationMatrix;
			//^^^^^^^^^^!!!! * parentBone->toRoot;
			
			/*Write_Log_String(hLog, "компоненты для toRoot матрица\n");
			PrintD3DXMATRIXToLogFile(hLog, vBoneHierarchy[parentBone->arrayChildInd[i]].TransformationMatrix);
			PrintD3DXMATRIXToLogFile(hLog, parentBone->toRoot);*/			
			UpdateToRoots(parentBone->arrayChildInd[i]);
		}
	}
}
//void MYANIMATIONCTRL::CreateOptTriList()
//{
//	//primitive groups from NvTriStrip
//	PrimitiveGroup *g_pPrimitiveGroupsList;
//	PrimitiveGroup *g_pPrimitiveGroupsStrip;
//	// NVTriStrip setup code
//	SetCacheSize(CACHESIZE_GEFORCE3);
//	SetStitchStrips(true);
//	SetMinStripSize(0);
//
//	//First, we create the optimized list indices
//	SetListsOnly(true);
//
//	unsigned short* pIndices = NULL;
//	HR(buffInd->Lock(0, 0, (void**)&pIndices, 0)); //D3DLOCK_DISCARD D3DLOCK_READONLY			
//
//	unsigned short numSections;	
//	GenerateStrips(pIndices, nFaces * 3, &g_pPrimitiveGroupsList, &numSections);
//	buffInd->Unlock();	
//
//	//copy optimized data to index buffer
//	gd3dDevice->CreateIndexBuffer(g_pPrimitiveGroupsList[0].numIndices * sizeof(unsigned short), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
//		D3DPOOL_DEFAULT, &g_pIndexBufferOptList, NULL);
//
//	unsigned short* pDest;
//	g_pIndexBufferOptList->Lock(0, g_pPrimitiveGroupsList[0].numIndices * sizeof(unsigned short), (void**)&pDest, 0);
//	memcpy(pDest, g_pPrimitiveGroupsList[0].indices, g_pPrimitiveGroupsList[0].numIndices * sizeof(unsigned short));
//	g_pIndexBufferOptList->Unlock();
////---------------------------------------------------------------------------
//	//Next, we create the optimized strip indices
//	SetListsOnly(false);
//
//	pIndices = NULL;
//	HR(buffInd->Lock(0, 0, (void**)&pIndices, 0));
//
//	GenerateStrips(pIndices, nFaces * 3, &g_pPrimitiveGroupsStrip, &numSections);
//	buffInd->Unlock();
//
//	//copy optimized data to index buffer
//	gd3dDevice->CreateIndexBuffer(g_pPrimitiveGroupsStrip[0].numIndices * sizeof(unsigned short), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
//		D3DPOOL_DEFAULT, &g_pIndexBufferOptStrip, NULL);
//	g_pIndexBufferOptStrip->Lock(0, g_pPrimitiveGroupsStrip[0].numIndices * sizeof(unsigned short), (void**)&pDest, 0);
//	memcpy(pDest, g_pPrimitiveGroupsStrip[0].indices, g_pPrimitiveGroupsStrip[0].numIndices * sizeof(unsigned short));
//	g_pIndexBufferOptStrip->Unlock();
//
//	dwNumFacesStrip = g_pPrimitiveGroupsStrip[0].numIndices - 2;
//}
DWORD MYANIMATIONCTRL::GetTotalVert() const
{
	return nVerts * vObj.size();
}

DWORD MYANIMATIONCTRL::GetTotalFace() const
{
	return nFaces * vObj.size(); //dwNumFacesStrip 
}


void MYANIMATIONCTRL::BuildMainBBoxVertBuffer()
{
	HR(gd3dDevice->CreateVertexBuffer( 24 * sizeof (VertexPos),D3DUSAGE_WRITEONLY, 
										0,D3DPOOL_SYSTEMMEM , &buffMainBBox, 0));

	FillBoundBoxVertBuffer(buffMainBBox, mainBBox);
}

void MYANIMATIONCTRL::BuildBonesBBoxVertBuffer()
{
	HR(gd3dDevice->CreateVertexBuffer( 24 * vBoneHierarchy.size() * sizeof (VertexCol), D3DUSAGE_WRITEONLY, 
										0, D3DPOOL_SYSTEMMEM, &bonesBBoxVertBuff, 0));

	VertexCol *v = 0;		
	HR(bonesBBoxVertBuff->Lock(0, 0,  (void**)&v, 0));
	DWORD i = 0;
    for( ; i < vBoneHierarchy.size(); ++i)
    {
		AABB bb = vBoneHierarchy[i].boundBox;
		VertexCol v1;
		v1.pos = bb.minPt;											v1.col = i;
		VertexCol v2;
		v2.pos = D3DXVECTOR3(bb.minPt.x, bb.maxPt.y, bb.minPt.z);	v2.col = i;
		VertexCol v3;
		v3.pos = D3DXVECTOR3(bb.maxPt.x, bb.maxPt.y, bb.minPt.z);	v3.col = i;		
		VertexCol v4;
		v4.pos = D3DXVECTOR3(bb.maxPt.x, bb.minPt.y, bb.minPt.z);	v4.col = i;		
		VertexCol v5;
		v5.pos = D3DXVECTOR3(bb.minPt.x, bb.minPt.y, bb.maxPt.z);	v5.col = i;	
		VertexCol v6;
		v6.pos = D3DXVECTOR3(bb.minPt.x, bb.maxPt.y, bb.maxPt.z);	v6.col = i;	
		VertexCol v7;
		v7.pos = bb.maxPt;											v7.col = i;										
		VertexCol v8;
		v8.pos = D3DXVECTOR3(bb.maxPt.x, bb.minPt.y, bb.maxPt.z);	v8.col = i;			

		v[(24*i)+0] = v1;
		v[(24*i)+1] = v2;
		v[(24*i)+2] = v2;
		v[(24*i)+3] = v3;
		v[(24*i)+4] = v3;
		v[(24*i)+5] = v4;
		v[(24*i)+6] = v4;
		v[(24*i)+7] = v1;

		v[(24*i)+8] = v5;
		v[(24*i)+9] = v6;
		v[(24*i)+10] = v6;
		v[(24*i)+11] = v7;
		v[(24*i)+12] = v7;
		v[(24*i)+13] = v8;
		v[(24*i)+14] = v8;
		v[(24*i)+15] = v5;

		v[(24*i)+16] = v1;
		v[(24*i)+17] = v5;
		v[(24*i)+18] = v2;
		v[(24*i)+19] = v6;
		v[(24*i)+20] = v3;
		v[(24*i)+21] = v7;
		v[(24*i)+22] = v4;
		v[(24*i)+23] = v8;	
		
	}
	HR(bonesBBoxVertBuff->Unlock());
}