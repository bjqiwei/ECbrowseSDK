#pragma once
#include "CCPVncDef.h"
#include <cstring>  // memcpy

void AssignUWord32ToBuffer(VNC_UWord8* dataBuffer, VNC_UWord32 value);
void AssignUWord24ToBuffer(VNC_UWord8* dataBuffer, VNC_UWord32 value);
void AssignUWord16ToBuffer(VNC_UWord8* dataBuffer, VNC_UWord16 value);

    /**
     * Converts a network-ordered two-byte input buffer to a host-ordered value.
     * \param[in] dataBuffer Network-ordered two-byte buffer to convert.
     * \return Host-ordered value.
     */
    VNC_UWord16 BufferToUWord16(const VNC_UWord8* dataBuffer);

    /**
     * Converts a network-ordered three-byte input buffer to a host-ordered value.
     * \param[in] dataBuffer Network-ordered three-byte buffer to convert.
     * \return Host-ordered value.
     */
    VNC_UWord32 BufferToUWord24(const VNC_UWord8* dataBuffer);

    /**
     * Converts a network-ordered four-byte input buffer to a host-ordered value.
     * \param[in] dataBuffer Network-ordered four-byte buffer to convert.
     * \return Host-ordered value.
     */
    VNC_UWord32 BufferToUWord32(const VNC_UWord8* dataBuffer);


	VNC_Word32 BuildPacketHeader(VNC_UWord8* dataBuffer, VncPacketHeader &header);

	VNC_Word32 BuildVncDataPacket(VNC_UWord8* dataBuffer,  VncDataPacket &packet);

	VNC_Word32 BuildVncMsgPacket(VNC_UWord8* dataBuffer,VncMsgPacket &msgPacket);

	VNC_Word32 ParseVncPacketHeader(VncPacketHeader &header, VNC_UWord8* dataBuffer, VNC_UWord32 dataLen);

	VNC_Word32 ParseVncDataPacket(VncDataPacket &dataPacket, VNC_UWord8* dataBuffer, VNC_UWord32 dataLen);

	VNC_Word32 ParseVncMsgPacket(VncMsgPacket &msgPacket, VNC_UWord8* dataBuffer, VNC_UWord32 dataLen);

	