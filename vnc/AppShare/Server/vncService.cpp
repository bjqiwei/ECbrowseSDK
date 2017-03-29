//  Copyright (C) 2002 RealVNC Ltd. All Rights Reserved.
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


// vncService

// Implementation of service-oriented functionality of WinVNC

#include "stdhdrs.h"

// Header

#include "vncService.h"

#include <lmcons.h>
#include "omnithread.h"
#include "WinVNC.h"

// OS-SPECIFIC ROUTINES

// Create an instance of the vncService class to cause the static fields to be
// initialised properly

vncService init;

DWORD	g_platform_id;
DWORD	g_version_major;
DWORD	g_version_minor;

vncService::vncService()
{
    OSVERSIONINFO osversioninfo;
    osversioninfo.dwOSVersionInfoSize = sizeof(osversioninfo);

    // Get the current OS version
    if (!GetVersionEx(&osversioninfo))
	    g_platform_id = 0;
    g_platform_id = osversioninfo.dwPlatformId;
	g_version_major = osversioninfo.dwMajorVersion;
	g_version_minor = osversioninfo.dwMinorVersion;
}

// IsWin95 - returns a BOOL indicating whether the current OS is Win95
BOOL
vncService::IsWin95()
{
	return (g_platform_id == VER_PLATFORM_WIN32_WINDOWS);
}

// IsWinNT - returns a bool indicating whether the current OS is WinNT
BOOL
vncService::IsWinNT()
{
	return (g_platform_id == VER_PLATFORM_WIN32_NT);
}

// Version info
DWORD
vncService::VersionMajor()
{
	return g_version_major;
}

DWORD
vncService::VersionMinor()
{
	return g_version_minor;
}

// Static routines only used on Windows NT to ensure we're in the right desktop
// These routines are generally available to any thread at any time.

// - SelectDesktop(HDESK)
// Switches the current thread into a different desktop by desktop handle
// This call takes care of all the evil memory management involved

BOOL
vncService::SelectHDESK(HDESK new_desktop)
{
	// Are we running on NT?
	if (IsWinNT())
	{
		HDESK old_desktop = GetThreadDesktop(GetCurrentThreadId());

		DWORD dummy;
		char new_name[256];

		if (!GetUserObjectInformation(new_desktop, UOI_NAME, &new_name, 256, &dummy)) {
			vnclog.Print(LL_INTERR, VNCLOG("GetUserObjectInformation() failed\n"));
			return FALSE;
		}

		vnclog.Print(LL_INTINFO, VNCLOG("SelectHDESK() to %s (%x) from %x\n"),
					 new_name, new_desktop, old_desktop);

		// Switch the desktop
		if(!SetThreadDesktop(new_desktop)) {
			vnclog.Print(LL_INTERR, VNCLOG("unable to SetThreadDesktop(), error=%d\n"), GetLastError());
			return FALSE;
		}

		// Switched successfully - destroy the old desktop
		if (!CloseDesktop(old_desktop))
			vnclog.Print(LL_INTERR, VNCLOG("SelectHDESK failed to close old desktop %x, error=%d\n"), old_desktop, GetLastError());

		return TRUE;
	}

	return TRUE;
}

// - SelectDesktop(char *)
// Switches the current thread into a different desktop, by name
// Calling with a valid desktop name will place the thread in that desktop.
// Calling with a NULL name will place the thread in the current input desktop.

BOOL
vncService::SelectDesktop(char *name)
{
	// Are we running on NT?
	if (IsWinNT())
	{
		HDESK desktop;

		if (name != NULL)
		{
			// Attempt to open the named desktop
			desktop = OpenDesktop(name, 0, FALSE,
				DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
				DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
				DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
				DESKTOP_SWITCHDESKTOP | GENERIC_WRITE);
		}
		else
		{
			// No, so open the input desktop
			desktop = OpenInputDesktop(0, FALSE,
				DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
				DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
				DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
				DESKTOP_SWITCHDESKTOP | GENERIC_WRITE);
		}

		// Did we succeed?
		if (desktop == NULL) {
			vnclog.Print(LL_INTERR, VNCLOG("unable to open desktop, error=%d\n"), GetLastError());
			return FALSE;
		}

		// Switch to the new desktop
		if (!SelectHDESK(desktop)) {
			// Failed to enter the new desktop, so free it!
			vnclog.Print(LL_INTERR, VNCLOG("SelectDesktop() failed to select desktop\n"));
			if (!CloseDesktop(desktop))
				vnclog.Print(LL_INTERR, VNCLOG("SelectDesktop failed to close desktop, error=%d\n"), GetLastError());
			return FALSE;
		}

		// We successfully switched desktops!
		return TRUE;
	}

	return (name == NULL);
}

// NT only function to establish whether we're on the current input desktop

BOOL
vncService::InputDesktopSelected()
{
	// Are we running on NT?
	if (IsWinNT())
	{
		// Get the input and thread desktops
		HDESK threaddesktop = GetThreadDesktop(GetCurrentThreadId());
		HDESK inputdesktop = OpenInputDesktop(0, FALSE,
				DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
				DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
				DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
				DESKTOP_SWITCHDESKTOP | GENERIC_WRITE);

		if (inputdesktop == NULL) {
			// Returning TRUE on ERROR_BUSY fixes the bug #1109102.
			// FIXME: Probably this is not the most correct way to do it.
			return (GetLastError() == ERROR_BUSY) ? TRUE : FALSE;
		}

		DWORD dummy;
		char threadname[256];
		char inputname[256];

		if (!GetUserObjectInformation(threaddesktop, UOI_NAME, &threadname, 256, &dummy)) {
			if (!CloseDesktop(inputdesktop))
				vnclog.Print(LL_INTWARN, VNCLOG("failed to close input desktop\n"));
			return FALSE;
		}
		_ASSERT(dummy <= 256);
		if (!GetUserObjectInformation(inputdesktop, UOI_NAME, &inputname, 256, &dummy)) {
			if (!CloseDesktop(inputdesktop))
				vnclog.Print(LL_INTWARN, VNCLOG("failed to close input desktop\n"));
			return FALSE;
		}
		_ASSERT(dummy <= 256);

		if (!CloseDesktop(inputdesktop))
			vnclog.Print(LL_INTWARN, VNCLOG("failed to close input desktop\n"));

		if (strcmp(threadname, inputname) != 0)
			return FALSE;
	}

	return TRUE;
}

// Static routine used to fool Winlogon into thinking CtrlAltDel was pressed

void *
SimulateCtrlAltDelThreadFn(void *context)
{
	HDESK old_desktop = GetThreadDesktop(GetCurrentThreadId());

	// Switch into the Winlogon desktop
	if (!vncService::SelectDesktop("Winlogon"))
	{
		vnclog.Print(LL_INTERR, VNCLOG("failed to select logon desktop\n"));
		return FALSE;
	}

	vnclog.Print(LL_ALL, VNCLOG("generating ctrl-alt-del\n"));

	HWND hwndCtrlAltDel = FindWindow("SAS window class", "SAS window");
	if (hwndCtrlAltDel == NULL) {
		vnclog.Print(LL_INTERR, VNCLOG("\"SAS window\" not found\n"));
		hwndCtrlAltDel = HWND_BROADCAST;
	}

	PostMessage(hwndCtrlAltDel, WM_HOTKEY, 0, MAKELONG(MOD_ALT | MOD_CONTROL, VK_DELETE));

	// Switch back to our original desktop
	if (old_desktop != NULL)
		vncService::SelectHDESK(old_desktop);

	return NULL;
}

// Static routine to simulate Ctrl-Alt-Del locally

BOOL
vncService::SimulateCtrlAltDel()
{
	vnclog.Print(LL_ALL, VNCLOG("preparing to generate ctrl-alt-del\n"));

	// Are we running on NT?
	if (IsWinNT())
	{
		vnclog.Print(LL_ALL, VNCLOG("spawn ctrl-alt-del thread...\n"));

		// We simulate Ctrl+Alt+Del by posting a WM_HOTKEY message to the
		// "SAS window" on the Winlogon desktop.
		// This requires that the current thread is part of the Winlogon desktop.
		// But the current thread has hooks set & a window open, so it can't
		// switch desktops, so I instead spawn a new thread & let that do the work...

		omni_thread *thread = omni_thread::create(SimulateCtrlAltDelThreadFn);
		if (thread == NULL)
			return FALSE;
		thread->join(NULL);

		return TRUE;
	}

	return TRUE;
}