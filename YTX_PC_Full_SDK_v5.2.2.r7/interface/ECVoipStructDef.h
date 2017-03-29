#ifndef _EC_VOIP_STRUCT_DEFINE_H
#define _EC_VOIP_STRUCT_DEFINE_H

#include "ECStructDef.h"

enum {//呼叫类型
	EC_VOICE_CALL=0,//语音VoIP电话
	EC_VIDEO_CALL,//音视频VoIP电话
	EC_VOICE_CALL_LANDING,//语音落地电话
};
typedef enum{
	VEUnknown = -1,
	VECallProceeding,/** 外呼，服务器回100Tring */
	VECallAlerting, /** 外呼对方振铃 */
	VECallFailed,   /** 外呼失败 */
	VECallAnswered, /** 通话，外呼和来电 */
	VECallReleased, /** 释放呼叫请求中 */
    VECallEnd
}ECVoipEventCode;
enum {//摄像头偏转角度
	EC_ROTATE_AUTO,
	EC_ROTATE_0,
	EC_ROTATE_90,
	EC_ROTATE_180,
	EC_ROTATE_270
};
enum//设置音频处理的开关类型
{
    EC_AUDIO_AGC,//自动增益控制
    EC_AUDIO_EC,//回声消除
    EC_AUDIO_NS//噪声抑制
};
/*
AUDIO_NS: 噪声抑制，主要是去掉原始语音中的噪声。
默认是开启的。
下面的模式都可以用，值越大，语音越纯净，噪声越小，可能会导致语音丢失。现在底层默认是开启最大的kNsVeryHighSuppression。
*/
enum NsMode    // type of Noise Suppression
{
    EC_NsUnchanged = 0,   // previously set mode
    EC_NsDefault,         // platform default
    EC_NsConference,      // conferencing default
    EC_NsLowSuppression,  // lowest suppression
    EC_NsModerateSuppression,
    EC_NsHighSuppression,
    EC_NsVeryHighSuppression,     // highest suppression
};
/*
AUDIO_AGC: 自动增益控制，如果语音声音小,会放大声音;如果声音很大,会调低声音。
因为开启后，周围的小声音也会变大，导致杂音，底层默认是关闭的。
如果开启，模式建议值是kAgcAdaptiveDigital。
*/
enum AgcMode                  // type of Automatic Gain Control
{
    EC_AgcUnchanged = 0,        // previously set mode
    EC_AgcDefault,              // platform default
    // adaptive mode for use when analog volume control exists (e.g. for
    // PC softphone)
    EC_AgcAdaptiveAnalog,
    // scaling takes place in the digital domain (e.g. for conference servers
    // and embedded devices)
    EC_AgcAdaptiveDigital,
    // can be used on embedded devices where the capture signal level
    // is predictable
    EC_AgcFixedDigital
};
/*
AUDIO_EC:回声消除。
默认是开启的。如果关闭，可能导致回声。
如果开启，模式建议值是kEcConference。
*/
// EC modes
enum EcMode                  // type of Echo Control
{
    EC_EcUnchanged = 0,          // previously set mode
    EC_EcDefault,                // platform default
    EC_EcConference,             // conferencing default (aggressive AEC)
    EC_EcAec,                    // Acoustic Echo Cancellation
    EC_EcAecm,                   // AEC mobile
};
enum  {//音频编码
    EC_codec_iLBC,
    EC_codec_G729,
    EC_codec_PCMU,
    EC_codec_PCMA,
    EC_codec_H264,
    EC_codec_SILK8K,
    EC_codec_AMR,
	EC_codec_VP8,
	EC_codec_SILK16K,
};

typedef enum {//共享设备类型
	EC_Share_Device_Type_Screen =0,//屏幕共享
	EC_Share_Device_Type_Windows,//窗体共享
} ECShareDeviceType;

#pragma pack (1)
typedef struct _ECMediaStatisticsInfo
{
	unsigned short fractionLost;//上次调用获取统计后这一段时间的丢包率，范围是0~255，255是100%丢失。
	unsigned int cumulativeLost;//开始通话后的所有的丢包总个数
	unsigned int extendedMax;   //开始通话后应该收到的包总个数
	unsigned int jitterSamples; //抖动
	int rttMs;                  //延迟时间，单位是ms
	unsigned int bytesSent;     //开始通话后发送的总字节数
	unsigned int packetsSent;   //开始通话后发送的总RTP包个数
	unsigned int bytesReceived; //开始通话后收到的总字节数
	unsigned int packetsReceived;//开始通话后收到的总RTP包个数

	_ECMediaStatisticsInfo()
	{
		memset(this,0,sizeof(_ECMediaStatisticsInfo));
	}

	_ECMediaStatisticsInfo(const _ECMediaStatisticsInfo& rOther)
	{
		*this = rOther;
	}

	_ECMediaStatisticsInfo& operator=(const _ECMediaStatisticsInfo& rOther);

} ECMediaStatisticsInfo;

typedef struct _ECCameraCapability
{
	int width;//宽
	int height;//高
	int maxfps;//最大帧率
	_ECCameraCapability()
	{
		memset(this,0,sizeof(_ECCameraCapability));
	}
	_ECCameraCapability(const _ECCameraCapability& rOther)
	{
		*this = rOther;
	}

	_ECCameraCapability& operator=(const _ECCameraCapability& rOther);

} ECCameraCapability;

typedef struct _ECCameraInfo
{
	int index;//摄像头索引
	char name[BUF_LEN_256];	//摄像头名称
	char id[BUF_LEN_256];//摄像头ID
	int capabilityCount;//摄像头能力个数
	ECCameraCapability *capability;//摄像头能力首指针

	_ECCameraInfo()
	{
		memset(this,0,sizeof(_ECCameraInfo));
	}

	_ECCameraInfo(const _ECCameraInfo& rOther)
	{
		*this = rOther;
	}

	_ECCameraInfo& operator=(const _ECCameraInfo& rOther);

}  ECCameraInfo ;

typedef struct _ECSpeakerInfo//喇叭
{
	int index;//索引
	char name[BUF_LEN_128];  //utf8 设备名称
	char guid[BUF_LEN_128];	  //utf8 设备号

	_ECSpeakerInfo()
	{
		memset(this,0,sizeof(_ECSpeakerInfo));
	}

	_ECSpeakerInfo(const _ECSpeakerInfo& rOther)
	{
		*this = rOther;
	}

	_ECSpeakerInfo& operator=(const _ECSpeakerInfo& rOther);

}  ECSpeakerInfo ;

typedef struct _ECMicroPhoneInfo//麦克风
{
	int index;//索引
	char name[BUF_LEN_128];   //utf8设备名称
	char guid[BUF_LEN_128];    //utf8设备号

	_ECMicroPhoneInfo()
	{
		memset(this,0,sizeof(_ECMicroPhoneInfo));
	}

	_ECMicroPhoneInfo(const _ECMicroPhoneInfo& rOther)
	{
		*this = rOther;
	}

	_ECMicroPhoneInfo& operator=(const _ECMicroPhoneInfo& rOther);

}  ECMicroPhoneInfo ;

typedef struct _ECShareWindowInfo//共享窗体
{
	long long id;//ID
	// Title of the window in UTF-8 encoding.
	char title[BUF_LEN_512];//标题

	_ECShareWindowInfo()
	{
		memset(this,0,sizeof(_ECShareWindowInfo));
	}

	_ECShareWindowInfo(const _ECShareWindowInfo& rOther)
	{
		*this = rOther;
	}

	_ECShareWindowInfo& operator=(const _ECShareWindowInfo& rOther);

}ECShareWindowInfo;

typedef struct _VoIPCall{
	ECVoipEventCode eCode;//事件状态
	int callType;//通话类型enum {//呼叫类型EC_VOICE_CALL=0,//语音VoIP电话EC_VIDEO_CALL,//音视频VoIP电话EC_VOICE_CALL_LANDING,//语音落地电话};
	char callid[BUF_LEN_64];//通话ID
	char caller[BUF_LEN_64];//主叫
	char callee[BUF_LEN_64];//被叫
	_VoIPCall()
	{
		memset(this,0,sizeof(_VoIPCall));
	}
}ECVoIPCallMsg;

typedef struct _VoIPComingInfo{//来电消息
	int callType;//通话类型enum {//呼叫类型EC_VOICE_CALL=0,//语音VoIP电话EC_VIDEO_CALL,//音视频VoIP电话EC_VOICE_CALL_LANDING,//语音落地电话};
	char callid[BUF_LEN_64];//通话ID
	char caller[BUF_LEN_64];//主叫
	char nickname[BUF_LEN_128];//昵称
	char display[BUF_LEN_20];//显号
	_VoIPComingInfo()
	{
		memset(this,0,sizeof(_VoIPComingInfo));
	}
}ECVoIPComingInfo;

typedef struct _CallBackInfo{//双向回呼消息
	char caller[BUF_LEN_64];//主叫
	char called[BUF_LEN_64];//被叫
	char callerDisplay[BUF_LEN_20];//主叫显号
	char calledDisplay[BUF_LEN_20];//被叫显号
	_CallBackInfo()
	{
		memset(this,0,sizeof(_CallBackInfo));	
	}
}ECCallBackInfo;


/****************************VOIP回调接口******************************/
typedef void (*onMakeCallBack)(unsigned int matchKey,int reason,ECCallBackInfo *pInfo);//回拨请求。reason：200成功，其他报错；
typedef void (*onCallEvents)(int reason,const ECVoIPCallMsg * call);//事件状态
typedef void (*onCallIncoming)(ECVoIPComingInfo *pComing);  //音视频呼叫接到邀请
typedef void (*onDtmfReceived)(const char *callId, char dtmf);//收到DTMF按键时的回调。callId 通话ID；	
typedef void (*onSwitchCallMediaTypeRequest)(const char*callId,int video); // 收到对方更新媒体请求。 callId 通话ID；video：1  请求增加视频（需要响应） 0:请求删除视频（可以不响应，底层自动去除视频）
typedef void (*onSwitchCallMediaTypeResponse)(const char*callId,int video);  // 对方应答媒体状态结果。callId 通话ID； video 1 有视频 0 无视频
typedef void (*onRemoteVideoRatio)(const char *CallidOrConferenceId, int width, int height, int type, const char *member);//远端视频媒体分辨率变化时上报。type : 0 点对点,1 视频会议，2 共享会议；type=0 CallidOrConferenceId为通话id，type>0 CallidOrConferenceId为会议id；member有效

//回调结构体
typedef struct _VoipCallBackInterface{
	onCallEvents pfnonCallEvents;
	onMakeCallBack pfonMakeCallBack;
	onCallIncoming pfonCallIncoming;
	onDtmfReceived pfonDtmfReceived;
	onSwitchCallMediaTypeRequest pfonSwitchCallMediaTypeRequest; 
	onSwitchCallMediaTypeResponse pfonSwitchCallMediaTypeResponse; 
	onRemoteVideoRatio pfonRemoteVideoRatio;
	_VoipCallBackInterface():pfnonCallEvents(NULL),pfonMakeCallBack(NULL),pfonCallIncoming(NULL),pfonDtmfReceived(NULL),
		pfonSwitchCallMediaTypeRequest(NULL),pfonSwitchCallMediaTypeResponse(NULL),pfonRemoteVideoRatio(NULL)
		
	{

	}
}ECVoipCallBackInterface;

#pragma pack ()

#endif