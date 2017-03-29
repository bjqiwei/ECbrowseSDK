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

#include "VNCviewerApp32.h"
#include "vncviewer.h"
#include "Exception.h"
#include "VncViewerDllImpl.h"

VNCviewerApp32::VNCviewerApp32(HINSTANCE hInstance,CVncViewerDllImpl& rVncViewerDllImpl) 
: VNCviewerApp(hInstance)
, m_pViewer(NULL)
, m_rVncViewerDllImpl(rVncViewerDllImpl)
{
	// Load a requested keyboard layout
	if (m_options.m_kbdSpecified) {
		HKL hkl = LoadKeyboardLayout(  m_options.m_kbdname, 
			KLF_ACTIVATE | KLF_REPLACELANG | KLF_REORDER  );
		if (hkl == NULL) {
			MessageBox(NULL, _T("Error loading specified keyboard layout"), 
				_T("VNC info"), MB_OK | MB_ICONSTOP);
			exit(1);
		}
	}
}

VNCviewerApp32::~VNCviewerApp32() 
{
	if(m_pViewer)
	{
		ClientConnection* pOldViewer = m_pViewer;
		m_pViewer = NULL;
		pOldViewer->CloseClientConnection();
	}
}
	
bool VNCviewerApp32::CreateViewer(HWND parent)
{
	m_pViewer = new ClientConnection(this,parent);
	try 
	{
		m_pViewer->Run();
		return true;
	}catch (Exception &e) 
	{
		e.Report();
	}
	
	delete m_pViewer;
	m_pViewer = NULL;
	return false;
}

bool VNCviewerApp32::DeregisterConnection(ClientConnection *pConn)
{
	if(m_pViewer)
	{
		m_pViewer = NULL;
		m_rVncViewerDllImpl.m_rNotify.OnVncViewerClose();
	}
	return true;
}

void VNCviewerApp32::SendVncFrameData(const char* pData,int nDataLen)
{
	m_rVncViewerDllImpl.m_rNotify.SendVncFrameData(pData,nDataLen);
}

void VNCviewerApp32::OnWinVncFrameData(const char* pData,int nDataLen)
{
	if(m_pViewer)
	{
		m_pViewer->OnWinVncFrameData(pData,nDataLen);
	}
}

void VNCviewerApp32::RequestFullScreenUpdate(void)
{
	if(m_pViewer)
	{
		m_pViewer->RequestFullScreenUpdate();
	}
}