#pragma once

#include "stdhdrs.h"
#include "vncviewer.h"
#include "Exception.h"
#ifdef UNDER_CE
#include "omnithreadce.h"
#else
#include "omnithread.h"
#endif

// All logging is done via the log object

class VNCviewerApp32;
#include "../include/VncViewerDll.h"
class CVncViewerDllImpl
	: public IVNCViewerDll
{
public:
	CVncViewerDllImpl(IVNCViewerDllNotify& rNotify,HWND parent);
	virtual ~CVncViewerDllImpl(void);

	bool Connect(void);
	void ReleaseConnections(void);
	void OnWinVncFrameData(const char* pData,int nDataLen);
	void RequestFullScreenUpdate(void);
	void SetViewOnly(bool bViewOnly);
	void SetRunning(bool bRunning);
	long GetViewerWnd();
	HANDLE GrabRect(int x,int y,int w,int h);
	void Reset();

public:
	IVNCViewerDllNotify& m_rNotify;
	VNCviewerApp32*	m_pVNCviewerApp32;
	HWND            m_pParent;
};
