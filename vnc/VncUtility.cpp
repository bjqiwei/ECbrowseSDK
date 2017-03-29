#include "stdafx.h"
#include "VncUtility.h"

#define WEBRTC_LITTLE_ENDIAN

/* for RTP/RTCP
    All integer fields are carried in network byte order, that is, most
    significant byte (octet) first.  AKA big-endian.
*/
void AssignUWord32ToBuffer(VNC_UWord8* dataBuffer, VNC_UWord32 value) {
	dataBuffer[0] = static_cast<VNC_UWord8>((value >> 24) & 0xFF);
	dataBuffer[1] = static_cast<VNC_UWord8>((value >> 16) & 0xFF);
	dataBuffer[2] = static_cast<VNC_UWord8>((value >> 8) & 0xFF);
	dataBuffer[3] = static_cast<VNC_UWord8>(value & 0xFF) & 0xFF;
}

void AssignUWord24ToBuffer(VNC_UWord8* dataBuffer, VNC_UWord32 value) {
  dataBuffer[0] = static_cast<VNC_UWord8>(value >> 16);
  dataBuffer[1] = static_cast<VNC_UWord8>(value >> 8);
  dataBuffer[2] = static_cast<VNC_UWord8>(value & 0xFF);
}

void AssignUWord16ToBuffer(VNC_UWord8* dataBuffer, VNC_UWord16 value) {
  dataBuffer[0] = static_cast<VNC_UWord8>(value >> 8);
  dataBuffer[1] = static_cast<VNC_UWord8>(value & 0xFF);
}

VNC_UWord16 BufferToUWord16(const VNC_UWord8* dataBuffer) {
  return (dataBuffer[0] << 8) + dataBuffer[1];
}

VNC_UWord32 BufferToUWord24(const VNC_UWord8* dataBuffer) {
  return (dataBuffer[0] << 16) + (dataBuffer[1] << 8) + dataBuffer[2];
}

VNC_UWord32 BufferToUWord32(const VNC_UWord8* dataBuffer) {
  return (dataBuffer[0] << 24) + (dataBuffer[1] << 16) + (dataBuffer[2] << 8) +
      dataBuffer[3];
}

/*
|3b Version|5b Client Type|8b Msg Type|16b Room ID|16b Msg ID| 
*/
VNC_Word32 BuildPacketHeader(VNC_UWord8* dataBuffer, VncPacketHeader &header)
{	
	dataBuffer[0] =  header.clientType | 0x20;
	dataBuffer[1] = header.msgType;
	AssignUWord16ToBuffer(dataBuffer + 2, header.roomID);
	AssignUWord16ToBuffer(dataBuffer + 4, header.msgID);
	return 6;
}

/*
|MsgHeader|32b Data Len|32b Data Total Len|16b Packet Seq|1b Begin Mark|1b End Mark|6b Reserved|DATA| 
*/
VNC_Word32 BuildVncDataPacket(VNC_UWord8* dataBuffer,  VncDataPacket &packet)
{
	BuildPacketHeader(dataBuffer, packet.header);

	AssignUWord32ToBuffer(dataBuffer+6, packet.payload_len);
	AssignUWord32ToBuffer(dataBuffer+10, packet.total_len);
	AssignUWord16ToBuffer(dataBuffer+14, packet.packet_seq);
	VNC_UWord8 * ptr = dataBuffer + 16;
	*ptr = (packet.begin_mark<< 7) | (packet.end_mark << 6);
	ptr++;
	memcpy(ptr, packet.payload, packet.payload_len);
	return packet.payload_len+17;
}

/*
|MsgHeader|32b Status ID|32b Status Description Len|Status Description| 
*/
VNC_Word32 BuildVncMsgPacket(VNC_UWord8* dataBuffer,VncMsgPacket &msgPacket)
{
	BuildPacketHeader(dataBuffer, msgPacket.header);

	AssignUWord32ToBuffer(dataBuffer+6, msgPacket.status);
	AssignUWord32ToBuffer(dataBuffer+10, msgPacket.status_description_len);
	if(msgPacket.status_description_len > 0)
		memcpy(dataBuffer+14, msgPacket.description, msgPacket.status_description_len);
	return msgPacket.status_description_len+14;
}

/*
|3b Version|5b Client Type|8b Msg Type|16b Room ID|16b Msg ID| 
*/
VNC_Word32 ParseVncPacketHeader(VncPacketHeader &header, VNC_UWord8* dataBuffer, VNC_UWord32 dataLen)
{
	if(dataLen < 6)
		return -1;

	// Version
	header.version  = dataBuffer[0] >> 5;
	//Client Type
	header.clientType = dataBuffer[0] & 0x1F;
	//Msg Type
	header.msgType = dataBuffer[1];
	//Room ID
	header.roomID = (dataBuffer[2]<<8) +  dataBuffer[3];	
	//Msg ID
	header.msgID = (dataBuffer[4]<<8) + dataBuffer[5];

	return 0;
}

/*
|MsgHeader|32b Data Len|32b Data Total Len|16b Packet Seq|1b Begin Mark|1b End Mark|6b Reserved|DATA| 
*/
VNC_Word32 ParseVncDataPacket(VncDataPacket &dataPacket, VNC_UWord8* dataBuffer, VNC_UWord32 dataLen)
{
	if(dataLen < 17)
		return -1;
	ParseVncPacketHeader(dataPacket.header, dataBuffer, dataLen);
	VNC_UWord8 * ptr = dataBuffer + 6;

	dataPacket.payload_len = *ptr++ << 24;
	dataPacket.payload_len += *ptr++ << 16;
	dataPacket.payload_len += *ptr++ << 8;
	dataPacket.payload_len += *ptr++;

	dataPacket.total_len = *ptr++ << 24;
	dataPacket.total_len += *ptr++ << 16;
	dataPacket.total_len += *ptr++ << 8;
	dataPacket.total_len += *ptr++;

	dataPacket.packet_seq = *ptr++ << 8;
	dataPacket.packet_seq += *ptr++;

	dataPacket.begin_mark = *ptr >> 7;
	dataPacket.end_mark =  (*ptr  & 0x40) >> 6;

	ptr++;
	if(dataLen < dataPacket.payload_len + 13 || dataPacket.payload_len > MAX_SEND_DATA_LEN)
		return -1;
	memcpy(dataPacket.payload, ptr, dataPacket.payload_len);
	return 0;
}

/*
|MsgHeader|32b Status ID|32b Status Description Len|Status Description| 
*/
VNC_Word32 ParseVncMsgPacket(VncMsgPacket &msgPacket, VNC_UWord8* dataBuffer, VNC_UWord32 dataLen)
{
	if(dataLen < 14)
		return -1;
	ParseVncPacketHeader(msgPacket.header, dataBuffer, dataLen);

	VNC_UWord8 * ptr = dataBuffer + 6;
	msgPacket.status = *ptr++ << 24;
	msgPacket.status += *ptr++ << 16;
	msgPacket.status += *ptr++ << 8;
	msgPacket.status += *ptr++;

	msgPacket.status_description_len = *ptr++ << 24;
	msgPacket.status_description_len += *ptr++ << 16;
	msgPacket.status_description_len += *ptr++ << 8;
	msgPacket.status_description_len += *ptr++;

	if(msgPacket.status_description_len > (MAX_STATUS_DES_LEN-1) || dataLen < msgPacket.status_description_len+14)
		return -1;
	if(msgPacket.status_description_len > 0)
	{
		memcpy(msgPacket.description, ptr, msgPacket.status_description_len);
		msgPacket.description[msgPacket.status_description_len] = '\0';
	}
	return 0;
}
