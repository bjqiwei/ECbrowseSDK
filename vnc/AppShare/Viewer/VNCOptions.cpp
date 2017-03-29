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


// VNCOptions.cpp: implementation of the VNCOptions class.

#include "stdhdrs.h"
#include "vncviewer.h"
#include "VNCOptions.h"
#include "Exception.h"
#include "commctrl.h"
VNCOptions::VNCOptions()
{
	m_ViewOnly = false;
	m_FullScreen = true; //hubintest false
	m_skipprompt = false;
	m_Use8Bit = false;
	m_PreferredEncoding = rfbEncodingTight;
	m_SwapMouse = false;
	m_Emul3Buttons = true;
	m_Emul3Timeout = 100; // milliseconds
	m_Emul3Fuzz = 4;      // pixels away before emulation is cancelled
	m_Shared = true;
	m_DeiconifyOnBell = false;
	m_DisableClipboard = false;
	m_localCursor = DOTCURSOR;
	m_FitWindow = true;
	m_scaling = true;
	m_scale_num = 100;
	m_scale_den = 100;
	
	m_hWindow = 0;
	m_kbdname[0] = '\0';
	m_kbdSpecified = false;
	
	m_logLevel = 0;
	m_logToConsole = false;
	m_logToFile = false;
	strcpy(m_logFilename ,"vncviewer.log");
	
	m_delay=0;
	m_connectionSpecified = false;
	m_configSpecified = false;
	m_configFilename[0] = '\0';
	m_restricted = false;

	m_useCompressLevel = false;
	m_compressLevel = 6;
	m_enableJpegCompression = true;
	m_jpegQualityLevel = 6;
	m_requestShapeUpdates = true;
	m_ignoreShapeUpdates = false;
	
#ifdef UNDER_CE
	m_palmpc = false;
	
	// Check for PalmPC aspect 
	HDC temp_hdc = GetDC(NULL);
	int screen_width = GetDeviceCaps(temp_hdc, HORZRES);
	if (screen_width < 320)
	{
		m_palmpc = true;
	}
	ReleaseDC(NULL,temp_hdc);

	m_slowgdi = false;
#endif
}


VNCOptions& VNCOptions::operator=(VNCOptions& s)
{
	m_ViewOnly			= s.m_ViewOnly;
	m_FullScreen		= s.m_FullScreen;
	m_Use8Bit			= s.m_Use8Bit;
	m_PreferredEncoding = s.m_PreferredEncoding;
	m_SwapMouse			= s.m_SwapMouse;
	m_Emul3Buttons		= s.m_Emul3Buttons;
	m_Emul3Timeout		= s.m_Emul3Timeout;
	m_Emul3Fuzz			= s.m_Emul3Fuzz;      // pixels away before emulation is cancelled
	m_Shared			= s.m_Shared;
	m_DeiconifyOnBell	= s.m_DeiconifyOnBell;
	m_DisableClipboard  = s.m_DisableClipboard;
	m_scaling			= s.m_scaling;
	m_FitWindow			= s.m_FitWindow;
	m_scale_num			= s.m_scale_num;
	m_scale_den			= s.m_scale_den;
	m_localCursor		= s.m_localCursor;
	m_hWindow			= s.m_hWindow;

	strcpy(m_kbdname, s.m_kbdname);
	m_kbdSpecified		= s.m_kbdSpecified;
	
	m_logLevel			= s.m_logLevel;
	m_logToConsole		= s.m_logToConsole;
	m_logToFile			= s.m_logToFile;
	strcpy(m_logFilename, s.m_logFilename);

	m_delay				= s.m_delay;
	m_connectionSpecified = s.m_connectionSpecified;
	m_configSpecified   = s.m_configSpecified;
	strcpy(m_configFilename, s.m_configFilename);

	m_restricted		= s.m_restricted;

	m_useCompressLevel		= s.m_useCompressLevel;
	m_compressLevel			= s.m_compressLevel;
	m_enableJpegCompression	= s.m_enableJpegCompression;
	m_jpegQualityLevel		= s.m_jpegQualityLevel;
	m_requestShapeUpdates	= s.m_requestShapeUpdates;
	m_ignoreShapeUpdates	= s.m_ignoreShapeUpdates;

#ifdef UNDER_CE
	m_palmpc			= s.m_palmpc;
	m_slowgdi			= s.m_slowgdi;
#endif
	return *this;
}

VNCOptions::~VNCOptions()
{
	
}

inline bool SwitchMatch(LPCTSTR arg, LPCTSTR swtch) {
	return (arg[0] == '-' || arg[0] == '/') &&
		(_tcsicmp(&arg[1], swtch) == 0);
}

static void ArgError(LPTSTR msg) {
    MessageBox(NULL,  msg, _T("Argument error"),MB_OK | MB_TOPMOST | MB_ICONSTOP);
}

// Greatest common denominator, by Euclid
int gcd(int a, int b) {
	if (a < b) return gcd(b, a);
	if (b == 0) return a;
	return gcd(b, a % b);
}

void VNCOptions::FixScaling() {
	if (m_scale_num < 1 || m_scale_den < 1) {
		MessageBox(NULL,  _T("Invalid scale factor - resetting to normal scale"), 
				_T("Argument error"),MB_OK | MB_TOPMOST | MB_ICONWARNING);
		m_scale_num = 1;
		m_scale_den = 1;	
		m_scaling = false;
	}
	int g = gcd(m_scale_num, m_scale_den);
	m_scale_num /= g;
	m_scale_den /= g;
}