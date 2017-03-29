#include ".\winvncdllimpl.h"

//#pragma   comment(linker,   "/NODEFAULTLIB:LIBC.lib")   
//#pragma   comment(linker,   "/NODEFAULTLIB:LIBCD.lib")   

IWinVNCDll* IWinVNCDll::CreateWinVNCImpl(IWinVNCDllNotify& rNotify,BOOL bScreenRecord)
{
	
	CWinVNCDllImpl* pWinVNCDllImpl = new CWinVNCDllImpl(rNotify);
	if(pWinVNCDllImpl)
	{
		if(pWinVNCDllImpl->Connect(bScreenRecord) == false)
		{
			pWinVNCDllImpl->ReleaseConnections();
			pWinVNCDllImpl = NULL;
		}
	}
	return pWinVNCDllImpl;
	
}

CWinVNCDllImpl::CWinVNCDllImpl(IWinVNCDllNotify& rNotify)
: m_pVncServer(NULL)
, m_rNotify(rNotify)
{
}

CWinVNCDllImpl::~CWinVNCDllImpl(void)
{
}

bool CWinVNCDllImpl::Connect(BOOL bScreenRecord)
{
	m_pVncServer = new vncServer(*this,bScreenRecord);
	if(m_pVncServer == NULL)
	{
		return false;
	}

	VSocket* pVSocket = new VSocket;
	if(pVSocket == NULL)
	{
		return false;
	}
	if(m_pVncServer->AddClient(pVSocket) == false)
	{
		return false;
	}

	return true;
}

void CWinVNCDllImpl::ReleaseConnections(void)
{
	if(m_pVncServer)
	{
		delete m_pVncServer;
		m_pVncServer = NULL;
	}

	delete this;
}

bool CWinVNCDllImpl::GetRfbServerInitMsg(char* pData,int& nDataLen)
{
	if(m_pVncServer)
	{
		return m_pVncServer->GetRfbServerInitMsg(pData,nDataLen);
	}
	return false;
}

void CWinVNCDllImpl::RecvVncViewerData(const char* pData,int nLen)
{
	if(m_pVncServer)
	{
		m_pVncServer->RecvVncViewerData(pData,nLen);
	}
}

void CWinVNCDllImpl::WindowShared(BOOL enable)
{
	if(m_pVncServer)
	{
		m_pVncServer->WindowShared(enable);
	}
}

BOOL CWinVNCDllImpl::WindowShared()
{
	if(m_pVncServer)
	{
		return m_pVncServer->WindowShared();
	}
	return FALSE;
}

void CWinVNCDllImpl::SetMatchSizeFields(int left, int top, int right, int bottom)
{
	if(m_pVncServer)
	{
		m_pVncServer->SetMatchSizeFields(left,top,right,bottom);
	}
}

void CWinVNCDllImpl::SetWindowShared(HWND hWnd)
{
	if(m_pVncServer)
	{
		m_pVncServer->SetWindowShared(hWnd);
	}
}

HWND CWinVNCDllImpl::GetWindowShared()
{
	if(m_pVncServer)
	{
		return m_pVncServer->GetWindowShared();
	}
	return NULL;
}

RECT CWinVNCDllImpl::GetSharedRect()
{
	RECT rcDst;
	rcDst.left = rcDst.right = rcDst.top = rcDst.bottom = 0;
	if(m_pVncServer)
	{
		rcDst = m_pVncServer->GetSharedRect();
	}
	return rcDst;
}

BOOL CWinVNCDllImpl::FullScreen()
{
	if(m_pVncServer)
	{
		return m_pVncServer->FullScreen();
	}
	return FALSE;
}

void CWinVNCDllImpl::FullScreen(BOOL enable)
{
	if(m_pVncServer)
	{
		m_pVncServer->FullScreen(enable);
	}
}

BOOL CWinVNCDllImpl::ScreenAreaShared()
{
	if(m_pVncServer)
	{
		return m_pVncServer->ScreenAreaShared();
	}
	return FALSE;
}

void CWinVNCDllImpl::ScreenAreaShared(BOOL enable)
{
	if(m_pVncServer)
	{
		m_pVncServer->ScreenAreaShared(enable);
	}
}

void CWinVNCDllImpl::SetQualityLevel(int nLevel)
{
	if(m_pVncServer)
	{
		m_pVncServer->SetQualityLevel(nLevel);
	}
}

int CWinVNCDllImpl::GetQualityLevel(void)
{
	if(m_pVncServer)
	{
		return m_pVncServer->GetQualityLevel();
	}
	return 0;
}

void CWinVNCDllImpl::EnableRemoteInput(bool bEnabled)
{
	if(bEnabled)
	{
		if(m_pVncServer)
		{
			m_pVncServer->EnableRemoteInputs(TRUE);
			m_pVncServer->BlockRemoteInput(FALSE);
		}
	}
	else
	{
		if(m_pVncServer)
		{
			m_pVncServer->EnableRemoteInputs(FALSE);
			m_pVncServer->BlockRemoteInput(TRUE);
		}
	}
}

bool CWinVNCDllImpl::IsEnableRemoteInput(void)
{
	if(m_pVncServer && m_pVncServer->RemoteInputsEnabled())
	{
		return true;
	}
	return false;
}

void CWinVNCDllImpl::EnableBrocast(bool bEnabled)
{
	if(m_pVncServer)
	{
		m_pVncServer->m_bBrocast = bEnabled;
	}
}

void CWinVNCDllImpl::CaptureKeyFrame(void)
{
	if(m_pVncServer)
	{
		m_pVncServer->CaptureKeyFrame();
	}
}
void CWinVNCDllImpl::SetCaptureInterval(int nTime)
{
	if(m_pVncServer)
	{
		m_pVncServer->SetCaptureInterval(nTime);
	}
}
int  CWinVNCDllImpl::GetCaptureInterval(void)
{
	if(m_pVncServer)
	{
		return m_pVncServer->GetCaptureInterval();
	}
	return 0;
}