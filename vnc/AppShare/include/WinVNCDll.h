#ifndef __WINVNCDLL_H__
#define __WINVNCDLL_H__

class IWinVNCDllNotify
{
public:
	virtual void OnVNCFrameData(const char* pFrameData,int nDataLen)=0;
	virtual void OnVNCScreenData(const char* pFrameData,int nDataLen)=0;
	virtual void OnDisplaychanged() =0;
};
class IWinVNCDll
{
public:
	static IWinVNCDll* CreateWinVNCImpl(IWinVNCDllNotify& rNotify,BOOL bScreenRecord=FALSE);

	virtual void ReleaseConnections(void)=0;
	virtual bool GetRfbServerInitMsg(char* pData,int& nDataLen)=0;
	virtual void RecvVncViewerData(const char* pData,int nLen)=0;
    virtual void WindowShared(BOOL enable)=0;
    virtual BOOL WindowShared()=0;
    virtual void SetMatchSizeFields(int left, int top, int right, int bottom)=0;
	virtual void SetWindowShared(HWND hWnd)=0;
    virtual HWND GetWindowShared()=0;
	virtual RECT GetSharedRect ()=0;
	virtual BOOL FullScreen()=0;
	virtual void FullScreen(BOOL enable)=0;
	virtual BOOL ScreenAreaShared()=0;
	virtual void ScreenAreaShared(BOOL enable)=0;
	virtual void SetQualityLevel(int nLevel)=0;
	virtual int GetQualityLevel(void)=0;
	virtual void EnableRemoteInput(bool bEnabled)=0;
	virtual bool IsEnableRemoteInput(void)=0;
	virtual void EnableBrocast(bool bEnabled)=0; 
	virtual void CaptureKeyFrame(void)=0;
	virtual void SetCaptureInterval(int nTime)=0;//设置采集间隔时间
	virtual int  GetCaptureInterval(void)=0;//获取采集间隔时间
};
#endif