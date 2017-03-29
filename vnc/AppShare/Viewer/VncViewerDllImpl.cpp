#include ".\vncviewerdllimpl.h"
#include "VNCviewerApp32.h"

vLog vnclog;

IVNCViewerDll* IVNCViewerDll::CreateVNCViewerImpl(HWND& parent,IVNCViewerDllNotify& rNotify)
{
	
	CVncViewerDllImpl* pVncViewerDllImpl = new CVncViewerDllImpl(rNotify,parent);
	if(pVncViewerDllImpl)
	{
		if(pVncViewerDllImpl->Connect() == false)
		{
			pVncViewerDllImpl->ReleaseConnections();
			pVncViewerDllImpl = NULL;
		}
	}
	return pVncViewerDllImpl;
	
}

CVncViewerDllImpl::CVncViewerDllImpl(IVNCViewerDllNotify& rNotify,HWND parent)
: m_rNotify(rNotify)
, m_pVNCviewerApp32(NULL)
,m_pParent(parent)
{
}

CVncViewerDllImpl::~CVncViewerDllImpl(void)
{
}

bool CVncViewerDllImpl::Connect(void)
{
	m_pVNCviewerApp32 = new VNCviewerApp32(NULL,*this);
	if(m_pVNCviewerApp32)
	{
		return m_pVNCviewerApp32->CreateViewer(m_pParent);
	}
	return false;
}

void CVncViewerDllImpl::ReleaseConnections(void)
{
	if(m_pVNCviewerApp32)
	{
		delete m_pVNCviewerApp32;
		m_pVNCviewerApp32 = NULL;
	}
	delete this;
}

void CVncViewerDllImpl::OnWinVncFrameData(const char* pData,int nDataLen)
{
	if(m_pVNCviewerApp32)
	{
		m_pVNCviewerApp32->OnWinVncFrameData(pData,nDataLen);
	}
}

void CVncViewerDllImpl::RequestFullScreenUpdate(void)
{
	if(m_pVNCviewerApp32)
	{
		m_pVNCviewerApp32->RequestFullScreenUpdate();
	}
}

void CVncViewerDllImpl::SetViewOnly(bool bViewOnly)
{
	if(m_pVNCviewerApp32 && m_pVNCviewerApp32->m_pViewer)
	{
		m_pVNCviewerApp32->m_pViewer->SetViewOnly(bViewOnly);
	}
}

void CVncViewerDllImpl::SetRunning(bool bRunning)
{
	if(m_pVNCviewerApp32 && m_pVNCviewerApp32->m_pViewer)
	{
		m_pVNCviewerApp32->m_pViewer->SetRunning(bRunning);
	}
}
long CVncViewerDllImpl::GetViewerWnd()
{
	if(m_pVNCviewerApp32 && m_pVNCviewerApp32->m_pViewer)
	{
		return m_pVNCviewerApp32->m_pViewer->GetWnd();
	}
	return 0;
}
HANDLE CVncViewerDllImpl::GrabRect(int x,int y,int w,int h)
{
	if(m_pVNCviewerApp32 && m_pVNCviewerApp32->m_pViewer)
	{
		return m_pVNCviewerApp32->m_pViewer->GrabRect(x,y,w,h);
	}
	return 0;
}
void CVncViewerDllImpl::Reset()
{
	if(m_pVNCviewerApp32 && m_pVNCviewerApp32->m_pViewer)
	{
		m_pVNCviewerApp32->m_pViewer->Reset();
	}
}