// MediaPack.h: interface for the CMediaPack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEDIAPACK_H__67CF67CF_BCFA_4295_A94D_CE3DEA13590B__INCLUDED_)
#define AFX_MEDIAPACK_H__67CF67CF_BCFA_4295_A94D_CE3DEA13590B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMediaPack  
{
public:
	CMediaPack();
	virtual ~CMediaPack();

	void SetRawData(PBYTE pRawData, int nRawDataLen);
	BOOL GetData(PBYTE pData, int nBufferLen, int& nDataLen);
	BYTE* GetDataPtr(int nBufferLen, int& nDataLen);
	int  GetPackLen();
public:
	int  m_packType;
	int  m_nParam;
	LONGLONG nTimeStart;
	LONGLONG nTimeStop;
	int bIsKeyframe;
	int  Sequence;

private:
	PBYTE				m_pData;
	int					m_nDataLen;

};

#endif // !defined(AFX_MEDIAPACK_H__67CF67CF_BCFA_4295_A94D_CE3DEA13590B__INCLUDED_)
