#include <d3dx9.h>

#ifndef VERTEX_H
#define VERTEX_H

extern IDirect3DDevice9* gd3dDevice;

// Call in constructor and destructor, respectively, of derived application class.
void InitAllVertexDeclarations();
void DestroyAllVertexDeclarations();
//===============================================================
struct VertexRHW
{
	/*VertexPos():pos(0.0f, 0.0f, 0.0f), tex0(0.0f, 0.0f){}
 	VertexPos(float x, float y, float z, float u, float v):pos(x,y,z), tex0(u,v){}
	D3DXVECTOR3 pos;*/

	VertexRHW():pos(0.0f, 0.0f, 0.0f, 0.0f), tex0(0.0f, 0.0f){}
 	VertexRHW(float x, float y, float z, float rhw, float u, float v):pos(x,y,z, rhw), tex0(u,v){}	

	D3DXVECTOR4 pos;
	D3DXVECTOR2 tex0;
	static IDirect3DVertexDeclaration9* Decl;
};
//===============================================================
struct Letter
{
	Letter():pos(0.0f, 0.0f), row(0), col(0){}
	Letter(float x, float y, float r, float c):pos(x,y), row(r), col(c){}
	Letter(const D3DXVECTOR2& v, const float r, const float c):pos(v), row(r), col(c){}

	D3DXVECTOR2 pos;
	float			row;
	float			col;
	static IDirect3DVertexDeclaration9* Decl;
};
//===============================================================
//struct VertexPos
//{
//	VertexPos():pos(0.0f, 0.0f, 0.0f){}
//	VertexPos(float x, float y, float z):pos(x,y,z){}
//	VertexPos(const D3DXVECTOR3& v):pos(v){}
//
//	D3DXVECTOR3 pos;
//	static IDirect3DVertexDeclaration9* Decl;
//};

//===============================================================
struct VertexCol
{
	VertexCol():pos(0.0f, 0.0f, 0.0f),col(0x00000000){}
	VertexCol(float x, float y, float z, D3DCOLOR c):pos(x,y,z), col(c){}
	VertexCol(const D3DXVECTOR3& v, D3DCOLOR c):pos(v),col(c){}

	D3DXVECTOR3 pos;
	DWORD		col; //D3DCOLOR
	static IDirect3DVertexDeclaration9* Decl;
};

//===============================================================
struct VertexPN
{
	VertexPN()
		:pos(0.0f, 0.0f, 0.0f),
		normal(0.0f, 0.0f, 0.0f){}
	VertexPN(float x, float y, float z, 
		float nx, float ny, float nz):pos(x,y,z), normal(nx,ny,nz){}
	VertexPN(const D3DXVECTOR3& v, const D3DXVECTOR3& n)
		:pos(v),normal(n){}

	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	static IDirect3DVertexDeclaration9* Decl;
};

//===============================================================
struct VertexPNT
{
	VertexPNT()
		:pos(0.0f, 0.0f, 0.0f),
		normal(0.0f, 0.0f, 0.0f),
		tex0(0.0f, 0.0f){}
	VertexPNT(float x, float y, float z, 
		float nx, float ny, float nz,
		float u, float v):pos(x,y,z), normal(nx,ny,nz), tex0(u,v){}
	VertexPNT(const D3DXVECTOR3& v, const D3DXVECTOR3& n, const D3DXVECTOR2& uv)
		:pos(v),normal(n), tex0(uv){}

	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 tex0;

	static IDirect3DVertexDeclaration9* Decl;
};
//===============================================================
struct VertexPNTB
{
	VertexPNTB()
		:pos(0.0f, 0.0f, 0.0f),
		normal(0.0f, 0.0f, 0.0f),
		tex0(0.0f, 0.0f),
		boneInd(0),		
		weight(0.0f){}

	VertexPNTB(float x, float y, float z, 
			   float nx, float ny, float nz,
			   float u, float v,
			   DWORD b, float w) :pos(x,y,z), normal(nx,ny,nz), tex0(u,v), 
			   boneInd(b), weight(w) {}

	VertexPNTB(const D3DXVECTOR3& v, const D3DXVECTOR3& n, const D3DXVECTOR2& uv, const DWORD &b, const float &w)
		:pos(v),normal(n), tex0(uv), boneInd(b), weight(w) {}

	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 tex0;
	DWORD		boneInd;
	float		weight;

	static IDirect3DVertexDeclaration9* Decl;
};
//===============================================================
struct GrassVertex
{
	GrassVertex()
		:pos(0.0f, 0.0f, 0.0f),
		tex0(0.0f, 0.0f),
		amplitude(0.0f){}
	GrassVertex(const D3DXVECTOR3& v, 
		const D3DXVECTOR2& uv, float amp)
		:pos(v), tex0(uv), amplitude(amp){}
	D3DXVECTOR3 pos;
	D3DXVECTOR3 quadPos;
	D3DXVECTOR2 tex0;
	float amplitude; // for wind oscillation.
	D3DCOLOR colorOffset;

	static IDirect3DVertexDeclaration9* Decl;
};
//===============================================================
struct NMapVertex
{	
	D3DXVECTOR3 pos;
	D3DXVECTOR2 tex0;	
	D3DXVECTOR3 binormal;
	D3DXVECTOR3 normal;
	D3DXVECTOR3 tangent;	

	static IDirect3DVertexDeclaration9* Decl;
};
//===============================================================
struct Particle
{
	D3DXVECTOR3 initialPos;
	D3DXVECTOR3 initialVelocity;
	float       initialSize; // In pixels.
	float       initialTime;
	float       lifeTime;
	float       mass;
	D3DCOLOR    initialColor;

	static IDirect3DVertexDeclaration9* Decl;
};

#endif // VERTEX_H