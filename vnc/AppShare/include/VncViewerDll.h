#ifndef __VNCVIEWERDLL_H__
#define __VNCVIEWERDLL_H__

class IVNCViewerDllNotify
{
public:
	virtual void OnVncViewerClose(void)=0;
	virtual void SendVncFrameData(const char* pData,int nDataLen)=0;
};
class IVNCViewerDll
{
public:

	static IVNCViewerDll* CreateVNCViewerImpl(HWND& parent,IVNCViewerDllNotify& rNotify);

	virtual void ReleaseConnections(void)=0;
	virtual void OnWinVncFrameData(const char* pData,int nDataLen)=0;
	virtual void RequestFullScreenUpdate(void)=0;
	virtual void SetViewOnly(bool bViewOnly)=0;
	virtual void SetRunning(bool bRunning)=0;
	virtual long GetViewerWnd()=0;
	virtual HANDLE GrabRect(int x,int y,int w,int h) = 0;
	virtual void Reset()=0;
};
#endif