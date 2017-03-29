// AppShareDll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "AppShareDll.h"
#include "AppShareInterface.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CAppShareDllApp

BEGIN_MESSAGE_MAP(CAppShareDllApp, CWinApp)
	//{{AFX_MSG_MAP(CAppShareDllApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAppShareDllApp construction

CAppShareDllApp::CAppShareDllApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAppShareDllApp object

CAppShareDllApp theApp;

IWinVNCDll*					g_pIWinVNCDll=NULL;
IVNCViewerDll*              g_pIVNCViewerDll=NULL;
IWinVNCDll*					g_pIWinScreenVNCDll=NULL;

IWinVNCDll* CreateAppServer(IWinVNCDllNotify& rNotify)
{
	g_pIWinVNCDll = IWinVNCDll::CreateWinVNCImpl(rNotify);
	return g_pIWinVNCDll;
}
void StopAppServer()
{
	if (g_pIWinVNCDll)
		g_pIWinVNCDll->ReleaseConnections();
	g_pIWinVNCDll = NULL;
}

IWinVNCDll* CreateScreenRecord(IWinVNCDllNotify& rNotify)
{
	g_pIWinScreenVNCDll = IWinVNCDll::CreateWinVNCImpl(rNotify,TRUE);
	return g_pIWinScreenVNCDll;
}
void StopScreenRecord()
{
	if (g_pIWinScreenVNCDll)
		g_pIWinScreenVNCDll->ReleaseConnections();
	g_pIWinScreenVNCDll = NULL;
}

IVNCViewerDll* CreateAppViewer(HWND& parent,IVNCViewerDllNotify& rNotify)
{
	g_pIVNCViewerDll = IVNCViewerDll::CreateVNCViewerImpl(parent,rNotify);
	return g_pIVNCViewerDll;
}
void StopAppViewer()
{
	if (g_pIVNCViewerDll)
		g_pIVNCViewerDll->ReleaseConnections();
	g_pIVNCViewerDll = NULL;
}
