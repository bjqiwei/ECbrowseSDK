#ifndef __BPS_COUNTER_H__
#define __BPS_COUNTER_H__

#include <afxmt.h>

template<class T, int AVG_TIME_LENGTH> 
class BPSCounter
{
private:
	typedef void (T::*PFNBPSCB)(DWORD dwChannelID, double fBPS_Avg, double fBPS_Now, DWORD nHitCount);
	CCriticalSection			m_Section;
	PFNBPSCB					m_pfnBPSCB;
	T							*m_pEventHandlerInstance;
	DWORD						m_dwChannelID;
	
	BOOL						m_bStarted;

	double						m_fBPS_Avg[AVG_TIME_LENGTH];
	int							m_nBPS_Avg_Count;
	int							m_nBPS_Avg_Index;
	
	double						m_fBPS_Now;
	double						m_fBPS_Last;
	DWORD						m_StartTime_Now;
	DWORD						m_nBitCount_Now;
	DWORD						m_nHitCount;

public:
	explicit BPSCounter(DWORD dwChannelID, PFNBPSCB _Pm, T* _P)
		: m_dwChannelID(dwChannelID)
		, m_pfnBPSCB(_Pm), m_pEventHandlerInstance(_P)
		, m_bStarted(FALSE) {}
	~BPSCounter() { Stop(); }
	void SetEventHandlerInstance(T* _P)
	{
		m_pEventHandlerInstance = _P;
	}

public:
	void Start()
	{
		CSingleLock lock(&m_Section);
		lock.Lock();
		if (m_bStarted)
		{
			lock.Unlock();
			return;
		}
		
		m_fBPS_Now			= 0.0;
		m_fBPS_Last			= 0.0;
		m_nBitCount_Now		= 0;
		m_nHitCount			= 0;

		for (int i=0; i < AVG_TIME_LENGTH; i++)
			m_fBPS_Avg[i] = 0.0;
		m_nBPS_Avg_Count = 0;
		m_nBPS_Avg_Index = 0;

		m_bStarted			= TRUE;
		
		lock.Unlock();
	}
	void Stop()
	{
		CSingleLock lock(&m_Section);
		lock.Lock();
		
		if (m_bStarted)
		{
			m_bStarted = FALSE;
		}
		
		lock.Unlock();
	}
	void Update(DWORD nBytes)
	{
		CSingleLock lock(&m_Section);
		lock.Lock();

		if (!m_bStarted)
		{
			Start();
		}
		
		DWORD tNow = GetTickCount();
		m_nBitCount_Now += (nBytes << 3);
		++ m_nHitCount;
		if (tNow - m_StartTime_Now > 1000)
		{
			m_fBPS_Now = (double) m_nBitCount_Now / double (tNow - m_StartTime_Now) * (double)1000.0;
			
			m_fBPS_Avg[m_nBPS_Avg_Index++] = m_fBPS_Now;
			if (m_nBPS_Avg_Index >= AVG_TIME_LENGTH)
				m_nBPS_Avg_Index = 0;
			if (m_nBPS_Avg_Count < AVG_TIME_LENGTH)
				m_nBPS_Avg_Count++;
			
			m_StartTime_Now = tNow;
			m_nBitCount_Now = 0;
			if (m_pfnBPSCB && m_pEventHandlerInstance)
			{
				double fBPS_Now		= m_fBPS_Now;
				double fBPS_Avg		= 0.0;
				int nHitCount		= 0;
				for (int i=0; i < m_nBPS_Avg_Count; i++)
					fBPS_Avg += m_fBPS_Avg[i];
				if (m_nBPS_Avg_Count > 0)
					fBPS_Avg /= m_nBPS_Avg_Count;
				if (m_fBPS_Now > m_fBPS_Last)	// if speed up fast, down slow
					fBPS_Avg = m_fBPS_Now;
				m_fBPS_Last = m_fBPS_Now;
				nHitCount			= m_nHitCount;
				lock.Unlock();
				(m_pEventHandlerInstance->*m_pfnBPSCB)(m_dwChannelID, fBPS_Avg, fBPS_Now, nHitCount);
			}
			m_nHitCount	= 0;
		}
		lock.Unlock();
	}
};

#endif//__BPS_COUNTER_H__
