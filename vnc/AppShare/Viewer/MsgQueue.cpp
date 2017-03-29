// MsgQueue.cpp: implementation of the CMsgQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MsgQueue.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define MAX_PACKET_SIZE		4*1024		// unit is B
#define MAX_BUFFER_SIZE		1024*2		// unit is KB
#define MIN_BUFFER_SIZE		1			// unit is KB

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsgQueue::CMsgQueue()
{
	InitializeCriticalSection(&m_csUsedFlag);
	m_pcBuffer		= NULL;	
	m_pCurAddress	= NULL;
	m_lBufferLen	= 0;	
	m_bDiscard		= FALSE;
	m_bInit			= FALSE;
	
}

CMsgQueue::~CMsgQueue()
{
	UnInitQueue();
	LeaveCriticalSection(&m_csUsedFlag);
	DeleteCriticalSection(&m_csUsedFlag);
}
  

/*
 *	function name		HasQueueElement
	description			judge that if it has data in the buffer
	intput parameters	N/A
	output parameters	N/A
	return value		TRUE or FALSE
 */
BOOL CMsgQueue::HasQueueElement()
{
	//return m_ListMsg.GetCount()>0;
	BOOL bRet = FALSE;
	EnterCriticalSection(&m_csUsedFlag);
	bRet = (m_pCurAddress-m_pcBuffer) >0;
	LeaveCriticalSection(&m_csUsedFlag);
	return bRet;
}
/*
 *	function name		Addmessage
	description			add message into the buffer
	intput parameters	data and data's length
	output parameters	N/A
	return value		type is long, <0 Failed; >=0 success the value is the current address
						-2 the class was't initialized;-3 the buffer is full;
						-4 unknow error

 */
long CMsgQueue::AddMessage(void *pBuf, long lLen)
{
	if(lLen > m_lBufferLen || lLen <= 0 )
		return -1;	
	if(!m_bInit)
		return -2;
	//find the ptr rapidly 4Bytes *n
	
	int nFactSize = GetFactSize(lLen);
	EnterCriticalSection(&m_csUsedFlag);
	{
	
		long lLenmsglength = m_pCurAddress - m_pcBuffer;
		ASSERT(lLenmsglength >= 0);
		ASSERT((lLenmsglength - m_lBufferLen) <= 0);
	}
	if((long)((long)(m_pCurAddress - m_pcBuffer) + nFactSize + sizeof(long)) > m_lBufferLen)
	{
		if(m_bDiscard)
		{
			int nCount = 0;
			while((long)(RemoveHead() + nFactSize + sizeof(long)) > m_lBufferLen)
			{
				nCount ++;
				if(nCount > 10000)	// avoid dead loop
				{
					LeaveCriticalSection(&m_csUsedFlag);
					return -4;
				}
			}
		}
		else
		{
			LeaveCriticalSection(&m_csUsedFlag);
			return -3;
		}
	}
	CopyMemory(m_pCurAddress,&lLen,sizeof(long));
	m_pCurAddress += sizeof(long);
	CopyMemory(m_pCurAddress,pBuf,lLen);
	m_pCurAddress += nFactSize;
	{
		long lLenmsglength = m_pCurAddress - m_pcBuffer;
		ASSERT(lLenmsglength >= 0);
		ASSERT((lLenmsglength - m_lBufferLen) <= 0);
	}
	long	lRet = m_pCurAddress - m_pcBuffer;
	LeaveCriticalSection(&m_csUsedFlag);
	return lRet;	
}
/*
 *	function name		GetFactSize
	description			gain the data's size in buffer
	intput parameters	the data length
	output parameters	N/A
	return value		the data's size in buffer 4B * n

 */
long CMsgQueue::GetFactSize(long lLenMsg)
{
	long lMod = lLenMsg % 4;
	long lLen = lLenMsg / 4;
	long lFactSize = 0;
	if(lMod > 0)
	{
		lFactSize = (lLen + 1) * 4;
	}
	else
	{
		lFactSize = lLenMsg;
	}
	return lFactSize;
}
/*
 *	function name		RemoveHead
	description			remove the first data in the buffer
	intput parameters	N/A
	output parameters	N/A
	return value		current length of buffer

 */
long CMsgQueue::RemoveHead()
{
	long	lMsgLen		= *(long*)m_pcBuffer;
	int		nFactSize	= GetFactSize(lMsgLen);
	long	lCurLen		= m_pCurAddress - m_pcBuffer;
	if((long)(sizeof(long) + nFactSize) > lCurLen)
	{
		m_pCurAddress	= m_pcBuffer;
	}
	else
	{
		CopyMemory(m_pcBuffer,m_pcBuffer + nFactSize + sizeof(long),lCurLen - nFactSize - sizeof(long));
		m_pCurAddress	-= nFactSize;
		m_pCurAddress	-= sizeof(long);
	}
	long	lRet		= m_pCurAddress - m_pcBuffer;
	return lRet;
}
/*
 *	function name		GetMessage
	description			get the first data place into the pBuf
	intput parameters	the ptr of receive buffer
	output parameters	the data length
	return value		TRUE--success FALSE--failed

 */
BOOL CMsgQueue::GetMessage(long &lLen, void *pBuf)
{
	
	lLen		= 0;
	if(!m_bInit)
		return FALSE;
	BOOL	bRet = FALSE;
	EnterCriticalSection(&m_csUsedFlag);
	long	lMsgLen = m_pCurAddress - m_pcBuffer;
	if(lMsgLen > sizeof(long))
	{
		lLen = *(long*)m_pcBuffer;
		int nFactSize = GetFactSize(lLen);
		if((long)(nFactSize + sizeof(long)) > lMsgLen)
		{
			m_pCurAddress	= m_pcBuffer;
			lLen			= 0;
			bRet			= FALSE;
		}
		else
		{
			CopyMemory(pBuf,m_pcBuffer + sizeof(long),lLen);		
			CopyMemory(m_pcBuffer,m_pcBuffer + sizeof(long) + nFactSize,lMsgLen - nFactSize - sizeof(long));
			m_pCurAddress -= nFactSize;
			m_pCurAddress -= sizeof(long);
			bRet = TRUE;
		}
	}
	LeaveCriticalSection(&m_csUsedFlag);
	return bRet;
}
/*
 *	function name		InitQueue
	description			Initiliaze the class
	intput parameters	the max buffer length,auto deleted the first data when the 
						buffer is full
	output parameters	N/A
	return value		TRUE--success FALSE--failed

 */
BOOL CMsgQueue::InitQueue(long lBufferLen, BOOL IsDiscard,BOOL bUnitK)
{
	if(m_bInit)
		return TRUE;
	BOOL	bRet = FALSE;
	if(bUnitK)
	{
		if(lBufferLen > MAX_BUFFER_SIZE)
			lBufferLen = MAX_BUFFER_SIZE;

		if(lBufferLen < MIN_BUFFER_SIZE)
			lBufferLen = MIN_BUFFER_SIZE;

		m_lBufferLen	= GetFactSize(lBufferLen);
		m_lBufferLen	*= 1024;
		m_pcBuffer		= new char[m_lBufferLen];	// delete uninit
		//m_lBufferLen	= lBufferLen *1024;
		if(m_pcBuffer == NULL)
		{
			return FALSE;
		}		
	}else
	{
		if(lBufferLen > MAX_BUFFER_SIZE*1024)
			lBufferLen = MAX_BUFFER_SIZE * 1024;
		if(lBufferLen < MIN_BUFFER_SIZE)
			lBufferLen = MIN_BUFFER_SIZE;
		m_lBufferLen	= GetFactSize((lBufferLen /1024) + 1);
		m_lBufferLen	*= 1024;
		m_pcBuffer		= new char[m_lBufferLen];	// delete uninit
		m_lBufferLen	= lBufferLen;
		if(m_pcBuffer == NULL)
		{
			return FALSE;
		}
	}
	m_pCurAddress	= m_pcBuffer;
	m_bDiscard		= IsDiscard;
	m_bInit			= TRUE;
	return TRUE;
}
/*
 *	function name		UnInitQueue
	description			UnInitiliaze the class
	intput parameters	N/A
	output parameters	N/A
	return value		N/A

 */
void CMsgQueue::UnInitQueue()
{
	//if(m_ListMsg.GetCount() > 0)
	//	m_ListMsg.RemoveAll();
	if(m_bInit)
	{
		EnterCriticalSection(&m_csUsedFlag);
		if(m_pcBuffer != NULL)
		{
			delete [] m_pcBuffer;
			m_pcBuffer	= NULL;
		}
		m_pCurAddress	= NULL;
		m_lBufferLen	= 0;
		LeaveCriticalSection(&m_csUsedFlag);
		m_bDiscard		= FALSE;
		m_bInit			= FALSE;
	}
}
/*
 *	function name		RemoveAllMessage
	description			remove all message in the buffer
	intput parameters	N/A
	output parameters	N/A
	return value		N/A

 */
void CMsgQueue::RemoveAllMessage()
{
	EnterCriticalSection(&m_csUsedFlag);
	m_pCurAddress = m_pcBuffer;
	LeaveCriticalSection(&m_csUsedFlag);
}

long CMsgQueue::GetFreeLength()
{
	long lRet = 0;
	//EnterCriticalSection(&m_csUsedFlag);
	lRet = m_pCurAddress - m_pcBuffer;
	//EnterCriticalSection(&m_csUsedFlag);
	lRet = m_lBufferLen - lRet - sizeof(long);	
	return lRet;
}

void CMsgQueue::PeekAllMessage(long &lLen, void *pBuf)
{
	if(!m_bInit)
		return ;
	if(pBuf == NULL)
	{
		lLen = 0;
		return;
	}
	if(lLen < m_lBufferLen)
	{
		lLen = 0;
		return;
	}
	EnterCriticalSection(&m_csUsedFlag);
	char	*pCur = m_pcBuffer;
	int		nLen = 0;
	long	lLenMsg = lLen;
	lLen = 0;
	while((m_pCurAddress - pCur) > 0)
	{
		long lLenIn = *(long *)pCur;
		if((long)(lLen + lLenIn + sizeof(long)) > lLenMsg)
		{
			break;	
		}
		CopyMemory((char *)pBuf + nLen,pCur,lLenIn + sizeof(long));
		
		lLen += lLenIn + sizeof(long);
		nLen += lLenIn + sizeof(long);
		int nFactSize = GetFactSize(lLenIn);
		pCur += nFactSize + sizeof(long);
		
	}
	LeaveCriticalSection(&m_csUsedFlag);
}

long CMsgQueue::GetDataLength()
{
	long lRet = 0;
	//EnterCriticalSection(&m_csUsedFlag);
	lRet = m_pCurAddress - m_pcBuffer;
	if(lRet < 0)
		lRet = 0;
	return lRet;
}
