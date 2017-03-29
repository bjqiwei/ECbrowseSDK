// VncViewerDll.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdhdrs.h"
#include "vncviewer.h"

HINSTANCE	hAppInstance;
Log vnclog;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		hAppInstance = (HINSTANCE)hModule;
	}
    return TRUE;
}