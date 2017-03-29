#ifndef __TMUTEX_H__
#define __TMUTEX_H__

class TMutex
{
private:
	CRITICAL_SECTION Handle;

public:
	TMutex()
	{
		::InitializeCriticalSection(&Handle); 
	}
	~TMutex()
	{
		::DeleteCriticalSection(&Handle);
	}
	void Wait()
	{
		::EnterCriticalSection(&Handle);
	}
	void Post()
	{
		::LeaveCriticalSection(&Handle);
	}
	void Lock()			 { Wait(); }
	void Unlock()		 {Post(); }
};
#endif
