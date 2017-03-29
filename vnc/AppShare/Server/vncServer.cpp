//  Copyright (C) 2000 Tridia Corporation. All Rights Reserved.
//  Copyright (C) 1999 AT&T Laboratories Cambridge. All Rights Reserved.
//
//  This file is part of the VNC system.
//
//  The VNC system is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
//  USA.
//
// TightVNC distribution homepage on the Web: http://www.tightvnc.com/
//
// If the source code for the VNC system is not available from the place 
// whence you received this file, check http://www.uk.research.att.com/vnc or contact
// the authors on vnc@uk.research.att.com for information on obtaining it.


// vncServer.cpp

// vncServer class implementation

// Includes
#include "stdhdrs.h"
#include <omnithread.h>
#include <string.h>
#include <lmcons.h>

#include "WinVNC.h"
#include "vncServer.h"
#include "vncClient.h"
#include "vncService.h"
#include "WinVNCDllImpl.h"

vncServer::vncServer(CWinVNCDllImpl& rWinVNCDllImpl,BOOL bRecordScreen)
: m_rWinVNCDllImpl(rWinVNCDllImpl)
, m_bBrocast(false)
{
	m_bRecordScreen = bRecordScreen;
	// Initialise some important stuffs...
	m_desktop = NULL;

	// Set the polling mode options
	m_poll_fullscreen = FALSE;
	m_poll_foreground = FALSE;
	m_poll_undercursor = FALSE;

	m_poll_oneventonly = FALSE;
	m_poll_consoleonly = FALSE;

	m_dont_set_hooks = FALSE;
	//coder
	m_dont_use_driver = TRUE;
	m_driver_direct_access_en = TRUE;

	// Set the input options
	m_enable_remote_inputs = TRUE;
	m_disable_local_inputs = FALSE;

	m_pVncClient = NULL;

	// Signal set when a client quits
	m_clientquitsig = new omni_condition(&m_clientsLock);
	m_local_input_priority = FALSE;

	m_full_screen = TRUE;
	m_WindowShared= FALSE;
	m_hwndShared = NULL;
	m_screen_area = FALSE;
	m_primary_display_only_shared = FALSE;
	m_disable_time = 3;
	SetSharedRect(GetScreenRect());
	SetPollingCycle(300);
	PollingCycleChanged(false);
	m_cursor_pos.x = 0;
	m_cursor_pos.y = 0;

	m_nCaptureInterval = 600;

	// initialize
	m_blank_screen = FALSE;

#ifdef HORIZONLIVE
	m_full_screen = FALSE;
	m_WindowShared= TRUE;
	m_local_input_priority = TRUE;
	m_remote_mouse = 1;
	m_remote_keyboard = 1;
#endif
}

vncServer::~vncServer()
{
	vnclog.Print(LL_STATE, VNCLOG("shutting down server object\n"));

	// Remove any active clients!
	KillAuthClients();

	// Wait for all the clients to die
	WaitUntilAuthEmpty();

	// Don't free the desktop until no KillClient is likely to free it
	{	omni_mutex_lock l(m_desktopLock);

		if (m_desktop != NULL)
		{
			delete m_desktop;
			m_desktop = NULL;
		}
	}

	if (m_clientquitsig != NULL)
	{
		delete m_clientquitsig;
		m_clientquitsig = NULL;
	}

	vnclog.Print(LL_STATE, VNCLOG("shutting down server object(4)\n"));
}

bool vncServer::AddClient(VSocket *socket)
{
	omni_mutex_lock l1(m_desktopLock);
	omni_mutex_lock l(m_clientsLock);

	// Create a new client and add it to the relevant client list
	m_pVncClient = new vncClient();
	if (m_pVncClient == NULL)
	{
		vnclog.Print(LL_CONNERR, VNCLOG("failed to allocate client object\n"));
		delete socket;
		return false;
	}
	m_pVncClient->SetScreenRecordMode(m_bRecordScreen);

	if (m_desktop == NULL)
	{
		m_desktop = new vncDesktop(m_bRecordScreen);
		if (m_desktop == NULL)
		{
			vnclog.Print(LL_CONNERR, VNCLOG("failed to allocate desktop object\n"));
			delete socket;
			return false;
		}
		if (!m_desktop->Init(this))
		{
			vnclog.Print(LL_CONNERR, VNCLOG("failed to initialize desktop object\n"));

			delete m_desktop;
			m_desktop = NULL;

			delete socket;
			return false;
		}
	}

	// Create a buffer object for this client
	vncBuffer *buffer = new vncBuffer(m_desktop);
	if (buffer == NULL)
	{
		vnclog.Print(LL_CONNERR, VNCLOG("failed to allocate buffer object\n"));
		delete socket;
		return false;
	}

	// Tell the client about this new buffer
	m_pVncClient->SetBuffer(buffer);
	// Set the client's settings
	m_pVncClient->EnableKeyboard(m_enable_remote_inputs);
	m_pVncClient->EnablePointer(m_enable_remote_inputs);

	// Start the client
	if (!m_pVncClient->Init(this, socket))
	{
		// The client will delete the socket for us...
		vnclog.Print(LL_CONNERR, VNCLOG("failed to initialize client object\n"));
		delete m_pVncClient;
		return false;
	}

	vnclog.Print(LL_INTINFO, VNCLOG("AddClient() done\n"));

	m_nLastTime = GetTickCount();
	return true;
}

void
vncServer::KillAuthClients()
{
	omni_mutex_lock l(m_clientsLock);
	if(m_pVncClient)
	{
		m_pVncClient->Kill();
	}

	vnclog.Print(LL_INTINFO, VNCLOG("KillAuthClients() done\n"));
}

void
vncServer::WaitUntilAuthEmpty()
{
	omni_mutex_lock l(m_clientsLock);
	if(m_pVncClient)
	{
		// Wait for a client to quit
		m_clientquitsig->wait();
	}
}

BOOL
vncServer::RemoteEventReceived()
{
	BOOL result = FALSE;
	omni_mutex_lock l(m_clientsLock);

	if(m_pVncClient)
	{
		result = m_pVncClient->RemoteEventReceived();
	}
	return result;
}

void
vncServer::BlockRemoteInput(BOOL block)
{
	omni_mutex_lock l(m_clientsLock);
	if(m_pVncClient)
	{
		m_pVncClient->BlockInput(block);
	}
}

// RemoveClient should ONLY EVER be used by the client to remove itself.
void
vncServer::RemoveClient()
{
	omni_mutex_lock l1(m_desktopLock);
	{	
		omni_mutex_lock l2(m_clientsLock);

		m_pVncClient = NULL;
		// Signal that a client has quit
		m_clientquitsig->signal();

	} // Unlock the clientLock

	if (m_desktop != NULL)
	{
		vnclog.Print(LL_STATE, VNCLOG("deleting desktop server\n"));

		// Delete the screen server
		delete m_desktop;
		m_desktop = NULL;
	}

	vnclog.Print(LL_INTINFO, VNCLOG("RemoveClient() done\n"));
}

// Client->Desktop update signalling
void
vncServer::RequestUpdate()
{
	omni_mutex_lock l(m_desktopLock);
	if (m_desktop != NULL)
	{
		m_desktop->RequestUpdate();
	}
}

// Update handling
void
vncServer::TriggerUpdate(BOOL bWait)
{
/*	if (bWait)
	{
		int dt = GetTickCount() - m_nLastTime;
		if (dt < m_nCaptureInterval) return;
		m_nLastTime = GetTickCount();
	}*/

	omni_mutex_lock l(m_clientsLock);
	if(m_pVncClient)
	{
		m_pVncClient->TriggerUpdate();
	}
}

void
vncServer::UpdateRect(RECT &rect)
{
	omni_mutex_lock l(m_clientsLock);
	if(m_pVncClient)
	{
		m_pVncClient->UpdateRect(rect);
	}
}

void
vncServer::UpdateRegion(vncRegion &region)
{
	omni_mutex_lock l(m_clientsLock);
	if(m_pVncClient)
	{
		m_pVncClient->UpdateRegion(region);
	}
}

void
vncServer::CopyRect(RECT &dest, POINT &source)
{
	omni_mutex_lock l(m_clientsLock);
	if(m_pVncClient)
	{
		m_pVncClient->CopyRect(dest, source);
	}
}

void
vncServer::UpdateMouse()
{
	omni_mutex_lock l(m_clientsLock);
	if(m_pVncClient)
	{
		m_pVncClient->UpdateMouse();
	}
}

void
vncServer::UpdateClipText(LPSTR text)
{
	omni_mutex_lock l(m_clientsLock);
	if(m_pVncClient)
	{
		m_pVncClient->UpdateClipText(text);
	}
}

void
vncServer::UpdatePalette()
{
	omni_mutex_lock l(m_clientsLock);
	if(m_pVncClient)
	{
		m_pVncClient->UpdatePalette();
	}
}

void
vncServer::UpdateLocalClipText(LPSTR text)
{
	omni_mutex_lock l(m_desktopLock);

	if (m_desktop != NULL)
		m_desktop->SetClipText(text);
}

// Changing hook settings

void
vncServer::DontSetHooks(BOOL enable)
{
	m_dont_set_hooks = enable;
	if (m_desktop != NULL)
		m_desktop->TryActivateHooks();
}

// Changing use driver settings

void
vncServer::DontUseDriver(BOOL enable)
{
	m_dont_use_driver = enable;
}

void
vncServer::DriverDirectAccess(BOOL enable)
{
	m_driver_direct_access_en = enable;
}

// Remote input handling
void
vncServer::EnableRemoteInputs(BOOL enable)
{
	m_enable_remote_inputs = enable;
}

BOOL vncServer::RemoteInputsEnabled()
{
	return m_enable_remote_inputs;
}

// Local input handling
void
vncServer::DisableLocalInputs(BOOL disable)
{
	if ( m_disable_local_inputs != disable )
	{
		m_disable_local_inputs = disable;
		m_desktop->SetLocalInputDisableHook(disable);
	}
}

BOOL vncServer::LocalInputsDisabled()
{
	return m_disable_local_inputs;
}

void vncServer::LocalInputPriority(BOOL disable)
{
	if( m_local_input_priority != disable )
	{
		m_local_input_priority = disable;
		m_remote_mouse = 0;
		m_remote_keyboard = 0;
		m_desktop->SetLocalInputPriorityHook(disable);
	}
	
}

void
vncServer::GetScreenInfo(int &width, int &height, int &depth)
{
	rfbServerInitMsg scrinfo;

	omni_mutex_lock l(m_desktopLock);

	vnclog.Print(LL_INTINFO, VNCLOG("GetScreenInfo called\n"));

	// Is a desktop object currently active?
	if (m_desktop == NULL)
	{
		vncDesktop desktop;

		// No, so create a dummy desktop and interrogate it
		if (!desktop.Init(this))
		{
			scrinfo.framebufferWidth = 0;
			scrinfo.framebufferHeight = 0;
			scrinfo.format.bitsPerPixel = 0;
		}
		else
		{
			desktop.FillDisplayInfo(&scrinfo);
		}
	}
	else
	{
		m_desktop->FillDisplayInfo(&scrinfo);
	}

	// Get the info from the scrinfo structure
	width = m_shared_rect.right - m_shared_rect.left;
	height = m_shared_rect.bottom - m_shared_rect.top;
	depth = scrinfo.format.bitsPerPixel;
}

inline BOOL
MatchStringToTemplate(const char *addr, size_t addrlen,
				      const char *filtstr, size_t filtlen) {
	if (filtlen == 0)
		return 1;
	if (addrlen < filtlen)
		return 0;
	for (size_t x = 0; x < filtlen; x++) {
		if (addr[x] != filtstr[x])
			return 0;
	}
	if ((addrlen > filtlen) && (addr[filtlen] != '.'))
		return 0;
	return 1;
}

void
vncServer::SetWindowShared(HWND hWnd)
{
	m_hwndShared=hWnd;
}

void  vncServer::SetMatchSizeFields(int left,int top,int right,int bottom)
{
	RECT trect = GetScreenRect();

/*	if ( right - left < 32 )
		right = left + 32;
	
	if ( bottom - top < 32)
		bottom = top + 32 ;*/

	if( right > trect.right )
		right = trect.right;
	if( bottom > trect.bottom )
		bottom = trect.bottom;
	if( left < trect.left)
		left = trect.left;
	if( top < trect.top)
		top = trect.top;

 
	m_screenarea_rect.left=left;
	m_screenarea_rect.top=top;
	m_screenarea_rect.bottom=bottom;
	m_screenarea_rect.right=right;
}


void 
vncServer::SetKeyboardCounter(int count)
{
		
	omni_mutex_lock l(m_clientsLock);
	if (LocalInputPriority() && vncService::IsWin95())
	{
		m_remote_keyboard += count;
		if (count == 0)
			m_remote_keyboard = 0;
	}       
	
}

void 
vncServer::SetMouseCounter(int count, POINT &cursor_pos, BOOL mousemove)
{
	if( (mousemove) && ( abs (m_cursor_pos.x - cursor_pos.x)==0 
		&&  abs (m_cursor_pos.y - cursor_pos.y)==0 ) ) 
		return;
	
	omni_mutex_lock l(m_clientsLock);
	if (LocalInputPriority() && vncService::IsWin95())
	{
		m_remote_mouse += count;
		if (count == 0)
			m_remote_mouse = 0;

		m_cursor_pos.x = cursor_pos.x;
		m_cursor_pos.y = cursor_pos.y;
		
	
	}
	
}

void 
vncServer::SetNewFBSize(BOOL sendnewfb)
{
	omni_mutex_lock l(m_clientsLock);
	if(m_pVncClient)
	{
		m_pVncClient->SetNewFBSize( sendnewfb);
	}
}


BOOL 
vncServer::FullRgnRequested()
{
	omni_mutex_lock l(m_clientsLock);
	if(m_pVncClient)
	{
		return m_pVncClient->FullRgnRequested();
	}

	return FALSE;
}

bool vncServer::IsUpdateWant(void)
{
	omni_mutex_lock l(m_clientsLock);
	if(m_pVncClient)
	{
		return m_pVncClient->IsUpdateWant();
	}
	return false;
}
void  vncServer::DisplayChange()
{
	m_rWinVNCDllImpl.m_rNotify.OnDisplaychanged();
}
BOOL 
vncServer::IncrRgnRequested()
{
	omni_mutex_lock l(m_clientsLock);
	if(m_pVncClient)
	{
		return m_pVncClient->IncrRgnRequested();
	}
	
	return FALSE;
}

void 
vncServer::UpdateLocalFormat()
{
	omni_mutex_lock l(m_clientsLock);
	if(m_pVncClient)
	{
		m_pVncClient->UpdateLocalFormat();
	}
}

void 
vncServer::SetPollingCycle(UINT msec)
{
	if (m_polling_cycle != msec && msec > 10) {
		m_polling_cycle = msec;
		PollingCycleChanged(true);
	}
}

BOOL
vncServer::DriverActive() {
	return (m_desktop != NULL) ? m_desktop->DriverActive() : FALSE;
}

typedef HMONITOR (WINAPI* pMonitorFromPoint)(POINT,DWORD);
typedef BOOL (WINAPI* pGetMonitorInfo)(HMONITOR,LPMONITORINFO);

BOOL vncServer::SetShareMonitorFromPoint(POINT pt)
{
	HINSTANCE  hInstUser32 = LoadLibrary("User32.DLL");
	if (!hInstUser32) return FALSE;  
	pMonitorFromPoint pMFP = (pMonitorFromPoint)GetProcAddress(hInstUser32, "MonitorFromPoint");
	pGetMonitorInfo pGMI = (pGetMonitorInfo)GetProcAddress(hInstUser32, "GetMonitorInfoA");
	if (!pMFP || !pGMI)
	{
		vnclog.Print(
			LL_INTERR,
			VNCLOG("Can not import '%s' and '%s' from '%s'.\n"),
			"MonitorFromPoint",
			"GetMonitorInfoA",
			"User32.DLL");
		FreeLibrary(hInstUser32);
		return FALSE;
	}

	HMONITOR hm = pMFP(pt, MONITOR_DEFAULTTONEAREST);
	if (!hm)
	{
		FreeLibrary(hInstUser32);
		return FALSE;
	}
	MONITORINFO	moninfo;
	moninfo.cbSize = sizeof(moninfo);
	if (!pGMI(hm, &moninfo))
	{
		FreeLibrary(hInstUser32);
		return FALSE;
	}

	FullScreen(FALSE);
	WindowShared(FALSE);
	ScreenAreaShared(TRUE);
	PrimaryDisplayOnlyShared(FALSE);

	SetMatchSizeFields(
		moninfo.rcMonitor.left,
		moninfo.rcMonitor.top,
		moninfo.rcMonitor.right,
		moninfo.rcMonitor.bottom);

	FreeLibrary(hInstUser32);
	return TRUE;
}

//coder
void vncServer::BrocastUpdate(const char* pData,int nDataLen)
{
	m_rWinVNCDllImpl.m_rNotify.OnVNCFrameData(pData,nDataLen);
}
void vncServer::OnScreenData(const char* pData,int nDataLen)
{
	m_rWinVNCDllImpl.m_rNotify.OnVNCScreenData(pData,nDataLen);
}
bool vncServer::GetRfbServerInitMsg(char* pData,int& nDataLen)
{
	omni_mutex_lock l(m_clientsLock);
	if(m_pVncClient)
	{
		return m_pVncClient->GetRfbServerInitMsg(pData,nDataLen);
	}
	return false;
}

bool vncServer::RecvVncViewerData(const char* pData,int nLen)
{
	//omni_mutex_lock l(m_clientsLock);
	if(m_pVncClient)
	{
		return m_pVncClient->RecvVncViewerData(pData,nLen);
	}
	return false;
}

void vncServer::SetQualityLevel(int nLevel)
{
	//omni_mutex_lock l(m_clientsLock);
	if(m_pVncClient)
	{
		m_pVncClient->SetQualityLevel(nLevel);
	}
}

int vncServer::GetQualityLevel(void)
{	
	//omni_mutex_lock l(m_clientsLock);
	if(m_pVncClient)
	{
		return m_pVncClient->GetQualityLevel();
	}
	return 0;
}

void vncServer::CaptureKeyFrame(void)
{
	omni_mutex_lock l(m_clientsLock);
	if(m_pVncClient)
	{
		m_pVncClient->CaptureKeyFrame();
	}
/*	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = 1024;
	r.bottom = 768;
	if (m_desktop)
		m_desktop->bDbgDumpSurfBuffers(r);*/
}
void vncServer::SetCaptureInterval(int nTime)
{
	m_nCaptureInterval = nTime;
	if (m_nCaptureInterval < 100)
		m_nCaptureInterval = 100;
}
int  vncServer::GetCaptureInterval(void)
{
	return m_nCaptureInterval;
}