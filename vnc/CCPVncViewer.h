#pragma once

#include <WTypes.h>
#include "NetComm.h"
#include "AppShareInterface.h"
#include "VncViewerDll.h"
#include <string>
#include "CCPVncDef.h"
#include "Queue.h"
#include <log4cplus/logger.h>
#include "TMutex.h"
typedef enum
{
	EVncViewerState_Normal,
	EVncViewerState_Reqing,
	EVncViewerState_Sharing,
	EVncViewerState_Quiting,
	EVncViewerState_Quited,
}EVncViewerState;

class CCPVncViewerObserver
{
public:
	virtual void OnVncViewerCreateRoom(int roomID, int status)=0;
	virtual void OnVncViewerRoomClosed(int roomID, int status)=0;
	virtual void OnVncViewerJoinRoom(int roomID, int status)=0;
	virtual void OnVncViewerQuitRoom(int roomID, int status)=0;

	virtual void ViewerQuitRoom(int roomID) = 0;

	virtual void OnVncRecvReqRemoteControl(int roomID)=0;
	virtual void OnVncCancelReqRemoteControl(int roomID)=0;
	virtual void OnVncStopRemoteControl(int roomID)=0;
	virtual void OnVncAcceptRemoteControl(int roomID)=0;
	virtual void OnVncRejectRemoteControl(int roomID)=0;

};

using namespace std;
class CCPVncViewer :public IVNCViewerDllNotify
{
public:
	CCPVncViewer(CCPVncViewerObserver *observer);
	~CCPVncViewer(void);

	int initViewer(const char *svrIP, int svrPort, HWND &parent);
	int uninitViewer();

	//�������䣬  ���������ط���id�������ϲ�
	int createShareRoom();

	//���뷿�䣬����Զ��
	int joinShareRoom(int roomID);

	//�˳������Զ��
	int quitShareRoom(int roomID);
	
	//����Զ�̿���
	int reqRemoteControl(int roomID);
	
	//ֹͣ����Զ�̿���
	int stopRemoteControl(int roomID);

	int acceptRemoteControl(int roomID);
	int rejectRemoteControl(int roomID);
		
	//��ȡԶ����ʾ����
	HWND GetViewerWnd();

	//����
	int refreshDesktopWnd(int roomId);

	int RecvDataThreadFunc();
	int ProcessDataThreadFunc();
	
	//from IVNCViewerDllNotify
	virtual void OnVncViewerClose(void);
	virtual void SendVncFrameData(const char* pData,int nDataLen);
private:

	void ProcessRecvData(const char *pRecvData, int nRecvLen);
	unsigned short getDataSequence();
	int sendDataToRUDP(const char *pData, int nDataLen);

	uint32_t HandleVncMsgPacket(VncMsgPacket &msgPacket);
	
	uint32_t HandleVncDataPacket(VncDataPacket &dataPacket);
private:
	IVNCViewerDll *m_ViewerDll;
	int m_RoomID;
	string m_SvrIP;
	int m_SvrPort;
	HWND  m_ParentWnd;
	netcomm::IConnectionFactory *m_ConFactory;
	netcomm::IConnection  *m_RudpConnection;
	bool    m_bReconnectServering;

	bool    m_bExitFromSrv;//�Ƿ�������ർ���˳�

	HANDLE m_RecvThread;
	static unsigned __stdcall Invoke_RecvDataThread(void* args);

	HANDLE m_ProcessThread;
	static unsigned __stdcall Invoke_ProcessDataThread(void* args);
	
	bool m_Recving;
	bool m_Processing;
	bool m_Sending;
	CRITICAL_SECTION	  m_ThreadSection;
	char m_SendBuffer[MAX_SEND_BUFER_LEN];

	VncFrameData *m_CurVncFrame;
	stdvnc::deque<VncFrameData*>  m_RecvQ;
	TMutex          m_csRecvQ;
	unsigned short m_CurDataSeq;

	stdvnc::deque<VncMsgPacket*>  m_RequestQ;
	TMutex          m_csRequestQ;

	VncViewerState m_ViewerState;
	VncRemoteControState m_RCState;
	//VncPacketHeader *m_SendHeader;
	CCPVncViewerObserver *m_Observer;
	log4cplus::Logger log;
};