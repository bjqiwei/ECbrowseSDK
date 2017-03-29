// testDlg.cpp : implementation file
//

#include "stdafx.h"
#include "test.h"
#include "testDlg.h"
#include "MediaPack.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestDlg dialog

CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestDlg::IDD, pParent)
	,m_BPSCounter_Send(0, &CTestDlg::OnReportBPS, NULL)
{
	m_BPSCounter_Send.SetEventHandlerInstance(this);
	//{{AFX_DATA_INIT(CTestDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pIWinVNCDll = NULL;
	m_pIVNCViewerDll = NULL;
	viewerWnd = NULL;
	m_nOutBps = 0;
}
CTestDlg::~CTestDlg()
{
	if (m_pIWinVNCDll)
	{
		m_pIWinVNCDll->ReleaseConnections();
		m_pIWinVNCDll = NULL;
	}
	if(m_pIVNCViewerDll)
	{
		m_pIVNCViewerDll->ReleaseConnections();
		m_pIVNCViewerDll = NULL;
	}
}
void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
	//{{AFX_MSG_MAP(CTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestDlg message handlers

RECT GetScreenRect2()
{
	RECT screenrect;
	{
		screenrect.left = 0;
		screenrect.top = 0;
		screenrect.right = GetSystemMetrics(SM_CXSCREEN);
		screenrect.bottom = GetSystemMetrics(SM_CYSCREEN);
	}
	return screenrect;
}


void CTestDlg::OnVNCFrameData(const char* pFrameData,int nDataLen)
{
/*	m_nSendLen += nDataLen;
	return ;
	if (m_bOk) return;
	if (nDataLen > 2000)
	TRACE("VNC DATA %d\n",nDataLen);
	CMediaPack* packet = new CMediaPack();
	packet->SetRawData((PBYTE)pFrameData, nDataLen);
	int nLen = m_CacheData.Put((char *)&packet, sizeof(CMediaPack*));
*/

/*	CMediaPack* packet = new CMediaPack();
	packet->SetRawData((PBYTE)pFrameData, nDataLen);
	int nLen = m_CacheData.Put((char *)&packet, sizeof(CMediaPack*));
	m_BPSCounter_Send.Update(nDataLen);
//	TRACE("data :%d t:%d\n",nDataLen,GetTickCount());
	return;  */

	if(m_pIVNCViewerDll)
	{
		m_pIVNCViewerDll->OnWinVncFrameData(pFrameData,nDataLen);
	}
	m_BPSCounter_Send.Update(nDataLen);
}
void CTestDlg::OnVNCScreenData(const char* pFrameData,int nDataLen)
{

}
void CTestDlg::OnVncViewerClose(void)
{
}
void CTestDlg::SendVncFrameData(const char* pData,int nDataLen)
{
	return ;
	if (m_pIWinVNCDll)
		m_pIWinVNCDll->RecvVncViewerData(pData,nDataLen);
}
void CTestDlg::OnReportBPS(DWORD dwChannelID, double fBPS_Avg, double fBPS_Now, DWORD nHitCount)
{
	m_nOutBps = (int)(fBPS_Now/8192);

	CString str;
	str.Format("Á÷Á¿: %d KB",m_nOutBps);
	SetWindowText(str);
/*	UIMessage uimsg;
	uimsg.messageID = NtfMSG_REPORT_BPS;
	uimsg.mType = dwChannelID;
	uimsg.nParam1 = fBPS_Avg;
	uimsg.nParam2 = fBPS_Now;
	uimsg.nParam3 = nHitCount;
	SendMessageToUI(uimsg);*/
}
BOOL CTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_CacheData.Create(sizeof(CMediaPack*) * 30000);
//	SetTimer(10,2000,NULL);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTestDlg::OnOK() 
{
	// TODO: Add extra validation here
	m_nSendLen = 0;
	m_bOk = FALSE;
	if(m_pIWinVNCDll == NULL)
	{
		m_pIWinVNCDll = IWinVNCDll::CreateWinVNCImpl(*this);
		if(m_pIWinVNCDll == NULL)
		{
			return ;
		}
	}
	if (m_pIWinVNCDll)
	{
		m_pIWinVNCDll->EnableBrocast(true);
		m_pIWinVNCDll->SetQualityLevel(6);
		m_pIWinVNCDll->CaptureKeyFrame();
		m_pIWinVNCDll->EnableRemoteInput(true);
		m_pIWinVNCDll->SetCaptureInterval(100);

//		m_pIWinVNCDll->WindowShared(TRUE);
//		m_pIWinVNCDll->SetWindowShared(GetDesktopWindow()->m_hWnd);
	//	m_pIWinVNCDll->EnableBrocast(true);
	//	m_pIWinVNCDll->SetQualityLevel(9);
	//	RECT temp = GetScreenRect2();
	//	m_pIWinVNCDll->SetMatchSizeFields(temp.left, temp.top, temp.right, temp.bottom);

		char pData[8192];
		int ulDataLen = 0;
		if(m_pIWinVNCDll->GetRfbServerInitMsg(pData,ulDataLen))
		{
			m_pIVNCViewerDll = IVNCViewerDll::CreateVNCViewerImpl(m_hWnd,*this);
			if(m_pIVNCViewerDll)
			{
				m_pIVNCViewerDll->OnWinVncFrameData(pData,ulDataLen);
				/*CMediaPack* packet = new CMediaPack();
				packet->SetRawData((PBYTE)pData, ulDataLen);
				int nLen = m_CacheData.Put((char *)&packet, sizeof(CMediaPack*));

*/
				m_pIVNCViewerDll->SetViewOnly(true);
				m_pIVNCViewerDll->SetRunning(true);

				viewerWnd = (HWND)m_pIVNCViewerDll->GetViewerWnd();
				if (viewerWnd)
				{
					CRect r;
					GetClientRect(r);
					::MoveWindow(viewerWnd,r.left,r.top,r.Width(),r.Height()-100,TRUE);				
				}
			}
			else
			{
				;//
			}
			
			
			SetTimer(100,50,NULL);
		}

		
	}

//	CDialog::OnOK();
}

void CTestDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (viewerWnd)
	{
		::MoveWindow(viewerWnd,0,0,cx,cy-100,TRUE);
	}
	
}

void CTestDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	if (m_pIWinVNCDll)
		m_pIWinVNCDll->CaptureKeyFrame();
}

void CTestDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 10)
	{
		if (m_pIWinVNCDll)
		m_pIWinVNCDll->CaptureKeyFrame();
	}
	else if (nIDEvent == 100)
	{
		CMediaPack *pPacket = NULL;
		int count = 30;
		while(--count > 0 && !m_CacheData.IsEmpty())
		{
			int nLen = m_CacheData.Get((char *)&pPacket, sizeof(CMediaPack *));
			if (pPacket)
			{
				BYTE * pData = pPacket->GetDataPtr(1024*1024*10, nLen);
				if (pData && nLen > 0)
				{
					if(m_pIVNCViewerDll)
					{
						m_pIVNCViewerDll->OnWinVncFrameData((char*)pData,nLen);
					}
				}
			}
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CTestDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	CMediaPack *pPacket = NULL;
	try
	{
		while (1)
		{
			if (m_CacheData.IsEmpty()) 
				break;
			int nLen = m_CacheData.Get((char *)&pPacket, sizeof(CMediaPack *));
			if (0 != nLen)
			{
				ASSERT (sizeof(CMediaPack *) == nLen);
				ASSERT (pPacket);
				delete pPacket;
			}
		}
	}
	catch (...)
	{
		;
	}
		
	if (m_pIWinVNCDll)
	{
		m_pIWinVNCDll->ReleaseConnections();
		m_pIWinVNCDll = NULL;
	}
	if(m_pIVNCViewerDll)
	{
		m_pIVNCViewerDll->ReleaseConnections();
		m_pIVNCViewerDll = NULL;
	}
//	CDialog::OnCancel();
}

void CTestDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here
	CString str;
	str.Format("Send len %d",m_nSendLen);
	AfxMessageBox(str);
	return ;
	m_bOk = TRUE;
	SetTimer(100,50,NULL);
/*	int count = 0;
	while(1)
	{
		CMediaPack *pPacket = NULL;
		if (m_CacheData.IsEmpty())
			break;
		int nLen = m_CacheData.Get((char *)&pPacket, sizeof(CMediaPack *));
		if (pPacket)
		{
			BYTE * pData = pPacket->GetDataPtr(1024*1024*10, nLen);
			if (pData && nLen > 0)
			{
				if(m_pIVNCViewerDll)
				{
					m_pIVNCViewerDll->OnWinVncFrameData((char*)pData,nLen);
				}
			}
		}
		Sleep(20);
	}*/
	
				
}
