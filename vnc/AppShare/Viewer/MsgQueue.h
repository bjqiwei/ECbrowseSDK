// MsgQueue.h: interface for the CMsgQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGQUEUE_H__497AF0F6_8669_48A2_8257_239CDE2C49DD__INCLUDED_)
#define AFX_MSGQUEUE_H__497AF0F6_8669_48A2_8257_239CDE2C49DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMsgQueue 
{
public:
	CMsgQueue();
	virtual ~CMsgQueue();

public:
	long				GetDataLength();
	void				PeekAllMessage(long &lLen,void *pBuf);
	long				GetFreeLength();
	void				RemoveAllMessage();
	void				UnInitQueue();
	BOOL				InitQueue(long lBufferLen,BOOL IsDiscard,BOOL bUnitK = TRUE);
	BOOL				GetMessage(long &lLen, void *pBuf);
	long				AddMessage(void *pBuf, long lLen);
	BOOL				HasQueueElement();	

private:
	long				RemoveHead();
	long				GetFactSize(long lLenMsg);

	char*				m_pcBuffer;		// Store the message, the buffer is assigned by user
										// The buffer size is 4K*n Bytes
	char*				m_pCurAddress;	// Current address of the buffer
	long				m_lBufferLen;	// The buffer size
	CRITICAL_SECTION	m_csUsedFlag;	// this is a lock, Control m_pcBuffer's operation
	BOOL				m_bDiscard;		// the flag is discard pre message  if the buffer is full
	BOOL				m_bInit;		// the flag is the class initialized
};

#endif // !defined(AFX_MSGQUEUE_H__497AF0F6_8669_48A2_8257_239CDE2C49DD__INCLUDED_)
