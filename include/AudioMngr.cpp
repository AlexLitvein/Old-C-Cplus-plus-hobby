#include "AudioMngr.h"


AudioMngr *g_AudioMngr = 0;

AudioMngr::AudioMngr(void)
{
}
void AudioMngr::SetTickVolume( float fVolume )
{
	iTickCategory = pEngine->GetCategory( "Tick" );
    HR(pEngine->SetVolume( iTickCategory, fVolume ));    
}
void AudioMngr::SetBoomVolume( float fVolume )
{
    iBoomCategory = pEngine->GetCategory( "Boom" );	
    HR(pEngine->SetVolume( iBoomCategory, fVolume )); 
	
}
void AudioMngr::Play(int indCue)
{
	pSoundBank->Play( indCue, 0, 0, NULL );
}
AudioMngr::~AudioMngr(void)
{
	// Shutdown XACT
    //
    // Note that pEngine->ShutDown is synchronous and will take some time to complete 
    // if there are still playing cues.  Also pEngine->ShutDown() is generally only 
    // called when a game exits and is not the preferred method of changing audio 
    // resources. To know when it is safe to free wave/sound bank data without 
    // shutting down the XACT engine, use the XACTNOTIFICATIONTYPE_SOUNDBANKDESTROYED 
    // or XACTNOTIFICATIONTYPE_WAVEBANKDESTROYED notifications 
    if( pEngine )
    {
        pEngine->ShutDown();
        pEngine->Release();
    }

    if( pbSoundBank ) delete[] pbSoundBank;
    pbSoundBank = NULL;

    // After pEngine->ShutDown() returns it is safe to release memory mapped files
    if( pbWaveBank ) UnmapViewOfFile( pbWaveBank );
    pbWaveBank = NULL;

    CoUninitialize();
}

HRESULT AudioMngr::PrepareXACT()
{
    HRESULT hr;
    //WCHAR str[MAX_PATH];
    HANDLE hFile;
    DWORD dwFileSize;
    DWORD dwBytesRead;
    HANDLE hMapFile;    

    hr = CoInitializeEx( NULL, COINIT_MULTITHREADED );  // COINIT_APARTMENTTHREADED will work too
    if( SUCCEEDED( hr ) )
    {
        DWORD dwCreationFlags = 0;
        dwCreationFlags |= XACT_FLAG_API_AUDITION_MODE;
        //dwCreationFlags |= XACT_FLAG_API_DEBUG_MODE;

        hr = XACT3CreateEngine( dwCreationFlags, &pEngine );
    }
    if( FAILED( hr ) || pEngine == NULL ) return E_FAIL;

	//------------------------------------------------------------------------------------
	// Load the global settings file and pass it into XACTInitialize
    VOID* pGlobalSettingsData = NULL;
    DWORD dwGlobalSettingsFileSize = 0;
    bool bSuccess = false;
   
    hFile = CreateFile( "Clock.xgs", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
    if( hFile )
    {
        dwGlobalSettingsFileSize = GetFileSize( hFile, NULL );
        if( dwGlobalSettingsFileSize != INVALID_FILE_SIZE )
        {
            // Using CoTaskMemAlloc so that XACT can clean up this data when its done
            pGlobalSettingsData = CoTaskMemAlloc( dwGlobalSettingsFileSize );
            if( pGlobalSettingsData )
            {
                if( 0 != ReadFile( hFile, pGlobalSettingsData, dwGlobalSettingsFileSize, &dwBytesRead, NULL ) )
                {
                    bSuccess = true;
                }
            }
        }
        CloseHandle( hFile );
    }
    //--------------------------------------------------------------------------------------

    // Initialize & create the XACT runtime 
    XACT_RUNTIME_PARAMETERS xrParams = {0};
	xrParams.pGlobalSettingsBuffer = pGlobalSettingsData;
    xrParams.globalSettingsBufferSize = dwGlobalSettingsFileSize;
    xrParams.globalSettingsFlags = XACT_FLAG_GLOBAL_SETTINGS_MANAGEDATA; // this will tell XACT to delete[] the buffer when its unneeded
    xrParams.lookAheadTime = 250;
    hr = pEngine->Initialize( &xrParams );
    if( FAILED( hr ) ) return hr; 	

    // Create an "in memory" XACT wave bank file using memory mapped file IO
    // Memory mapped files tend to be the fastest for most situations assuming you 
    // have enough virtual address space for a full map of the file
    hr = E_FAIL; // assume failure
    hFile = CreateFile( "Wave Bank.xwb", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );

    if( hFile != INVALID_HANDLE_VALUE )
    {
        dwFileSize = GetFileSize( hFile, NULL );
        if( dwFileSize != -1 )
        {
            hMapFile = CreateFileMapping( hFile, NULL, PAGE_READONLY, 0, dwFileSize, NULL );
            if( hMapFile )
            {
                pbWaveBank = MapViewOfFile( hMapFile, FILE_MAP_READ, 0, 0, 0 );
                if( pbWaveBank )
                {
                    hr = pEngine->CreateInMemoryWaveBank( pbWaveBank, dwFileSize, 0, 0, &pWaveBank );
                }
                CloseHandle( hMapFile ); // pbWaveBank maintains a handle on the file so close this unneeded handle
            }
        }
        CloseHandle( hFile ); // pbWaveBank maintains a handle on the file so close this unneeded handle
    }
    if( FAILED( hr ) ) return E_FAIL; // CleanupXACT() will cleanup state before exiting

    // Read and register the sound bank file with XACT.  Do not use memory mapped file IO because the 
    // memory needs to be read/write and the working set of sound banks are small.    
    hr = E_FAIL; // assume failure
    hFile = CreateFile( "Sound Bank.xsb", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );

    if( hFile != INVALID_HANDLE_VALUE )
    {
        dwFileSize = GetFileSize( hFile, NULL );
        if( dwFileSize != -1 )
        {
            // Allocate the data here and free the data when recieving the sound bank destroyed notification
            pbSoundBank = new BYTE[dwFileSize];
            if( pbSoundBank )
            {
                if( 0 != ReadFile( hFile, pbSoundBank, dwFileSize, &dwBytesRead, NULL ) )
                {
                    hr = pEngine->CreateSoundBank( pbSoundBank, dwFileSize, 0, 0, &pSoundBank );
                }
            }
        }
        CloseHandle( hFile );
    }
    if( FAILED( hr ) ) return E_FAIL; // CleanupXACT() will cleanup state before exiting

    // Get the sound cue index from the sound bank
    //
    // Note that if the cue does not exist in the sound bank, the index will be XACTINDEX_INVALID
    // however this is ok especially during development.  The Play or Prepare call will just fail.
    iTick = pSoundBank->GetCueIndex("Tick");
	iPrepareBoom = pSoundBank->GetCueIndex("PrepareBoom");
	iBoom = pSoundBank->GetCueIndex("Boom");

    return S_OK;
}