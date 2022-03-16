#pragma once

#include <windows.h>

//// {C766451C-9036-465c-B191-A3BA332F3634}
//static const GUID OBJECT_HEADER = 
//{ 0xc766451c, 0x9036, 0x465c, { 0xb1, 0x91, 0xa3, 0xba, 0x33, 0x2f, 0x36, 0x34 } };
//
//// {C5B298E0-4DF5-436b-A387-B33DE7BF690F}
//static const GUID VERTEX_POS = 
//{ 0xc5b298e0, 0x4df5, 0x436b, { 0xa3, 0x87, 0xb3, 0x3d, 0xe7, 0xbf, 0x69, 0xf } };
//
//// {B95197B8-F5D7-40a8-B0CD-CE19C77475B7}
//static const GUID TRI_VERT_IDX = 
//{ 0xb95197b8, 0xf5d7, 0x40a8, { 0xb0, 0xcd, 0xce, 0x19, 0xc7, 0x74, 0x75, 0xb7 } };
//
//// {8F472D3D-9FE5-48dd-9770-74E317DF5FA7}
//static const GUID TRI_NODE_IDX = 
//{ 0x8f472d3d, 0x9fe5, 0x48dd, { 0x97, 0x70, 0x74, 0xe3, 0x17, 0xdf, 0x5f, 0xa7 } };
//
//// {77D0FFF2-FE18-411f-8D97-1E58BD1162CA}
//static const GUID NODE_TRI_MAP = 
//{ 0x77d0fff2, 0xfe18, 0x411f, { 0x8d, 0x97, 0x1e, 0x58, 0xbd, 0x11, 0x62, 0xca } };
//
//// {ABA9F7BA-F965-4133-97C8-03F13A55C8F7}
//static const GUID NODE_NORMAL = 
//{ 0xaba9f7ba, 0xf965, 0x4133, { 0x97, 0xc8, 0x3, 0xf1, 0x3a, 0x55, 0xc8, 0xf7 } };
//
//// {1665A22F-955F-446f-A019-2B872D61EC79}
//static const GUID NODE_TEX_COORD = 
//{ 0x1665a22f, 0x955f, 0x446f, { 0xa0, 0x19, 0x2b, 0x87, 0x2d, 0x61, 0xec, 0x79 } };
//
//// {173B8034-7DD9-4e6d-9021-575641ACDF15}
//static const GUID HEADER_ANIMATIONS = 
//{ 0x173b8034, 0x7dd9, 0x4e6d, { 0x90, 0x21, 0x57, 0x56, 0x41, 0xac, 0xdf, 0x15 } };
//
//// {36520475-7FBD-42f8-849C-9CE7AB6A5F54}
//static const GUID HEADER_TERRASUBSEC = 
//{ 0x36520475, 0x7fbd, 0x42f8, { 0x84, 0x9c, 0x9c, 0xe7, 0xab, 0x6a, 0x5f, 0x54 } };
//
//// {4E57C7EA-30CC-4732-B4A6-0F1DBCF00093}
//static const GUID HEADER_ANIMSET = 
//{ 0x4e57c7ea, 0x30cc, 0x4732, { 0xb4, 0xa6, 0xf, 0x1d, 0xbc, 0xf0, 0x0, 0x93 } };
//
//// кол-во карт - размер - тип - имя - ...
//// {6F1909F2-C6E8-4cfd-962D-5DB1BCDD7D0B}
//static const GUID TEXTURE_MAP = 
//{ 0x6f1909f2, 0xc6e8, 0x4cfd, { 0x96, 0x2d, 0x5d, 0xb1, 0xbc, 0xdd, 0x7d, 0xb } };
//
//// {C1B37684-6524-4db9-84FD-C8128BEB8D88}
//static const GUID HEADER_MATERIAL = 
//{ 0xc1b37684, 0x6524, 0x4db9, { 0x84, 0xfd, 0xc8, 0x12, 0x8b, 0xeb, 0x8d, 0x88 } };
//
//// {8EFFE6E7-C925-4ef3-B470-E2E0A5BB3CFD}
//static const GUID HEADER_BONEHIERARCHY = 
//{ 0x8effe6e7, 0xc925, 0x4ef3, { 0xb4, 0x70, 0xe2, 0xe0, 0xa5, 0xbb, 0x3c, 0xfd } };
//
//// {CDD5FF26-710B-41f4-81CF-000A3258D74F}
//static const GUID HEADER_MAINBOUNDBOX = 
//{ 0xcdd5ff26, 0x710b, 0x41f4, { 0x81, 0xcf, 0x0, 0xa, 0x32, 0x58, 0xd7, 0x4f } };

enum ITEM_TYPE
{	
	VTX_POS,
	VTX_NRM,
	VTX_IDX,
	VTX_TEX,
	TEX_MAP
};

enum TEXMAP_TYPE/* : DWORD*/
{	
	MAP_DIFFUSE,
	MAP_NORMAL
};

struct GFILEITEM		// !!! порядок не менять !!!
{	
	DWORD type;
	DWORD size;		// размер в элементах исходного массива
	DWORD offset;
	DWORD dw;
};

bool FindHeader(HANDLE f, GUID guid, DWORD counter);

template< class T >
// !!! Кроме массивов CDoubleArray !!!
void OutputArray(HANDLE h, T& in_array)
{	
	DWORD dw;	
	LONG nVals = in_array.GetCount();
	for ( LONG i = 0; i < nVals; i++ )
	{
		WriteFile(h, &in_array[i], sizeof(T), &dw, NULL);
	}
}

// !!! Кроме массивов CDoubleArray !!!
template< class T, class Y >
void OutputArrayEx(HANDLE h, ITEM_TYPE type, T& vData, Y& vAlloc, DWORD add)
{	
	DWORD dw;	
	LONG nVals = vData.GetCount();

	vAlloc.Add(type);
	vAlloc.Add(nVals);
	vAlloc.Add(SetFilePointer(h, 0, 0, FILE_CURRENT));
	vAlloc.Add(add);
	
	for ( LONG i = 0; i < nVals; i++ )
	{
		WriteFile(h, &vData[i], sizeof(T)/*4*/, &dw, NULL);
	}
}
//template< class T >
//void OutputValue(HANDLE h, GUID guid, T& value)
//{	
//	DWORD dw;
//	WriteFile(h, &guid, sizeof(GUID), &dw, 0);
//	//WriteFile(h, &nVals, sizeof(T), &dw, 0);
//	WriteFile(h, &value, 4, &dw, NULL);	
//}

template< class T, class Y >
void LoadArray(HANDLE h, GUID guid, T& in_array, Y data)
{
	DWORD dw, count;
	if(FindHeader(h, guid, 1))
	{
		ReadFile( h, &count, sizeof(DWORD), &dw, NULL );
		count /= sizeof(Y) >> 2;
		for(DWORD i = 0; i < count; ++i )
		{
			ReadFile( h, &data, sizeof(Y), &dw, NULL );
			in_array.push_back(data);					
		}
	}
}