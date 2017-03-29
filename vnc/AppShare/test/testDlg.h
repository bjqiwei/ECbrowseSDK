// testDlg.h : header file
//

#if !defined(AFX_TESTDLG_H__9EC47102_D6F7_44CB_B1D0_7069E4CB5A24__INCLUDED_)
#define AFX_TESTDLG_H__9EC47102_D6F7_44CB_B1D0_7069E4CB5A24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTestDlg dialog
#include "WinVNCDll.h"
#include "VncViewerDll.h"
#include "BPSCounter.h"
#include "FIFO.h"
class CTestDlg : public CDialog, public IWinVNCDllNotify,public IVNCViewerDllNotify
{
// Construction
public:
	CTestDlg(CWnd* pParent = NULL);	// standard constructor
	~CTestDlg();

	void OnDisplaychanged(){;}
	void OnVNCFrameData(const char* pFrameData,int nDataLen);
	virtual void OnVNCScreenData(const char* pFrameData,int nDataLen);

	void OnVncViewerClose(void);
	void SendVncFrameData(const char* pData,int nDataLen);


	virtual void OnReportBPS(DWORD dwChannelID, double fBPS_Avg, double fBPS_Now, DWORD nHitCount);

	BPSCounter<CTestDlg, 3> m_BPSCounter_Send;

	IWinVNCDll*					m_pIWinVNCDll;
	IVNCViewerDll*              m_pIVNCViewerDll;

	CFIFO			m_CacheData;
	BOOL            m_bOk;
	int             m_nSendLen;
	HWND                        viewerWnd;
	int  m_nOutBps;
// Dialog Data
	//{{AFX_DATA(CTestDlg)
	enum { IDD = IDD_TEST_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnButton1();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnCancel();
	afx_msg void OnButton2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTDLG_H__9EC47102_D6F7_44CB_B1D0_7069E4CB5A24__INCLUDED_)
