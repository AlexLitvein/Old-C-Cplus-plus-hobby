#pragma once

#include "Debug_first.h"
#include "d3dUtil.h"
#include <xact3.h>

#include "Debug_last.h"

class AudioMngr
{
public:
	AudioMngr(void);
	HRESULT PrepareXACT();
	void Play(int indCue);
	void SetTickVolume( float fVolume );
	void SetBoomVolume( float fVolume );
	
	~AudioMngr(void);

	CRITICAL_SECTION cs;
	IXACT3Engine* pEngine;	
    IXACT3WaveBank* pWaveBank;
    IXACT3SoundBank* pSoundBank;
    XACTINDEX iTick;
	XACTINDEX iPrepareBoom;
	XACTINDEX iBoom;

	XACTCATEGORY iTickCategory;
    XACTCATEGORY iBoomCategory;

    VOID* pbWaveBank; // Handle to wave bank data.  Its memory mapped so call UnmapViewOfFile() upon cleanup to release file
    VOID* pbSoundBank; // Pointer to sound bank data.  Call delete on it when the sound bank is destroyed

	
};
