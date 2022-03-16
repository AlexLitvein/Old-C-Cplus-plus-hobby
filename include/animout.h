#pragma once

#include "windows.h"
//#include "afxwin.h"
#include <vector>
#include "Max.h"
#include "decomp.h"
#include "units.h"
#include "asciitok.h"
#include "MyHelper.h"
#include "loadexpgeom.h"

extern Interface			*giPtr;
extern HANDLE				ghFile;
extern HANDLE				ghLog;
extern bool					useSample;
extern float				gKeyFrameStep;

extern void SetDXMatrix(D3DXMATRIX &m1, Matrix3 &m2);
extern void SetDXMatrixForAnim(D3DXMATRIX &m1, Matrix3 &m2);
extern void SwapAxis(D3DXMATRIX &out, D3DXMATRIX &in);

#define ALMOST_ZERO 1.0e-3f

void ExportAnimKeys(int stTick, int endTick, INode* node);
BOOL EqualPoint3(Point3 p1, Point3 p2);
BOOL IsKnownController(Control* cont);
BOOL CheckForAnimation(INode* node, BOOL& bPos, BOOL& bRot, BOOL& bScale);
void DumpPosSample(INode* node); 
void DumpRotSample(INode* node); 
void DumpScaleSample(INode* node); 
void DumpPoint3Keys(Control* cont);
void DumpFloatKeys(Control* cont);
void DumpPosKeys(Control* cont);
void DumpRotKeys(Control* cont);
void DumpScaleKeys(Control* cont);

void PrintMatrixToLogFile(HANDLE file, Matrix3 &m);