// ATLVideoObject.cpp : Implementation of CATLVideoObject
#include "stdafx.h"
#include "ATLVideoObject.h"


// CATLVideoObject

LRESULT CATLVideoObject::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// TODO: Add your message handler code here and/or call default
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " Control m_hWnd:" << m_hWnd);
	return TRUE;
}


STDMETHODIMP CATLVideoObject::CreateChild(LONG* result)
{
	// TODO:  在此添加实现代码
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	CChildWindow * window = new CChildWindow();
	window->Create(this->m_hWnd);
	window->ShowWindow(SW_SHOWNORMAL);
	RECT rc;
	this->GetWindowRect(&rc);
	window->MoveWindow(0, 0, (rc.right - rc.left) / 3, (rc.bottom - rc.top) / 3);
	m_ChildWindow[window->m_hWnd] = window;
	*result = (LONG)window->m_hWnd;
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " Create Window:" << *result);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return S_OK;
}


STDMETHODIMP CATLVideoObject::RemoveChild(LONG window, LONG* result)
{
	// TODO:  在此添加实现代码
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	if (window == 0)
	{
		LOG4CPLUS_INFO(log, __FUNCTION__ << " remove all child window.");
		while (!m_ChildWindow.empty())
		{
			std::map<HWND, CChildWindow*>::const_iterator it = m_ChildWindow.begin();
			it->second->DestroyWindow();
			delete it->second;
			m_ChildWindow.erase(it);
		}
		*result = 0;
	}
	else{
		std::map<HWND, CChildWindow*>::const_iterator it = m_ChildWindow.find((HWND)window);
		if (it != m_ChildWindow.end())
		{
			it->second->DestroyWindow();
			delete it->second;
			m_ChildWindow.erase(it);
			*result = 0;
			LOG4CPLUS_INFO(log, __FUNCTION__ << " remove child window:" << window);
		}
		else{
			*result = -1;
			LOG4CPLUS_WARN(log, __FUNCTION__ << " not find child window:" << window);
		}
	}
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return S_OK;
}


STDMETHODIMP CATLVideoObject::MoveWindow(LONG window, LONG x, LONG y, LONG width, LONG height, LONG* result)
{
	// TODO:  在此添加实现代码
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	std::map<HWND, CChildWindow*>::const_iterator it = m_ChildWindow.find((HWND)window);
	if (it != m_ChildWindow.end())
	{
		it->second->MoveWindow(x, y, width, height);
		*result = 0;
		LOG4CPLUS_INFO(log, __FUNCTION__ << " move child window:" << window << " x:" << x << " y:" << y
			<< " width:" << width << " height:" << height);
	}
	else{
		*result = -1;
		LOG4CPLUS_WARN(log, __FUNCTION__ << " not find child window:" << window);
	}
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return S_OK;
}


LRESULT CATLVideoObject::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ":" << m_hWnd);
	while (!m_ChildWindow.empty())
	{
		std::map<HWND, CChildWindow*>::const_iterator it = m_ChildWindow.begin();
		it->second->DestroyWindow();
		delete it->second;
		m_ChildWindow.erase(it);
	}
	return 0;
}
