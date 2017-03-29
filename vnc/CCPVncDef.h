#pragma once

#include <stdint.h>

typedef int8_t              VNC_Word8;
typedef int16_t             VNC_Word16;
typedef int32_t             VNC_Word32;
typedef int64_t             VNC_Word64;
typedef uint8_t             VNC_UWord8;
typedef uint16_t            VNC_UWord16;
typedef uint32_t            VNC_UWord32;
typedef uint64_t            VNC_UWord64;

#define   MAX_SEND_BUFER_LEN    1500
#define  MAX_SEND_DATA_LEN   1300
#define  MAX_STATUS_DES_LEN  256
#define  HEADER_VERSION  1

enum VncViewerState
{
	VncViewerState_UnInit,
	VncViewerState_Init,
	VncViewerState_RoomCreating,
	VncViewerState_RoomJoining,
	VncViewerState_Sharing,
	VncViewerState_Quiting,
	VncViewerState_Quited
};
enum VncRemoteControState
{
	VncRemoteControlState_Normal,
	VncRemoteControlState_ReqControling,
	VncRemoteControlState_RecvReqControling,
	VncRemoteControlState_Controling
};

//enum VncServerState
//{
//	VncServerState_Normal,
//	VncServerState_Creating,
//	VncServerState_Joining,
//	VncServerState_Running,
//	VncServerState_Quiting,
//	VncServerState_Quited
//};

enum VncServerState
{
	VncServerState_UnInit,
	VncServerState_Init,
	VncServerState_Creating,
	VncServerState_Joining,
	VncServerState_Sharing,
	VncServerState_Quiting,
	VncServerState_Quited
};

#define MSG_REQ_BASE	0
#define MSG_RSP_BASE  100

enum EMsgType
{
	EMsgReqType_CreateRoom			= MSG_REQ_BASE + 1,
	EMsgReqType_JoinRoom				= MSG_REQ_BASE + 2,
	EMsgReqType_QuitRoom				= MSG_REQ_BASE + 3,
	EMsgReqType_VncData				= MSG_REQ_BASE + 4,
	EMsgReqType_ReqControl			= MSG_REQ_BASE + 5,

	EMsgRspType_CreatRoom			= MSG_RSP_BASE + 1,
	EMsgRspType_RoomClosed			= MSG_RSP_BASE + 2,
	EMsgRspType_JoinRoom				= MSG_RSP_BASE + 3,
	EMsgRspType_QuitRoom				= MSG_RSP_BASE + 4,
	EMsgRspType_ReqInitMsg			= MSG_RSP_BASE + 5,
	EMsgRspType_ReqControl           = MSG_RSP_BASE + 6,
	EMsgRspType_AcceptControl        = MSG_RSP_BASE + 7,
	EMsgRspType_ExitRoom             = MSG_RSP_BASE + 8
};

enum EClientType
{
	EClientType_VncViewer  = 1,
	EClientType_VncServer = 2,
	EClientType_Server = 3
};

typedef struct _VncFrameData
{
	char *frameData;
	int totalLen;
	int dataLen;
	unsigned short packetSeq;
}VncFrameData;

typedef struct _VncPacketHeader {
	uint8_t		version;			//协议版本号
	uint8_t		clientType;		//消息发送者类型，1.kVncViewer 2.kVncServer 3.kServer
	uint8_t		msgType;		//请求或响应消息
	uint16_t	roomID;			    //房间ID，大于0
	uint16_t	msgID;				//消息发送者自定义消息id，请求消息和响应消息的msg_id一样
}VncPacketHeader;

typedef struct _VncDataPacket {
	VncPacketHeader header;
	uint32_t        payload_len;
	uint32_t        total_len;
	uint16_t        packet_seq;
	uint8_t          begin_mark;
	uint8_t          end_mark;
	unsigned char    payload[MAX_SEND_DATA_LEN];
}VncDataPacket;

typedef struct _VncMsgPacket {
	VncPacketHeader header;
	int32_t          status;
	uint32_t        status_description_len;
	char				 description[MAX_STATUS_DES_LEN];
}VncMsgPacket;

typedef struct VncCallbackMsg {
	int roomID;
	int status;
	int msgType; //
}VncCallbackMsg;

enum EVncCallbackType {
	EVncCallbackType_CreateRoom,
	EVncCallbackType_RoomClosed,
	EVncCallbackType_JoinRoom,
	EVncCallbackType_QuitRoom,
	EVncCallbackType_RecvReqRemoteControl,
	EVncCallbackType_CancelReqRemoteControl,
	EVncCallbackType_StopRemoteControl,
	EVncCallbackType_AcceptRemoteControl,
	EVncCallbackType_RejectRemoteControl
};