#include "stdhdrs.h"
#include "PSyncPoint.h"

PSyncPoint::PSyncPoint(void)
{
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

PSyncPoint::~PSyncPoint(void)
{
	if(m_hEvent) 
	{
		CloseHandle(m_hEvent);
		m_hEvent=NULL;
	}
}

bool PSyncPoint::Wait(unsigned long ulTimeout)
{
	unsigned long ulResult = WaitForSingleObject(m_hEvent, ulTimeout);
	return (ulResult != WAIT_TIMEOUT);
}

void PSyncPoint::Signal(void)
{
	SetEvent(m_hEvent);
}