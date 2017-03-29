#pragma once

#if defined(BUILD_NETCOMM_DLL)
  #define MYEXPORT __declspec(dllexport)
#else
  #define MYEXPORT __declspec(dllimport)
#endif

namespace netcomm {

enum eConnectionType
{
	CONNECTION_TYPE_TCP,
	CONNECTION_TYPE_RUDP,
	CONNECTION_TYPE_UDP,
};

enum eProxyType
{
	PROXY_TYPE_NONE,
	PROXY_TYPE_HTTP,
	PROXY_TYPE_SOCKS,
};

enum eProtocolType
{
	PROTOCOL_TYPE_SIP,
	PROTOCOL_TYPE_XMPP,
	PROTOCOL_TYPE_RTP,
	PROTOCOL_TYPE_HTTP,

	PROTOCOL_TYPE_CXMPP,
	PROTOCOL_TYPE_CUSTOM,
};

#define USE_PROXY							0x00000001

#define SUBTYPE_NONE						0

typedef struct _ClientConnectionInfo
{
	int			nConnectionType;
	DWORD		nFlags;
	LPCTSTR		szHostname;
	int			nPort;

	LPCTSTR		szLocalname;
	int			nLocalPort;

	int			nProxyType;
	LPCTSTR		szProxyHostname;
	int			nProxyPort;
	LPCTSTR		szProxyUsername;
	LPCTSTR		szProxyPassword;
} CLIENTCONNECTIONINFO;

typedef struct _ServerConnectionInfo
{
	int			nConnectionType;
	LPCTSTR		szHostname;// 如果为空 则邦定方式为INADDR_ANY
	int			nPort;
} SERVERCONNECTIONINFO;

typedef struct _P2PConnectionInfo
{
	int			nConnectionType;
	LPCTSTR		szHostname;
	int			nPort;
	LPCTSTR		szRemotename;
	int			nRemotePort;
} P2PCONNECTIONINFO;

enum eConnectionState
{
	CONNECTION_STATE_CLOSED,
	CONNECTION_STATE_CONNECTING,
	CONNECTION_STATE_CONNECTED,
	CONNECTION_STATE_LISTENING
};

class IConnection;

class IConnectionFactory
{
public:
	virtual void Release() = 0;

	virtual IConnection* CreateConnection(P2PCONNECTIONINFO& conninfo) const = 0;
	virtual IConnection* CreateConnection(CLIENTCONNECTIONINFO&) const = 0;
	virtual IConnection* CreateConnection(SERVERCONNECTIONINFO&) const = 0;
};

#define CONNECTION_ERROR_SUCCESSFUL								0
#define CONNECTION_ERROR_INVALID_SUBTYPE_VALUE					-1
#define CONNECTION_ERROR_REGISTER_MULTIPLE_SUBTYPE_IN_CLIENT	-2
#define CONNECTION_ERROR_SUBTYPE_ALREADY_REGISTERED				-3
#define CONNECTION_ERROR_NOT_SUPPORT_REGISTER_IN_RUNTIME		-4
#define CONNECTION_ERROR_CONNECT_IN_LISTEN_CONNECTION			-5
#define CONNECTION_ERROR_CONNECT_ALREADY_CALLED					-6
#define CONNECTION_ERROR_CREATE_CONNECTION_THREAD_FAILED		-7
#define CONNECTION_ERROR_LISTEN_IN_CLIENT_CONNECTION			-8
#define CONNECTION_ERROR_ALREADY_LISTENING						-9
#define CONNECTION_ERROR_BIND_ADDRESS_FAILED					-10
#define CONNECTION_ERROR_CREATE_SOCKET_FAILED					-11
#define CONNECTION_ERROR_CREATE_UDP_STREAM_FAILED				-12
#define CONNECTION_ERROR_UNSUPPORT_CONNECTION_TYPE				-13
#define CONNECTION_ERROR_NOT_CONNECTED							-14
#define CONNECTION_ERROR_TIMEOUT								-15
#define CONNECTION_NEGOTIATE_SUBTYPE_BAD_REQUEST				-16
#define CONNECTION_NEGOTIATE_SUBTYPE_BAD_REPLY					-17
#define CONNECTION_NEGOTIATE_SUBTYPE_REJECTED					-18

class IConnection
{
public:
	virtual ULONG Release() = 0;

	virtual void RegisterProtocol(UINT nprotocol) = 0;

	virtual int GetState() const = 0;
	virtual int GetProtocol() = 0;

	virtual char* GetIp() const = 0;
	virtual int   GetPort() const = 0;

	virtual char* GetBindIp()  = 0;
	virtual int   GetBindPort()  = 0;
	
	virtual int TestProxyConnect() = 0; 

	virtual void MakeHoleTo(const char* peerIp,int peerPort,char* buf,int len) = 0;
	virtual void ResMakeHole(char* buf,int len) = 0;
	// for udp
	virtual void ChangeUdp2Rudp() = 0;
	virtual void ChangeRudp2Udp() = 0;

	virtual int Connect() = 0;
	virtual int Listen() = 0;
	virtual IConnection *Accept() = 0;
	virtual void Close() = 0;

	virtual int GetAcceptCount() = 0;

	virtual int Send(const char FAR * buf, int len) = 0;
	virtual int SendTo(const char FAR * buf, int len,const char FAR *to) = 0;
	virtual int Recv(char FAR * buf, int len) = 0;
	virtual int RecvFrom(char FAR * buf, int len,char FAR* fromip,int& fromport) = 0;

};

} // namespace netcomm

#ifdef __cplusplus
extern "C" {
	/*MYEXPORT*/ netcomm::IConnectionFactory* CreateConnectionFactory(int nPortCount=4);
}
#endif
