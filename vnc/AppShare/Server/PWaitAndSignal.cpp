#include "stdhdrs.h"
#include "PWaitAndSignal.h"

PWaitAndSignal::PWaitAndSignal(PCritSec& rCritSec)
: m_rCritSec(rCritSec)
{
	m_rCritSec.Wait();
}

PWaitAndSignal::~PWaitAndSignal(void)
{
	m_rCritSec.Signal();
}
