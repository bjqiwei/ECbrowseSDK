class VSocket;

#if (!defined(_ATT_VSOCKET_DEFINED))
#define _ATT_VSOCKET_DEFINED

#include <omnithread.h>
#include "VTypes.h"
#include "MemOutStream.h"
#include "QueueChannel.h"

using namespace rdr;
// The main socket class
class VSocket 
	: public MemOutStream
{
public:
  VSocket();
  virtual ~VSocket();

  VBool Close();
  VBool ReadExact(char *buff, const VCard bufflen);
  VBool SendQueued(const char *buff, const VCard bufflen);
  VBool RecvVncViewerData(const char* pData,int nLen);
 
protected:
  omni_mutex queue_lock;
  CQueueChannel	m_QueueChannel;
};

#endif // _ATT_VSOCKET_DEFINED
