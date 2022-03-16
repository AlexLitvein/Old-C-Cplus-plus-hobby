#include "MError.h"

char err_buff[ERRBUFF_SIZE];

size_t g_lastError = M_OK;

const char *GetLastErrorText()
{
	return &g_errStr[g_lastError - ERR_CODES][0];	
}

void ShowLastErrorWin(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code
    LPVOID lpMsgBuf;   
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process
	 StringCchPrintf((LPTSTR)err_buff, ERRBUFF_SIZE, TEXT("%s failed with error %d: %s"), 
					 lpszFunction, dw, lpMsgBuf); 

    MessageBox(NULL, (LPCTSTR)err_buff, TEXT("Error"), MB_OK); 
}
