// MediaPack.cpp: implementation of the CMediaPack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MediaPack.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define MAX_PACKET_LENGTH 1024*3 
CMediaPack::CMediaPack()
: m_pData(NULL), m_nDataLen(0)
{
	m_packType = 0;
	m_nParam = 0;
	nTimeStart = 0;
	nTimeStop = 0;
	bIsKeyframe = true;
	Sequence = 0;
	try
	{
		m_pData = new BYTE[MAX_PACKET_LENGTH];		
	}
	catch(...)
	{
		throw "new Packetd fail!";
	}
}

CMediaPack::~CMediaPack()
{
	if (m_pData)
	{
		delete [] m_pData;
		m_pData = NULL;
	}
}

void CMediaPack::SetRawData(PBYTE pRawData, int nRawDataLen)
{
	if (nRawDataLen <=0) return ;
	if (nRawDataLen >MAX_PACKET_LENGTH )
	{
		if (m_pData)
		{
			delete [] m_pData;
			m_pData = NULL;
		}
		try
		{
			m_pData = new BYTE[nRawDataLen];		
		}
		catch(...)
		{
			throw "new Packetd fail!";
		}
	}
	CopyMemory(m_pData, pRawData, nRawDataLen);
	m_nDataLen = nRawDataLen;
}
BOOL CMediaPack::GetData(PBYTE pData, int nBufferLen, int& nDataLen)
{
	nDataLen = 0 ;
	if (nBufferLen < m_nDataLen)
		return FALSE;
	nDataLen = m_nDataLen ;
	CopyMemory(pData, m_pData , m_nDataLen);
	return TRUE;
}
BYTE* CMediaPack::GetDataPtr(int nBufferLen, int& nDataLen)
{
	nDataLen = m_nDataLen ;
	if (nBufferLen < nDataLen)
		nDataLen = 0;
	if (!m_pData) return NULL;
	return m_pData;
}
int CMediaPack::GetPackLen()
{
	return m_nDataLen;
}