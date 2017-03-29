#pragma once
#include "PCritSec.h"
#include "PSyncPoint.h"

class CQueueChannel
{
public:
	CQueueChannel(void);
	virtual ~CQueueChannel(void);
public:
	bool Open(unsigned long ulQueueSize);
	bool Close(void);
	bool Clear();
	bool Read(unsigned char* pBuffer,unsigned long& ulCount);
	bool Write(unsigned char* pBuffer, unsigned long& ulCount);
	void SetReadTimeout(unsigned long ulReadTimeout);
	void SetWriteTimeout(unsigned long ulWriteTimeout);
	unsigned long GetLength(void);
	long GetSize();
protected:
    PCritSec   m_Mutex;
    unsigned char* m_pQueueBuffer;
    unsigned long m_ulQueueSize,m_ulQueueLength,m_ulEnqueuePos,m_ulDequeuePos;
    PSyncPoint m_UnEmpty;
    PSyncPoint m_UnFull;
    unsigned long m_ulReadTimeout;
    unsigned long m_ulWriteTimeout;
};
