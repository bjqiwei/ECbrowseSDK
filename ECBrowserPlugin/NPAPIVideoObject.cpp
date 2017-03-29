#include "stdafx.h"
#include "NPAPIVideoObject.h"
#include <log4cplus/loggingmacros.h>
#include "ChildWindow.h"


NPAPIVideoObject::NPAPIVideoObject(NPP pNPInstance) :NPAPIPluginBase(pNPInstance)
{
	this->log = log4cplus::Logger::getInstance("NPAPIVideoObject");

	this->pHWND_id = NPN_GetStringIdentifier("HWND");

	this->fCreateChild_id = NPN_GetStringIdentifier("CreateChild");
	this->fRemoveChild_id = NPN_GetStringIdentifier("RemoveChild");
	this->fMoveWindow_id = NPN_GetStringIdentifier("MoveWindow");
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " Constructor  m_hWnd:" << m_hWnd);

}

bool NPAPIVideoObject::HasProperty(const NPIdentifier name) const
{
	//LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	bool result = (this->pHWND_id == name);
	const char * strName = NPN_UTF8FromIdentifier(name);
	if (result)
	{
		LOG4CPLUS_DEBUG(log, getName() << " HasProperty:" << strName);
	}else
	{
		LOG4CPLUS_WARN(log, getName() << " NoHasProperty:" << strName);
	}
	NPN_MemFree((void *)strName);
	//LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return result;
}

bool NPAPIVideoObject::GetProperty(const NPIdentifier name, NPVariant *result) const
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	VOID_TO_NPVARIANT(*result);
	bool res = true;
	const char * strName = NPN_UTF8FromIdentifier(name);
	if (name == this->pHWND_id) {
		INT32_TO_NPVARIANT((int32_t)m_hWnd, *result);
	}else
	{
		res = false;
		LOG4CPLUS_WARN(log, getName() << " No GetProperty:" << strName);
	}
	NPN_MemFree((void *)strName);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return res;
}

bool NPAPIVideoObject::HasMethod(const NPIdentifier name) const
{
	//LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	bool result = (name == this->fCreateChild_id
		|| name == this->fRemoveChild_id
		|| name == this->fMoveWindow_id);

	const char * strName = NPN_UTF8FromIdentifier(name);
	if (result)
	{
		LOG4CPLUS_DEBUG(log, getName() << " HasMethod:" << strName);
	}
	else
	{
		LOG4CPLUS_WARN(log, getName() << " NoHasMethod:" << strName);
	}
	NPN_MemFree((void *)strName);

	//LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return result;
}

bool NPAPIVideoObject::Invoke(const NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	//LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	if (name == this->fCreateChild_id)
	{
		CChildWindow * window = new CChildWindow();
		window->Create(this->m_hWnd);
		window->ShowWindow(SW_SHOWNORMAL);
		RECT rc;
		GetWindowRect(this->m_hWnd, &rc);
		window->MoveWindow(0, 0, (rc.right - rc.left) / 3, (rc.bottom - rc.top) / 3);
		m_ChildWindow[window->m_hWnd] = window;
		LOG4CPLUS_TRACE(log, " Create Window:" << (LONG)window->m_hWnd);
		INT32_TO_NPVARIANT((LONG)window->m_hWnd, *result);
		return true;
	}
	else if (name == this->fRemoveChild_id)
	{
		if (argCount < 1)
		{
			LOG4CPLUS_ERROR(log, "Login missing parameter.");
			return false;
		}
		int32_t window;
		NPVARIANT_TO_INT32_SAFE(args[0], window);

		if (window == 0)
		{
			LOG4CPLUS_INFO(log, "RemoveChild remove all child window.");
			while (!m_ChildWindow.empty())
			{
				std::map<HWND, CChildWindow*>::const_iterator it = m_ChildWindow.begin();
				it->second->DestroyWindow();
				delete it->second;
				m_ChildWindow.erase(it);
			}
			INT32_TO_NPVARIANT(0, *result);
		}
		else{
			std::map<HWND, CChildWindow*>::const_iterator it = m_ChildWindow.find((HWND)window);
			if (it != m_ChildWindow.end())
			{
				it->second->DestroyWindow();
				delete it->second;
				m_ChildWindow.erase(it);
				INT32_TO_NPVARIANT(0, *result);
				LOG4CPLUS_INFO(log, " remove child window:" << window);
			}
			else{
				INT32_TO_NPVARIANT(-1, *result);
				LOG4CPLUS_WARN(log, " not find child window:" << window);
			}
		}
		
		return true;
	}
	else if (name == this->fMoveWindow_id)
	{
		int32_t window;
		int32_t x;
		int32_t y;
		int32_t width;
		int32_t height;
		if (argCount < 5)
		{
			LOG4CPLUS_ERROR(log, "Login missing parameter.");
			return false;
		}
		NPVARIANT_TO_INT32_SAFE(args[0], window);
		NPVARIANT_TO_INT32_SAFE(args[1], x);
		NPVARIANT_TO_INT32_SAFE(args[2], y);
		NPVARIANT_TO_INT32_SAFE(args[3], width);
		NPVARIANT_TO_INT32_SAFE(args[4], height);		

		std::map<HWND, CChildWindow*>::const_iterator it = m_ChildWindow.find((HWND)window);
		if (it != m_ChildWindow.end())
		{
			it->second->MoveWindow(x, y, width, height);
			INT32_TO_NPVARIANT(0, *result);
			LOG4CPLUS_INFO(log, __FUNCTION__ << " move child window:" << window << " x:" << x << " y:" << y
				<< " width:" << width << " height:" << height);
		}
		else{
			INT32_TO_NPVARIANT(-1, *result);
			LOG4CPLUS_WARN(log, __FUNCTION__ << " not find child window:" << window);
		}

		return true;
	}
	
	//LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return false;
}
NPBool NPAPIVideoObject::init(NPWindow* pNPWindow)
{
	NPAPIPluginBase::init(pNPWindow);
	LONG style = GetWindowLong(this->m_hWnd, GWL_STYLE);
	style = style | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	SetWindowLong(this->m_hWnd, GWL_STYLE, style);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ << " set windows style WS_CLIPSIBLINGS | WS_CLIPCHILDREN");
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " m_hWnd:" << m_hWnd);

	return TRUE;
}

void NPAPIVideoObject::shut()
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ":" << m_hWnd);
	while (!m_ChildWindow.empty())
	{
		std::map<HWND, CChildWindow*>::const_iterator it = m_ChildWindow.begin();
		it->second->DestroyWindow();
		delete it->second;
		m_ChildWindow.erase(it);
	}
	NPAPIPluginBase::shut();
}

NPAPIVideoObject::~NPAPIVideoObject(void)
{

}
