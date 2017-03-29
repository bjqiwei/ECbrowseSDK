//  Copyright (C) 2001-2006 Constantin Kaplinsky. All Rights Reserved.
//  Copyright (C) 2002 Vladimir Vologzhanin. All Rights Reserved.
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


// vncClient.cpp

// The per-client object.  This object takes care of all per-client stuff,
// such as socket input and buffering of updates.

// vncClient class handles the following functions:
// - Recieves requests from the connected client and
//   handles them
// - Handles incoming updates properly, using a vncBuffer
//   object to keep track of screen changes
// It uses a vncBuffer and is passed the vncDesktop and
// vncServer to communicate with.

// Includes
#include "stdhdrs.h"
#include <omnithread.h>
#include "resource.h"

// Custom
#include "vncClient.h"
#include "VSocket.h"
#include "vncDesktop.h"
#include "vncRegion.h"
#include "vncBuffer.h"
#include "vncService.h"
#include "vncKeymap.h"
#include "Windows.h"
extern "C" {
#include "d3des.h"
}

//#include <atlbase.h>

// vncClient thread class

class vncClientThread : public omni_thread
{
public:
	char * ConvertPath(char *path);

	// Init
	virtual BOOL Init(vncClient *client,
					  vncServer *server,
					  VSocket *socket);

	// The main thread function
	virtual void run(void *arg);

protected:
	virtual ~vncClientThread();

	// Fields
protected:
	VSocket *m_socket;
	vncServer *m_server;
	vncClient *m_client;
};

vncClientThread::~vncClientThread()
{
	// If we have a client object then delete it
	if (m_client != NULL)
		delete m_client;
}

BOOL
vncClientThread::Init(vncClient *client, vncServer *server, VSocket *socket)
{
	// Save the server pointer and window handle
	m_server = server;
	m_socket = socket;
	m_client = client;

	// Start the thread
	start();

	return TRUE;
}

void
ClearKeyState(BYTE key)
{
	// This routine is used by the VNC client handler to clear the
	// CAPSLOCK, NUMLOCK and SCROLL-LOCK states.

	BYTE keyState[256];

	GetKeyboardState((LPBYTE)&keyState);

	if(keyState[key] & 1)
	{
		// Simulate the key being pressed
		keybd_event(key, 0, KEYEVENTF_EXTENDEDKEY, 0);

		// Simulate it being release
		keybd_event(key, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	}
}

void
vncClientThread::run(void *arg)
{
	// All this thread does is go into a socket-recieve loop,
	// waiting for stuff on the given socket

	// IMPORTANT : ALWAYS call RemoveClient on the server before quitting
	// this thread.

	// Save the handle to the thread's original desktop
	HDESK home_desktop = GetThreadDesktop(GetCurrentThreadId());

	// LOCK INITIAL SETUP
	// All clients have the m_protocol_ready flag set to FALSE initially, to prevent
	// updates and suchlike interfering with the initial protocol negotiations.

/*	m_client->m_buffer->SetQualityLevel(6);
	m_client->m_buffer->SetCompressLevel(6);
	m_client->m_buffer->EnableXCursor(TRUE);
	m_client->m_buffer->EnableRichCursor(TRUE);
	m_client->m_buffer->EnableLastRect(TRUE);

	m_client->m_use_PointerPos = TRUE;
	m_client->m_use_NewFBSize = TRUE;

	m_client->m_cursor_update_pending = FALSE;
	m_client->m_cursor_update_sent = FALSE;
	m_client->m_cursor_pos_changed = FALSE;

	// Client wants us to use CopyRect
	m_client->m_copyrect_use = TRUE;*/

	// Authenticated OK - remove from blacklist and remove timeout
	vnclog.Print(LL_INTINFO, VNCLOG("authenticated connection\n"));

	// INIT PIXEL FORMAT

	// Get the screen format
	m_client->m_fullscreen = m_client->m_buffer->GetSize();

	// UNLOCK INITIAL SETUP
	// Initial negotiation is complete, so set the protocol ready flag
	{
		omni_mutex_lock l(m_client->m_regionLock);
		m_client->m_protocol_ready = TRUE;
	}

	// Clear the CapsLock and NumLock keys
	if (m_client->IsKeyboardEnabled())
	{
		ClearKeyState(VK_CAPITAL);
		// *** JNW - removed because people complain it's wrong
		//ClearKeyState(VK_NUMLOCK);
		ClearKeyState(VK_SCROLL);
	}

	// MAIN LOOP

	BOOL connected = TRUE;
	while (connected)
	{
		rfbClientToServerMsg msg;

		// Ensure that we're running in the correct desktop
		if (!vncService::InputDesktopSelected())
			if (!vncService::SelectDesktop(NULL))
				break;

		// Try to read a message ID
		if (!m_socket->ReadExact((char *)&msg.type, sizeof(msg.type)))
		{
			connected = FALSE;
			break;
		}

		// What to do is determined by the message id
		switch(msg.type)
		{

		case rfbSetPixelFormat:
			// Read the rest of the message:
			if (!m_socket->ReadExact(((char *) &msg)+1, sz_rfbSetPixelFormatMsg-1))
			{
				connected = FALSE;
				break;
			}

			// Swap the relevant bits.
			msg.spf.format.redMax = Swap16IfLE(msg.spf.format.redMax);
			msg.spf.format.greenMax = Swap16IfLE(msg.spf.format.greenMax);
			msg.spf.format.blueMax = Swap16IfLE(msg.spf.format.blueMax);

			{
				omni_mutex_lock l(m_client->m_regionLock);

				// Tell the buffer object of the change
				if (!m_client->m_buffer->SetClientFormat(msg.spf.format))
				{
					vnclog.Print(LL_CONNERR, VNCLOG("remote pixel format invalid\n"));

					connected = FALSE;
				}

				// Set the palette-changed flag, just in case...
				m_client->m_palettechanged = TRUE;
			}
			break;

		case rfbSetEncodings:
			// Read the rest of the message:
			if (!m_socket->ReadExact(((char *) &msg)+1, sz_rfbSetEncodingsMsg-1))
			{
				connected = FALSE;
				break;
			}

			m_client->m_buffer->SetQualityLevel(-1);
			m_client->m_buffer->SetCompressLevel(6);
			m_client->m_buffer->EnableXCursor(FALSE);
			m_client->m_buffer->EnableRichCursor(FALSE);
			m_client->m_buffer->EnableLastRect(FALSE);
			m_client->m_use_PointerPos = FALSE;
			m_client->m_use_NewFBSize = FALSE;

			m_client->m_cursor_update_pending = FALSE;
			m_client->m_cursor_update_sent = FALSE;
			m_client->m_cursor_pos_changed = FALSE;

			// Read in the preferred encodings
			msg.se.nEncodings = Swap16IfLE(msg.se.nEncodings);
			{
				int x;
				BOOL encoding_set = FALSE;
				BOOL shapeupdates_requested = FALSE;
				BOOL pointerpos_requested = FALSE;

				{
					omni_mutex_lock l(m_client->m_regionLock);
					// By default, don't use copyrect!
					m_client->m_copyrect_use = FALSE;
				}

				for (x = 0; x < msg.se.nEncodings; x++)
				{
					omni_mutex_lock l(m_client->m_regionLock);
					CARD32 encoding;

					// Read an encoding in
					if (!m_socket->ReadExact((char *)&encoding, sizeof(encoding)))
					{
						connected = FALSE;
						break;
					}

					// Is this the CopyRect encoding (a special case)?
					if (Swap32IfLE(encoding) == rfbEncodingCopyRect)
					{
						// Client wants us to use CopyRect
						m_client->m_copyrect_use = TRUE;
						continue;
					}

					// Is this an XCursor encoding request?
					if (Swap32IfLE(encoding) == rfbEncodingXCursor) {
						m_client->m_buffer->EnableXCursor(TRUE);
						shapeupdates_requested = TRUE;
						vnclog.Print(LL_INTINFO, VNCLOG("X-style cursor shape updates enabled\n"));
						continue;
					}

					// Is this a RichCursor encoding request?
					if (Swap32IfLE(encoding) == rfbEncodingRichCursor) {
						m_client->m_buffer->EnableRichCursor(TRUE);
						shapeupdates_requested = TRUE;
						vnclog.Print(LL_INTINFO, VNCLOG("Full-color cursor shape updates enabled\n"));
						continue;
					}

					// Is this a CompressLevel encoding?
					if ((Swap32IfLE(encoding) >= rfbEncodingCompressLevel0) &&
						(Swap32IfLE(encoding) <= rfbEncodingCompressLevel9))
					{
						// Client specified encoding-specific compression level
						int level = (int)(Swap32IfLE(encoding) - rfbEncodingCompressLevel0);
						m_client->m_buffer->SetCompressLevel(level);
						vnclog.Print(LL_INTINFO, VNCLOG("compression level requested: %d\n"), level);
						continue;
					}

					// Is this a QualityLevel encoding?
					if ((Swap32IfLE(encoding) >= rfbEncodingQualityLevel0) &&
						(Swap32IfLE(encoding) <= rfbEncodingQualityLevel9))
					{
						// Client specified image quality level used for JPEG compression
						int level = (int)(Swap32IfLE(encoding) - rfbEncodingQualityLevel0);
						m_client->m_buffer->SetQualityLevel(level);
						vnclog.Print(LL_INTINFO, VNCLOG("image quality level requested: %d\n"), level);
						continue;
					}

					// Is this a PointerPos encoding request?
					if (Swap32IfLE(encoding) == rfbEncodingPointerPos) {
						pointerpos_requested = TRUE;
						continue;
					}

					// Is this a LastRect encoding request?
					if (Swap32IfLE(encoding) == rfbEncodingLastRect) {
						m_client->m_buffer->EnableLastRect(TRUE);
						vnclog.Print(LL_INTINFO, VNCLOG("LastRect protocol extension enabled\n"));
						continue;
					}

					// Is this a NewFBSize encoding request?
					if (Swap32IfLE(encoding) == rfbEncodingNewFBSize) {
						m_client->m_use_NewFBSize = TRUE;
						vnclog.Print(LL_INTINFO, VNCLOG("NewFBSize protocol extension enabled\n"));
						continue;
					}

					// Have we already found a suitable encoding?
					if (!encoding_set)
					{
						// omni_mutex_lock l(m_client->m_regionLock);

						// No, so try the buffer to see if this encoding will work...
						if (m_client->m_buffer->SetEncoding(Swap32IfLE(encoding))) {
							encoding_set = TRUE;
						}

					}
				}

				// Enable CursorPos encoding only if cursor shape updates were
				// requested by the client.
				if (shapeupdates_requested && pointerpos_requested) {
					m_client->m_use_PointerPos = TRUE;
					m_client->SetCursorPosChanged();
					vnclog.Print(LL_INTINFO, VNCLOG("PointerPos protocol extension enabled\n"));
				}

				// If no encoding worked then default to RAW!
				// FIXME: Protocol extensions won't work in this case.
				if (!encoding_set)
				{
					omni_mutex_lock l(m_client->m_regionLock);

					vnclog.Print(LL_INTINFO, VNCLOG("defaulting to raw encoder\n"));

					if (!m_client->m_buffer->SetEncoding(Swap32IfLE(rfbEncodingRaw)))
					{
						vnclog.Print(LL_INTERR, VNCLOG("failed to select raw encoder!\n"));

						connected = FALSE;
					}
				}
			}

			break;

		case rfbFramebufferUpdateRequest:
			// Read the rest of the message:
			if (!m_socket->ReadExact(((char *) &msg)+1, sz_rfbFramebufferUpdateRequestMsg-1))
			{
				connected = FALSE;
				break;
			}
			{
				RECT update;
				RECT sharedRect;
				{
					omni_mutex_lock l(m_client->m_regionLock);

					sharedRect = m_server->GetSharedRect();
					// Get the specified rectangle as the region to send updates for.
					update.left = Swap16IfLE(msg.fur.x)+ sharedRect.left;
					update.top = Swap16IfLE(msg.fur.y)+ sharedRect.top;
					update.right = update.left + Swap16IfLE(msg.fur.w);

					_ASSERTE(Swap16IfLE(msg.fur.x) >= 0);
					_ASSERTE(Swap16IfLE(msg.fur.y) >= 0);

					//if (update.right > m_client->m_fullscreen.right)
					//	update.right = m_client->m_fullscreen.right;
					if (update.right > sharedRect.right)
						update.right = sharedRect.right;
					if (update.left < sharedRect.left)
						update.left = sharedRect.left;

					update.bottom = update.top + Swap16IfLE(msg.fur.h);
					//if (update.bottom > m_client->m_fullscreen.bottom)
					//	update.bottom = m_client->m_fullscreen.bottom;
					if (update.bottom > sharedRect.bottom)
						update.bottom = sharedRect.bottom;
					if (update.top < sharedRect.top)
						update.top = sharedRect.top;

					// Set the update-wanted flag to true
					m_client->m_updatewanted = TRUE;

					// Clip the rectangle to the screen
					if (IntersectRect(&update, &update, &sharedRect))
					{
						// Is this request for an incremental region?
						//if (msg.fur.incremental)
						{
							// Yes, so add it to the incremental region
							m_client->m_incr_rgn.AddRect(update);
						}
						//else
						//{
						//	// No, so add it to the full update region
						//	m_client->m_full_rgn.AddRect(update);

						//	// Disable any pending CopyRect
						//	m_client->m_copyrect_set = FALSE;
						//}
					}
					// Trigger an update
					m_server->RequestUpdate();
				}
			}
			break;

		case rfbKeyEvent:
			// Read the rest of the message:
			if (m_socket->ReadExact(((char *) &msg)+1, sz_rfbKeyEventMsg-1))
			{
				if (m_client->IsKeyboardEnabled() && !m_client->IsInputBlocked())
				{
					msg.ke.key = Swap32IfLE(msg.ke.key);
					// Get the keymapper to do the work
					vncKeymap::keyEvent(msg.ke.key, msg.ke.down != 0,
						m_client->m_server);
					m_client->m_remoteevent = TRUE;
				}
			}
			break;

		case rfbPointerEvent:
			// Read the rest of the message:
			if (m_socket->ReadExact(((char *) &msg)+1, sz_rfbPointerEventMsg-1))
			{
				if (m_client->IsPointerEnabled() && !m_client->IsInputBlocked())
				{
					// Convert the coords to Big Endian
					msg.pe.x = Swap16IfLE(msg.pe.x);
					msg.pe.y = Swap16IfLE(msg.pe.y);

					// Remember cursor position for this client
					m_client->m_cursor_pos.x = msg.pe.x;
					m_client->m_cursor_pos.y = msg.pe.y;

					// if we share only one window...

					RECT coord;
					{
						omni_mutex_lock l(m_client->m_regionLock);

						coord = m_server->GetSharedRect();
					}

					// to put position relative to screen
					msg.pe.x = (CARD16)(msg.pe.x + coord.left);
					msg.pe.y = (CARD16)(msg.pe.y + coord.top);

					// Work out the flags for this event
					DWORD flags = MOUSEEVENTF_ABSOLUTE;
					flags |= MOUSEEVENTF_MOVE;
					m_server->SetMouseCounter(1, m_client->m_cursor_pos, false );

					if ( (msg.pe.buttonMask & rfbButton1Mask) != 
						(m_client->m_ptrevent.buttonMask & rfbButton1Mask) )
					{
						if (GetSystemMetrics(SM_SWAPBUTTON))
							flags |= (msg.pe.buttonMask & rfbButton1Mask) 
							? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
						else
							flags |= (msg.pe.buttonMask & rfbButton1Mask) 
							? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
						m_server->SetMouseCounter(1, m_client->m_cursor_pos, false);
					}
					if ( (msg.pe.buttonMask & rfbButton2Mask) != 
						(m_client->m_ptrevent.buttonMask & rfbButton2Mask) )
					{
						flags |= (msg.pe.buttonMask & rfbButton2Mask) 
							? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP;
						m_server->SetMouseCounter(1, m_client->m_cursor_pos, false);
					}
					if ( (msg.pe.buttonMask & rfbButton3Mask) != 
						(m_client->m_ptrevent.buttonMask & rfbButton3Mask) )
					{
						if (GetSystemMetrics(SM_SWAPBUTTON))
							flags |= (msg.pe.buttonMask & rfbButton3Mask) 
							? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
						else
							flags |= (msg.pe.buttonMask & rfbButton3Mask) 
							? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
						m_server->SetMouseCounter(1, m_client->m_cursor_pos, false);
					}

					// Treat buttons 4 and 5 presses as mouse wheel events
					DWORD wheel_movement = 0;
					if ((msg.pe.buttonMask & rfbButton4Mask) != 0 &&
						(m_client->m_ptrevent.buttonMask & rfbButton4Mask) == 0)
					{
						flags |= MOUSEEVENTF_WHEEL;
						wheel_movement = (DWORD)+120;
					}
					else if ((msg.pe.buttonMask & rfbButton5Mask) != 0 &&
						(m_client->m_ptrevent.buttonMask & rfbButton5Mask) == 0)
					{
						flags |= MOUSEEVENTF_WHEEL;
						wheel_movement = (DWORD)-120;
					}

					// Generate coordinate values
// PRB: should it be really only primary rect?
					HWND temp = GetDesktopWindow();
					GetWindowRect(temp,&coord);

					unsigned long x = (msg.pe.x * 65535) / (coord.right - coord.left - 1);
					unsigned long y = (msg.pe.y * 65535) / (coord.bottom - coord.top - 1);

					// Do the pointer event
					::mouse_event(flags, (DWORD)x, (DWORD)y, wheel_movement, 0);
					// Save the old position
					m_client->m_ptrevent = msg.pe;

					// Flag that a remote event occurred
					m_client->m_remoteevent = TRUE;
					m_client->m_pointer_event_time = time(NULL);

					// Flag that the mouse moved
					// FIXME: It should not set m_cursor_pos_changed here.
					m_client->UpdateMouse();

					// Trigger an update
					m_server->RequestUpdate();
				}
			}
			break;

		case rfbClientCutText:
			// Read the rest of the message:
			if (m_socket->ReadExact(((char *) &msg)+1, sz_rfbClientCutTextMsg-1))
			{
				// Allocate storage for the text
				const UINT length = Swap32IfLE(msg.cct.length);
				char *text = new char [length+1];
				if (text == NULL)
					break;
				text[length] = 0;

				// Read in the text
				if (!m_socket->ReadExact(text, length)) {
					delete [] text;
					break;
				}

				// Get the server to update the local clipboard
				if (m_client->IsKeyboardEnabled() && m_client->IsPointerEnabled())
					m_server->UpdateLocalClipText(text);

				// Free the clip text we read
				delete [] text;
			}
			break;

		default:
			// Unknown message, so fail!
			vnclog.Print(LL_CLIENTS, VNCLOG("invalid message received : %d\n"),
						 (int)msg.type);
			connected = FALSE;
		}
	}

	// Move into the thread's original desktop
	vncService::SelectHDESK(home_desktop);

	// Quit this thread.  This will automatically delete the thread and the
	// associated client.

	// Remove the client from the server, just in case!
	m_server->RemoveClient();
}

// The vncClient itself

vncClient::vncClient()
{
	vnclog.Print(LL_INTINFO, VNCLOG("vncClient() executing...\n"));

	m_socket = NULL;
	m_buffer = NULL;

	m_keyboardenabled = FALSE;
	m_pointerenabled = FALSE;
	m_inputblocked = FALSE;

	m_copyrect_use = FALSE;

	m_mousemoved = FALSE;
	m_ptrevent.buttonMask = 0;
	m_ptrevent.x = 0;
	m_ptrevent.y = 0;

	m_cursor_update_pending = FALSE;
	m_cursor_update_sent = FALSE;
	m_cursor_pos_changed = FALSE;
	m_pointer_event_time = (time_t)0;
	m_cursor_pos.x = -1;
	m_cursor_pos.y = -1;

	m_thread = NULL;
	m_updatewanted = FALSE;

	m_palettechanged = FALSE;

	m_copyrect_set = FALSE;

	m_remoteevent = FALSE;

	// IMPORTANT: Initially, client is not protocol-ready.
	m_protocol_ready = FALSE;
	m_fb_size_changed = FALSE;

	m_use_NewFBSize = FALSE;
	m_bRecordScreen = FALSE;

}

vncClient::~vncClient()
{
	vnclog.Print(LL_INTINFO, VNCLOG("~vncClient() executing...\n"));

	// If we have a socket then kill it
	if (m_socket != NULL)
	{
		vnclog.Print(LL_INTINFO, VNCLOG("deleting socket\n"));

		delete m_socket;
		m_socket = NULL;
	}

	// Kill the screen buffer
	if (m_buffer != NULL)
	{
		vnclog.Print(LL_INTINFO, VNCLOG("deleting buffer\n"));

		delete m_buffer;
		m_buffer = NULL;
	}
}

// Init
BOOL
vncClient::Init(vncServer *server,
				VSocket *socket)
{
	// Save the server id;
	m_server = server;

	// Save the socket
	m_socket = socket;
	if (m_buffer)
	{
		//m_buffer->SetQualityLevel(6);
		m_buffer->SetQualityLevel(3);
		m_buffer->SetCompressLevel(9);
		m_buffer->EnableXCursor(TRUE);
		m_buffer->EnableRichCursor(TRUE);
		m_buffer->EnableLastRect(TRUE);
	}
	
	m_use_PointerPos = TRUE;
	m_use_NewFBSize = TRUE;
	
	m_cursor_update_pending = FALSE;
	m_cursor_update_sent = FALSE;
	m_cursor_pos_changed = FALSE;
	
	m_copyrect_use = TRUE;

	// Spawn the child thread here
	m_thread = new vncClientThread;
	if (m_thread == NULL)
		return FALSE;
	return ((vncClientThread *)m_thread)->Init(this, m_server, m_socket);

	return FALSE;
}

void
vncClient::Kill()
{
	if (m_socket != NULL)
		m_socket->Close();
}

// Client manipulation functions for use by the server
void
vncClient::SetBuffer(vncBuffer *buffer)
{
	// Until authenticated, the client object has no access
	// to the screen buffer.  This means that there only need
	// be a buffer when there's at least one authenticated client.
	m_buffer = buffer;
}


void
vncClient::TriggerUpdate()
{
	// Lock the updates stored so far
	omni_mutex_lock l(m_regionLock);
	if (!m_protocol_ready)
		return;

	if (IsUpdateWant())
	{
		// Check if cursor shape update has to be sent
		m_cursor_update_pending = m_buffer->IsCursorUpdatePending();

		// Send an update if one is waiting
		if (!m_changed_rgn.IsEmpty() ||
			!m_full_rgn.IsEmpty() ||
			m_copyrect_set ||
			m_cursor_update_pending ||
			m_cursor_pos_changed ||
			(m_mousemoved && !m_use_PointerPos))
		{
			// Has the palette changed?
			if (m_palettechanged)
			{
				m_palettechanged = FALSE;
				//coder
				//if (!SendPalette())
				//	return;
			}

			// Now send the update
			m_updatewanted = !SendUpdate();
		}
	}
}

void
vncClient::UpdateMouse()
{
	if (!m_mousemoved && !m_cursor_update_sent)	{
		omni_mutex_lock l(m_regionLock);

		if (IntersectRect(&m_oldmousepos, &m_oldmousepos, &m_server->GetSharedRect()))
			m_changed_rgn.AddRect(m_oldmousepos);

		m_mousemoved = TRUE;
	} else if (m_use_PointerPos) {
		omni_mutex_lock l(m_regionLock);

		SetCursorPosChanged();
	}
}

void
vncClient::UpdateRect(RECT &rect)
{
	// Add the rectangle to the update region
	if (IsRectEmpty(&rect))
		return;

	omni_mutex_lock l(m_regionLock);

	if (IntersectRect(&rect, &rect, &m_server->GetSharedRect()))
		m_changed_rgn.AddRect(rect);
}

void
vncClient::UpdateRegion(vncRegion &region)
{
	// Merge our current update region with the supplied one
	if (region.IsEmpty())
		return;

	{
		omni_mutex_lock l(m_regionLock);

		// Merge the two
		vncRegion dummy;
		dummy.AddRect(m_server->GetSharedRect());
		region.Intersect(dummy);

		m_changed_rgn.Combine(region);
	}
}

void
vncClient::CopyRect(RECT &dest, POINT &source)
{
	// If copyrect is disabled then just redraw the region!
	if (!m_copyrect_use)
	{
		UpdateRect(dest);
		return;
	}

	{
		omni_mutex_lock l(m_regionLock);

		// Clip the destination to the screen
		RECT destrect;
		if (!IntersectRect(&destrect, &dest, &m_server->GetSharedRect()))
			return;

		// Adjust the source correspondingly
		source.x = source.x + (destrect.left - dest.left);
		source.y = source.y + (destrect.top - dest.top);

		// Work out the source rectangle
		RECT srcrect;

		// Is this a continuation of an earlier window drag?
		if (m_copyrect_set &&
			((source.x == m_copyrect_rect.left) && (source.y == m_copyrect_rect.top)))
		{
			// Yes, so use the old source position
			srcrect.left = m_copyrect_src.x;
			srcrect.top = m_copyrect_src.y;
		}
		else
		{
			// No, so use this source position
			srcrect.left = source.x;
			srcrect.top = source.y;
		}

		// And fill out the right & bottom using the dest rect
		srcrect.right = destrect.right-destrect.left + srcrect.left;
		srcrect.bottom = destrect.bottom-destrect.top + srcrect.top;

		// Clip the source to the screen
		RECT srcrect2;
		if (!IntersectRect(&srcrect2, &srcrect, &m_server->GetSharedRect()))
			return;

		// Correct the destination rectangle
		destrect.left += (srcrect2.left - srcrect.left);
		destrect.top += (srcrect2.top - srcrect.top);
		destrect.right = srcrect2.right-srcrect2.left + destrect.left;
		destrect.bottom = srcrect2.bottom-srcrect2.top + destrect.top;

		// Is there an existing CopyRect rectangle?
		if (m_copyrect_set)
		{
			// Yes, so compare their areas!
			if (((destrect.right-destrect.left) * (destrect.bottom-destrect.top))
				< ((m_copyrect_rect.right-m_copyrect_rect.left) * (m_copyrect_rect.bottom-m_copyrect_rect.top)))
				return;
		}

		// Set the copyrect...
		m_copyrect_rect = destrect;
		m_copyrect_src.x = srcrect2.left;
		m_copyrect_src.y = srcrect2.top;

		m_copyrect_set = TRUE;
	}
}

void
vncClient::UpdateClipText(LPSTR text)
{
	if (!m_protocol_ready) return;

	// Don't send the clipboard contents to a view-only client
	if (!IsKeyboardEnabled() || !IsPointerEnabled())
		return;

	// Lock out any update sends and send clip text to the client
	omni_mutex_lock l(m_sendUpdateLock);

	rfbServerCutTextMsg message;
	message.length = Swap32IfLE(strlen(text));
	if (!SendRFBMsg(rfbServerCutText, (BYTE *) &message, sizeof(message)))
	{
		Kill();
		return;
	}
	if (!m_socket->SendQueued(text, strlen(text)))
	{
		Kill();
		return;
	}

	//coder
	m_server->BrocastUpdate((char*)m_socket->data(),m_socket->length());
	m_socket->clearAndZero();
}

void
vncClient::UpdatePalette()
{
	omni_mutex_lock l(m_regionLock);

	m_palettechanged = TRUE;
}

// Internal methods
BOOL
vncClient::SendRFBMsg(CARD8 type, BYTE *buffer, int buflen)
{
	// Set the message type
	((rfbServerToClientMsg *)buffer)->type = type;

	// Send the message
	if (!m_socket->SendQueued((char *) buffer, buflen))
	{
		vnclog.Print(LL_CONNERR, VNCLOG("failed to send RFB message to client\n"));

		Kill();
		return FALSE;
	}
	return TRUE;
}


BOOL vncClient::SendUpdate()
{
#ifndef _DEBUG
	try
	{
#endif
		// First, check if we need to send pending NewFBSize message
		if (m_use_NewFBSize && m_fb_size_changed) {
			SetNewFBSize(TRUE);
			return TRUE;
		}

		vncRegion toBeSent;			// Region to actually be sent
		rectlist toBeSentList;		// List of rectangles to actually send
		vncRegion toBeDone;			// Region to check

		// Prepare to send cursor position update if necessary
		if (m_cursor_pos_changed) {
			POINT cursor_pos;
			if (!GetCursorPos(&cursor_pos)) {
				cursor_pos.x = 0;
				cursor_pos.y = 0;
			}
			RECT shared_rect = m_server->GetSharedRect();
			cursor_pos.x -= shared_rect.left;
			cursor_pos.y -= shared_rect.top;
			if (cursor_pos.x < 0) {
				cursor_pos.x = 0;
			} else if (cursor_pos.x >= shared_rect.right - shared_rect.left) {
				cursor_pos.x = shared_rect.right - shared_rect.left - 1;
			}
			if (cursor_pos.y < 0) {
				cursor_pos.y = 0;
			} else if (cursor_pos.y >= shared_rect.bottom - shared_rect.top) {
				cursor_pos.y = shared_rect.bottom - shared_rect.top - 1;
			}
			if (cursor_pos.x == m_cursor_pos.x && cursor_pos.y == m_cursor_pos.y) {
				m_cursor_pos_changed = FALSE;
			} else {
				m_cursor_pos.x = cursor_pos.x;
				m_cursor_pos.y = cursor_pos.y;
			}
		}

		toBeSent.Clear();
		if(!m_full_rgn.IsEmpty()) 
		{
			m_incr_rgn.Clear();
			m_copyrect_set = false;
			toBeSent.Combine(m_full_rgn);
			m_changed_rgn.Clear();
			m_full_rgn.Clear();
		} 
		else 
		{
			if (IncrRgnRequested()) 
			{
				if (!m_changed_rgn.IsEmpty()) 
				{
					// Get region to send from vncDesktop
					toBeSent.Combine(m_changed_rgn);

					// Mouse stuff for the case when cursor shape updates are off
					if (!m_cursor_update_sent && !m_cursor_update_pending) 
					{
						// If the mouse hasn't moved, see if its position is in the rect
						// we're sending. If so, make sure the full mouse rect is sent.
						if (!m_mousemoved) 
						{
							vncRegion tmpMouseRgn;
							tmpMouseRgn.AddRect(m_oldmousepos);
							tmpMouseRgn.Intersect(toBeSent);
							if (!tmpMouseRgn.IsEmpty()) 
								m_mousemoved = TRUE;
						}
						// If the mouse has moved (or otherwise needs an update):
						if (m_mousemoved) 
						{
							// Include an update for its previous position
							if (IntersectRect(&m_oldmousepos, &m_oldmousepos, &m_server->GetSharedRect())) 
								toBeSent.AddRect(m_oldmousepos);
							// Update the cached mouse position
							m_oldmousepos = m_buffer->GrabMouse();
							// Include an update for its current position
							if (IntersectRect(&m_oldmousepos, &m_oldmousepos, &m_server->GetSharedRect())) 
								toBeSent.AddRect(m_oldmousepos);
							// Indicate the move has been handled
							m_mousemoved = FALSE;
						}
					}
					m_changed_rgn.Clear();
				}
			}
		}

		// Get the list of changed rectangles!
		int numrects = 0;
		if (toBeSent.Rectangles(toBeSentList))
		{
			// Find out how many rectangles this update will contain
			rectlist::iterator i;
			int numsubrects;
			for (i=toBeSentList.begin(); i != toBeSentList.end(); i++)
			{
				numsubrects = m_buffer->GetNumCodedRects(*i);

				// Skip remaining rectangles if an encoder will use LastRect extension.
				if (numsubrects == 0) {
					numrects = 0xFFFF;
					break;
				}
				numrects += numsubrects;
			}
		}

		if (numrects != 0xFFFF) {
			// Count cursor shape and cursor position updates.
			if (m_cursor_update_pending)
				numrects++;
			if (m_cursor_pos_changed)
				numrects++;
			// Handle the copyrect region
			if (m_copyrect_set)
				numrects++;
			// If there are no rectangles then return
			if (numrects != 0)
				m_incr_rgn.Clear();
			else
				return FALSE;
		}

		omni_mutex_lock l(m_sendUpdateLock);

		if (m_bRecordScreen)
		{
	        if(m_buffer)
			   m_server->OnScreenData((char*)m_buffer->GetMainBuffer(),m_buffer->GetMainBufferSize());
			return TRUE;
		}
		// Otherwise, send <number of rectangles> header
		//rfbFramebufferUpdateMsg header;
		//header.nRects = Swap16IfLE(numrects);
		//if (!SendRFBMsg(rfbFramebufferUpdate, (BYTE *) &header, sz_rfbFramebufferUpdateMsg))
		//	return TRUE;

		// Send mouse cursor shape update
		if (m_cursor_update_pending) {
			if (!SendCursorShapeUpdate())
			{
				m_socket->clearAndZero();
				return TRUE;
			}
			//coder
			m_server->BrocastUpdate((char*)m_socket->data(),m_socket->length());
			m_socket->clearAndZero();
		}

		// Send cursor position update
		if (m_cursor_pos_changed) {
			if (!SendCursorPosUpdate())
			{
				m_socket->clearAndZero();
				return TRUE;
			}
			//coder
			m_server->BrocastUpdate((char*)m_socket->data(),m_socket->length());
			m_socket->clearAndZero();
		}

		// Encode & send the copyrect
		if (m_copyrect_set) {
			m_copyrect_set = FALSE;
			if(!SendCopyRect(m_copyrect_rect, m_copyrect_src))
			{
				m_socket->clearAndZero();
				return TRUE;
			}
			//coder
			m_server->BrocastUpdate((char*)m_socket->data(),m_socket->length());
			m_socket->clearAndZero();
		}

		// Encode & send the actual rectangles
		if (!SendRectangles(toBeSentList))
			return TRUE;

		// Send LastRect marker if needed.
		//if (numrects == 0xFFFF) 
		{
			if (!SendLastRect())
			{
				m_socket->clearAndZero();
				return TRUE;
			}

			//coder
			m_server->BrocastUpdate((char*)m_socket->data(),m_socket->length());
			m_socket->clearAndZero();
		}

		// Both lists should be empty when we exit
		_ASSERT(toBeSentList.empty());
#ifndef _DEBUG
	}
	catch (...)
	{
		vnclog.Print(LL_INTERR, VNCLOG("vncClient::SendUpdate caught an exception.\n"));
		throw;
	}
#endif

	return TRUE;
}


void vncClient::get_slicerects(const RECT &inr,rectlist& rects) 
{
	rects.clear();
	int maxS= 1024*768;

	int inArea=(inr.right-inr.left)*(inr.bottom-inr.top);
	if (inArea <= maxS) 
	{
		rects.push_front(inr);
		return ;
	}

	int numb = inArea/maxS;
	numb += inArea%maxS?1:0;
	
	int top = 0;
	int left = 0;
	int bottom = 0;
	int right = 0;

	int dy = (inr.bottom-inr.top)/numb;
	dy += (inr.bottom-inr.top)%numb?1:0;

	for (int yy=0; yy <numb; yy++ )
	{
		top = inr.top + yy*dy;
		bottom = top + dy;
		if (bottom > inr.bottom)
			bottom = inr.bottom;
		RECT r;
		r.left = inr.left;
		r.top = top;
		r.right = inr.right;
		r.bottom = bottom;
		rects.push_front(r);
	}
}
// Send a set of rectangles
BOOL
vncClient::SendRectangles(rectlist &rects)
{
	RECT rect;
	//coder
	//int i = 0;
	// Work through the list of rectangles, sending each one
	char buffer[300];
	int t1 = GetTickCount();
	sprintf(buffer,"SendRectangles begin t1=%d \n",t1);
	OutputDebugString(buffer);

	while(!rects.empty())
	{
		rect = rects.front();
		vnclog.Print(LL_CLIENTS, VNCLOG("SendRectangles %d,%d,%d,%d \n"),rect.left,rect.top,rect.right,rect.bottom);

		RECT rect2;
		rectlist SentList;	
		get_slicerects(rect,SentList);

		sprintf(buffer,"SendRectangles %d,%d,%d,%d  \n",rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
		OutputDebugString(buffer);

	/*	while(!SentList.empty())
		{
			rect2 = SentList.front();
			if (!SendRectangle(rect2))
			{
				m_socket->clearAndZero();
				return FALSE;
			}
			else
			{
				//coder
				m_server->BrocastUpdate((char*)m_socket->data(),m_socket->length());
				sprintf(buffer,"SendRectangles1111111111 %d,%d,%d,%d len:%d \n",rect2.left,rect2.top,rect2.right-rect2.left,rect2.bottom-rect2.top,m_socket->length());
				OutputDebugString(buffer);

				m_socket->clearAndZero();				
			}
			
			SentList.pop_front();
		}*/


		if (!SendRectangle(rect))
		{
			m_socket->clearAndZero();
			return FALSE;
		}
		else
		{
			//coder
			m_server->BrocastUpdate((char*)m_socket->data(),m_socket->length());
			//sprintf(buffer,"SendRectangles1111111111 %d,%d,%d,%d len:%d \n",rect2.left,rect2.top,rect2.right-rect2.left,rect2.bottom-rect2.top,m_socket->length());
			OutputDebugString(buffer);
			m_socket->clearAndZero();
		}

		rects.pop_front();
	}
	rects.clear();
	sprintf(buffer,"SendRectangles end dt=%d  \n",GetTickCount()-t1);
	OutputDebugString(buffer);

	return TRUE;
}

// Tell the encoder to send a single rectangle
BOOL vncClient::SendRectangle(RECT &rect)
{
	RECT sharedRect;
	{
		omni_mutex_lock l(m_regionLock);
		sharedRect = m_server->GetSharedRect();
	}

	IntersectRect(&rect, &rect, &sharedRect);
	// Get the buffer to encode the rectangle
	UINT bytes = m_buffer->TranslateRect(
		rect,
		m_socket,
		sharedRect.left,
		sharedRect.top);

    // Send the encoded data
    m_socket->SendQueued((char *)(m_buffer->GetClientBuffer()), bytes);
	return TRUE;
}

// Send a single CopyRect message
BOOL vncClient::SendCopyRect(RECT &dest, POINT &source)
{
	RECT rc_shr = m_server->GetSharedRect();

	// Create the message header
	rfbFramebufferUpdateRectHeader copyrecthdr;
	copyrecthdr.r.x = Swap16IfLE(dest.left - rc_shr.left);
	copyrecthdr.r.y = Swap16IfLE(dest.top - rc_shr.top);

	copyrecthdr.r.w = Swap16IfLE(dest.right-dest.left);
	copyrecthdr.r.h = Swap16IfLE(dest.bottom-dest.top);
	copyrecthdr.encoding = Swap32IfLE(rfbEncodingCopyRect);

	// Create the CopyRect-specific section
	rfbCopyRect copyrectbody;
	copyrectbody.srcX = Swap16IfLE(source.x - rc_shr.left);
	copyrectbody.srcY = Swap16IfLE(source.y - rc_shr.top);

	// Now send the message;
	if (!m_socket->SendQueued((char *)&copyrecthdr, sizeof(copyrecthdr)))
		return FALSE;
	if (!m_socket->SendQueued((char *)&copyrectbody, sizeof(copyrectbody)))
		return FALSE;

	return TRUE;
}

// Send LastRect marker indicating that there are no more rectangles to send
BOOL
vncClient::SendLastRect()
{
	// Create the message header
	rfbFramebufferUpdateRectHeader hdr;
	hdr.r.x = 0;
	hdr.r.y = 0;
	hdr.r.w = 0;
	hdr.r.h = 0;
	hdr.encoding = Swap32IfLE(rfbEncodingLastRect);

	// Now send the message;
	if (!m_socket->SendQueued((char *)&hdr, sizeof(hdr)))
		return FALSE;

	return TRUE;
}

// Send the encoder-generated palette to the client
// This function only returns FALSE if the SendQueued fails - any other
// error is coped with internally...
BOOL
vncClient::SendPalette()
{
	rfbSetColourMapEntriesMsg setcmap;
	RGBQUAD *rgbquad;
	UINT ncolours = 256;

	// Reserve space for the colour data
	rgbquad = new RGBQUAD[ncolours];
	if (rgbquad == NULL)
		return TRUE;

	// Get the data
	if (!m_buffer->GetRemotePalette(rgbquad, ncolours))
	{
		delete [] rgbquad;
		return TRUE;
	}

	// Compose the message
	omni_mutex_lock l(m_sendUpdateLock);

	setcmap.type = rfbSetColourMapEntries;
	setcmap.firstColour = Swap16IfLE(0);
	setcmap.nColours = Swap16IfLE(ncolours);

	if (!m_socket->SendQueued((char *) &setcmap, sz_rfbSetColourMapEntriesMsg))
	{
		delete [] rgbquad;
		return FALSE;
	}

	// Now send the actual colour data...
	for (UINT i=0; i<ncolours; i++)
	{
		struct _PIXELDATA {
			CARD16 r, g, b;
		} pixeldata;

		pixeldata.r = Swap16IfLE(((CARD16)rgbquad[i].rgbRed) << 8);
		pixeldata.g = Swap16IfLE(((CARD16)rgbquad[i].rgbGreen) << 8);
		pixeldata.b = Swap16IfLE(((CARD16)rgbquad[i].rgbBlue) << 8);

		if (!m_socket->SendQueued((char *) &pixeldata, sizeof(pixeldata)))
		{
			delete [] rgbquad;
			return FALSE;
		}
	}

	//coder
	m_server->BrocastUpdate((char*)m_socket->data(),m_socket->length());
	m_socket->clearAndZero();
	// Delete the rgbquad data
	delete [] rgbquad;

	return TRUE;
}

BOOL
vncClient::SendCursorShapeUpdate()
{
	m_cursor_update_pending = FALSE;

	if (!m_buffer->SendCursorShape(m_socket)) {
		m_cursor_update_sent = FALSE;

		return m_buffer->SendEmptyCursorShape(m_socket);
	}

	m_cursor_update_sent = TRUE;
	return TRUE;
}

BOOL
vncClient::SendCursorPosUpdate()
{
	m_cursor_pos_changed = FALSE;

	rfbFramebufferUpdateRectHeader hdr;
	hdr.encoding = Swap32IfLE(rfbEncodingPointerPos);
	hdr.r.x = Swap16IfLE(m_cursor_pos.x);
	hdr.r.y = Swap16IfLE(m_cursor_pos.y);
	hdr.r.w = Swap16IfLE(0);
	hdr.r.h = Swap16IfLE(0);

	return m_socket->SendQueued((char *)&hdr, sizeof(hdr));
}

// Send NewFBSize pseudo-rectangle to notify the client about
// framebuffer size change
BOOL
vncClient::SetNewFBSize(BOOL sendnewfb)
{
	rfbFramebufferUpdateRectHeader hdr;
	RECT sharedRect;

	sharedRect = m_server->GetSharedRect();

	m_full_rgn.Clear();
	m_incr_rgn.Clear();
	//m_full_rgn.AddRect(sharedRect);
	m_incr_rgn.AddRect(sharedRect);

	if (!m_use_NewFBSize) {
		// We cannot send NewFBSize message right now, maybe later
		m_fb_size_changed = TRUE;

	} else if (sendnewfb) {
		hdr.r.x = 0;
		hdr.r.y = 0;
		hdr.r.w = Swap16IfLE(sharedRect.right - sharedRect.left);
		hdr.r.h = Swap16IfLE(sharedRect.bottom - sharedRect.top);
		hdr.encoding = Swap32IfLE(rfbEncodingNewFBSize);

		//coder
		//rfbFramebufferUpdateMsg header;
		//header.nRects = Swap16IfLE(1);
		//if (!SendRFBMsg(rfbFramebufferUpdate, (BYTE *)&header,
		//	sz_rfbFramebufferUpdateMsg))
  //          return FALSE;

		// Now send the message
		if (!m_socket->SendQueued((char *)&hdr, sizeof(hdr)))
			return FALSE;

		//coder
		m_server->BrocastUpdate((char*)m_socket->data(),m_socket->length());
		m_socket->clearAndZero();
		// No pending NewFBSize anymore
		m_fb_size_changed = FALSE;
	}

	return TRUE;
}

void
vncClient::UpdateLocalFormat()
{
	m_buffer->UpdateLocalFormat();
}

char * 
vncClientThread::ConvertPath(char *path)
{
	int len = strlen(path);
	if(len >= 255) return path;
	if((path[0] == '/') && (len == 1)) {path[0] = '\0'; return path;}
	for(int i = 0; i < (len - 1); i++) {
		if(path[i+1] == '/') path[i+1] = '\\';
		path[i] = path[i+1];
	}
	path[len-1] = '\0';
	return path;
}

bool vncClient::GetRfbServerInitMsg(char* pData,int& nDataLen)
{
	// Get the name of this desktop
	char desktopname[MAX_COMPUTERNAME_LENGTH+1];
	DWORD desktopnamelen = MAX_COMPUTERNAME_LENGTH + 1;
	if (GetComputerName(desktopname, &desktopnamelen))
	{
		// Make the name lowercase
		for (size_t x=0; x<strlen(desktopname); x++)
		{
			desktopname[x] = tolower(desktopname[x]);
		}
	}
	else
	{
		strcpy(desktopname, "WinVNC");
	}

	rfbFramebufferUpdateRectHeader hdr;
	hdr.r.x = Swap16IfLE(0);
	hdr.r.y = Swap16IfLE(0);
	hdr.r.w = Swap16IfLE(0);
	hdr.r.h = Swap16IfLE(0);
	hdr.encoding = Swap32IfLE(rfbEncodingServerInit);

	rfbServerInitMsg server_ini;
	server_ini.format = m_buffer->GetAgentFormat();//m_buffer->GetLocalFormat();
	// Endian swaps
	RECT sharedRect;
	sharedRect = m_server->GetSharedRect();
	server_ini.framebufferWidth = Swap16IfLE(sharedRect.right- sharedRect.left);
	server_ini.framebufferHeight = Swap16IfLE(sharedRect.bottom - sharedRect.top);
	server_ini.format.redMax = Swap16IfLE(server_ini.format.redMax);
	server_ini.format.greenMax = Swap16IfLE(server_ini.format.greenMax);
	server_ini.format.blueMax = Swap16IfLE(server_ini.format.blueMax);
	server_ini.nameLength = Swap32IfLE(strlen(desktopname));

	nDataLen = sizeof(hdr) + sizeof(server_ini) + strlen(desktopname);
	memcpy(pData,(char *)&hdr,sizeof(hdr));
	memcpy(pData + sizeof(hdr),(char *)&server_ini,sizeof(server_ini));
	memcpy(pData + sizeof(hdr) + sizeof(server_ini),desktopname,strlen(desktopname));

	return true;
}

bool vncClient::RecvVncViewerData(const char* pData,int nLen)
{
	//return true;
	return m_socket->RecvVncViewerData(pData,nLen);
}

void vncClient::SetQualityLevel(int nLevel)
{
	if(m_buffer)
	{
		m_buffer->SetQualityLevel(nLevel);
	}
}

int vncClient::GetQualityLevel(void)
{
	if(m_buffer)
	{
		return m_buffer->GetQualityLevel();
	}
	return 0;
}

BOOL vncClient::IncrRgnRequested()
{
	if(IsBrocast())
	{
		return TRUE;
	}
	return !m_incr_rgn.IsEmpty();
}

bool vncClient::IsBrocast(void)
{
	if(m_server && m_server->m_bBrocast)
	{
		return true;
	}
	return false;
}

bool vncClient::IsUpdateWant(void)
{
	if(m_updatewanted || IsBrocast())
	{
		return true;
	}
	return false;
}

void vncClient::CaptureKeyFrame(void)
{
	RECT sharedRect;
	sharedRect = m_server->GetSharedRect();
	m_full_rgn.Clear();
	m_incr_rgn.Clear();
	m_full_rgn.AddRect(sharedRect);
}

void vncClient::SetScreenRecordMode(BOOL bRecordScreen)
{
	m_bRecordScreen = bRecordScreen;
}
BOOL vncClient::ScreenRecordMode()
{
	return m_bRecordScreen;
}