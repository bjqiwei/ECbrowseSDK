// AppShareDll.h : main header file for the APPSHAREDLL DLL
//

#if !defined(AFX_APPSHAREDLL_H__F7D10785_550D_4B93_8407_C4E907438DFA__INCLUDED_)
#define AFX_APPSHAREDLL_H__F7D10785_550D_4B93_8407_C4E907438DFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAppShareDllApp
// See AppShareDll.cpp for the implementation of this class
//

class CAppShareDllApp : public CWinApp
{
public:
	CAppShareDllApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppShareDllApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAppShareDllApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPSHAREDLL_H__F7D10785_550D_4B93_8407_C4E907438DFA__INCLUDED_)
