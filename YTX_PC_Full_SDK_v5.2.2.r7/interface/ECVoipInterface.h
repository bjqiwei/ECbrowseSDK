#ifndef _EC_VOIP_INTERFACE_H
#define _EC_VOIP_INTERFACE_H

#include "ECVoipStructDef.h"
#ifdef __cplusplus
extern "C" 
{
#endif
//////////////////////////////////////////////////回调函数接口///////////////////////////////////////////////////////////
	//回调设置接口，必须在主调前调用
	EC_CALL_API void SetVoIPCallBackFuction(ECVoipCallBackInterface *VoipInterface);


//////////////////////////////////////////////////主调函数接口///////////////////////////////////////////////////////////
	    /*! @function
     ********************************************************************************
     函数名   : MakeCallBack
     功能     : 回拨
	 参数     : [OUT] matchKey      : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  caller		: 主叫方号码。根据呼叫类型不同，格式也不同
				[IN]  called		: 被叫方号码。根据呼叫类型不同，格式也不同
				[IN]  callerDisplay	: 主叫方名称。
				[IN]  calledDisplay	: 被叫方名称。   
     返回值   : 是否成功 0：成功； 非0失败
     回调函数 ：(*onMakeCallBack)
     *******************************************************************************/
    EC_CALL_API int MakeCallBack(unsigned int *matchKey,const char *caller, const char *called, const char *callerDisplay,const char *calledDisplay);
    
	/*! @function
	********************************************************************************
	函数名   : MakeCall
	功能     : 发起呼叫		
	参数     : 	[IN]  callType	  : 呼叫类型enum {VOICE_CALL,//语音VoIP电话  VIDEO_CALL,//视频VoIP电话   VOICE_CALL_LANDING//语音落地电话};
				[IN]  called	  : 被叫方号码。			                                          
				[out]OutCallid    : 此路呼叫的callid
	返回值   : 是否成功 0：成功； 非0失败
	回调函数 ：void (*onCallEvents)
	*******************************************************************************/
	EC_CALL_API int MakeCall(const char **OutCallid,int callType, const char *called);

	/*! @function
	********************************************************************************
	函数名   : AcceptCall
	功能     : 应答呼入。可以选择媒体类型
	参数     : [IN]  callId	  : 当前呼叫的唯一标识
	返回值   : 是否成功 0：成功； 非0失败
	回调函数 ：void (*onCallEvents)
	*******************************************************************************/
	EC_CALL_API int AcceptCall(const char *callId);

	/*! @function
	********************************************************************************
	函数名   : ReleaseCall
	功能     : 挂机。二十秒没有语音流，SDK自动挂机
	参数     : [IN]  callId	  : 当前呼叫的唯一标识。
	参数     ：[IN]  reason   ：挂机原因。正常挂机0，取消呼叫0，拒接175603，被叫忙175486
	返回值   : 是否成功 0：成功； 非0失败
	回调函数 ：void (*onCallEvents)
	*******************************************************************************/
	EC_CALL_API int ReleaseCall(const char *callId, int reason);

	/*! @function
	********************************************************************************
	函数名   : SendDTMF
	功能     : 发送按键信息
	参数     : [IN]  callId	  : 当前呼叫的唯一标识.
			   [IN]	 dtmf	  : 一个按键值
	返回值   : 是否成功 0：成功； 非0失败
	回调函数 ：void (*onDtmfReceived)
	*******************************************************************************/
	EC_CALL_API int SendDTMF(const char *callId, const char dtmf);

	/*! @function
	********************************************************************************
	函数名   : GetCurrentCall
	功能     : 获取当前通话的callId 
	参数     : 
	返回值   : 成功为 callId ，失败为NULL
	*******************************************************************************/
	EC_CALL_API const char* GetCurrentCall();

	/*! @function
	********************************************************************************
	函数名   : SetMute
	功能     : 通话过程中设置麦克风设备静音，自己能听到对方的声音，通话对方听不到自己的声音。
	参数     : [IN]  mute : 是否开启
	返回值   : 是否成功 0：成功； 非0失败
	*******************************************************************************/
	EC_CALL_API int SetMute(bool mute);

	/*! @function
	********************************************************************************
	函数名   : GetMuteStatus(
	功能     : 获取麦克风设备静音状态
	参数     : 无
	返回值   :  true 开启； false关闭
	*******************************************************************************/
	bool EC_CALL_API GetMuteStatus();

	/*! @function
	********************************************************************************
	函数名   : SetSoftMute
	功能     : 通话过程中设置软静音，自己能听到对方的声音，通话对方听不到自己的声音。
	参数     : [IN]  callid	: 当前呼叫的唯一标识
			   [IN]  on     : 是否开启
	返回值   : 是否成功 0：成功； 非0失败
	*******************************************************************************/
	EC_CALL_API int SetSoftMute(const char *callid,bool on);

	/*! @function
	********************************************************************************
	函数名   : GetSoftMuteStatus
	功能     : 获取软静音状态
	参数     : [IN]  callid	: 当前呼叫的唯一标识
			   [IN]  bMute  : 是否开启
	返回值   : 是否成功 0：成功； 非0失败
	*******************************************************************************/
	EC_CALL_API int GetSoftMuteStatus(const char *callid,bool* bMute);

    /*! @function
    ********************************************************************************
     函数名   : SetAudioConfigEnabled
     功能     : 设置音频处理的开关
     参数     : [IN] type : 音频处理类型.  AUDIO_AGC, AUDIO_EC, AUDIO_NS
                [IN] enabled:  AGC默认关闭; EC和NS默认开启.
                [IN] mode: 各自对应的模式中的枚举值: AgcMode、EcMode、NsMode.
     返回值   :  是否成功 0：成功； 非0失败
    *****************************************************************************/
    EC_CALL_API int SetAudioConfigEnabled(int type, bool enabled, int mode);

	/*! @function
    ********************************************************************************
     函数名   : GetAudioConfigEnabled
     功能     : 获得音频处理的开关状态
     参数     :  [IN] type : 音频处理类型. AUDIO_AGC, AUDIO_EC, AUDIO_NS
                 [OUT] enabled:  AGC默认关闭; EC和NS默认开启.
                 [OUT] mode: 各自对应的模式中的枚举值: AgcMode、EcMode、NsMode.
     返回值   :  是否成功 0：成功； 非0失败
    *****************************************************************************/
    EC_CALL_API int GetAudioConfigEnabled(int type, bool *enabled, int *mode);

	 /*! @function
     ********************************************************************************
     函数名   : GetCodecEnabled
     功能     : 设置支持的编解码方式，默认全部都支持
     参数     : [IN]  type : 编解码类型.
						enum  {
							codec_iLBC,
							codec_G729,
							codec_PCMU,
							codec_PCMA,
							codec_H264,
							codec_SILK8K,
							codec_AMR,
							codec_VP8,
							codec_SILK16K,
						};
                [IN] enabled: 0 不支持；1 支持
     返回值   :  是否成功 0：成功； 非0失败
     *******************************************************************************/
	EC_CALL_API int SetCodecEnabled(int type,bool enabled);

	 /*! @function
     ********************************************************************************
     函数名   : GetCodecEnabled
     功能     : 获得编解码方式是否被支持
     参数     : [IN]  type : 编解码类型.
                     codec_iLBC,
                     codec_G729,
                     codec_PCMU,
                     codec_PCMA,
                     codec_H264,
                     codec_SILK8K,
     返回值   :  true 支持；false不支持
     *******************************************************************************/
	bool EC_CALL_API GetCodecEnabled(int type);

	/*! @function
     ********************************************************************************
     函数名   : SetCodecNack
     功能     : 设置媒体流重传。打开后通话质量变好，但是延迟可能增加
     参数     : [IN] bAudioNack:音频开关,底层默认true
                [IN] bVideoNack:视频开关,底层默认true
     返回值   :  是否成功 0：成功； 非0失败
     *******************************************************************************/
	EC_CALL_API int SetCodecNack(bool bAudioNack,bool bVideoNack);

	 /*! @function
     ********************************************************************************
	 函数名   : GetCodecNack
     功能     : 获得媒体流重传当前设置值。
     参数     : [OUT] bAudioNack:音频开关
                [OUT] bVideoNack:视频开关
     返回值   :  是否成功 0：成功； 非0失败
     *******************************************************************************/
	EC_CALL_API int GetCodecNack(bool *bAudioNack,bool *bVideoNack);

	 /*! @function
     ********************************************************************************
     函数名   : GetCallStatistics
     功能     : 获取通话中的统计数据
     参数     : [IN] callId     : 当前呼叫的唯一标识.
			  : [IN] bVideo     : true 统计视频通话数据，false 统计音频通话数据
                [OUT] statistics:  CallStatisticsInfo结构的统计数据
     返回值   :  是否成功 0：成功； 非0失败
     *****************************************************************************/
	EC_CALL_API int GetCallStatistics(const char *callId, bool bVideo,ECMediaStatisticsInfo *statistics);

 	 /*! @function
     ********************************************************************************
     函数名   :  SetVideoView
	 功能     :  设置视频通话显示。呼叫前和应答前调用
	 参数     :  [IN] view      : 对方显示视图
				 [IN] localView : 本地显示视图
     返回值   :  0 成功； 非0失败 
     *********************************************************************************/
	EC_CALL_API int SetVideoView( void *view, void *localView);
	
	 /*! @function
     ********************************************************************************
     函数名   : SetVideoBitRates
     功能     : 设置视频压缩的码流。如果调用，需要在建立视频通话前 
     参数     :  [IN] bitrates :  视频码流，单位kb/s
	 返回值   :  无
     ***************************************************************************/
    EC_CALL_API void SetVideoBitRates(int bitrates);

	 /*! @function
     ********************************************************************************
     函数名   : SetRing
	 功能     : 设置本地振铃。当来电时，本地播放振铃音。默认为播放.\\ring.wav
	 参数     : [IN] fileName : 声音文件，格式为.wav,采样率16000HZ,16bit,单声道。
     返回值   : 是否成功 0：成功； 非0失败
     *********************************************************************************/
	 EC_CALL_API int SetRing(const char* fileName);

	 /*! @function
     ********************************************************************************
     函数名   : SetRingback
	 功能     : 设置本地回铃音。当外呼VoIP时，本地播放回铃音。默认为播放.\\ringback.wav
	 参数     : [IN] fileName : 声音文件，格式为.wav.wav,采样率16000HZ,16bit,单声道。
     返回值   : 是否成功 0：成功； 非0失败
     *********************************************************************************/
	 EC_CALL_API int SetRingback(const char*fileName);

	 /*! @function
     ********************************************************************************
     函数名   : RequestSwitchCallMediaType
     功能     : 更新通话中媒体类型。voice <->video,注意：该接口只能在视频通话时才能调用。也就是说在一开始创建
				的是视频通话，后可以调用该函数转换成音频通话，此时的音频通话还可以调用该函数来切换到视频通话，
				如果一开始创建的就是音频通话则不可以调用该函数来切换视频通话。另外当从视频切换到音频是不需要对方
				同意即可转换，但从音频切换到视频时必须要对方调用CCPresponseSwitchCallMediaType接口，并把video置为
				1才可以，否则转换不成功。
     参数     : [IN]  callId   : 当前呼叫的唯一标识.
                [IN]  video    : 1 添加视频； 0 去除视频
     返回值   : 是否成功 0：成功； 非0失败
	 回调函数 ：对方 void (*onSwitchCallMediaTypeRequest)
	            本地 void (*onSwitchCallMediaTypeResponse) 
     *******************************************************************************/
     EC_CALL_API int RequestSwitchCallMediaType(const char *callId, int video);
    
    /*! @function
     ********************************************************************************
     函数名   : ResponseSwitchCallMediaType
     功能     : 回复对方的更新通话中媒体类型请求。当音频恢复视频请求时调用，删除视频请求时无效
     参数     : [IN]  callId   : 当前呼叫的唯一标识.
                [IN]  video    : 1 添加视频； 0 去除视频
     返回值   : 是否成功 0：成功； 非0失败
     *******************************************************************************/
     EC_CALL_API int ResponseSwitchCallMediaType(const char *callId, int video);
	 
	 /*! @function
     ********************************************************************************
     函数名   : GetCameraInfo
     功能     : 查询摄像头信息。
     参数     : [IN] info :  摄像头信息CameraInfo
     返回值   : 摄像头个数
     *********************************************************************************/
	EC_CALL_API int GetCameraInfo(ECCameraInfo **  info);

	 /*! @function
     ********************************************************************************
     函数名   : SelectCamera
     功能     : 选择摄像头。可以在通话过程中选择；如果不调用，底层将使用系统默认摄像头
     参数     :  [IN] cameraIndex     : CameraInfo的index值。从CCPgetCameraInfo获取
				 [IN] capabilityIndex : CameraCapability的index值。范围[0,capabilityCount-1]
				 [IN] fps             : 最大帧数       
				 [IN] rotate          : 旋转的角度（enum {ROTATE_AUTO,ROTATE_0,ROTATE_90,ROTATE_180,ROTATE_270};中的值）
				 [IN] force           : 是否强制启动本SDK调用的摄像头。默认选false
     返回值   :  是否成功 0：成功； 非0失败
     *********************************************************************************/
	 EC_CALL_API int SelectCamera(int cameraIndex, int capabilityIndex,int fps,int rotate,bool force);

     /*! @function ************************************************************************ 
	 函数名 : GetNetworkStatistic
	 功能 : 获取通话的网络流量信息
	 参数 : [IN]  callId       : 当前呼叫的唯一标识。会议时，输入会议id
	        [OUT] duration     ：媒体交互的持续时间，单位秒，可能为0。不能作为计费结算时长，只是媒体流的统计累积时长；存储8字节
	        [OUT] sendTotalWifi：在duration时间内，网络发送的总流量，单位字节。存储8字节
	        [OUT] recvTotalWifi：在duration时间内，网络接收的总流量，单位字节。存储8字节
	 返回值 : 是否成功 0：成功； 非0失败
	***********************************************************************/
	EC_CALL_API int GetNetworkStatistic(const char *callId, long long *duration,long long *sendTotalWifi, long long *recvTotalWifi);

	    /*! @function
    ********************************************************************************
     函数名   : GetSpeakerVolume
     参数    : [IN] volume ：范围：【0~255】
     返回值   : 是否成功 0：成功； 非0失败
    *******************************************************************************/
	EC_CALL_API  int GetSpeakerVolume(unsigned int* volume);

	/*! @function
    ********************************************************************************
     函数名   : SetSpeakerVolume
     功能     : 设置扬声器音量的默认值
     参数     : [IN] volume ：范围：【0~255】
     返回值   : 0：是否成功 0：成功； 非0失败
    *******************************************************************************/
	EC_CALL_API  int SetSpeakerVolume(unsigned int volume);

	/*! @function
    ********************************************************************************
     函数名   : SetDtxEnabled
     功能     : 舒适噪音开关，默认是关闭。关闭后没有舒适噪音，减少带宽。 
     参数     : [IN]  enabled :   true 打开; false 关闭    
     返回值   : 无 
	***************************************************************************/
   EC_CALL_API void SetDtxEnabled(bool enabled);
  
   /*! @function
	********************************************************************************
	函数名   : CCPsetSelfPhoneNumber
	功能     : 设置用户虚拟数据 电话号码
	参数     : [IN]  phoneNumber : 电话号码
	返回值   : 无
	*******************************************************************************/
	EC_CALL_API void SetSelfPhoneNumber(const char * phoneNumber);

	/*! @function
	********************************************************************************
	函数名   : CCPsetSelfName
	功能     : 设置用户虚拟数据 昵称
	参数     : [IN]  nickName : 昵称
	返回值   : 是否成功 0：成功； 非0失败
	*******************************************************************************/
	EC_CALL_API void SetSelfName(const char * nickName);
    
	/*! @function
	********************************************************************************
	函数名   : EnableLoudsSpeaker
	功能     : 设置扬声器状态,
	参数     : [IN]  enable : 是否开启
	返回值   : 是否成功 0：成功； 非0失败
	*******************************************************************************/
	EC_CALL_API int EnableLoudsSpeaker(bool enable);

	/*! @function
	********************************************************************************
	函数名   : GetLoudsSpeakerStatus
	功能     : 获取当前扬声器否开启状态
	参数     : 
	返回值   : true 开启； false关闭
	*******************************************************************************/
	EC_CALL_API bool GetLoudsSpeakerStatus();

	/*! @function
	********************************************************************************
	函数名   : SetLocalCamera
	功能     : 通话过程中设置本端摄像头开启关闭，自己能看到对方，通话对方看不到自己。
	参数     : [IN]  callid	: 当前呼叫的唯一标识
			   [IN]  on     : 是否开启
	返回值   : 是否成功 0：成功； 非0失败
	*******************************************************************************/
	EC_CALL_API int SetLocalCamera(const char *callid,bool on);

			
	/*! @function
	********************************************************************************
	函数名   : SetLocalShareDevice
	功能     : 通话过程中设置本端数据共享开启关闭，通话对方看不到自己。
	参数     : [IN]  callid	: 当前呼叫的唯一标识
			   [IN]  on     : 是否开启
	返回值   : 是否成功 0：成功； 非0失败
	*******************************************************************************/
	EC_CALL_API int SetLocalShareDevice(const char *callid,bool on);

	/*! @function
     ********************************************************************************
     函数名   : ResetVideoView
     功能     : 通话中重置窗体
     参数     : [IN] callid   : 当前呼叫的唯一标识.
                [IN] view      : 对方显示视图
				[IN] localView : 本地显示视图
     返回值   : 是否成功 0：成功； 非0失败
     *******************************************************************************/
     EC_CALL_API int ResetVideoView(const char *callid, void* remoteView,void *localView);

	 /*! @function
     ********************************************************************************
     函数名   : GetCallType
     功能     : 获取通话的类型。思科后协商，需要在通话后获取真正呼叫类型
     参数     : [IN]  callid	  : 当前呼叫的唯一标识.
     返回值   : 成功返回 enum {//呼叫类型
				EC_VOICE_CALL=0,//语音VoIP电话
				EC_VIDEO_CALL,//音视频VoIP电话
				EC_VOICE_CALL_LANDING,//语音落地电话
				EC_SHARE_CALL,//共享VoIP电话
				EC_VOICE_SHARE_CALL,//语音和共享VoIP电话
				EC_VIDEO_SHARE_CALL,//音视频和共享VoIP电话
				};中值; 其他值失败
     *******************************************************************************/
     EC_CALL_API int GetCallType(const char *callid);

	 /*! @function
     ********************************************************************************
     函数名   : GetShareScreenInfo
     功能     : 查询屏幕信息。
     参数     : [IN] screenId :屏幕信息  
     返回值   : 个数
     *********************************************************************************/
	 EC_CALL_API int GetShareScreenInfo(long long** screenId);

	/*! @function
     ********************************************************************************
     函数名   : GetShareWindowInfo
     功能     : 查询窗口信息。
     参数     : [IN] info :窗口信息ECShareWindowInfo
     返回值   : 个数
     *********************************************************************************/
	 EC_CALL_API int GetShareWindowInfo(ECShareWindowInfo **  info);

	 /*! @function
     ********************************************************************************
     函数名   : SelectShareDevice
     功能     : 选择共享。可以在通话过程中已经共享后选择；
     参数     :  [IN] type : 共享设备类行0 screen; 1 windows
				 [IN] id   : 共享设备id
     返回值   :  是否成功 0：成功； 非0失败
     *********************************************************************************/
	 EC_CALL_API int SelectShareDevice(int type,long long id);

	 /*! @function ************************************************************************
	 函数名 : getLocalVideoSnapshot
	 功能 : 本地快照。
	 参数 : [IN] callid : 当前呼叫的唯一标识.
	 [IN] fileName : 快照文件保存的全路径，含后缀名.bmp
	 返回值 : 是否成功 0：成功； 非0失败
	 ***********************************************************************/
	 EC_CALL_API int GetLocalVideoSnapshot(const char* callid, const char* fileName);
	 EC_CALL_API int GetLocalVideoSnapshotEx(const char* callid, unsigned char **buf, unsigned int *size, unsigned int *width, unsigned int *height); 

	 /*! @function ************************************************************************
	 函数名 : getRemoteVideoSnapshot
	 功能 : 远程快照。
	 参数 : [IN] callid : 当前呼叫的唯一标识.
	 [IN] fileName : 快照文件保存的全路径，含后缀名.bmp
	 返回值 : 是否成功 0：成功； 非0失败
	 ***********************************************************************/
	 EC_CALL_API int GetRemoteVideoSnapshot(const char* callid, const char* fileName);
	 EC_CALL_API int GetRemoteVideoSnapshotEx(const char* callid, unsigned char **buf, unsigned int *size, unsigned int *width, unsigned int *height);

	 /*! @function ************************************************************************
	 函数名 : startRecordScreen
	 功能 : 通话过程中，开始录屏
	 参数 : [IN] callid : 会话id；
	 [IN] filename：录屏保存的文件名；
	 [IN] bitrate：调节录屏压缩码流，默认640；
	 [IN] fps：录屏的帧数，默认10帧每秒；
	 [IN] type: 录屏的屏幕选择， 0：主屏 1：辅屏
	 返回值 : 0：成功 -1：失败
	 ***********************************************************************/
	 EC_CALL_API int StartRecordScreen(const char *callid, const char *filename, int bitrate, int fps, int type);


	 /*! @function ************************************************************************
	 函数名 : startRecordScreenEx
	 功能 : 通话过程中，开始录屏
	 参数 : [IN] callid : 会话id；
	 [IN] filename：录屏保存的文件名；
	 [IN] bitrate：调节录屏压缩码流，默认640；
	 [IN] fps：录屏的帧数，默认10帧每秒；
	 [IN] type: 录屏的屏幕选择， 0：主屏 1：辅屏
	 [IN] left, top, width, height : 屏幕区域
	 返回值 : 0：成功 -1：失败
	 ***********************************************************************/
	 EC_CALL_API int StartRecordScreenEx(const char *callid, const char *filename, int bitrate, int fps, int type, int left, int top, int width, int height);

	 /*! @function ************************************************************************
	 函数名 : stopRecordScreen
	 功能 : 停止录屏，通话结束时，会主动调用本函数。
	 参数 : [IN] callid : 会话id；
	 返回值 : 0：成功 -1：失败
	 ***********************************************************************/
	 EC_CALL_API int StopRecordScreen(const char *callid);

#ifdef __cplusplus
}
#endif

#endif