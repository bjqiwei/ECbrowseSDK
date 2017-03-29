#pragma once

#include <WTypes.h>
#include "NetComm.h"
#include "AppShareInterface.h"
#include "WinVNCDll.h"
#include <string>
#include "Queue.h"
#include "CCPVncDef.h"
#include "TMutex.h"
#include <log4cplus/logger.h>
using namespace std;

typedef enum
{
	EVncServerState_Normal,
	EVncServerState_RecvReqing,
	EVncServerState_Sharing,
	EVncServerState_Quiting
}EVncServerState;

class CCPVncServerObserver
{
public:
	virtual void OnVncServerCreateRoom(int roomID, int status)=0;
	virtual void OnVncServerRoomClosed(int roomID, int status)=0;
	virtual void OnVncServerJoinRoom(int roomID, int status)=0;
	virtual void OnVncServerQuitRoom(int roomID, int status)=0;

	virtual void ServerQuitRoom(int roomID) = 0;
	//virtual void OnVncServerReqRemoteControl(int roomID,  int status) {};
	//virtual void OnVncServerRespRemoteControl(int roomID,  int status) {};

	virtual void OnVncRecvReqRemoteControl(int roomID)=0;
	virtual void OnVncCancelReqRemoteControl(int roomID)=0;
	virtual void OnVncStopRemoteControl(int roomID)=0;
	virtual void OnVncAcceptRemoteControl(int roomID)=0;
	virtual void OnVncRejectRemoteControl(int roomID)=0;
};

class CCPVncServer :public IWinVNCDllNotify
{
public:
	CCPVncServer(CCPVncServerObserver *observer);
	~CCPVncServer(void);

	//�������䣬  ���������ط���id�������ϲ�
	int createShareRoom();
	//���뷿�䣬����Զ��
	int joinShareRoom(int roomID);
	//�˳������Զ��
	int quitShareRoom(int roomID);
	
	void ProcessRecvData(const char *pData, int nDataLen);
	

	//int startShareDesktop(int roomId, const char* svrIP, int svrPort);
	//int stopShareDesktop(int roomId);

	int reqRemoteControl(int roomId);
	int stopRemoteControl(int roomID);

	int acceptRemoteControl(int roomId);
	int rejectRemoteControl(int roomId);

	int CaptureKeyFrame(int roomId);
	
	int setWindowShared(HWND wnd);
	
	int RecvDataThread();
	void SendDataThread();

	int sendDataToRUDP(const char *pRecvData, int nRecvLen);

	virtual void OnVNCFrameData(const char* pFrameData,int nDataLen);
	virtual void OnVNCScreenData(const char* pFrameData,int nDataLen);
	virtual void OnDisplaychanged();

	unsigned short getDataSequence();
	int ProcessDataThread();

	int initServer(int roomID, const char *svrIP, int svrPort);
	int uninitServer();

private:
	uint32_t HandleVncMsgPacket(VncMsgPacket &msgPacket);
	uint32_t HandleVncDataPacket(VncDataPacket &dataPacket);

private:
	IWinVNCDll *m_WinVncDll;
	int m_RoomID;
	string m_SvrIP;
	int m_SvrPort;
	CRITICAL_SECTION	  m_ThreadSection;
	netcomm::IConnectionFactory *m_ConFactory;
	netcomm::IConnection  *m_RudpConnection;
	bool    m_bReconnectServering;

	bool    m_bExitFromSrv;//�Ƿ�������ർ���˳�

	HANDLE m_RecvThread;
	HANDLE m_hKillEventRecv;	
	static unsigned __stdcall Invoke_RecvDataThread(void* args);
	HANDLE m_ProcessThread;
	HANDLE m_hKillEventProcess;
	static unsigned __stdcall Invoke_ProcessDataThread(void* args);
	HANDLE m_SendThread;
	HANDLE m_hKillEventSend;	// Terminate the listen thread	
	static unsigned __stdcall Invoke_SendDataThread(void* args);
	


	bool m_Sending;
	//HANDLE  m_hExitRecvEvent;
	//HANDLE  m_hExitProcessEvent;
	char m_SendBuffer[MAX_SEND_BUFER_LEN];

	VncFrameData *m_CurVncFrame;
	stdvnc::deque<VncFrameData*>  m_RecvQ;
	TMutex          m_csRecvQ;
	unsigned short m_CurDataSeq;

	stdvnc::deque<VncMsgPacket*>  m_RequestQ;
	TMutex          m_csRequestQ;

	stdvnc::deque<VncFrameData*>  m_VncDataSendQ;
	TMutex          m_csVncDataSendQ;

	//VncPacketHeader *m_SendHeader;
	VncServerState m_ServerState;
	VncRemoteControState m_RCState;
	CCPVncServerObserver *m_Observer;
	int     m_nTimeInterval;
	log4cplus::Logger serverlog;
};