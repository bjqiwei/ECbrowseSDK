#ifndef _EC_MEETING_STRUCT_DEFINE_H
#define _EC_MEETING_STRUCT_DEFINE_H

#include "ECStructDef.h"

typedef enum
{
	MeetingType_Unknown		= -1,//未知
	MeetingType_Voice			= 1,//音频会议
	MeetingType_Video			= 2,//视频会议
	MeetingType_Interphone		= 3,//实时对讲
	MeetingType_Screen			= 4,//数据会议
	MeetingType_Screen_Voice	= 5,//音频+数据会议
	MeetingType_Screen_Vedio	= 6,//视频+数据会议
	MeetingType_End

}ECMeetingType;

//权限类型
typedef enum
{
	AuthorityType_None		 = 1,//无权限
	AuthorityType_Permission = 2,//有权限

	AuthorityType_End

}ECAuthorityType;

//权限状态类型
typedef enum
{
	AuthStateType_None		= 0,//无权限（普通参会者）
	AuthStateType_Chairman	= 1,//主持人（具有会议管理权限）
	AuthStateType_Chief		= 2,//主讲人（当前显示其视频或数据屏幕或语音）

	AuthStateType_End

}ECAuthStateType;

//会议角色
typedef enum
{
	Role_Unknown	= 0,
	Role_Actor		= 1,//参与者
	Role_Creator	= 2,//创建者
	Role_End
}ECRole;

typedef enum
{
	VideoState_Publish   = 1,//已发布
	VideoState_UnPublish = 2 //未发布

}ECVideoState;
typedef enum
{
	VideoDataType_Camera   = 1,//摄像头源
	VideoDataType_Data = 2 //数据源

}ECVideoDataType;
//实时对讲通知消息类型
typedef enum
{
	/** 邀请加入实时对讲 */
	InterphoneMsgType_Invite = 201,

	/** 加入实时对讲 */
	InterphoneMsgType_Join = 202,

	/** 退出实时对讲 */
	InterphoneMsgType_Exit = 203,

	/** 结束实时对讲 */
	InterphoneMsgType_Over = 204,

	/** 控麦 */
	InterphoneMsgType_ControlMic = 205,

	/** 放麦 */
	InterphoneMsgType_ReleaseMic = 206

}ECInterphoneMsgType;

//语音会议通知消息类型
typedef enum
{
	/** 加入聊天室 */
	VoiceMeetingMsgType_Join = 301,

	/** 退出聊天室 */
	VoiceMeetingMsgType_Exit = 302,

	/** 删除聊天室 */
	VoiceMeetingMsgType_Delete = 303,

	/** 删除聊天室成员 */
	VoiceMeetingMsgType_RemoveMember = 304,

	/** 设置聊天室成员可听可讲 */
	VoiceMeetingMsgType_Forbid = 305,

	/** 拒绝 */
	VoiceMeetingMsgType_Refuse = 306

}ECVoiceMeetingMsgType;

//视频会议通知消息类型
typedef enum
{
	/** 加入聊天室 */
	VideoMeetingMsgType_Join = 601,

	/** 退出聊天室 */
	VideoMeetingMsgType_Exit = 602,

	/** 删除聊天室 */
	VideoMeetingMsgType_Delete = 603,

	/** 踢出聊天室成员 */
	VideoMeetingMsgType_RemoveMember = 604,

	/** 设置聊天室成员可听可讲 */
	VideoMeetingMsgType_Forbid = 606,

	/** 发布视频 */
	VideoMeetingMsgType_Publish = 607,

	/** 取消发布视频 */
	VideoMeetingMsgType_Unpublish = 608,

	/** 拒绝 */
	VideoMeetingMsgType_Refuse = 609

}ECVideoMeetingMsgType;

typedef enum {//会议申请视频资源回调原因值
    EC_Video_Conference_status_RequestTimeout = -1,//请求超时
    EC_Video_Conference_status_OK = 0,//请求成功，上报上层，这里自动设置视频窗口
    EC_Video_Conference_status_NotExist,//会议不存在
    EC_Video_Conference_status_UserExclusive,//自己不在会议中
    EC_Video_Conference_status_RequestedUserExclusive,//请求目标不在会议中
    EC_Video_Conference_status_RequestedUserNoVideo//请求目标没有视频
} ECVideo_Conference_status;

#pragma pack (1)
typedef struct _ECMeetingInviteInfo
{
	char Id[BUF_LEN_128];//会议ID
	char **members;//成员数组
	char *userdata;//自定义数据
	int  memberCount; //members number
	char isSpeak;//0不可讲 1可讲
	char isListen;//0不可听 1可听

	_ECMeetingInviteInfo()
	{
		memset(this,0,sizeof(_ECMeetingInviteInfo));
	}
}ECMeetingInviteInfo;

typedef struct _ECMeetingDeleteMemberInfo
{
	char Id[BUF_LEN_128];//会议ID
	char member[BUF_LEN_64];//成员
	int  meetingType;//会议类型。参考ECMeetingType
	char isVoIP;//是否VoIP账号。0否，1是

	_ECMeetingDeleteMemberInfo()
	{
		memset(this,0,sizeof(_ECMeetingDeleteMemberInfo));
	}
}ECMeetingDeleteMemberInfo;

typedef struct _MeetingComingInfo{
	int  meetingType;//会议类型。参考ECMeetingType
	char Id[BUF_LEN_128];//会议ID
	char callid[BUF_LEN_64];//通话ID
	char caller[BUF_LEN_64]; //主叫
	char nickname[BUF_LEN_128];//昵称
	char display[BUF_LEN_20];//显号
	char sdkUserData[BUF_LEN_4096];//扩展
	_MeetingComingInfo()
	{
		memset(this,0,sizeof(_MeetingComingInfo));
	}
}ECMeetingComingInfo;
typedef struct _ECInterphoneMsg
{
	int var;//消息类型
	int type;//权限1：无权限，2：有权限 ECInterphoneMsgType
	char receiver[BUF_LEN_64];//接收者
	char interphoneid[BUF_LEN_64];//实时对讲接入Id
	char from[BUF_LEN_64];//实时对讲发送端voip号码
	char datecreated[BUF_LEN_20];//实时对讲的创建时间
	char state[BUF_LEN_64];//状态
	char userdata[BUF_LEN_1024];//扩展字符串
	char member[BUF_LEN_64];//成员变量
	_ECInterphoneMsg()
	{
		memset(this,0,sizeof(_ECInterphoneMsg));
	}

	_ECInterphoneMsg(const _ECInterphoneMsg& rOther)
	{
		*this = rOther;
	}

	//_ECInterphoneMsg& operator=(const _ECInterphoneMsg& rOther);

}ECInterphoneMsg;


typedef struct _ECVoiceMeetingMsg
{
	int var;//消息类型
	int type;//权限1：无权限，2：有权限 ECMeetingType
	char sender[BUF_LEN_64];//发送者
	char receiver[BUF_LEN_64];//接收者
	char chatroomid[BUF_LEN_64];//语音群聊房间Id
	char forbid[BUF_LEN_8];//群聊forbid禁言禁听状态，11：可听可讲 10：可听禁言 01：禁听可讲 00：禁听禁言
	char state[BUF_LEN_64];//状态
	char userdata[BUF_LEN_1024];//扩展字符串
	char member[BUF_LEN_64];//成员变量
	char isVoIP;//是否VoIP账号。0否，1是
	_ECVoiceMeetingMsg()
	{
		memset(this,0,sizeof(_ECVoiceMeetingMsg));
	}

	_ECVoiceMeetingMsg(const _ECVoiceMeetingMsg& rOther)
	{
		*this = rOther;
	}

	//_ECVoiceMeetingMsg& operator=(const _ECVoiceMeetingMsg& rOther);

}ECVoiceMeetingMsg;

typedef struct _ECVideoMeetingMsg
{
	int var;//消息类型
	int type;//权限1：无权限，2：有权限
	int videoState;// 1、已发布 2、未发布 
	int dataState;// 1、已发布 2、未发布
	char sender[BUF_LEN_64];//发送者
	char receiver[BUF_LEN_64];//接收者
	char roomid[BUF_LEN_64];//视频会议接入Id
	char forbid[BUF_LEN_8];//群聊forbid禁言禁听状态，11：可听可讲 10：可听禁言 01：禁听可讲 00：禁听禁言
	char state[BUF_LEN_64];//状态
	char userdata[BUF_LEN_1024];//扩展字符串
	char member[BUF_LEN_64];//成员变量
	char isVoIP;//是否VoIP账号。0否，1是
	_ECVideoMeetingMsg()
	{
		memset(this,0,sizeof(_ECVideoMeetingMsg));
	}

	_ECVideoMeetingMsg(const _ECVideoMeetingMsg& rOther)
	{
		*this = rOther;
	}

	//_ECVideoMeetingMsg& operator=(const _ECVideoMeetingMsg& rOther);

}ECVideoMeetingMsg;

//会议成员信息
typedef struct _ECMeetingMemberInfo
{
	int meetingType;		//会议类型。参考ECMeetingType
	int type; // 1、参与者  2、创建者
	int videoState;// 1、已发布 2、未发布  视频会议有效
	int dataState;// 1、已发布 2、未发布
	char forbid[BUF_LEN_8];//11,10,01,00分别对应可听可讲
	char member[BUF_LEN_64];//成员id
	char state[BUF_LEN_64];//状态
	char userdata[BUF_LEN_1024];//自定义数据
	char isVoIP;//是否VoIP账号。0否，1是
	char isOnline; //实时对讲中有效 1未加入实时对讲中 2已加入实时对讲中
	char isMic; //实时对讲中有效 1未控麦 2已控麦
	_ECMeetingMemberInfo()
	{
		memset(this,0,sizeof(_ECMeetingMemberInfo));
	}
}ECMeetingMemberInfo;

//会议信息
typedef struct _ECMeetingInfo 
{
	int  meetingType;//会议类型。参考ECMeetingType
	int  confirm; // 加入房间是否需要验证 1、不需要 2、需要
	int  square; //最大可参会人数
	int  joinedCount; // 参与人数
	char Id[BUF_LEN_128]; // 会议ID
	char name[BUF_LEN_128];     // 房间名称
	char creator[BUF_LEN_64];   // 创建者
	char keywords[BUF_LEN_128]; // 业务属性，可选字段

	_ECMeetingInfo()
	{
		memset(this,0,sizeof(_ECMeetingInfo));
	}

}ECMeetingInfo;

typedef struct _ECMeetingMemberSpeakListenInfo
{
	int   meetingType;//会议类型
	char  Id[BUF_LEN_128];//会议id
	char  member[BUF_LEN_64];//成员
	int   speaklisen ;   //1、禁言 2、可讲 3、禁听 4、可听
	bool  isVoIP;//是否voip
}ECMeetingMemberSpeakListenInfo;

typedef struct _ECMeetingMemberStateInfo
{
	int   meetingType;//会议类型
	char  Id[BUF_LEN_128];//会议id
	char  member[BUF_LEN_64];//成员
	int   authority;//成员
	bool  isVoIP;//是否voip
	char *state;//用户状态
}ECMeetingMemberStateInfo;


/********************************************会议回调接口***********************************************/
typedef void (*onMeetingInviteIncoming)(ECMeetingComingInfo *pCome);//会议接到邀请。
typedef void (*onReceiveInterphoneMeetingMessage)(ECInterphoneMsg* pInterphoneMsg);// 收到实时对讲里面的通知消息。
typedef void (*onReceiveVoiceMeetingMessage)(ECVoiceMeetingMsg* pVoiceMeetingMsg);// 收到语音会议里面的通知消息。
typedef void (*onReceiveVideoMeetingMessage)(ECVideoMeetingMsg* pVideoMeetingMsg);// 收到视频会议里面的通知消息。
typedef void (*onCreateMultimediaMeeting)(unsigned int matchKey,int reason, ECMeetingInfo *pInfo);//创建音频、视频会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
typedef void (*onJoinMeeting)(int reason, const char* conferenceId);//主动加入会议的回调。//reason：200成功，其他报错；conferenceId 会议ID；
typedef void (*onExitMeeting)(const char* conferenceId);//退出会议。conferenceId 会议ID；
typedef void (*onQueryMeetingMembers)(unsigned int matchKey, int reason,const char* conferenceId,int count, ECMeetingMemberInfo* pMembers);//查询加入回忆的成员。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；count 成员数组个数，pMembers 成员数组
typedef void (*onDismissMultiMediaMeeting)(unsigned int matchKey, int reason,const char*conferenceId);//解散实时对讲、音频、视频会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；
typedef void (*onQueryMultiMediaMeetings)(unsigned int matchKey, int reason,int count,ECMeetingInfo* pMeetingInfo);//查询会议信息的回调。matchKey 匹配主调键值；reason：200成功，其他报错；count 会议数组个数，pMembers 会议数组
typedef void (*onDeleteMemberMultiMediaMeeting)(unsigned int matchKey, int reason,ECMeetingDeleteMemberInfo *pInfo);//删除会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
typedef void (*onInviteJoinMultiMediaMeeting)(unsigned int matchKey, int reason,ECMeetingInviteInfo *pInfo);//邀请加入会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
typedef void (*onCreateInterphoneMeeting)(unsigned int matchKey, int reason, const char* interphoneId);//创建实时对讲会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；interphoneId 实时对讲ID
typedef void (*onExitInterphoneMeeting)(const char* interphoneId);//本回调由onExitMeeting由代替。interphoneId 实时对讲ID
typedef void (*onControlInterphoneMic)(unsigned int matchKey, int reason, const char* controller,const char* interphoneId,bool requestIsControl);//实时对讲抢麦放麦的回调。matchKey 匹配主调键值；reason：200成功，其他报错；controller 控麦成员；interphoneId 实时对讲ID；requestIsControl 是否发起控麦
typedef void (*onPublishVideo)(unsigned int matchKey, int reason,const char* conferenceId);//发布视频的回调。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；
typedef void (*onUnpublishVideo)(unsigned int matchKey, int reason,const char* conferenceId);//取消视频的回调。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；
typedef void (*onRequestConferenceMemberVideo)(int reason,const char *conferenceId, const char *member,int type);//视频会议时，请求会议成员视频数据。reason 详见Video_Conference_status；conferenceId 会议ID；member成员；type详见ECVideoDataType；
typedef void (*onCancelConferenceMemberVideo)(int reason,const char *conferenceId, const char *member,int type);//视频会议时，取消会议成员视频数据响应。reason 详见Video_Conference_status；conferenceId 会议ID；tmember成员；ype详见ECVideoDataType；
typedef void (*onSetMeetingSpeakListen)(unsigned int matchKey, int reason,ECMeetingMemberSpeakListenInfo* pInfo);//设置会议成员听讲状态的回调。matchKey 匹配主调键值；reason：200成功，其他报错；

typedef void (*onSetMeetingMemberState)(unsigned int matchKey, int reason,ECMeetingMemberStateInfo* pInfo);//设置会议成员权限状态的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
typedef void (*onPublishShareData)(unsigned int matchKey, int reason,const char* conferenceId);//发布共享数据的回调。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；
typedef void (*onUnpublishShareData)(unsigned int matchKey, int reason,const char* conferenceId);//取消共享数据的回调。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；


//回调结构体
typedef struct _MeetingCallBackInterface
{
	onCreateInterphoneMeeting pfonCreateInterphoneMeeting;
	onControlInterphoneMic pfonControlInterphoneMic;
	onReceiveInterphoneMeetingMessage pfonReceiveInterphoneMeetingMessage; 
	onMeetingInviteIncoming pfonMeetingIncoming;
	onReceiveVoiceMeetingMessage pfonReceiveVoiceMeetingMessage; 
	onReceiveVideoMeetingMessage pfonReceiveVideoMeetingMessage; 
	onCreateMultimediaMeeting pfonCreateMultimediaMeeting;
	onJoinMeeting	pfonJoinMeeting;
	onQueryMeetingMembers pfonQueryMeetingMembers;
	onDismissMultiMediaMeeting pfonDismissMultiMediaMeeting;
	onQueryMultiMediaMeetings pfonQueryMultiMediaMeetings;
	onDeleteMemberMultiMediaMeeting pfonDeleteMemberMultiMediaMeeting;
	onInviteJoinMultiMediaMeeting pfonInviteJoinMultiMediaMeeting;
	onPublishVideo pfonPublishVideo;
	onUnpublishVideo pfonUnpublishVideo;
	onRequestConferenceMemberVideo pfonRequestConferenceMemberVideo;
	onCancelConferenceMemberVideo pfonCancelConferenceMemberVideo;
	onExitMeeting pfonExitMeeting;
	onExitInterphoneMeeting pfonExitInterphoneMeeting;
	onSetMeetingSpeakListen pfonSetMeetingSpeakListen;

	onSetMeetingMemberState pfonSetMeetingMemberState;
	onPublishShareData pfonPublishShareData;
	onUnpublishShareData pfonUnpublishShareData;

	_MeetingCallBackInterface():pfonMeetingIncoming(NULL),pfonReceiveInterphoneMeetingMessage(NULL),pfonReceiveVoiceMeetingMessage(NULL),
		pfonReceiveVideoMeetingMessage(NULL),pfonCreateMultimediaMeeting(NULL),pfonJoinMeeting(NULL),pfonQueryMeetingMembers(NULL),
		pfonDismissMultiMediaMeeting(NULL),pfonQueryMultiMediaMeetings(NULL),pfonDeleteMemberMultiMediaMeeting(NULL),
		pfonInviteJoinMultiMediaMeeting(NULL),pfonCreateInterphoneMeeting(NULL),pfonControlInterphoneMic(NULL),pfonPublishVideo(NULL),
		pfonUnpublishVideo(NULL),pfonRequestConferenceMemberVideo(NULL),pfonCancelConferenceMemberVideo(NULL),pfonExitMeeting(NULL),
		pfonExitInterphoneMeeting(NULL),pfonSetMeetingSpeakListen(NULL)

		,pfonSetMeetingMemberState(NULL),pfonPublishShareData(NULL),pfonUnpublishShareData(NULL)

	{

	}

}ECMeetingCallBackInterface;

#pragma pack ()
#endif