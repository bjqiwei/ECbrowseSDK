#ifndef _AppShare_Dll
#define _AppShare_Dll

#ifdef DLl_EXPORT
   #define EXPORT_CLASS   __declspec(dllexport)
   #define EXPORT_API  extern "C" __declspec(dllexport)
#else 
   #define EXPORT_CLASS   __declspec(dllimport )
   #define EXPORT_API  extern "C" __declspec(dllimport )
#endif

#include "WinVNCDll.h"
#include "VncViewerDll.h"
EXPORT_API IWinVNCDll* CreateAppServer(IWinVNCDllNotify& rNotify);
EXPORT_API void StopAppServer();
EXPORT_API IVNCViewerDll* CreateAppViewer(HWND& parent,IVNCViewerDllNotify& rNotify);
EXPORT_API void StopAppViewer();

EXPORT_API IWinVNCDll* CreateScreenRecord(IWinVNCDllNotify& rNotify);
EXPORT_API void StopScreenRecord();
#endif