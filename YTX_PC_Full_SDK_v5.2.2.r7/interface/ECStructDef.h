/*
 *  Copyright (c) 2013-2014 The CCP project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a Beijing Speedtong Information Technology Co.,Ltd license
 *  that can be found in the LICENSE file in the root of the web site.
 *
 *   http://www.yuntongxun.com
 *
 *  An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef _EC_APP_DEFINE_H
#define _EC_APP_DEFINE_H

#include  <string.h>

#ifdef _MSC_VER
#define EC_CALL_API __declspec(dllexport)
#else
#define EC_CALL_API
#endif

#ifndef __cplusplus
typedef enum {false, true} bool;
#endif

const static int BUF_LEN_1 = 1;
const static int BUF_LEN_8 = 8;
const static int BUF_LEN_20 = 20;
const static int BUF_LEN_33 = 33;
const static int BUF_LEN_64 = 64;
const static int BUF_LEN_128 = 128;
const static int BUF_LEN_256 = 256;
const static int BUF_LEN_512 = 512;
const static int BUF_LEN_1024 = 1024;
const static int BUF_LEN_4096 = 4096;

#define EC_Request_Success  0
#define EC_Response_Success 200

//错误码定义
enum {
	ERR_SERVICE_NOERROR				= 0,
	ERR_SERVICE_CONTEN_TOO_LONG     = 170001,//内容长度超过规定	
	ERR_SERVICE_POINTER_NULL		= 170002,//参数为空指针引起的错
	ERR_SERVICE_PARAMETER           = 170003,//参数错误

	ERR_SERVICE_EMPTY_STRING		= 170097,//字符串参数为空字符
	ERR_GET_PARAM_NULL				= 170098,//缓存获得参数错误
	ERR_SERVICE_OTHER_VOIP_CALL     = 170486,//当前存在其他通话
};

enum {//语音录放音 RECORD PLAY 返回值
	ERR_RECORD_WAVE_IN_OPEN =170200,//打开录音设备失败
	ERR_RECORD_NO_MEMORY=170201,//录音没缓存
	ERR_RECORD_WAVE_IN_GET_POSITION=170202,//录音结束读数据缓存错误
	ERR_RECORD_TIME_BYTES=170203,//录音时间模式错误
	ERR_RECORD_CREATE_FILE=170204,//创建本地录音文件错误
	ERR_RECORD_WAVE_IN_RESET=170205,//录音重置错误
	ERR_RECORD_WAVE_IN_UN_PREPARE_HEADER=170206,//录制头格式错误
	ERR_RECORD_GLOBAL_FREE=170207,//录音缓存释放错误
	ERR_RECORD_WAVE_IN_CLOSE=170208,//关闭录音设备失败
	ERR_PLAY_CREATE_FILE=170209,//打开本地录音文件错误
	ERR_PLAY_NOT_AMR_FILE=170210,//录音文件不是amr格式文件
	ERR_PLAY_WAVE_OUT_OPEN=170211,//打开播放设备失败
	ERR_PLAY_WAVE_OUT_PREPARE_HEADER=170212,//播放录音头格式错误
	ERR_PLAY_GLOBAL_FREE=170213,//播放缓存释放错误
	ERR_RECORD_IS_RECORDING=170214,//录音重复调用
	ERR_PLAY_IS_PLAYING=170215,//播放录音重复调用
	ERR_RECORD_PLAY_NULL=170216,//文件名为空
    ERR_PLAYER_CREATE_NULL=170217,//创建播放设备为空
    ERR_RECORD_FILE_EIXIST=170218,//录音文件已经存在
};
enum {//CCPClient err
	ERR_EC_SDK_ALREADY_INIT =171000,//重复初始化
	ERR_EC_NO_MEMORY,//无内存
	ERR_EC_INVALID_CALL_BACK_INTERFACE,//初始化时传入的回调函数结构体为空
	ERR_EC_SDK_UN_INIT,//SDK未初始化
	ERR_EC_NO_NETWORK,//无网络连接
	ERR_EC_NOT_SUPPORT_FUNC,//接口不支持
};
enum {//servicecore err
	ERR_EC_SERVICE_CORE_NULL =171030,//空指针,空内容
	ERR_EC_SERVICE_CORE_XML,//xml解析字段空指针
	ERR_EC_SERVICE_CORE_MAX_RELOGIN_TIME_OUT,//重连超过最大允许时间段，强制退出,需要外层sdk调用login
	ERR_EC_SERVICE_CORE_PROXY_ADDR_MAP_INSERT,//地址map插入错误
	ERR_EC_SERVICE_CORE_PROXY_ADDR_MAP_EMPTY,//地址map内容为空
	ERR_EC_SERVICE_CORE_XML_WRITE,//xml写字段空指针
	ERR_EC_SERVICE_CORE_COMPRESS,//压缩附件失败
	ERR_EC_SERVICE_CORE_UNCOMPRESS,//解压缩附件失败
};

enum {//ECserviceManage err
	ERR_EC_SERVICEMANAGE_NULL =171130,//空指针,空内容
	ERR_EC_SERVICEMANAGE_QUEUE_EMPTY,//队列为空
	ERR_EC_SERVICEMANAGE_CODER,//编解码错误
	ERR_EC_SERVICEMANAGE_MD5,//md5错误
	ERR_EC_SERVICEMANAGE_TIME_OUT_CHECK_INFO_MAP_INSERT,//超时map插入错误
	ERR_EC_SERVICEMANAGE_TIME_OUT_CHECK_INFO_MAP_FIND,//超时map查找错误
	ERR_EC_SERVICEMANAGE_TIME_OUT_ACK,//ACK超时
	ERR_EC_SERVICEMANAGE_TIME_OUT_RESPONSE,//RESPONSE超时
	ERR_EC_SERVICEMANAGE_VERIFY_TYPE_BY_TCP_MSDID,//通过流水号验证包类型
	ERR_EC_SERVICEMANAGE_SOCKET_SEND,//底层socket 发送失败
	ERR_EC_SERVICEMANAGE_SOCKET_RESEND,//底层socket 重复发送失败
	ERR_EC_SERVICEMANAGE_SOCKET_RECEIVING_RECONNECT,//接收200消息中,tcp中断重连
	ERR_EC_SERVICEMANAGE_MSG_CONTENT_COMPRESS_UNCOMPRESS,//文本内容压缩解压缩失败
	ERR_EC_SERVICEMANAGE_SOCKET_RECEIVING_LOGIN_AGAIN,//上层应用层用户输入登录（相对于用户自动登录），在等待回200消息中，再次登录报错
	ERR_EC_SERVICEMANAGE_PUT_REQUST_MORE_FREQUENTLY,//请求过于频繁
    ERR_EC_SERVICEMANAGE_OUT_OF_RANGE,//越界
	ERR_EC_SERVICEMANAGE_GET_USER_STAT_MORE_FREQUENTLY,//获得用户状态请求过于频繁
};

enum {//FileClient err
	ERR_EC_FILE_NULL =171250,//空指针,空内容
	ERR_EC_FILE_SYN_HTTP_REQUEST,//建立socket失败或发送接收数据错误
	ERR_EC_FILE_SYN_HTTP_STATAUS_CODE,//标准协议错误
	ERR_EC_FILE_JSON_PARSE,//JSON 解析错误
	ERR_EC_FILE_MD5,//md5错误
	ERR_EC_FILE_MEDIA_THREAD_INFO_MAP_INSERT,//插入多线程map
	ERR_EC_FILE_MEDIA_THREAD_INFO_MAP_FIND,//查找多线程map
	ERR_EC_FILE_CREATE_FILE,//创建或者打开文件错误
	ERR_EC_FILE_ENCODE_HTTP,//http封包编码错误
	ERR_EC_FILE_CANCEL_UP_OR_DOWN_NOW,//外部取消附件上传或者下载
	ERR_EC_FILE_SEND_WSAECONNRESET,//socket虚电路被远端复位
	ERR_EC_FILE_BODY_NULL,//发送下载请求后应答包包体为空
	ERR_EC_FILE_HEAD_LARGE,//发送下载请求后应答包头太长
	ERR_EC_FILE_RECV_NULL,//发送下载请求后应答包为空
	ERR_EC_FILE_RECV_BLOCK,//网络阻塞，收包长度为0，超时退出
	ERR_EC_FILE_RANGE_OUT_FILE_LENTH,//断点上传偏移超过文件长度
	ERR_EC_FILE_RANGE_OUT_FILE_OFFSET,//断点下载偏移超过申请长度
	ERR_EC_FILE_MULTIPLEX_HTTP_CLIENT_MAP_INSERT,//插入复用短连接map
	ERR_EC_FILE_MULTIPLEX_HTTP_CLIENT_MAP_FIND,//查找复用短连接map
	ERR_EC_FILE_CLIENT_DISCONNECT,//客户端登出或者注销后发送文件错误
};
enum {//ECcallManage err
	ERR_EC_CALLMANAGE_NULL =171500,//空指针,空内容
	ERR_EC_CALLMANAGE_CODER,//会话协商编解码错误
	ERR_EC_CALLMANAGE_SDP_CODER,//会话协商sdp编码错误
	ERR_EC_CALLMANAGE_USERDATA_TYPE,//设置UserData类型错误
	ERR_EC_CALLMANAGE_LOCAL_TIMER_EXPIRED,//本地会话超时
	ERR_EC_CALLMANAGE_LOCAL_INTER_ERROR,//本地会话内部错误
	ERR_EC_CALLMANAGE_SEND_TO_CONNECTOR,//向connector发送请求失败
	ERR_EC_CALLMANAGE_CAMERA_MAX_FPS,//摄像头最大帧异常
};

enum {//releaseCall挂机原因
	EC_ReasonNone=0,//0一般的错误，
	EC_ReasonDeclined=175603,//被叫拒绝应答, 当被叫为落地时，客户端不会收到此错误码，只会听到到被叫忙的播音
	EC_ReasonCallMissed=175408,//被叫超时不应答
	EC_ReasonBusy=175486,//被叫忙

	EC_ReasonAuthenticationFail=175700,//700  第三方鉴权地址连接失败
	EC_ReasonMainaccountArrearage,  //701  主账号余额不足
	EC_ReasonMainaccountInvalid,    //702  主账号无效（未找到应用信息）
	EC_ReasonCallRestrict,          //703  呼叫受限 ，外呼号码限制呼叫
	EC_ReasonSameVoipId,            //704  应用未上线，仅限呼叫已配置测试号码
	EC_ReasonSubaccountArrearage,   //705  第三方鉴权失败，子账号余额不足
	EC_ReasonNoCalled,              //706   无被叫号码
	EC_ReasonConferenceDismissed,   //707   呼入会议号已解散不存在
	EC_ReasonConferencePasswordAuthentification //708   呼入会议号密码验证失败
};

//环境类型
typedef enum
{
	EnvType_Produce = 0,//生产环境地址
	EnvType_Sandbox = 1 //沙盒环境地址

}ECEnvType;

//主状态类型
typedef enum
{
	State_Unknown	= 0,
	State_Online	= 1,//在线
	State_Offline	= 2,//离线
	State_End

}ECStateType;

//在线子类型状态
typedef enum
{
	SubState_Unknown	= 0,
	SubState_NotDisturb	= 1,//请勿打扰
	SubState_Away		= 2,//离开
	SubState_Busy		= 3,//忙碌
	SubState_Hide		= 4,//隐身
	SubState_End

}ECSubStateType;

//网络类型
typedef enum
{
	NetWork_None	= 0,//无网络
	NetWork_Wifi	= 1,//wifi
	NetWork_4G		= 2,//4G
	NetWork_3G		= 3,//3G
	NetWork_2G		= 4,//2g
	NetWork_Internet= 5,//固网
	NetWork_Other	= 6,//其他
	NetWork_End

}ECNetWorkType;

typedef enum {
	EC_LOG_LEVEL_ERR	=5 ,//5以前预留给应用层层日志
	EC_LOG_LEVEL_INFO,
	EC_LOG_LEVEL_DEBUG,

	EC_LOG_LEVEL_LAYER_ERR	=10 ,//10以前预留给上层层日志
	EC_LOG_LEVEL_LAYER_INFO,
	EC_LOG_LEVEL_LAYER_DEBUG,
	EC_LOG_LEVEL_LAYER_MEDIA_ERR,
	EC_LOG_LEVEL_LAYER_MEDIA_INFO,
	EC_LOG_LEVEL_LAYER_MEDIA_DEBUG,
	EC_LOG_LEVEL_END=99
}EClogLevel;
/**
 * 登录认证类型
 */
typedef enum
{
    /** 正常认证模式，服务器认证appKey、appToken、username字段 默认值 */
    LoginAuthType_NormalAuth = 1,

    /** 密码认证模式，服务器认证appKey、username、userPassword字段 */
    LoginAuthType_PasswordAuth = 3,
    
    /** MD5 Token认证方式，服务器认证appKey、username、timestamp、MD5Token字段
     * (该鉴权方式是最安全的方式，用户在自己的服务器根据规则生成MD5，在不暴露apptoken的情况下进行鉴权，且生成的MD5 Token在平台上有失效时间)
     */
    LoginAuthType_MD5TokenAuth = 4,

	/** 临时密码认证模式，服务器认证appKey、username、userPassword字段 */
    LoginAuthType_PasswordAuth_Temporary = 5,
}LoginAuthType;

typedef enum{
	State_ConnectSuccess,//连接成功
	State_Connecting,//连接中
	State_ConnectFailed,//连接失败
	State_ConnectKickOff,//同设备登录被踢
	State_ConnectForceLogOff//强制下线
}ECConnectStateCode;

#pragma pack (1)

typedef struct _LoginInfo{
	LoginAuthType authType; // 认证类型
	char  *username;//登录用户名，需要第三方用户自己维护
	char  *userPassword;//用户密码字段
	char  *appKey;//用户在云通讯平台生成应用时的appKey
	char  *appToken;//用户在云通讯平台生成应用时的appToken
	char  *timestamp;//用户生成MD5的时间戳 yyyyMMddHHmmss
	char  *MD5Token;//用户生成的MD5值 MD5(appId+userName+timestamp+apptoken)

	_LoginInfo()
	{
		memset(this,0,sizeof(_LoginInfo));
	}

}ECLoginInfo;



typedef struct _ECConnectState{
	ECConnectStateCode code;
	int reason;//服务器返回错误码
	_ECConnectState():code(State_Connecting),reason(200)
	{}
}ECConnectState;

typedef struct _ECPersonInfo
{
	unsigned long long version;// 资料版本号
	int  sex;					// 性别  1:男  2：女
	char nickName[BUF_LEN_128]; // 用户名字 
	char birth[BUF_LEN_20];    // 生日  格式 2015-12-31
	char sign[BUF_LEN_1024];   // 签名
	_ECPersonInfo()
	{
		memset(this,0,sizeof(_ECPersonInfo));
	}
	_ECPersonInfo(const _ECPersonInfo& rOther)
	{
		*this = rOther;
	}

}ECPersonInfo;

typedef struct _PersonState
{
	char accound[BUF_LEN_128];//账号
	char nickName[BUF_LEN_128];//昵称（Push状态消息有此字段）
	char timestamp[BUF_LEN_20];//服务器时间戳,毫秒数（Push状态消息有此字段）
	char userData[BUF_LEN_4096];//扩展（Push状态消息有此字段）
	int	 netType;//参考ECNetWorkType
	int	 state;//参考ECStateType
	int	 subState;//参考ECSubStateType（Push状态消息有此字段）
	int  device;//终端类型（主动获取回调有此字段） 1: Android Phone 2: iPhone  10: iPad  11: Android Pad  20: PC  (Just Allowed phone 2 PC(Pad) login)
	_PersonState()
	{
		memset(this,0,sizeof(_PersonState));
	}
}ECPersonState;

typedef struct _ECMultiDeviceState
{
	int	 state;//1在线 2离线
	int  device;//终端类型（主动获取回调有此字段） 1: Android Phone 2: iPhone  10: iPad  11: Android Pad  20: PC  (Just Allowed phone 2 PC(Pad) login)
	_ECMultiDeviceState()
	{
		memset(this,0,sizeof(_ECMultiDeviceState));
	}
}ECMultiDeviceState;

/****************************登录接口的回调******************************/
typedef void (*onLogInfo)(const char* loginfo); // 用于接收底层的log信息,调试出现的问题.
typedef void (*onLogOut)(int reason);//登出回调。reason：200成功，其他报错；
typedef void (*onConnectState)(const ECConnectState state );//连接回调；
typedef void (*onSetPersonInfo)(unsigned int matchKey, int reason, unsigned long long version);//设置个人资料。reason：200成功，其他报错；version:个人资料版本号
typedef void (*onGetPersonInfo)(unsigned int matchKey, int reason, ECPersonInfo *pPersonInfo);//获取个人资料。reason：200成功，其他报错；
typedef void (*onGetUserState)(unsigned int matchKey, int reason, int count,ECPersonState *pStateArr);//获取用户状态，reason：200成功，其他报错;count 数组个数，pStateArr 用户状态数组首指针
typedef void (*onPublishPresence)(unsigned int matchKey, int reason);//发布用户状态，reason：200成功，其他报错
typedef void (*onReceiveFriendsPublishPresence)(int count,ECPersonState *pStateArr);//收到服务器推送的用户状态变化,count 数组个数，pStateArr 用户状态数组首指针
typedef void (*onSoftVersion)(const char* softVersion, int updateMode, const char* updateDesc);//应用软件最新版本号升级提示，供应用自己升级用。softVersion：软件最新版本号；updateMode:更新模式  1：手动更新 2：强制更新；updateDesc：软件更新说明 
typedef void (*onGetOnlineMultiDevice)(unsigned int matchKey, int reason, int count,ECMultiDeviceState *pMultiDeviceStateArr);//获得登录者多设备登录状态变化回调,reason：200成功，其他报错count 数组个数，pMultiDeviceStateArr 多设备登录状态数组首指针
typedef void (*onReceiveMultiDeviceState)(int count,ECMultiDeviceState *pMultiDeviceStateArr);//收到服务器推送的登录者多设备登录状态变化,count 数组个数，pMultiDeviceStateArr 多设备登录状态数组首指针

typedef void (*onSendCmdToRest)(unsigned int matchKey, int reason);//rest交换回调
typedef void (*onPublishPresence_RX)(unsigned int matchKey, int reason);//发布用户状态 reason：200成功，其他报错
typedef void (*onReceiveFriendsPublishPresence_RX)(int count, ECPersonState *pStateArr);//收到服务器推送的用户状态变化,count 数组个数，pStateArr 用户状态数组首指针

//回调结构体
typedef struct _GeneralCallBackInterface{
	onLogInfo pfonLogInfo; 
	onLogOut  pfonLogOut;
	onConnectState pfonConnectState;
	onSetPersonInfo pfonSetPersonInfo;
	onGetPersonInfo pfonGetPersonInfo;
	onGetUserState pfonGetUserState;
	onPublishPresence pfonPublishPresence;
	onReceiveFriendsPublishPresence pfonReceiveFriendsPublishPresence;
	onSoftVersion pfonSoftVersion;
	onGetOnlineMultiDevice pfonGetOnlineMultiDevice;
	onReceiveMultiDeviceState pfonReceiveMultiDeviceState;

	onSendCmdToRest pfonSendCmdToRest;
	onPublishPresence_RX pfonPublishPresence_RX;
	onReceiveFriendsPublishPresence_RX pfonReceiveFriendsPublishPresence_RX;

	_GeneralCallBackInterface():pfonLogInfo(NULL),pfonLogOut(NULL),pfonConnectState(NULL),
		pfonSetPersonInfo(NULL),pfonGetPersonInfo(NULL),pfonGetUserState(NULL),pfonPublishPresence(NULL),
		pfonReceiveFriendsPublishPresence(NULL),pfonSoftVersion(NULL),pfonGetOnlineMultiDevice(NULL),
		pfonReceiveMultiDeviceState(NULL)

		,pfonSendCmdToRest(NULL)
		,pfonPublishPresence_RX(NULL)
		,pfonReceiveFriendsPublishPresence_RX(NULL)

	{

	}
}ECGeneralCallBackInterface;
#pragma pack ()

#endif 
