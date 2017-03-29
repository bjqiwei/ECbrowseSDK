// ChildWindow.h : CChildWindow 的声明

#pragma once

#include "resource.h"       // 主符号

#include <atlhost.h>

using namespace ATL;

// CChildWindow

class CChildWindow : 
	public CAxDialogImpl<CChildWindow>
{
public:
	CChildWindow()
	{
	}

	~CChildWindow()
	{
	}

	enum { IDD = IDD_ATLVIDEOOBJECT };

BEGIN_MSG_MAP(CChildWindow)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	CHAIN_MSG_MAP(CAxDialogImpl<CChildWindow>)
END_MSG_MAP()

// 处理程序原型: 
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CAxDialogImpl<CChildWindow>::OnInitDialog(uMsg, wParam, lParam, bHandled);
		bHandled = TRUE;
		return 1;  // 使系统设置焦点
	}

};


