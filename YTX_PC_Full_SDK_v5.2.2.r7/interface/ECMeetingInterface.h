#ifndef _EC_MEETING_INTERFACE_H
#define _EC_MEETING_INTERFACE_H

#include "ECMeetingStructDef.h"
#ifdef __cplusplus
extern "C" 
{
#endif

//////////////////////////////////////////////////回调函数接口///////////////////////////////////////////////////////////
	 //回调设置接口，必须在主调前调用
	 EC_CALL_API void SetMeetingCallBackFuction(ECMeetingCallBackInterface *meetingInterface);


//////////////////////////////////////////////////主调函数接口///////////////////////////////////////////////////////////

	/*! @function
     ********************************************************************************
     函数名   : RequestConferenceMemberVideo
     功能     : 视频会议中请求某一远端视频
     参数     : [IN]  conferenceId	   :  所在会议号.
                [IN]  conferencePasswd : 所在会议密码.可选
                [IN]  member           :  请求远端成员.
                [IN]  videoWindow      :  当成功请求时，展示该成员的窗口.
				[IN]  type             :  详见ECVideoDataType.默认摄像头像源
     返回值   : 成功 0 失败 -1(remoteSipNo为NULL) -2(videoWindow为NULL) -3(conferenceId为NULL) -4(confPasswd为NULL) -5(自己的sip号为NULL) -6(会议服务器的ip为NULL) -7(该账户的视频已经成功请求) -8(正在停止当前用户视频流，需要等待)
     回调函数 ：void (*onRequestConferenceMemberVideo)
	 *******************************************************************************/
    EC_CALL_API int RequestConferenceMemberVideo(const char *conferenceId, const char *conferencePasswd, const char *member, void *videoWindow,int type);

    /*! @function
     ********************************************************************************
     函数名   : CancelMemberVideo
     功能     : 视频会议中停止某一远端视频
     参数     : [IN]  conferenceId	   :  所在会议号.
                [IN]  conferencePasswd : 所在会议密码.可选
                [IN]  member           :  请求远端成员.
				[IN]  type             :  详见ECVideoDataType.默认摄像头像源
     返回值   : 成功 0 失败 -1(remoteSipNo为NULL) -3(conferenceId为NULL) -4(confPasswd为NULL) -5(自己的sip号为NULL) -6(会议服务器的ip为NULL) -7(该账户的视频没有成功请求)
	 回调函数 ：	void (*onCancelConferenceMemberVideo)
	 *******************************************************************************/
	 EC_CALL_API int CancelMemberVideo(const char *conferenceId, const char *conferencePasswd, const char *member,int type);
    
    /*! @function
     ********************************************************************************
     函数名   : ResetVideoConfWindow
     功能     : 视频会议中重置窗体
     参数     : [IN]  conferenceId:  所在会议号.
				[IN]  member      :  需要调整的成员.
                [IN]  newWindow	  :  新窗口.
				[IN]  type        :  详见ECVideoDataType.默认摄像头像源
    返回值   : 成功 0 失败 -1(不支持视频) -2(sip号为NULL) -3(newWindow为NULL) -4(找不到该sip号相关的资料)
     *******************************************************************************/
     EC_CALL_API int ResetVideoConfWindow(const char *conferenceId, const char *member, void *newWindow,int type);

	 /*! @function
	 ********************************************************************************
		函数名: SetMultiMediaMeetingBeforeCreate
		功能  : 设置创建会议参数
		参数  : 
			[IN]  callbackmode : 0不通知
								1容信产品现在使用{
									1、<CreateConf> 指定action： sdk_createconfret
									2、<CreateConf> 指定quiturl: sdk_quitconf
									3、<CreateConf> 指定mediaopturl: sdk_mediaopturl
									4、<CreateConf> 指定joinurl: sdk_joinconf
								}
			[IN] serverUserData :自定义数据
			[IN] reservation :此字段为预留字段,主要功能包括是否自动录像等
		返回值   : 是否成功 0：成功； 非0失败
	 *******************************************************************************/
	 EC_CALL_API int SetMultiMediaMeetingBeforeCreate(int callbackmode, const char* serverUserData, const char* reservation);

     /*! @function
     ********************************************************************************
     函数名: CreateMultimediaMeeting
     功能  : 创建会议
     参数  : 
	        [OUT] matchKey    : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
			[IN]  meetingType : 视频会议或音频会议：0是音频；1是视频
            [IN]  meetingName : 会议名称
            [IN]  password    : 会议密码
            [IN]  keywords    : 会议关键字
            [IN]  voiceMode   : 会议背景音模式 1：无提示音有背景音；2：有提示音有背景音；3：无提示音无背景音；
            [IN]  square      : 会议方数
			[IN]  bAutoJoin   : 创建者创建会议成功后，是否自动加入
            [IN]  autoClose   : 创建者退出后，是否自动关闭
            [IN]  autoDelete :  所有人退出后，是否自动解散
     返回值   : 是否成功 0：成功； 非0失败
	 回调函数：onCreateMultimediaMeeting
    *******************************************************************************/
    EC_CALL_API int CreateMultimediaMeeting(unsigned int *matchKey,int  meetingType, const char* meetingName, const char* password, 
		const char* keywords, int voiceMode, int square,bool bAutoJoin,bool autoClose, bool autoDelete);

	/*! @function
     ********************************************************************************
     函数名: JoinMeeting
     功能  : 加入会议
     参数  :[IN]  meetingType : 视频会议或音频会议：ECMeetingType
            [IN]  conferenceId: 会议id
            [IN]  password    : 会议密码
     返回值   : 是否成功 0：成功； 非0失败
	 回调函数：onJoinMeeting
    *******************************************************************************/
    EC_CALL_API int JoinMeeting(int  meetingType, const char* conferenceId, const char* password);

	/*! @function
    ********************************************************************************
     函数名: AcceptMeeting
     功能  : 应答会议
     参数  :[IN]  pCom :会议详情
	 返回值   : 是否成功 0：成功； 非0失败

    *******************************************************************************/
    EC_CALL_API int AcceptMeeting(ECMeetingComingInfo *pCom);

    /*! @function
    ********************************************************************************
     函数名: ExitMeeting
     功能  : 退出会议
     参数  :[IN]  conferenceId :会议id
     返回值   : 是否成功 0：成功； 非0失败
	 回调函数：onExitMeeting
    *******************************************************************************/
    EC_CALL_API int ExitMeeting(const char* conferenceId);

    /*! @function
    ********************************************************************************
     函数名: QueryMeetingMembers
     功能  : 获取会议成员列表
     参数  : [OUT] matchKey    : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
			 [IN]  meetingType : 会议类型
             [IN]  conferenceId: 会议号
     返回值   : 是否成功 0：成功； 非0失败
	 回调函数：onQueryMeetingMembers
    *******************************************************************************/
    EC_CALL_API int QueryMeetingMembers(unsigned int *matchKey, int meetingType, const char* conferenceId);
    
    /*! @function
    ********************************************************************************
     函数名: DismissMultiMediaMeeting
     功能  : 解散会议
     参数  : [OUT] matchKey    : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
			 [IN]  meetingType : 音频会议或视频会议
             [IN]  conferenceId: 会议id
     返回值   : 是否成功 0：成功； 非0失败
	 回调函数：onDismissMultiMediaMeeting
    *******************************************************************************/
    EC_CALL_API int DismissMultiMediaMeeting(unsigned int *matchKey, int meetingType, const char* conferenceId);
    
    /*! @function
    ********************************************************************************
     函数名: QueryMultiMediaMeetings
     功能  : 获取会议列表
     参数  : [OUT] matchKey    : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
			 [IN]  meetingType : 音频会议或视频会议
			 [IN]  keywords    : 会议关键字，搜索与关键字相关的会议列表
     返回值   :是否成功 0：成功； 非0失败
 	 回调函数：onQueryMultiMediaMeetings
   *******************************************************************************/
    EC_CALL_API int QueryMultiMediaMeetings(unsigned int *matchKey,int meetingType, const char* keywords);
    
	 /*! @function
	 ********************************************************************************
		函数名: setMultimediaMeetingBeforeInviteJoin
		功能  : 设置邀请加入会议参数
		参数  : [IN] serverUserData :服务端自定义数据
		返回值   : 是否成功 0：成功； 非0失败
	 *******************************************************************************/
	 EC_CALL_API int setMultimediaMeetingBeforeInviteJoin(const char* serverUserData);

    /*! @function
    ********************************************************************************
     函数名: InviteJoinMultiMediaMeeting
     功能  : 邀请加入会议
     参数  :[OUT] matchKey      : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
			[IN]  conferenceId  : 会议id	
            [IN]  members       : 成员数组
            [IN]  membercount   : 成员数
			[IN]  isLandingCall : 是否是落地邀请
            [IN]  isSpeak       : 是否可讲。false禁言，true可讲 默认true
            [IN]  isListen      : 是否可听。false禁听，true可听 默认true
			[IN]  disNumber     : 显示号码
            [IN]  userdata      : 自定义数据
     返回值   : 是否成功 0：成功； 非0失败
	 回调函数：onInviteJoinVoiceMultiMediaMeeting
    *******************************************************************************/
    EC_CALL_API int InviteJoinMultiMediaMeeting(unsigned int *matchKey,const char* conferenceId, const char** members, int membercount, bool blanding,
		bool isSpeak, bool isListen,const char* disNumber,const char* userdata);
    
    /*! @function
     ********************************************************************************
     函数名: DeleteMemberMultiMediaMeeting
     功能  : 踢出成员
     参数  :[OUT] matchKey     : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
			[IN]  meetingType  : 是否是落地邀请
            [IN]  conferenceId : 会议id
            [IN]  member       : 成员
            [IN]  isVoIP       : 是否VoIP账号
     返回值   : 是否成功 0：成功； 非0失败
	 回调函数：onDeleteMemberMultiMediaMeeting
     *******************************************************************************/
    EC_CALL_API int DeleteMemberMultiMediaMeeting(unsigned int *matchKey, int meetingType, const char* conferenceId, const char* member, bool isVoIP);
      
	/*! @function
     ********************************************************************************
     函数名: PublishVideo
     功能  : 发布视频
     参数  : [OUT] matchKey     : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
			 [IN]  conferenceId : 会议ID
     返回值   : 是否成功 0：成功； 非0失败
	 回调函数：onPublishVideo
     *******************************************************************************/
    EC_CALL_API int PublishVideo(unsigned int *matchKey, const char* conferenceId);
    
    /*! @function
     ********************************************************************************
     函数名: UnpublishVideo
     功能  : 取消发布视频
     参数  : [OUT] matchKey     : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
			 [IN]  conferenceId : 会议ID
     返回值   : 是否成功 0：成功； 非0失败
	 回调函数：onUnpublishVideo
    *******************************************************************************/
    EC_CALL_API int UnpublishVideo(unsigned int *matchKey, const char* conferenceId);

	//实时对讲部分
    /*! @function
    ********************************************************************************
     函数名: CreateInterphoneMeeting
     功能  : 创建实时对讲
     参数  :[OUT] matchKey    : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
			[IN]  members     : 成员数组
            [IN]  membercount : 成员数
     返回值   : 是否成功 0：成功； 非0失败
	 回调函数：onCreateInterphoneMeeting
    *******************************************************************************/
    EC_CALL_API int CreateInterphoneMeeting(unsigned int *matchKey, const char** members, int membercount);

	 /*! @function
    ********************************************************************************
     函数名: CreateInterphoneMeetingBackground
     功能  : 创建实时对讲
     参数  :[OUT] matchKey    : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
			[IN]  members     : 成员数组
            [IN]  membercount : 成员数
			[IN]  voiceMode   : 1、没有提示音有背景音 2、全部提示音 3、无提示音无背景音 缺省2
     返回值   : 是否成功 0：成功； 非0失败
	 回调函数：onCreateInterphoneMeeting
    *******************************************************************************/
    EC_CALL_API int CreateInterphoneMeetingBackground(unsigned int *matchKey, const char** members, int membercount, int voiceMode);
    
    /*! @function
    ********************************************************************************
     函数名: ControlInterphoneMic
     功能  : 控麦
     参数  : [OUT] matchKey : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
			[IN]  isControl : 是否控麦
            [IN]  meetingId : 会议ID
     返回值   : 是否成功 0：成功； 非0失败
	 回调函数：onControlInterphoneMic
    *******************************************************************************/
    EC_CALL_API int ControlInterphoneMic(unsigned int *matchKey, bool isControl, const char* interphoneId);

	 /*! @function
     ********************************************************************************
     函数名   : SetConfMemberState
     功能     : 设置会议成员权限状态
     参数     : [IN]  matchKey    : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
                [IN]  meetingType : 会议类型
                [IN]  meetingId   : 会议id
                [IN]  authority   : 权限设置 1.无操作权限 2.有操作权限
                [IN]  member      : 设置的成员账号
                [IN]  isVoIP      : 是否VoIP账号
                [IN]  state       : 用户状态 （业务自行定义）
                [IN]  userdata    : 扩展数据 暂时不用
     返回值   : 0成功，非0失败
     回调函数 ：void (*onSetMeetingMemberState)
     *******************************************************************************/
     EC_CALL_API int SetMeetingMemberState(unsigned int* matchKey,int meetingType, const char* meetingId, int authority, const char* member, bool isVoIP, const char* state, const char* userdata);
    
    /*! @function
     ********************************************************************************
     函数名   : SetSpeakListen
     功能     : 设置会议成员听讲状态
     参数     : [IN]  matchKey    : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
                [IN]  meetingType : 会议类型
                [IN]  meetingId : 会议id
                [IN]  member : 成员账号
                [IN]  isVoIP : 是否VoIP账号
                [IN]  option : 1、禁言 2、可讲 3、禁听 4、可听
     返回值   : 0成功，非0失败
     回调函数 ：void (*onSetMeetingSpeakListen)
     *******************************************************************************/
     EC_CALL_API int SetMeetingSpeakListen(unsigned int* matchKey, int meetingType, const char* meetingId, const char* member, bool isVoIP, int option);
    
    /*! @function
     ********************************************************************************
     函数名   : PublishShareData
     功能     : 发布会议共享数据
     参数     : [IN]  matchKey    : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
                [IN]  meetingType : 会议类型
                [IN]  meetingId : 会议id
     返回值   : 0成功，非0失败
     回调函数 ：void (*onPublishShareData)
     *******************************************************************************/
     EC_CALL_API int PublishShareData(unsigned int* matchKey, int meetingType, const char* meetingId);
    
    /*! @function
     ********************************************************************************
     函数名   : UnpublishShareData
     功能     : 取消发布会议共享数据
     参数     : [IN]  matchKey    : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
                [IN]  meetingType : 会议类型
                [IN]  meetingId : 会议id
     返回值   : 0成功，非0失败
     回调函数 ：void (*UnpublishShareData)
     *******************************************************************************/
     EC_CALL_API int UnpublishShareData(unsigned int* matchKey, int meetingType, const char* meetingId);


    
#ifdef __cplusplus
}
#endif

#endif