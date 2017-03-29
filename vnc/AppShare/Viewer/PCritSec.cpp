#include "stdhdrs.h"
#include "PCritSec.h"

PCritSec::PCritSec(void)
{
	InitializeCriticalSection(&m_CritSec);
}

PCritSec::~PCritSec(void)
{
	DeleteCriticalSection(&m_CritSec);
}

void PCritSec::Wait(void)
{
	EnterCriticalSection(&m_CritSec);
}

void PCritSec::Signal(void)
{
	LeaveCriticalSection(&m_CritSec);
}