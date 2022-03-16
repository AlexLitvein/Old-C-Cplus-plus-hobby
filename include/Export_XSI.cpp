#include "Export_XSI.h"

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