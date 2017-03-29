#pragma once

////////////////////////////
// System headers
#include "stdhdrs.h"

////////////////////////////
// Custom headers
#include "VSocket.h"
#include "WinVNC.h"

#include "vncServer.h"
#include "vncService.h"

#include "../include/WinVNCDll.h"
class CWinVNCDllImpl
	: public IWinVNCDll
{
public:
	CWinVNCDllImpl(IWinVNCDllNotify& rNotify);
	virtual ~CWinVNCDllImpl(void);

	bool Connect(BOOL bScreenRecord = FALSE);
	void ReleaseConnections(void);
	bool GetRfbServerInitMsg(char* pData,int& nDataLen);
	void RecvVncViewerData(const char* pData,int nLen);
    void WindowShared(BOOL enable);
    BOOL WindowShared();
    void SetMatchSizeFields(int left, int top, int right, int bottom);
	void SetWindowShared(HWND hWnd);
    HWND GetWindowShared();
	RECT GetSharedRect ();
	BOOL FullScreen();
	void FullScreen(BOOL enable);
	BOOL ScreenAreaShared();
	void ScreenAreaShared(BOOL enable);
	void SetQualityLevel(int nLevel);
	int GetQualityLevel(void);
	void EnableRemoteInput(bool bEnabled);
	bool IsEnableRemoteInput(void);
	void EnableBrocast(bool bEnabled); 
	void CaptureKeyFrame(void);
	void SetCaptureInterval(int nTime);
	int  GetCaptureInterval(void);
public:
	vncServer*	m_pVncServer;
	IWinVNCDllNotify& m_rNotify;
};
