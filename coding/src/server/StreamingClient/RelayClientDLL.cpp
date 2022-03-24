// RelayClientDLL.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

//#include "OSMemory.h"
#include "SocketUtils.h"
#include "ClientSession.h"


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved)
{
	BOOL dllMainResult = TRUE;
	
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:

			break;
		
		case DLL_THREAD_ATTACH:
			dllMainResult = dllMainResult;
			break;
		
		case DLL_THREAD_DETACH:
			break;
		
		case DLL_PROCESS_DETACH:
			{
				dllMainResult = dllMainResult;
			}
			break;
    }

    return TRUE;
}

