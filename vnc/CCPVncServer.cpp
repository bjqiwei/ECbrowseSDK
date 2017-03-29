#include "stdafx.h"
#include "CCPVncServer.h"
#include "CCPVncDef.h"
#include "VncUtility.h"
#include "log4cplus/loggingmacros.h"
#include <process.h>

#define MAX_PACKET_SIZE		4*1024	 // unit is B


CCPVncServer::CCPVncServer(CCPVncServerObserver *observer)
{
	serverlog = log4cplus::Logger::getInstance(("CCPVncServer"));
	m_Observer = observer;
	m_WinVncDll = NULL;
	m_RoomID = -1;
	m_SvrPort = 0;
	m_ConFactory  = NULL;
	m_RudpConnection = NULL;
	m_bReconnectServering = false;

	m_RecvThread = NULL;
	m_ProcessThread = NULL;
	m_SendThread = NULL;

	m_hKillEventSend = NULL;
	m_hKillEventRecv = NULL;
	m_hKillEventProcess = NULL;

	m_bExitFromSrv = false;
	m_Sending = FALSE;
	m_CurDataSeq = 0;
	m_CurVncFrame = NULL;
	m_ServerState = VncServerState_UnInit;
	m_RCState = VncRemoteControlState_Normal;
	InitializeCriticalSection(&m_ThreadSection);
}

CCPVncServer::~CCPVncServer(void)
{
	LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " start.");
	if(m_ServerState != VncServerState_UnInit) {
		uninitServer();
	}
	LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " end.");
	DeleteCriticalSection(&m_ThreadSection);

}

unsigned __stdcall CCPVncServer::Invoke_RecvDataThread(void* args)
{
	CCPVncServer* This = (CCPVncServer*)args;
	return This->RecvDataThread();
}
unsigned __stdcall CCPVncServer::Invoke_ProcessDataThread(void* args)
{
	CCPVncServer* This = (CCPVncServer*)args;
	return This->ProcessDataThread();
}
unsigned __stdcall CCPVncServer::Invoke_SendDataThread(void* args)
{
	CCPVncServer* This = (CCPVncServer*)args;
	This->SendDataThread();
	return 0;
}
void CCPVncServer::SendDataThread()
{
	int count = 3;
	while(WaitForSingleObject(m_hKillEventSend,3))
	{
		try
		{
			if (m_RudpConnection && !m_bReconnectServering)
			{
				// send cmd data
				m_csRequestQ.Lock();
				if (m_RudpConnection->GetState() == netcomm::CONNECTION_STATE_CONNECTED
					&& m_RequestQ.GetCount() > 0)
				{
					VncMsgPacket *msgPacket = NULL;
					uint8_t data_buffer[1500];
					uint32_t data_len = 0;
					count = 10;
					while (m_RequestQ.Peek(msgPacket) == 0 && --count>0)
					{
						data_len = BuildVncMsgPacket(data_buffer, *msgPacket);
						int ret = m_RudpConnection->Send((char*)data_buffer, data_len);
						if (ret == data_len) {
							m_RequestQ.Get(msgPacket);
							delete msgPacket;
							msgPacket = NULL;
						}
						else
						{
							break;
						}
					}
				}
				m_csRequestQ.Unlock();

				//send vnc data
				VncFrameData *vncFrame = NULL;
				uint8_t data_buffer[1500];
				uint32_t data_len = 0;
				count = 30;
				while (m_VncDataSendQ.Peek(vncFrame) == 0 && --count>0 && m_Sending)
				{
					if (vncFrame)
					{
						if (sendDataToRUDP(vncFrame->frameData, vncFrame->totalLen) == 0)
						{
							m_csVncDataSendQ.Lock();
							m_VncDataSendQ.Get(vncFrame);
							delete vncFrame;
							m_csVncDataSendQ.Unlock();
						}
						else
						{
							break;
						}
					}
				}
			}

		}
		catch (...)
		{
			;
		}
	}

	LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " end.");

}
int CCPVncServer::RecvDataThread()
{
	LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " start.");
	char recvBuff[MAX_PACKET_SIZE];
	int recvLen = 0;
	while(WaitForSingleObject(m_hKillEventRecv,3))
	{
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
		try
		{
			if (m_RudpConnection)
			{
				recvLen = m_RudpConnection->Recv(recvBuff, MAX_PACKET_SIZE);
				if (recvLen > 0)
					ProcessRecvData(recvBuff, recvLen);
				else if (recvLen == -1)
				{
					m_bReconnectServering = true;
					LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " vncserver RudpConnection disconnect reconnect start.");
					m_RudpConnection->Release();
					m_RudpConnection = NULL;
					::Sleep(50);

				}
			}
		}
		catch (...)
		{
			;
		}
	}

	/*while(m_Recving) {
		EnterCriticalSection(&m_ThreadSection);

		if( m_RudpConnection->GetState() == netcomm::CONNECTION_STATE_CONNECTED
			&& m_RequestQ.GetCount() > 0 ) {
			VncMsgPacket *msgPacket = NULL;
			uint8_t data_buffer[1500];
			uint32_t data_len = 0;
			while(m_RequestQ.Peek(msgPacket) == 0 ) {
				data_len = BuildVncMsgPacket(data_buffer, *msgPacket);
				int ret = m_RudpConnection->Send((char*)data_buffer, data_len);
				if(ret == data_len) {
					m_RequestQ.Get(msgPacket);
					delete msgPacket;
					msgPacket = NULL;
				} else {
					break;
				}
			}
		}
		
		recvLen = m_RudpConnection->Recv(recvBuff, MAX_PACKET_SIZE);
		LeaveCriticalSection(&m_ThreadSection);

		if (recvLen > 0) {
			ProcessRecvData(recvBuff, recvLen);
		}
		else 
		{
			::Sleep(5);
		}

	}*/

	LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " end.");
	return 0;
}

int CCPVncServer::ProcessDataThread()
{
	LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " start.");
	while(WaitForSingleObject(m_hKillEventProcess,3))
	{
		try
		{
			m_csRecvQ.Lock();
			if(m_RecvQ.GetCount() > 0) {
				VncFrameData * frame = NULL;
				if( m_RecvQ.Get(frame) == 0 )
				{
					if(frame->frameData) 
					{
						m_WinVncDll->RecvVncViewerData(frame->frameData, frame->dataLen);
						//fwrite(frame->frameData, 1, frame->dataLen, g_ServerFile);  //for test
					}
					delete frame->frameData;
					delete frame;
				}
			} 
			m_csRecvQ.Unlock();

		}
		catch (...)
		{
			;
		}

	/*	if (GetTickCount() - m_nTimeInterval > 10000)//10秒一个关键帧
		{
			m_nTimeInterval = GetTickCount();
			CaptureKeyFrame(m_RoomID);
		}*/
	}

/*	while(m_Processing)
	{
		m_csRecvQ.Lock();
		if(m_RecvQ.GetCount() > 0) {
			VncFrameData * frame = NULL;
			if( m_RecvQ.Get(frame) == 0 )
			{
				if(frame->frameData) 
				{
					m_WinVncDll->RecvVncViewerData(frame->frameData, frame->dataLen);
					//fwrite(frame->frameData, 1, frame->dataLen, g_ServerFile);  //for test
				}
				delete frame->frameData;
				delete frame;
			}
		} else {
			::Sleep(100);
		}
		m_csRecvQ.Unlock();
	}*/
	LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " end.");
	return 0;
}

int CCPVncServer::initServer(int roomID, const char *svrIP, int svrPort)
{
	LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " start.");

	LOG4CPLUS_DEBUG(serverlog, "servrIP:" << svrIP << ",svrPort:" << svrPort);
	if(m_ServerState != VncServerState_UnInit)
	{
		LOG4CPLUS_ERROR(serverlog, __FUNCTION__ << " server in wrong state " << m_ServerState);
		return -1;
	}

	m_WinVncDll = CreateAppServer(*this);
	if(!m_WinVncDll)
	{
		LOG4CPLUS_ERROR(serverlog, __FUNCTION__ << " CreateAppServer failed.");
		return -2;
	}

	m_ConFactory = CreateConnectionFactory();
	if(!m_ConFactory) {
		StopAppServer();
		m_WinVncDll = NULL;
		LOG4CPLUS_ERROR(serverlog, __FUNCTION__ << " CreateConnectionFactory failed.");
		return -3;
	}

	netcomm::CLIENTCONNECTIONINFO ConInfo;
	memset(&ConInfo, 0, sizeof(netcomm::CLIENTCONNECTIONINFO));
	ConInfo.nConnectionType = netcomm::CONNECTION_TYPE_RUDP;
	ConInfo.nFlags  = 0; //no use proxy
	ConInfo.szHostname = (LPCTSTR)svrIP;
	ConInfo.nPort = svrPort;
	m_RudpConnection =  m_ConFactory->CreateConnection(ConInfo);
	if(!m_RudpConnection)
	{
		StopAppServer();
		m_WinVncDll = NULL;
		m_ConFactory->Release();
		m_ConFactory = NULL;
		LOG4CPLUS_ERROR(serverlog, __FUNCTION__ << " CreateConnection failed.");
		return -4;
	}
	if(m_RudpConnection->Connect() < 0) {
		StopAppServer();
		m_WinVncDll = NULL;
		m_RudpConnection->Release();
		m_RudpConnection = NULL;
		m_ConFactory->Release();
		m_ConFactory = NULL;
		LOG4CPLUS_ERROR(serverlog, __FUNCTION__ << " Connect failed.");
		return -5;
	}

	m_hKillEventRecv = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hKillEventProcess =  CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hKillEventSend = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(m_hKillEventSend == INVALID_HANDLE_VALUE || m_hKillEventProcess == INVALID_HANDLE_VALUE || m_hKillEventRecv == INVALID_HANDLE_VALUE )
	{
		return -4 ;
	}
	m_SvrIP = svrIP;
	m_SvrPort = svrPort;
	m_RecvThread = (HANDLE)_beginthreadex(NULL, 0, &Invoke_RecvDataThread, this, 0, NULL);
	m_ProcessThread = (HANDLE)_beginthreadex(NULL, 0, &Invoke_ProcessDataThread, this, 0, NULL);
	m_SendThread = (HANDLE)_beginthreadex(NULL, 0, &Invoke_SendDataThread, this, 0, NULL);

	m_nTimeInterval = GetTickCount();

	m_bExitFromSrv = false;
	m_ServerState = VncServerState_Init;
	LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " end.");
	return 0;
}

int CCPVncServer::uninitServer()
{
	LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " start.");

//	EnterCriticalSection(&m_ThreadSection);

	if (m_ServerState == VncServerState_UnInit)
	{
		LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " state invalid. " << m_ServerState);
		return -1;
	}
	m_ServerState  = VncServerState_UnInit;
	m_RCState = VncRemoteControlState_Normal;

	m_Sending = FALSE;

	if (m_hKillEventSend)
	{
		SetEvent(m_hKillEventSend);
		WaitForSingleObject(m_SendThread, 10000);
		CloseHandle(m_SendThread);
		CloseHandle(m_hKillEventSend);
		m_SendThread = NULL;
		m_hKillEventSend = NULL;
	}
	if (m_hKillEventRecv)
	{
		SetEvent(m_hKillEventRecv);
		WaitForSingleObject(m_RecvThread, 10000);
		CloseHandle(m_RecvThread);
		CloseHandle(m_hKillEventRecv);
		m_RecvThread = NULL;
		m_hKillEventRecv = NULL;
	}
	if (m_hKillEventProcess)
	{
		SetEvent(m_hKillEventProcess);
		WaitForSingleObject(m_ProcessThread, 10000);
		CloseHandle(m_ProcessThread);
		CloseHandle(m_hKillEventProcess);
		m_ProcessThread = NULL;
		m_hKillEventProcess = NULL;
	}

	if(m_WinVncDll)
		StopAppServer();
	m_WinVncDll = NULL;


	if(m_RudpConnection)
		m_RudpConnection->Release();
	m_RudpConnection = NULL;

//	LeaveCriticalSection(&m_ThreadSection);
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

	VncFrameData* fData;
	m_csVncDataSendQ.Lock();
	while(!m_VncDataSendQ.Get(fData))
	{
		delete fData;
	}
	m_csVncDataSendQ.Unlock();

	m_csRecvQ.Lock();
	while(!m_RecvQ.Get(fData))
	{
		delete fData;
	}
	m_csRecvQ.Unlock();


	LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " end.");
	return 0;
}

//bool CCPVncServer::isVncServerInit()
//{
//	return (m_ServerState != VncServerState_UnInit);
//}


int CCPVncServer::createShareRoom()
{
	LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " start.");
	if(m_ServerState != VncServerState_Init)
	{
		LOG4CPLUS_ERROR(serverlog, __FUNCTION__ << " server in wrong state " << m_ServerState);
		return -1;
	}

	m_ServerState = VncServerState_Creating;
	VncMsgPacket *msgPacket = new VncMsgPacket;
	memset(msgPacket, 0, sizeof(VncMsgPacket));
	msgPacket->header.version = HEADER_VERSION;
	msgPacket->header.clientType = EClientType_VncServer;
	msgPacket->header.msgType = EMsgReqType_CreateRoom;
	msgPacket->header.msgID = 0;
	msgPacket->status = 0;
	msgPacket->status_description_len = 0;
	m_csRequestQ.Lock();
	m_RequestQ.push_back(msgPacket);	
	m_csRequestQ.Unlock();
	return 0;
}

int CCPVncServer::joinShareRoom(int roomID)
{
	LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " start..");
	if (m_ServerState == VncServerState_Init || m_ServerState == VncServerState_Sharing)
	{
		LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " roomID = " << roomID);
		m_ServerState = VncServerState_Joining;
		m_RoomID = roomID;
		VncMsgPacket *msgPacket = new VncMsgPacket;
		memset(msgPacket, 0, sizeof(VncMsgPacket));
		msgPacket->header.version = HEADER_VERSION;
		msgPacket->header.clientType = EClientType_VncServer;
		msgPacket->header.msgType = EMsgReqType_JoinRoom;
		msgPacket->header.roomID = roomID;
		msgPacket->header.msgID = 0;
		msgPacket->status = 0;
		msgPacket->status_description_len = 0;
		m_csRequestQ.Lock();
		m_RequestQ.push_back(msgPacket);
		m_csRequestQ.Unlock();
		LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " end.");
		return 0;
	}
	
	return -1;
}

int CCPVncServer::quitShareRoom(int roomID)
{
	LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " start.");
	if(m_ServerState == VncServerState_Sharing)
	{
		VncMsgPacket *msgPacket = new VncMsgPacket;
		memset(msgPacket, 0, sizeof(VncMsgPacket));
		msgPacket->header.version = HEADER_VERSION;
		msgPacket->header.clientType = EClientType_VncServer;
		msgPacket->header.msgType = EMsgReqType_QuitRoom;
		msgPacket->header.roomID = m_RoomID;
		msgPacket->header.msgID = 0;
		msgPacket->status = 0;
		msgPacket->status_description_len = 0;
		m_csRequestQ.Lock();
		m_RequestQ.push_back(msgPacket);
		m_csRequestQ.Unlock();
		LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " end.");
		return 0;
	}
	LOG4CPLUS_ERROR(serverlog, __FUNCTION__ << " state invalid." << m_ServerState);
	LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " end.");
	return -1;
}

void CCPVncServer::ProcessRecvData(const char *pRecvData, int nRecvLen)
{
	VncPacketHeader packetHeader;
	if( ParseVncPacketHeader(packetHeader, (unsigned char *)pRecvData, nRecvLen) < 0 )
		return;

	//LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " msgType = " << (int)packetHeader.msgType);

	if(packetHeader.msgType == EMsgReqType_VncData)
	{
		VncDataPacket dataPacket;
		if( ParseVncDataPacket(dataPacket, (unsigned char *)pRecvData, nRecvLen) < 0)
			return;
		HandleVncDataPacket(dataPacket);
	}
	else 
	{
		VncMsgPacket msgPacket;
		if( ParseVncMsgPacket(msgPacket, (unsigned char *)pRecvData, nRecvLen) < 0 )
			return;
		HandleVncMsgPacket(msgPacket);
	}
	return;
}


uint32_t CCPVncServer::HandleVncMsgPacket(VncMsgPacket &msgPacket)
{
	LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << "start." << (int)msgPacket.header.msgType);
	LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " msgType = " <<  (int)msgPacket.header.msgType << " status=" << msgPacket.status);

	switch(msgPacket.header.msgType)
	{
	case EMsgRspType_CreatRoom:
		{
			//if(msgPacket.status == 0  && msgPacket.header.roomID > 0)
			//{
			//	m_RoomID = msgPacket.header.roomID;
			//	m_Sending = TRUE;
			//	m_ServerState = VncServerState_Sharing;
			//	if(m_Observer)
			//		m_Observer->OnVncServerCreateRoom(m_RoomID, msgPacket.status);
			//}
			//else
			//{
			//	m_ServerState = VncServerState_Init;
			//	if(m_Observer)
			//		m_Observer->OnVncServerCreateRoom(0, msgPacket.status);
			//}				
		}
		break;
	case EMsgRspType_RoomClosed:
		{
			m_ServerState = VncServerState_Init;
			if(m_Observer)
				m_Observer->OnVncServerRoomClosed(m_RoomID, msgPacket.status);
			m_RoomID = 0;
		}
		break;
	case EMsgRspType_JoinRoom:
		{
			if(msgPacket.status == 0)
			{
				m_Sending = TRUE;
				if (m_ServerState != VncServerState_Sharing)
				{
					if (m_Observer)
						m_Observer->OnVncServerJoinRoom(m_RoomID, msgPacket.status);
					m_ServerState = VncServerState_Sharing;
				}
			}
			else
			{
				m_ServerState = VncServerState_Init;
				if(m_Observer)
					m_Observer->OnVncServerJoinRoom(m_RoomID, msgPacket.status);
			}
		}
		break;
	case EMsgRspType_QuitRoom:
		{
			m_ServerState = VncServerState_Init;
			if(m_Observer)
				m_Observer->OnVncServerQuitRoom(m_RoomID, msgPacket.status);
			m_RoomID = 0;
		}
		break;
	case EMsgRspType_ExitRoom://会议的其他成员退出了
		{
				LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << "EMsgRspType_ExitRoom.");
				m_bExitFromSrv = true;
				if (m_Observer)
					m_Observer->ServerQuitRoom(m_RoomID);
		}
		break;
	case EMsgRspType_ReqInitMsg:
		{
			LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << "EMsgRspType_ReqInitMsg.");
			if(m_ServerState == VncServerState_Sharing)
			{
				m_WinVncDll->EnableBrocast(true);
				m_WinVncDll->SetQualityLevel(6);
		//		m_WinVncDll->CaptureKeyFrame();
				m_WinVncDll->SetCaptureInterval(100);
				m_Sending = TRUE;

		//		CaptureKeyFrame(m_RoomID);
				char msgBuf[512];
				int msgLen = 0;
				m_WinVncDll->GetRfbServerInitMsg(msgBuf, msgLen);
				if(msgLen) {
					OnVNCFrameData(msgBuf,msgLen);
				}
				CaptureKeyFrame(m_RoomID);
			//	m_WinVncDll->CaptureKeyFrame();
			}
		}
		break;
	case EMsgReqType_ReqControl:
		{
			LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " EMsgReqType_ReqControl.");
			if(m_RCState == VncRemoteControlState_Normal)
			{
				if(msgPacket.status == 1)
				{
					m_RCState = VncRemoteControlState_RecvReqControling;
					if(m_Observer)
						m_Observer->OnVncRecvReqRemoteControl(m_RoomID);
				}
			}
			if(m_RCState == VncRemoteControlState_RecvReqControling)
			{
				if(msgPacket.status == 0)
				{
					m_RCState = VncRemoteControlState_Normal;
					if(m_Observer)
						m_Observer->OnVncCancelReqRemoteControl(m_RoomID);
				}
			}
			if(m_RCState == VncRemoteControlState_Controling)
			{
				if(msgPacket.status == 0)
				{
					m_RCState = VncRemoteControlState_Normal;
					if(m_Observer)
						m_Observer->OnVncStopRemoteControl(m_RoomID);
					m_WinVncDll->EnableRemoteInput(false);
				}
			}
		}
		break;
	case EMsgRspType_ReqControl:
		{
			LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << "EMsgRspType_ReqControl.");
			if(m_RCState == VncRemoteControlState_ReqControling)
			{
				if(msgPacket.status == 200)
				{
					m_RCState = VncRemoteControlState_Controling;
					m_WinVncDll->EnableRemoteInput(true);
					if(m_Observer)
						m_Observer->OnVncAcceptRemoteControl(m_RoomID);
				}
				else if(msgPacket.status == 603)
				{
					m_RCState = VncRemoteControlState_Normal;
					m_WinVncDll->EnableRemoteInput(false);
					if(m_Observer)
						m_Observer->OnVncRejectRemoteControl(m_RoomID);
				}
			}
		}
		break;
	}
	LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " end.");
	return 0;
}

uint32_t CCPVncServer::HandleVncDataPacket(VncDataPacket &dataPacket)
{
	//LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << "start.");
	if(dataPacket.header.roomID != m_RoomID)
		return -1;

	if( ( !m_CurVncFrame  || (dataPacket.packet_seq != m_CurVncFrame->packetSeq) )
		&& dataPacket.begin_mark) 
	{
		if(m_CurVncFrame)
			delete m_CurVncFrame;

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
		}
	}

	//LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " end.");
	return 0;
}

int CCPVncServer::reqRemoteControl(int roomID)
{
	LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " start..");

	if(m_ServerState != VncServerState_Sharing) {
		LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " state invalid." << m_ServerState);
		return -1;
	}

	if(m_RCState != VncRemoteControlState_ReqControling) {
		m_RCState = VncRemoteControlState_ReqControling;
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

	LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " end..");
	return 0;
}

int CCPVncServer::stopRemoteControl(int roomID)
{
	if(m_ServerState != VncServerState_Sharing)
	{
		LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " state invalid." << m_ServerState);
		return -1;
	}

	if(m_RCState == VncRemoteControlState_Controling || m_RCState == VncRemoteControlState_ReqControling) {
		m_WinVncDll->EnableRemoteInput(false);
		m_RCState = VncRemoteControlState_Normal;
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

	return 0;
}

int CCPVncServer::acceptRemoteControl(int roomId)
{
	if(m_RCState != VncRemoteControlState_RecvReqControling)
	{
		LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " state invalid." << m_RCState);
		return -1;
	}

	if(m_WinVncDll && roomId == m_RoomID) {
		m_WinVncDll->EnableRemoteInput(true);
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
		m_RCState = VncRemoteControlState_Controling;
		return 0;
	}
	return -1;
}

int CCPVncServer::rejectRemoteControl(int roomId)
{
	if(m_RCState != VncRemoteControlState_RecvReqControling)
	{
		LOG4CPLUS_TRACE(serverlog, __FUNCTION__ << " state invalid." << m_RCState);
		return -1;
	}
	if(m_WinVncDll && roomId == m_RoomID) {
		m_WinVncDll->EnableRemoteInput(false);
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
		m_RCState = VncRemoteControlState_Normal;
		return 0;
	}
	return -1;
}

int CCPVncServer::setWindowShared(HWND wnd)
{
	if(m_WinVncDll) {
		m_WinVncDll->SetWindowShared(wnd);
	}
	return 0;
}
int CCPVncServer::CaptureKeyFrame(int roomId)
{
		/*if(m_WinVncDll)
	{
	m_WinVncDll->EnableBrocast(true);
		m_WinVncDll->SetQualityLevel(6);
		m_WinVncDll->SetCaptureInterval(100);
		m_Sending = TRUE;
		char msgBuf[512];
	/	msgBuf[0] = 'r';
		msgBuf[1] = 'f';
		msgBuf[2] = 'i';
		msgBuf[3] = 'r';
		msgBuf[4] = 's';
		msgBuf[5] = 't'; /

		int msgLen = 0;
		m_WinVncDll->GetRfbServerInitMsg(msgBuf, msgLen);
		if(msgLen) {
			OnVNCFrameData(msgBuf,msgLen);
		}
		m_WinVncDll->CaptureKeyFrame();
	}*/

	if(m_WinVncDll) {
		m_WinVncDll->CaptureKeyFrame();
	}
	return 0;
}


//server
void CCPVncServer::OnVNCFrameData(const char* pFrameData,int nDataLen)
{
	if(nDataLen > 0)
	//	sendDataToRUDP(pFrameData, nDataLen);
	{
		VncFrameData *vncFrame = new VncFrameData;
	    memset(vncFrame, 0, sizeof(VncFrameData));
		vncFrame->frameData = new char[nDataLen];
		vncFrame->totalLen = nDataLen;
		memcpy(vncFrame->frameData, pFrameData, nDataLen);
		m_csVncDataSendQ.Lock();
		m_VncDataSendQ.push_back(vncFrame);	
		m_csVncDataSendQ.Unlock();
	}
}

void CCPVncServer::OnVNCScreenData(const char* pFrameData,int nDataLen)
{
	OnVNCFrameData(pFrameData,nDataLen);
	//if(nDataLen > 0)
		//sendDataToRUDP(pFrameData, nDataLen);
}

void CCPVncServer::OnDisplaychanged()
{

}

unsigned short CCPVncServer::getDataSequence()
{
	return ++m_CurDataSeq;
}

int CCPVncServer::sendDataToRUDP(const char *pData, int nDataLen)
{
	int alreadySendLen = 0;
	if(m_RudpConnection) {
		
		unsigned short dataSeq = getDataSequence();
		int sendFailedTimes = 0;

		while (alreadySendLen < nDataLen && m_Sending && sendFailedTimes < 100)
		{
		//	EnterCriticalSection(&m_ThreadSection);
			if (m_RudpConnection->GetState() != netcomm::CONNECTION_STATE_CONNECTED) {
				std::ostringstream erro ;
				erro << " Not Conned, State= "<< m_RudpConnection->GetState();
				LOG4CPLUS_ERROR(serverlog, __FUNCTION__ <<  erro.str());
			//	LeaveCriticalSection(&m_ThreadSection);
				return -1;
			}
		//	LeaveCriticalSection(&m_ThreadSection);

			VncDataPacket dataPacket;
			memset(&dataPacket, 0, sizeof(VncDataPacket));

			dataPacket.header.version = HEADER_VERSION;
			dataPacket.header.clientType = EClientType_VncServer;
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

			//fwrite(dataPacket.payload, 1, dataPacket.payload_len, g_ServerFile);  //for test


			int sendLen = BuildVncDataPacket((unsigned char *)m_SendBuffer,  dataPacket);
		//	EnterCriticalSection(&m_ThreadSection);
			int ret = m_RudpConnection->Send(m_SendBuffer, sendLen);
		//	LeaveCriticalSection(&m_ThreadSection);

			if( ret == sendLen) {
				alreadySendLen += dataPacket.payload_len;
			} else {
				sendFailedTimes++;
				std::ostringstream erro ;
				erro << " Vnc Server sendDataToRUDP sendFailedTimes="<< sendFailedTimes;
				LOG4CPLUS_ERROR(serverlog, __FUNCTION__ <<  erro.str());
				Sleep(10);
			}
		}

		//std::ostringstream erro ;
		//erro << " sendDataToRUDP dataSeq=" << dataSeq << " dataLen="<< nDataLen << " alreadySend="<< alreadySendLen << "failedTimes=" << sendFailedTimes;
		//LOG4CPLUS_TRACE(serverlog, __FUNCTION__ <<  erro.str());

	}
//	LeaveCriticalSection(&m_ThreadSection);
	return nDataLen-alreadySendLen;
}
