#pragma once

class PSyncPoint
{
public:
	PSyncPoint(void);
	virtual ~PSyncPoint(void);
public:
	bool Wait(unsigned long ulTimeout);
	void Signal(void);
protected:
	HANDLE m_hEvent;
};
