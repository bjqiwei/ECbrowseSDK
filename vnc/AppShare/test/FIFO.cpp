// FIFO.cpp : implementation file
//

#include "stdafx.h"
#include "FIFO.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFIFO

CFIFO::CFIFO()
{
	m_Size = DEFAULT_FIFO_SIZE;

	m_ReadIndex		= 0;
	m_WriteIndex	= 0;
	m_Length		= 0;

	m_pBuffer = new char[m_Size];	// Allocate memory, in case caller forget call Create.
	//TRACE(_T("ENTER FIFO ThreadId:%ld\n"),GetCurrentThreadId());
	InitializeCriticalSection(&m_csLock);
}

CFIFO::~CFIFO()
{
	LeaveCriticalSection(&m_csLock);
	
	DeleteCriticalSection(&m_csLock);
	//TRACE(_T("LEAVE FIFO ThreadId:%ld\n"),GetCurrentThreadId());
	if (m_pBuffer)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}
}

BOOL CFIFO::Create(int size)
{
	EnterCriticalSection(&m_csLock);

	if ( size != DEFAULT_FIFO_SIZE )
	{
		delete [] m_pBuffer;

		m_Size			= size;
		m_ReadIndex		= 0;
		m_WriteIndex	= 0;
		m_Length		= 0;
		
		m_pBuffer = NULL;

		m_pBuffer = new char[m_Size];
	}

	LeaveCriticalSection(&m_csLock);

	if ( m_pBuffer == NULL )
		return FALSE;

	return TRUE;
}
void CFIFO::Clear()
{
	EnterCriticalSection(&m_csLock);
	m_ReadIndex		= 0;
	m_WriteIndex	= 0;
	m_Length		= 0;
	LeaveCriticalSection(&m_csLock);

}
int	CFIFO::Put(const char* buffer, int len)
{
	if (m_pBuffer == NULL) return 0;
	int length;

	EnterCriticalSection(&m_csLock);

	if ( m_Length + len <= m_Size )
	{
		// enough buffer space
		length = len;
	}
	else
	{
		// not enough buffer space, can only accept part of the input data then buffer full.
		length = m_Size - m_Length;
	}

	if ( (m_ReadIndex < m_WriteIndex) || (m_Length == 0 ) )
	{
		// the buffer is not roll over, not reach buffer end

		if (m_WriteIndex + length <= m_Size)
		{
			// buffer is not going to be full
			CopyMemory(m_pBuffer + m_WriteIndex, buffer, length);
			
			m_WriteIndex += length;
			m_WriteIndex %= m_Size;
		}
		else
		{
			// buffer is going to be full
			CopyMemory(m_pBuffer + m_WriteIndex, buffer, m_Size - m_WriteIndex);
			
			// start from begining
			CopyMemory(m_pBuffer, buffer + m_Size - m_WriteIndex, length - (m_Size - m_WriteIndex));
			
			m_WriteIndex += length;
			m_WriteIndex %= m_Size;
		}

		m_Length += length;
	}
	else
	{
		// the buffer is roll over, data was reach the buffer end
		CopyMemory(m_pBuffer + m_WriteIndex, buffer, length);
			
		m_WriteIndex += length;
		m_WriteIndex %= m_Size;
		
		m_Length += length;
	}

	LeaveCriticalSection(&m_csLock);

	return length;
}

int	CFIFO::Get(char* buffer, int len)
{
	int length;

	EnterCriticalSection(&m_csLock);

	if( m_Length < len )
		length = m_Length;
	else
		length = len;

	if ( m_Length == 0 )
	{
		// fifo empty
		LeaveCriticalSection(&m_csLock);

		return 0;
	}

	if ( m_ReadIndex < m_WriteIndex )
	{
		// has not reached end of buffer
		CopyMemory(buffer, m_pBuffer + m_ReadIndex, length);
		
		m_Length -= length;
		m_ReadIndex += length;
		m_ReadIndex %= m_Size;
	}
	else
	{
		// has reached end of buffer

		if ( length <= (m_Size - m_ReadIndex) )
		{
			// not going to read till end of buffer
			CopyMemory(buffer, m_pBuffer + m_ReadIndex, length);

			m_Length -= length;
			m_ReadIndex += length;
			m_ReadIndex %= m_Size;
		}
		else
		{
			// reading will roll over
			CopyMemory(buffer, m_pBuffer + m_ReadIndex, m_Size - m_ReadIndex);

			// start from begining
			CopyMemory(buffer + m_Size - m_ReadIndex, m_pBuffer, length - (m_Size - m_ReadIndex));
			
			m_Length -= length;
			m_ReadIndex += length;
			m_ReadIndex %= m_Size;
		}
	}
		
	LeaveCriticalSection(&m_csLock);

	return length;
}

int CFIFO::Peek(char* buffer, int len)
{
	int length;
	
	EnterCriticalSection(&m_csLock);
	
	if( m_Length < len )
		length = m_Length;
	else
		length = len;
	
	if ( m_Length == 0 )
	{
		// fifo empty
		LeaveCriticalSection(&m_csLock);
		
		return 0;
	}
	
	if ( m_ReadIndex < m_WriteIndex )
	{
		// has not reached end of buffer
		CopyMemory(buffer, m_pBuffer + m_ReadIndex, length);
	}
	else
	{
		// has reached end of buffer
		
		if ( length <= (m_Size - m_ReadIndex) )
		{
			// not going to read till end of buffer
			CopyMemory(buffer, m_pBuffer + m_ReadIndex, length);
		}
		else
		{
			// reading will roll over
			CopyMemory(buffer, m_pBuffer + m_ReadIndex, m_Size - m_ReadIndex);
			
			// start from begining
			CopyMemory(buffer + m_Size - m_ReadIndex, m_pBuffer, length - (m_Size - m_ReadIndex));
		}
	}
	
	LeaveCriticalSection(&m_csLock);
	
	return length;
}

BOOL CFIFO::IsEmpty()
{
	if ( m_Length == 0 )
		return TRUE;
	else
		return FALSE;
}

int CFIFO::GetSize()
{
	return m_Size;
}

int CFIFO::GetLength()
{
	return m_Length;
}
