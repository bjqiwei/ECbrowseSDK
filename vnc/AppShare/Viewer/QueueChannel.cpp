#include "stdhdrs.h"
#include "QueueChannel.h"

CQueueChannel::CQueueChannel(void)
{
	m_pQueueBuffer = NULL;
	m_ulQueueSize = 0;
	m_ulQueueLength = m_ulEnqueuePos = m_ulDequeuePos = 0;
	m_ulReadTimeout = m_ulWriteTimeout = -1; 
}

CQueueChannel::~CQueueChannel(void)
{
}

bool CQueueChannel::Open(unsigned long ulQueueSize)
{
	if(ulQueueSize == 0)
	{
		return false;
	}

	m_Mutex.Wait();
	if(m_pQueueBuffer != NULL)
	{
		delete [] m_pQueueBuffer;
	}
	m_pQueueBuffer = new unsigned char[ulQueueSize];
	m_ulQueueSize = ulQueueSize;
	m_ulQueueLength = m_ulEnqueuePos = m_ulDequeuePos = 0;
	m_Mutex.Signal();

	m_UnEmpty.Signal();
	m_UnFull.Signal();

	return true;
}

bool CQueueChannel::Close(void)
{
	m_Mutex.Wait();
	if(m_pQueueBuffer != NULL)
	{
		delete [] m_pQueueBuffer;
	}
	m_pQueueBuffer = NULL;
	m_Mutex.Signal();
	m_UnEmpty.Signal();
	m_UnFull.Signal();
	return true;
}
bool CQueueChannel::Clear()
{
	m_Mutex.Wait();
	m_ulQueueLength = m_ulEnqueuePos = m_ulDequeuePos = 0;
	m_Mutex.Signal();
	return true;
}

bool CQueueChannel::Read(unsigned char* pBuffer,unsigned long& ulCount)
{
	m_Mutex.Wait();

	if(m_pQueueBuffer == NULL)
	{
		m_Mutex.Signal();
		return false;
	}
	/* If queue is empty then we should block for the time specifed in the
		read timeout.
	*/
	if(m_ulQueueLength == 0) 
	{
		// unlock the data
		m_Mutex.Signal();

		if (!m_UnEmpty.Wait(m_ulReadTimeout)) 
		{
			return false;
		}

		// relock the data
		m_Mutex.Wait();
	}

	if(m_pQueueBuffer == NULL)
	{
		m_Mutex.Signal();
		return false;
	}
	// To make things simpler, limit to amount to copy out of queue to till
	// the end of the linear part of memory. Another loop around will get
	// rest of data to dequeue
	unsigned long ulCopyLen = m_ulQueueSize - m_ulDequeuePos;

	// But do not copy more than has actually been queued
	if (ulCopyLen > m_ulQueueLength)
	{
		ulCopyLen = m_ulQueueLength;
	}

	// Or more than has been requested
	if (ulCopyLen > ulCount)
	{
		ulCopyLen = ulCount;
	}

	// Copy data out and increment pointer, decrement bytes yet to dequeue
	memcpy(pBuffer, m_pQueueBuffer + m_ulDequeuePos, ulCopyLen);
	ulCount = ulCopyLen;

	// Move the queue pointer along, wrapping to beginning
	m_ulDequeuePos += ulCopyLen;
	if (m_ulDequeuePos >= m_ulQueueSize)
	{
		m_ulDequeuePos = 0;
	}

	// If buffer was full, signal possibly blocked write of data to queue
	// that it can write to queue now.
	if (m_ulQueueLength == m_ulQueueSize) 
	{
		m_UnFull.Signal();
	}

	// Now decrement queue length by the amount we copied
	m_ulQueueLength -= ulCopyLen;

	// unlock the buffer
	m_Mutex.Signal();

	return true;
}

bool CQueueChannel::Write(unsigned char* pBuffer, unsigned long& ulCount)
{
	m_Mutex.Wait();

	if(m_pQueueBuffer == NULL)
	{
		m_Mutex.Signal();
		return false;
	}

	/* If queue is full then we should block for the time specifed in the
		write timeout.
	*/
	if(m_ulQueueLength == m_ulQueueSize) 
	{
		m_Mutex.Signal();

		if (!m_UnFull.Wait(m_ulWriteTimeout)) 
		{
			return false;
		}

		m_Mutex.Wait();
	}

	if(m_pQueueBuffer == NULL)
	{
		m_Mutex.Signal();
		return false;
	}

	// Calculate number of bytes to copy
	unsigned long ulCopyLen = ulCount;

	// First don't copy more than are availble in queue
	unsigned long ulBytesLeftInQueue = m_ulQueueSize - m_ulQueueLength;
	if (ulCopyLen > ulBytesLeftInQueue)
	{
		ulCopyLen = ulBytesLeftInQueue;
	}

	// Then to make things simpler, limit to amount left till the end of the
	// linear part of memory. Another loop around will get rest of data to queue
	unsigned long ulBytesLeftInUnwrapped = m_ulQueueSize - m_ulEnqueuePos;
	if (ulCopyLen > ulBytesLeftInUnwrapped)
	{
		ulCopyLen = ulBytesLeftInUnwrapped;
	}

	// Move the data in and increment pointer, decrement bytes yet to queue
	memcpy(m_pQueueBuffer + m_ulEnqueuePos, pBuffer, ulCopyLen);
	ulCount = ulCopyLen;

	// Move the queue pointer along, wrapping to beginning
	m_ulEnqueuePos += ulCopyLen;
	if (m_ulEnqueuePos >= m_ulQueueSize)
	{
		m_ulEnqueuePos = 0;
	}

	// see if we need to signal reader that queue was empty
	bool bQueueWasEmpty = (m_ulQueueLength == 0);

	// increment queue length by the amount we copied
	m_ulQueueLength += ulCopyLen;

	// signal reader if necessary
	if (bQueueWasEmpty) 
	{
		m_UnEmpty.Signal();
	}

	m_Mutex.Signal();

	return true;
}

void CQueueChannel::SetReadTimeout(unsigned long ulReadTimeout)
{
	m_ulReadTimeout = ulReadTimeout;
}

void CQueueChannel::SetWriteTimeout(unsigned long ulWriteTimeout)
{
	m_ulWriteTimeout = ulWriteTimeout;
}

unsigned long CQueueChannel::GetLength(void)
{
	return m_ulQueueLength;
}
long CQueueChannel::GetSize()
{
	return m_ulQueueSize;
}