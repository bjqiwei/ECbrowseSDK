class VSocket;

#include "stdhdrs.h"

#include "VTypes.h"
#include "VSocket.h"

VSocket::VSocket()
{
	m_QueueChannel.Open(524288);
}

VSocket::~VSocket()
{
  Close();
}

VBool
VSocket::Close()
{
	m_QueueChannel.Close();

	return VTrue;
}

VBool
VSocket::SendQueued(const char *buff, const VCard bufflen)
{
	omni_mutex_lock l(queue_lock);
	MemOutStream::writeBytes(buff,bufflen);
	
	return VTrue;
}

VBool VSocket::RecvVncViewerData(const char* pData,int nLen)
{
	unsigned long ulHasRead = 0;
	while(ulHasRead < nLen)
	{
		unsigned long ulLeft = nLen - ulHasRead;
		if(m_QueueChannel.Write((unsigned char*)pData + ulHasRead,ulLeft) == false)
		{
			return VFalse;
		}
		ulHasRead += ulLeft;
	}
	return VTrue;
}

VBool
VSocket::ReadExact(char *buff, const VCard bufflen)
{
	unsigned long ulHasRead = 0;
	while(ulHasRead < bufflen)
	{
		unsigned long ulLeft = bufflen - ulHasRead;
		if(m_QueueChannel.Read((unsigned char*)buff + ulHasRead,ulLeft) == false)
		{
			return VFalse;
		}
		ulHasRead += ulLeft;
	}
	return VTrue;
}