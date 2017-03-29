#include "stdafx.h"
#include "CCPVncViewer.h"
#include "Queue.h"
#include "VncUtility.h"
#include "log4cplus/loggingmacros.h"
#include <process.h>

#define MAX_PACKET_SIZE		4*1024		// unit is B


CCPVncViewer::CCPVncViewer(CCPVncViewerObserver *observer)
{
	this->log = log4cplus::Logger::getInstance(("CCPVncViewer"));
	m_Observer = observer;
	m_ViewerDll = NULL;
	m_RoomID = -1;
	m_SvrPort = 0;
	m_CurDataSeq = 0;
	m_ConFactory  = NULL;
	m_RudpConnection = NULL;
	m_bReconnectServering = false;
	m_bExitFromSrv = false;
	m_RecvThread = NULL;
	m_Recving = FALSE;
	m_Processing = FALSE;
	m_CurVncFrame = NULL;
	m_ProcessThread = NULL;
	m_ViewerState = VncViewerState_UnInit;
	LOG4CPLUS_DEBUG(log, __FUNCTION__",ViewerState:VncViewerState_UnInit");
	m_RCState = VncRemoteControlState_Normal;
	LOG4CPLUS_DEBUG(log, __FUNCTION__",RCState:VncRemoteControlState_Normal");
	InitializeCriticalSection(&m_ThreadSection);
	//g_ViewerFile = fopen("d:\\VncViewer.dat", "wb");
}

CCPVncViewer::~CCPVncViewer(void)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	if(m_ViewerState != VncViewerState_UnInit)
	{
		uninitViewer();
	}
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");

	DeleteCriticalSection(&m_ThreadSection);
	//fclose(g_ViewerFile);
}

unsigned __stdcall CCPVncViewer::Invoke_RecvDataThread(void* args)
{
	CCPVncViewer* This = (CCPVncViewer*)args;
	return This->RecvDataThreadFunc();
}

unsigned __stdcall CCPVncViewer::Invoke_ProcessDataThread(void* args)
{
	CCPVncViewer* This = (CCPVncViewer*)args;
	return This->ProcessDataThreadFunc();
}

int CCPVncViewer::RecvDataThreadFunc()
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	char *recvBuff = new char[MAX_PACKET_SIZE];
	int recvLen = 0;
	m_Recving = TRUE;
	while (m_Recving) {
//		EnterCriticalSection(&m_ThreadSection);
		if (m_bReconnectServering)// 断连重连
		{
			if (m_RudpConnection)
			{
				m_RudpConnection->Release();
				m_RudpConnection = NULL;
			}

			netcomm::CLIENTCONNECTIONINFO ConInfo;
			memset(&ConInfo, 0, sizeof(netcomm::CLIENTCONNECTIONINFO));
			ConInfo.nConnectionType = netcomm::CONNECTION_TYPE_RUDP;
			ConInfo.nFlags = 0; //no use proxy
			ConInfo.szHostname = (LPCTSTR)m_SvrIP.c_str();
			ConInfo.nPort = m_SvrPort;
			m_RudpConnection = m_ConFactory->CreateConnection(ConInfo);
			if (m_RudpConnection == NULL)
			{
				::Sleep(50);
				continue;
			}
			if (m_RudpConnection->Connect() < 0) {
				m_RudpConnection->Release();
				m_RudpConnection = NULL;
				::Sleep(50);
				continue;
			}
			m_bReconnectServering = false;
			joinShareRoom(m_RoomID);

		}

		if (m_RudpConnection &&  m_RudpConnection->GetState() == netcomm::CONNECTION_STATE_CONNECTED)
		{
			VncMsgPacket *msgPacket = NULL;
			m_csRequestQ.Lock();
			while(m_RequestQ.Peek(msgPacket) == 0 ) 
			{
				uint8_t data_buffer[1500];
				uint32_t data_len = 0;
				data_len = BuildVncMsgPacket(data_buffer, *msgPacket);
				int ret = m_RudpConnection->Send((char*)data_buffer, data_len);
				if(ret == data_len) 
				{
					m_RequestQ.Get(msgPacket);
					delete msgPacket;
					msgPacket = NULL;
				} 
				else
				{
					break;
				}
			}
			m_csRequestQ.Unlock();
		}
		if (!m_Recving) break;
		if (m_RudpConnection)
		{
			recvLen = m_RudpConnection->Recv(recvBuff, MAX_PACKET_SIZE);
			//	LeaveCriticalSection(&m_ThreadSection);

			if (recvLen > 0) {
				ProcessRecvData(recvBuff, recvLen);
			}
			else if (recvLen == -1)
			{
				m_bReconnectServering = true;
				LOG4CPLUS_TRACE(log, __FUNCTION__ << " RudpConnection disconnect reconnect start.");
				m_RudpConnection->Release();
				m_RudpConnection = NULL;
				::Sleep(50);

			}
			else  {
				::Sleep(1);
			}
		}
		else
			::Sleep(50);

	}
	delete []recvBuff;
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	_endthreadex(0);
	return 0;
}

int CCPVncViewer::ProcessDataThreadFunc()
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	m_Processing = TRUE;
	while(m_Processing)
	{
		VncFrameData * frame = NULL;
		m_csRecvQ.Lock();
		if(m_RecvQ.GetCount() > 0)
		{
			m_RecvQ.Get(frame);
		}
		m_csRecvQ.Unlock();
		if (frame && frame->frameData)
		{

			char *buf = frame->frameData;
			int len =  frame->dataLen;
			{
				m_ViewerDll->OnWinVncFrameData(frame->frameData, frame->dataLen);
				::OutputDebugString("other frame\n");
			}
			
			delete frame->frameData;
			delete frame;
		}
	    else 
		{
		  ::Sleep(10);
		}
	}
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	_endthreadex(0);
	return 0;
}

//创建房间，  服务器返回房间id，返回上层
int CCPVncViewer::createShareRoom()
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	if(m_ViewerState != VncViewerState_Init)
	{
		LOG4CPLUS_ERROR(log, __FUNCTION__ << " viewer in wrong state " << m_ViewerState);
		return -1;
	}

	VncMsgPacket *msgPacket = new VncMsgPacket;
	memset(msgPacket, 0, sizeof(VncMsgPacket));
	msgPacket->header.version = HEADER_VERSION;
	msgPacket->header.clientType = EClientType_VncViewer;
	msgPacket->header.msgType = EMsgReqType_CreateRoom;
	msgPacket->header.msgID = 0;
	msgPacket->status = 0;
	msgPacket->status_description_len = 0;
	m_csRequestQ.Lock();
	m_RequestQ.push_back(msgPacket);
	m_csRequestQ.Unlock();
	m_ViewerState = VncViewerState_RoomCreating;
	LOG4CPLUS_DEBUG(log, __FUNCTION__",ViewerState:VncViewerState_RoomCreating");
	m_RCState = VncRemoteControlState_Normal;
	LOG4CPLUS_DEBUG(log, __FUNCTION__",RCState:VncRemoteControlState_Normal");
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return 0;
}

int CCPVncViewer::quitShareRoom(int roomID)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start..");
	if(m_ViewerState == VncViewerState_Sharing)
	{
		m_ViewerState = VncViewerState_Quiting;	
		LOG4CPLUS_DEBUG(log, __FUNCTION__",ViewerState:VncViewerState_Quiting");
		VncMsgPacket *msgPacket = new VncMsgPacket;
		memset(msgPacket, 0, sizeof(VncMsgPacket));
		msgPacket->header.version = HEADER_VERSION;
		msgPacket->header.clientType = EClientType_VncViewer;
		msgPacket->header.msgType = EMsgReqType_QuitRoom;
		msgPacket->header.roomID = m_RoomID;
		msgPacket->header.msgID = 0;
		msgPacket->status = 0;
		msgPacket->status_description_len = 0;
		m_csRequestQ.Lock();
		m_RequestQ.push_back(msgPacket);
		m_csRequestQ.Unlock();
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " end..");
		return 0;
	}
	LOG4CPLUS_ERROR(log, __FUNCTION__ << " state invalid." << m_ViewerState);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end..");
	return -1;
}

//建立连接, 连接成功
int CCPVncViewer::joinShareRoom(int roomID)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	if (m_ViewerState == VncViewerState_Init || m_ViewerState == VncViewerState_Sharing)
	{
		m_RoomID = roomID;
		VncMsgPacket *msgPacket = new VncMsgPacket;
		memset(msgPacket, 0, sizeof(VncMsgPacket));
		msgPacket->header.version = HEADER_VERSION;
		msgPacket->header.clientType = EClientType_VncViewer;
		msgPacket->header.msgType = EMsgReqType_JoinRoom;
		msgPacket->header.roomID = m_RoomID;
		msgPacket->header.msgID = 0;
		msgPacket->status = 0;
		msgPacket->status_description_len = 0;
		m_csRequestQ.Lock();
		m_RequestQ.push_back(msgPacket);
		m_csRequestQ.Unlock();
		if (m_ViewerState == VncViewerState_Init)
		    m_ViewerState = VncViewerState_RoomJoining;
		LOG4CPLUS_DEBUG(log, __FUNCTION__",ViewerState:VncViewerState_RoomJoining");
		return 0;
	}
	

	//LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return -1;
}

int CCPVncViewer::reqRemoteControl(int roomID)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start..");
	
	if(m_ViewerState != VncViewerState_Sharing) {
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " state invalid." << m_ViewerState);
		return -1;
	}

	if(m_RCState == VncRemoteControlState_Normal) {
		m_RCState = VncRemoteControlState_ReqControling;
		LOG4CPLUS_DEBUG(log, __FUNCTION__",RCState:VncRemoteControlState_ReqControling");
		VncMsgPacket *msgPacket = new VncMsgPacket;
		memset(msgPacket, 0, sizeof(VncMsgPacket));
		msgPacket->header.version = HEADER_VERSION;
		msgPacket->header.clientType = EClientType_VncViewer;
		msgPacket->header.msgType =  EMsgReqType_ReqControl;
		msgPacket->header.roomID = m_RoomID;
		msgPacket->header.msgID = 0;
		msgPacket->status = 1;
		msgPacket->status_description_len = 0;
		m_csRequestQ.Lock();
		m_RequestQ.push_back(msgPacket);
		m_csRequestQ.Unlock();
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end..");
	return 0;
}

int CCPVncViewer::stopRemoteControl(int roomID)
{
	if(m_ViewerState != VncViewerState_Sharing)
	{
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " state invalid." << m_ViewerState);
		return -1;
	}

	if(m_RCState == VncRemoteControlState_Controling || m_RCState == VncRemoteControlState_ReqControling) {
		m_RCState = VncRemoteControlState_Normal;
		LOG4CPLUS_DEBUG(log, __FUNCTION__",RCState:VncRemoteControlState_Normal");
		m_ViewerDll->SetViewOnly(true);
		VncMsgPacket *msgPacket = new VncMsgPacket;
		memset(msgPacket, 0, sizeof(VncMsgPacket));
		msgPacket->header.version = HEADER_VERSION;
		msgPacket->header.clientType = EClientType_VncViewer;
		msgPacket->header.msgType =  EMsgReqType_ReqControl;
		msgPacket->header.roomID = m_RoomID;
		msgPacket->header.msgID = 0;
		msgPacket->status = 0;
		msgPacket->status_description_len = 0;
		m_csRequestQ.Lock();
		m_RequestQ.push_back(msgPacket);
		m_csRequestQ.Unlock();
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end..");
	return 0;
}

int CCPVncViewer::acceptRemoteControl(int roomID)
{
	if(m_RCState != VncRemoteControlState_RecvReqControling)
	{
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " state invalid." << m_RCState);
		return -1;
	}

	if(m_ViewerDll && roomID == m_RoomID) {
		m_RCState = VncRemoteControlState_Controling;
		LOG4CPLUS_DEBUG(log, __FUNCTION__",RCState:VncRemoteControlState_Controling");
		m_ViewerDll->SetViewOnly(false);
		VncMsgPacket *msgPacket = new VncMsgPacket;
		memset(msgPacket, 0, sizeof(VncMsgPacket));
		msgPacket->header.version = HEADER_VERSION;
		msgPacket->header.clientType = EClientType_VncServer;
		msgPacket->header.msgType = EMsgRspType_ReqControl;
		msgPacket->header.roomID = m_RoomID;
		msgPacket->header.msgID = 0;
		msgPacket->status = 200;
		msgPacket->status_description_len = 0;
		m_csRequestQ.Lock();
		m_RequestQ.push_back(msgPacket);
		m_csRequestQ.Unlock();
		return 0;
	}
	return -1;
}
int CCPVncViewer::rejectRemoteControl(int roomID)
{
	if(m_RCState != VncRemoteControlState_RecvReqControling)
	{
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " state invalid." << m_RCState);
		return -1;
	}
	if(m_ViewerDll && roomID == m_RoomID) {
		m_RCState = VncRemoteControlState_Normal;
		LOG4CPLUS_DEBUG(log, __FUNCTION__",RCState:VncRemoteControlState_Normal");
		m_ViewerDll->SetViewOnly(true);
		VncMsgPacket *msgPacket = new VncMsgPacket;
		memset(msgPacket, 0, sizeof(VncMsgPacket));
		msgPacket->header.version = HEADER_VERSION;
		msgPacket->header.clientType = EClientType_VncServer;
		msgPacket->header.msgType = EMsgRspType_ReqControl;
		msgPacket->header.roomID = m_RoomID;
		msgPacket->header.msgID = 0;
		msgPacket->status = 603;
		msgPacket->status_description_len = 0;
		m_csRequestQ.Lock();
		m_RequestQ.push_back(msgPacket);
		m_csRequestQ.Unlock();
		return 0;
	}
	return -1;
}

int CCPVncViewer::initViewer(const char *svrIP, int svrPort, HWND &parent)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start..");
	if(m_ViewerState != VncViewerState_UnInit)
	{
		LOG4CPLUS_ERROR(log, __FUNCTION__ << " viewer in wrong state " << m_ViewerState);
		return -1;
	}
	m_ParentWnd = parent;
	m_ViewerDll = CreateAppViewer(m_ParentWnd, *this);	
	if(!m_ViewerDll)
	{
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " CreateAppViewer failed.");
		return -2;
	}

	m_ViewerDll->SetViewOnly(true);
	m_ViewerDll->SetRunning(true);

	m_ConFactory = CreateConnectionFactory();
	if(!m_ConFactory)
	{
		StopAppViewer();	
		m_ViewerDll = NULL;
		LOG4CPLUS_ERROR(log, __FUNCTION__ << " CreateConnectionFactory  failed");
		return -3;
	}
	m_SvrIP = svrIP;
	m_SvrPort = svrPort;

	netcomm::CLIENTCONNECTIONINFO ConInfo;
	memset(&ConInfo, 0, sizeof(netcomm::CLIENTCONNECTIONINFO));
	ConInfo.nConnectionType = netcomm::CONNECTION_TYPE_RUDP;
	ConInfo.nFlags  = 0; //no use proxy
	ConInfo.szHostname = (LPCTSTR)svrIP;
	ConInfo.nPort = svrPort;
	m_RudpConnection =  m_ConFactory->CreateConnection(ConInfo);
	if(!m_RudpConnection)
	{
		StopAppViewer();	
		m_ViewerDll = NULL;
		m_ConFactory->Release();
		m_ConFactory = NULL;
		LOG4CPLUS_ERROR(log, __FUNCTION__ << "CreateConnection  failed");
		return -4;
	}
	if(m_RudpConnection->Connect() < 0) {
		StopAppViewer();	
		m_ViewerDll = NULL;
		m_RudpConnection->Release();
		m_RudpConnection = NULL;
		m_ConFactory->Release();
		m_ConFactory = NULL;
		LOG4CPLUS_ERROR(log, __FUNCTION__ << "Connect  failed");
		return -5;
	}

	//HWND viewerWnd  = (HWND)m_ViewerDll->GetViewerWnd();
	//if (viewerWnd)
	//{
	//	//GetClientRect(m_ParentWnd, &r);
	//	::MoveWindow(viewerWnd, 0, 0, 1000, 800, TRUE);
	//}

	unsigned threadID;
	m_RecvThread = (HANDLE)_beginthreadex(NULL, 0, &Invoke_RecvDataThread, this, 0, &threadID);
	m_ProcessThread = (HANDLE)_beginthreadex(NULL, 0, &Invoke_ProcessDataThread, this, 0, &threadID);
	m_ViewerState = VncViewerState_Init;
	m_bExitFromSrv = false;
	char buf[10];
	int recvLen = m_RudpConnection->Recv(buf, 10);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " ViewerState:VncViewerState_Init. " << recvLen);

//	LOG4CPLUS_DEBUG(log, __FUNCTION__",ViewerState:VncViewerState_Init");
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return 0;
}

int CCPVncViewer::uninitViewer()
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start..");
	EnterCriticalSection(&m_ThreadSection);

	if(m_ViewerState == VncViewerState_UnInit)
	{
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " state invalid. " << m_ViewerState);
		return -1;
	}
	m_ViewerState  = VncViewerState_UnInit;
	LOG4CPLUS_DEBUG(log, __FUNCTION__",ViewerState:VncViewerState_UnInit");

	m_Recving = false;
	m_Processing = false;

	WaitForSingleObject(m_RecvThread, INFINITE);
	WaitForSingleObject(m_ProcessThread, INFINITE);
	
	if(m_ViewerDll)
		StopAppViewer();
	m_ViewerDll = NULL;

	if(m_RudpConnection)
		m_RudpConnection->Release();
	m_RudpConnection = NULL;
	if(m_ConFactory)
		m_ConFactory->Release();
	m_ConFactory = NULL;

	VncMsgPacket * msgPacket;
	m_csRequestQ.Lock();
	while(!m_RequestQ.Get(msgPacket))
	{
		delete msgPacket;
	}
	m_csRequestQ.Unlock();
	LeaveCriticalSection(&m_ThreadSection);

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end..");
	return 0;
}

int CCPVncViewer::refreshDesktopWnd(int roomId)
{
	if(m_RoomID != roomId) {
		return -1;
	}
	
	m_ViewerDll->RequestFullScreenUpdate();
	return 0;
}

HWND CCPVncViewer::GetViewerWnd()
{
	if(m_ViewerDll)
		return (HWND)m_ViewerDll->GetViewerWnd();
	return NULL;
}

//viewer
void CCPVncViewer::OnVncViewerClose(void)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__);
}

void CCPVncViewer::SendVncFrameData(const char* pData,int nDataLen)
{
	if(nDataLen > 0 && m_ViewerState == VncViewerState_Sharing)
		sendDataToRUDP(pData, nDataLen);
}

unsigned short CCPVncViewer::getDataSequence()
{
	return ++m_CurDataSeq;
}

int CCPVncViewer::sendDataToRUDP(const char *pData, int nDataLen)
{
	if(m_RudpConnection && !m_bReconnectServering) {
		int alreadySendLen = 0;
		unsigned short dataSeq = getDataSequence();
		int sendFailedTimes = 0;

		while (m_RudpConnection && alreadySendLen < nDataLen && m_Sending  && sendFailedTimes < 300)
		{
			VncDataPacket dataPacket;
			memset(&dataPacket, 0, sizeof(VncDataPacket));

			dataPacket.header.version = HEADER_VERSION;
			dataPacket.header.clientType = EClientType_VncViewer;
			dataPacket.header.msgType = EMsgReqType_VncData;
			dataPacket.header.roomID = m_RoomID;
			dataPacket.header.msgID = 0;
						
			dataPacket.total_len = nDataLen;
			dataPacket.packet_seq = dataSeq;
			if(alreadySendLen == 0)
				dataPacket.begin_mark = 1;

			if( (nDataLen-alreadySendLen) > MAX_SEND_DATA_LEN) {
				dataPacket.payload_len = MAX_SEND_DATA_LEN;
			} else {
				dataPacket.end_mark = 1;
				dataPacket.payload_len = (nDataLen - alreadySendLen);
			}
			memcpy(dataPacket.payload, (unsigned char *)pData+alreadySendLen, dataPacket.payload_len);

			int sendLen = BuildVncDataPacket((unsigned char *)m_SendBuffer,  dataPacket);	

	//		EnterCriticalSection(&m_ThreadSection);
			int ret = m_RudpConnection->Send(m_SendBuffer, sendLen);
		//	LeaveCriticalSection(&m_ThreadSection);

			if (ret == sendLen) {
				alreadySendLen += dataPacket.payload_len;
			} else {
				sendFailedTimes++;
				std::ostringstream erro ;
				erro << "sendDataToRUDP sendFailedTimes="<< sendFailedTimes;
				LOG4CPLUS_ERROR(log, __FUNCTION__ <<  erro.str());
			}
		}

		//std::ostringstream erro ;
		//erro << " sendDataToRUDP dataSeq=" << dataSeq << " dataLen="<< nDataLen << " alreadySend="<< alreadySendLen << "failedTimes=" << sendFailedTimes;
		//LOG4CPLUS_TRACE(log, __FUNCTION__ <<  erro.str());
	}
	return 0;
}

uint32_t CCPVncViewer::HandleVncMsgPacket(VncMsgPacket &msgPacket)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start..");
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " msgType = " <<  (int)msgPacket.header.msgType << " status=" << msgPacket.status);

	//EnterCriticalSection(&m_ThreadSection);

	switch(msgPacket.header.msgType)
	{
	case EMsgRspType_CreatRoom:
		{
			if(m_ViewerState != VncViewerState_RoomCreating)
			{
				LOG4CPLUS_TRACE(log, __FUNCTION__ << " Viewer in wrong state " << m_ViewerState);
				break;
			}

			if(msgPacket.status == 0  && msgPacket.header.roomID > 0)
			{
				m_RoomID = msgPacket.header.roomID;
				m_Sending = TRUE;
				m_ViewerState = VncViewerState_Sharing;
				LOG4CPLUS_DEBUG(log, __FUNCTION__",ViewerState:VncViewerState_Sharing");
				if(m_Observer)
					m_Observer->OnVncViewerCreateRoom(m_RoomID, msgPacket.status);
			}
			else
			{
				m_ViewerState = VncViewerState_Init;
				LOG4CPLUS_DEBUG(log, __FUNCTION__",ViewerState:VncViewerState_Init");
				if(m_Observer)
					m_Observer->OnVncViewerCreateRoom(0, msgPacket.status);
			}				
		}
		break;
	case EMsgRspType_RoomClosed:
		{
			m_ViewerState = VncViewerState_Init;
			LOG4CPLUS_DEBUG(log, __FUNCTION__",ViewerState:VncViewerState_Init");
			if(m_Observer)
				m_Observer->OnVncViewerRoomClosed(m_RoomID, msgPacket.status);
			m_RoomID = 0;
		}
		break;
	case EMsgRspType_JoinRoom:
		{
			if(m_ViewerState != VncViewerState_RoomJoining)
			{
				LOG4CPLUS_TRACE(log, __FUNCTION__ << " Viewer in wrong state " << m_ViewerState);
				break;
			}

			if(msgPacket.status == 0 && msgPacket.header.roomID == m_RoomID)
			{
				m_Sending = TRUE;
				if (m_ViewerState != VncViewerState_Sharing)
				{
					m_ViewerState = VncViewerState_Sharing;
					LOG4CPLUS_DEBUG(log, __FUNCTION__",ViewerState:VncViewerState_Sharing");
					if (m_Observer)
						m_Observer->OnVncViewerJoinRoom(m_RoomID, msgPacket.status);
				}
			}
			else
			{
				m_ViewerState = VncViewerState_Init;
				LOG4CPLUS_DEBUG(log, __FUNCTION__",ViewerState:VncViewerState_Init");
				if(m_Observer)
					m_Observer->OnVncViewerJoinRoom(m_RoomID, msgPacket.status);
				m_RoomID = 0;
			}
		}
		break;
	case EMsgRspType_QuitRoom:
		{
			if(m_ViewerState != VncViewerState_Quiting)
			{
				LOG4CPLUS_TRACE(log, __FUNCTION__ << " Viewer in wrong state " << m_ViewerState);
				break;
			}
			m_ViewerState = VncViewerState_Init;
			LOG4CPLUS_DEBUG(log, __FUNCTION__",ViewerState:VncViewerState_Init");
			if(m_Observer)
				m_Observer->OnVncViewerQuitRoom(m_RoomID, msgPacket.status);
			m_RoomID = 0;
		}
		break;
	case EMsgRspType_ExitRoom://会议的其他成员退出了,在2人会议模式下也退出
		{
			LOG4CPLUS_TRACE(log, __FUNCTION__ << " EMsgRspType_ExitRoom.");
			m_bExitFromSrv = true;
			if (m_Observer)
				m_Observer->ViewerQuitRoom(m_RoomID);
		}
		break;
	case EMsgReqType_ReqControl:
		{
			LOG4CPLUS_TRACE(log, __FUNCTION__ << " EMsgReqType_ReqControl.");
			if(m_RCState == VncRemoteControlState_Normal)
			{
				if(msgPacket.status == 1)
				{
					m_RCState = VncRemoteControlState_RecvReqControling;
					LOG4CPLUS_DEBUG(log, __FUNCTION__",RCState:VncRemoteControlState_RecvReqControling");
					if(m_Observer)
						m_Observer->OnVncRecvReqRemoteControl(m_RoomID);
				}
			}
			if(m_RCState == VncRemoteControlState_RecvReqControling)
			{
				if(msgPacket.status == 0)
				{
					m_RCState = VncRemoteControlState_Normal;
					LOG4CPLUS_DEBUG(log, __FUNCTION__",RCState:VncRemoteControlState_Normal");
					if(m_Observer)
						m_Observer->OnVncCancelReqRemoteControl(m_RoomID);
				}
			}
			if(m_RCState == VncRemoteControlState_Controling)
			{
				if(msgPacket.status == 0)
				{
					m_RCState = VncRemoteControlState_Normal;
					LOG4CPLUS_DEBUG(log, __FUNCTION__",RCState:VncRemoteControlState_Normal");
					if(m_Observer)
						m_Observer->OnVncStopRemoteControl(m_RoomID);
					m_ViewerDll->SetViewOnly(true);
				}
			}
		}
		break;
	case EMsgRspType_ReqControl:
		{
			LOG4CPLUS_TRACE(log, __FUNCTION__ << " EMsgRspType_ReqControl.");
			if(m_RCState == VncRemoteControlState_ReqControling)
			{
				if(msgPacket.status == 200)
				{
					m_RCState = VncRemoteControlState_Controling;
					LOG4CPLUS_DEBUG(log, __FUNCTION__",RCState:VncRemoteControlState_Controling");
					m_ViewerDll->SetViewOnly(false);
					if(m_Observer)
						m_Observer->OnVncAcceptRemoteControl(m_RoomID);
				}
				else if(msgPacket.status == 603)
				{
					m_RCState = VncRemoteControlState_Normal;
					LOG4CPLUS_DEBUG(log, __FUNCTION__",RCState:VncRemoteControlState_Normal");
					m_ViewerDll->SetViewOnly(true);
					if(m_Observer)
						m_Observer->OnVncRejectRemoteControl(m_RoomID);
				}
			}
		}
		break;
	}
	//LeaveCriticalSection(&m_ThreadSection);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end..");
	return 0;
}

uint32_t CCPVncViewer::HandleVncDataPacket(VncDataPacket &dataPacket)
{
	if(dataPacket.header.roomID != m_RoomID)
	{
		LOG4CPLUS_ERROR(log, __FUNCTION__ <<  "HandleVncDataPacket wrong room id");
		return -1;
	}
	
	if( ( !m_CurVncFrame  || (dataPacket.packet_seq != m_CurVncFrame->packetSeq) )
		&& dataPacket.begin_mark) 
	{
		if(m_CurVncFrame)
		{
			std::ostringstream erro ;
			erro << "delete cur Vnc Frame seq=" << m_CurVncFrame->packetSeq << " totalLen="<< m_CurVncFrame->totalLen << "curLen="<< (uint32_t)m_CurVncFrame->dataLen;
			LOG4CPLUS_TRACE(log, __FUNCTION__ <<  erro.str());
			delete m_CurVncFrame;
		}

		m_CurVncFrame = new VncFrameData;
		memset(m_CurVncFrame, 0, sizeof(VncFrameData));
		m_CurVncFrame->frameData = new char[dataPacket.total_len];
		if(!m_CurVncFrame->frameData)
		{
			delete m_CurVncFrame;
			m_CurVncFrame = NULL;
			return -1;
		}
		m_CurVncFrame->totalLen = dataPacket.total_len;
		m_CurVncFrame->dataLen = 0;
		m_CurVncFrame->packetSeq = dataPacket.packet_seq;
	}

	if(m_CurVncFrame)
	{
		int vncDataLen = dataPacket.payload_len;
		if( (m_CurVncFrame->dataLen + vncDataLen) <= m_CurVncFrame->totalLen ) {
			memcpy(m_CurVncFrame->frameData+m_CurVncFrame->dataLen, dataPacket.payload, vncDataLen);
			m_CurVncFrame->dataLen += vncDataLen;
		}
		if(dataPacket.end_mark && m_CurVncFrame->dataLen == dataPacket.total_len)
		{
			m_csRecvQ.Lock();
			m_RecvQ.push_back(m_CurVncFrame);
			m_csRecvQ.Unlock();
			m_CurVncFrame = NULL;
			::OutputDebugString("FULL FRAME\n");
		}
	}
	return 0;
}

void CCPVncViewer::ProcessRecvData(const char *pRecvData, int nRecvLen)
{
	VncPacketHeader packetHeader;
	if( ParseVncPacketHeader(packetHeader, (unsigned char *)pRecvData, nRecvLen) < 0 )
	{
		LOG4CPLUS_ERROR(log, __FUNCTION__ <<  "ParseVncPacketHeader failed");
		return;
	}

	if(packetHeader.msgType == EMsgReqType_VncData)
	{
		VncDataPacket dataPacket;
		if( ParseVncDataPacket(dataPacket, (unsigned char *)pRecvData, nRecvLen) < 0) 
		{
			LOG4CPLUS_ERROR(log, __FUNCTION__ <<  "ParseVncDataPacket failed");
			return;
		}
		HandleVncDataPacket(dataPacket);
	}
	else
	{
		VncMsgPacket msgPacket;
		if( ParseVncMsgPacket(msgPacket, (unsigned char *)pRecvData, nRecvLen) < 0 )
			return;
		HandleVncMsgPacket(msgPacket);
	}
}
