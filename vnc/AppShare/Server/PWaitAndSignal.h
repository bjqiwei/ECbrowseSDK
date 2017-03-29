#pragma once
#include "PCritSec.h"

class PWaitAndSignal
{
public:
	PWaitAndSignal(PCritSec& rCritSec);
	virtual ~PWaitAndSignal(void);
protected:
	PCritSec&	m_rCritSec;
};
