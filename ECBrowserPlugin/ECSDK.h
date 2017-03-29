#pragma once
#include <ECStructDef.h>
#include <ECVoipStructDef.h>
#define ALL_EXPOSE
#include <ECImStructDef.h>
#include <ECMeetingStructDef.h>
#include <log4cplus/logger.h>
#include <vector>

typedef struct 
{
	unsigned int matchKey;
	int reason;
	unsigned long long version;
}EConSetPersonInfo;

typedef struct
{
	unsigned int matchKey;
	int reason;
	ECPersonInfo PersonInfo;
}EConGetPersonInfo;

typedef struct
{
	unsigned int matchKey;
	int reason;
	std::vector<ECPersonState> States;
}EConGetUserState;

typedef struct
{
	std::string softVersion;
	int updateMode;
	std::string updateDesc;
}EConSoftVersion;

typedef struct
{
	unsigned int matchKey;
	int reason;
	std::vector<ECMultiDeviceState> MultiDeviceStates;
}EConGetOnlineMultiDevice;

typedef struct 
{
	unsigned int matchKey;
	int reason;
	ECCallBackInfo CallBackInfo;
}EConMakeCallBack;

typedef struct
{
	std::string CallidOrConferenceId;
	int width;
	int height;
	int type;
	std::string member;
}EConRemoteVideoRatio;

typedef struct
{
	unsigned int matchKey;
	int reason;
	ECMessage msg;

}EConDownloadFileComplete;

typedef struct
{
	unsigned int matchKey;
	unsigned long long rateSize;
	unsigned long long fileSize;
	ECMessage msg;
}EConRateOfProgressAttach;

typedef struct
{
	unsigned int matchKey;
	int reason;
	ECMessage msg;

}EConSendMediaFile;

typedef struct 
{
	unsigned int matchKey;
	int reason;
	int type;
	std::string msgId;
}EConDeleteMessage;

typedef struct
{
	unsigned int tcpMsgIdOut;
	int reason;
}EConOperateMessage;

typedef struct{
	ECOperateMessageNoticeCode notice;
	ECDeleteMessageNtyMsg DeleteMsg;
	ECReadMessageNtyMsg ReadMsg;
	ECWithDrawMessageNtyMsg WithDrawMessage;

}EConReceiveOpreateNoticeMessage;

typedef struct
{
	unsigned int matchKey;
	int reason;
	std::string fileUrl;
}EConUploadVTMFileOrBuf;

typedef struct
{
	unsigned int matchKey;
	int reason;
	ECGroupDetailInfo info;

}EConCreateGroup;

typedef struct
{
	unsigned int matchKey;
	int reason;
	std::string groupid;

}EConDismissGroup;


typedef struct
{
	unsigned int matchKey;
	int reason;
	std::string groupid;

}EConQuitGroup;

typedef struct
{
	unsigned int matchKey;
	int reason;
	std::string groupid;

}EConJoinGroup;


typedef struct
{
	unsigned int matchKey;
	int reason;
	std::string groupid;
	std::string member;
	int confirm;

}EConReplyRequestJoinGroup;

typedef struct
{
	unsigned int matchKey;
	int reason;
	std::string groupid;
	std::vector<std::string> members;
	int confirm;

}EConInviteJoinGroup;


typedef struct
{
	unsigned int matchKey;
	int reason;
	std::string groupid;
	std::string member;
	int confirm;

}EConReplyInviteJoinGroup;

typedef struct
{
	unsigned int matchKey;
	int reason;
	std::vector<ECGroupSimpleInfo> group;

}EConQueryOwnGroup;


typedef struct
{
	unsigned int matchKey;
	int reason;
	ECGroupDetailInfo detail;

}EConQueryGroupDetail;


typedef struct
{
	unsigned int matchKey;
	int reason;
	ECModifyGroupInfo info;

}EConModifyGroup;


typedef struct
{
	unsigned int matchKey;
	int reason;
	int searchType;
	std::string keyword;
	std::vector<ECGroupSearchInfo> group;
	int pageNo;

}EConSearchPublicGroup;


typedef struct
{
	unsigned int matchKey;
	int reason;
	std::string groupid;
	std::vector<ECGroupMember> members;
}EConQueryGroupMember;


typedef struct
{
	unsigned int matchKey;
	int reason;
	std::string groupid;
	std::string member;
}EConDeleteGroupMember;


typedef struct
{
	unsigned int matchKey;
	int reason;
	ECGroupMemberCard card;
}EConQueryGroupMemberCard;


typedef struct
{
	unsigned int matchKey;
	int reason;
	ECGroupMemberCard card;
}EConModifyGroupMemberCard;


typedef struct
{
	unsigned int matchKey;
	int reason;
	std::string groupid;
	std::string member;
	int isBan;
}EConForbidMemberSpeakGroup;


typedef struct
{
	unsigned int matchKey;
	int reason;
	std::string groupid;
	bool notice;
}EConSetGroupMessageRule;

typedef struct
{
	ECMessageNoticeCode notice;
	ECRequestJoinGroupNtyMsg requestMsg;
	ECInviteJoinGroupNtyMsg inviteMsg;
	ECMemberJoinedGroupNtyMsg memberMsg;
	ECDismissGroupNtyMsg dismissMsg;
	ECQuitGroupNtyMsg quitMsg;
	ECRemoveGroupMemberNtyMsg removeMsg;
	ECReplyRequestJoinGroupNtyMsg replyrequestMsg;
	ECReplyInviteJoinGroupNtyMsg replyinviteMsg;
	ECModifyGroupNtyMsg modifyMsg;

}EConReceiveGroupNoticeMessage;

typedef struct
{
	int ms;
	std::string sessionId;

}EConRecordingTimeOut;


typedef struct
{
	std::string sessionId;
	int reason;

}EConFinishedPlaying;

typedef struct
{
	double amplitude;
	std::string sessionId;
}EConRecordingAmplitude;

typedef struct
{
	unsigned int matchKey;
	int reason;
	ECMessage msg;

}EConSendTextMessage;

typedef struct 
{
	unsigned int matchKey;
	int reason;
	ECMeetingInfo info;
}EConCreateMultimediaMeeting;

typedef struct 
{
	int reason;
	std::string conferenceId;
}EConJoinMeeting;

typedef struct 
{
	unsigned int matchKey;
	int reason;
	std::string conferenceId;
	std::vector<ECMeetingMemberInfo> Members;
}EConQueryMeetingMembers;


typedef struct 
{
	unsigned int matchKey;
	int reason;
	std::string conferenceId;
}EConDismissMultiMediaMeeting;


typedef struct 
{
	unsigned int matchKey;
	int reason;
	std::vector<ECMeetingInfo> infos;
}EConQueryMultiMediaMeetings;


typedef struct 
{
	unsigned int matchKey;
	int reason;
	ECMeetingDeleteMemberInfo info;
}EConDeleteMemberMultiMediaMeeting;

typedef struct
{
	std::string Id;//会议ID
	std::vector<std::string> members;//成员数组
	std::string userdata;//自定义数据
	char isSpeak;//0不可讲 1可讲
	char isListen;//0不可听 1可听

}EConMeetingInviteInfo;

typedef struct 
{
	unsigned int matchKey;
	int reason;
	EConMeetingInviteInfo info;

}EConInviteJoinMultiMediaMeeting;


typedef struct
{
	unsigned int matchKey;
	int reason;
	std::string interphoneId;
}EConCreateInterphoneMeeting;

typedef struct 
{
	unsigned int matchKey;
	int reason;
	ECMeetingMemberSpeakListenInfo Info;
}EConSetMeetingSpeakListen;

typedef struct
{
	unsigned int matchKey;
	int reason;
	std::string controller;
	std::string interphoneId;
	bool requestIsControl;
}EConControlInterphoneMic;


typedef struct
{
	unsigned int matchKey;
	int reason;
	std::string conferenceId;
}EConPublishVideo;


typedef struct
{
	unsigned int matchKey;
	int reason;
	std::string conferenceId;
}EConUnpublishVideo;



typedef struct
{
	int reason;
	std::string conferenceId;
	std::string member;
	int type;
}EConRequestConferenceMemberVideo;



typedef struct
{
	int reason;
	std::string conferenceId;
	std::string member;
	int type;
}EConCancelConferenceMemberVideo;


class ECSDK
{
public:
	ECSDK();
	~ECSDK();
private:
	log4cplus::Logger log;

private:
	 /****************************登录接口的回调******************************/
	static void sonLogInfo(const char* loginfo); // 用于接收底层的log信息,调试出现的问题.
	static void sonLogOut(int reason);//登出回调。reason：200成功，其他报错；
	static void sonConnectState(const ECConnectState state);//连接回调；
	static void sonSetPersonInfo(unsigned int matchKey, int reason, unsigned long long version);//设置个人资料。reason：200成功，其他报错；version:个人资料版本号
	static void sonGetPersonInfo(unsigned int matchKey, int reason, ECPersonInfo *pPersonInfo);//获取个人资料。reason：200成功，其他报错；
	static void sonGetUserState(unsigned int matchKey, int reason, int count, ECPersonState *pStateArr);//获取用户状态，reason：200成功，其他报错;count 数组个数，pStateArr 用户状态数组首指针
	static void sonPublishPresence(unsigned int matchKey, int reason);//发布用户状态，reason：200成功，其他报错
	static void sonReceiveFriendsPublishPresence(int count, ECPersonState *pStateArr);//收到服务器推送的用户状态变化,count 数组个数，pStateArr 用户状态数组首指针
	static void sonSoftVersion(const char* softVersion, int updateMode, const char* updateDesc);//应用软件最新版本号升级提示，供应用自己升级用。softVersion：软件最新版本号；updateMode:更新模式  1：手动更新 2：强制更新；updateDesc：软件更新说明 
	static void sonGetOnlineMultiDevice(unsigned int matchKey, int reason, int count, ECMultiDeviceState *pMultiDeviceStateArr);//获得登录者多设备登录状态变化回调,reason：200成功，其他报错count 数组个数，pMultiDeviceStateArr 多设备登录状态数组首指针
	static void sonReceiveMultiDeviceState(int count, ECMultiDeviceState *pMultiDeviceStateArr);//收到服务器推送的登录者多设备登录状态变化,count 数组个数，pMultiDeviceStateArr 多设备登录状态数组首指针

	/****************************VOIP回调接口******************************/
	static void sonMakeCallBack(unsigned int matchKey, int reason, ECCallBackInfo *pInfo);//回拨请求。reason：200成功，其他报错；
	static void sonCallEvents(int reason, const ECVoIPCallMsg * call);//事件状态
	static void sonCallIncoming(ECVoIPComingInfo *pComing);  //音视频呼叫接到邀请
	static void sonDtmfReceived(const char *callId, char dtmf);//收到DTMF按键时的回调。callId 通话ID；	
	static void sonSwitchCallMediaTypeRequest(const char*callId, int video); // 收到对方更新媒体请求。 callId 通话ID；video：1  请求增加视频（需要响应） 0:请求删除视频（可以不响应，底层自动去除视频）
	static void sonSwitchCallMediaTypeResponse(const char*callId, int video);  // 对方应答媒体状态结果。callId 通话ID； video 1 有视频 0 无视频
	static void sonRemoteVideoRatio(const char *CallidOrConferenceId, int width, int height, int type, const char *member);//远端视频媒体分辨率变化时上报。type : 0 点对点,1 视频会议，2 共享会议；type=0 CallidOrConferenceId为通话id，type>0 CallidOrConferenceId为会议id；member有效

	/****************************消息处理接口的回调******************************/
	static void sonOfflineMessageCount(int count);//离线条数；
	static int  sonGetOfflineMessage();//离线拉取条数。<0 全拉；0 不拉；大于0，只拉取最新的条数，
	static void sonOfflineMessageComplete();//拉取离线成功
	static void sonReceiveOfflineMessage(ECMessage *pMsg);//离线消息回调
	static void sonReceiveMessage(ECMessage *pMsg);//实时消息回调
	static void sonReceiveFile(ECMessage* pFileInfo);//用户收到发送者发送过来的文件等待用户决定是否下载文件的回调。该回调消息的消息类型(MsgType)默认是6
	static void sonDownloadFileComplete(unsigned int matchKey, int reason, ECMessage *msg);//下载文件回调，下载完成后被调用（下载失败也会被调）。reason:0成功，其他报错；offset 内容大小偏移量
	static void sonRateOfProgressAttach(unsigned int matchKey, unsigned long long rateSize, unsigned long long fileSize, ECMessage *msg);//上传下载文件进度。ratesize 已经完成进度大小（字节）；filezise 总文件大小（字节）
	static void sonSendTextMessage(unsigned int matchKey, int reason, ECMessage *msg);//发送消息回调。reason：200成功，其他报错；
	static void sonSendMediaFile(unsigned int matchKey, int reason, ECMessage *msg);//上传文件回调，上传完成后被调用（上传失败也会被调）。reason:0成功，其他报错；offset 内容大小偏移量
	static void sonDeleteMessage(unsigned int matchKey, int reason, int type, const char* msgId);//删除服务端消息。reason:0成功，其他报错；type:透传消息类型；msgId:
	static void sonOperateMessage(unsigned int tcpMsgIdOut, int reason);//删除消息
	static void sonReceiveOpreateNoticeMessage(const ECOperateNoticeMessage *pMsg);
	static void sonUploadVTMFileOrBuf(unsigned int matchKey, int reason, const char* fileUrl);//上传VTM文件回调, fileIdOut：文件id, fileUrl:文件服务器保存地址

	/****************************群组接口对应的回调******************************/
	static void sonCreateGroup(unsigned int matchKey, int reason, ECGroupDetailInfo *pInfo);//创建群组。matchKey 匹配主调键值；reason：200成功，其他报错；
	static void sonDismissGroup(unsigned int matchKey, int reason, const char* groupid);//解散群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
	static void sonQuitGroup(unsigned int matchKey, int reason, const char* groupid);//退出群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
	static void sonJoinGroup(unsigned int matchKey, int reason, const char* groupid);//加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
	static void sonReplyRequestJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int confirm);//回复请求加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员；confirm 1拒绝 2同意
	static void sonInviteJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char** members, int membercount, int confirm);//邀请加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；members 成员数组；membercount 数组个数 ；confirm 1拒绝 2同意
	static void sonReplyInviteJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int confirm);//回复邀请加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员；confirm 1拒绝 2同意
	static void sonQueryOwnGroup(unsigned int matchKey, int reason, int count, ECGroupSimpleInfo *group);//查询我的群组。matchKey 匹配主调键值；reason：200成功，其他报错；
	static void sonQueryGroupDetail(unsigned int matchKey, int reason, ECGroupDetailInfo *detail);//查询群组详情。matchKey 匹配主调键值；reason：200成功，其他报错；
	static void sonModifyGroup(unsigned int matchKey, int reason, ECModifyGroupInfo *pInfo);//修改群组信息。matchKey 匹配主调键值；reason：200成功，其他报错；
	static void sonSearchPublicGroup(unsigned int matchKey, int reason, int searchType, const char* keyword, int count, ECGroupSearchInfo *group);//搜索公共群组。matchKey 匹配主调键值；reason：200成功，其他报错；searchType 搜索类型 1根据GroupId，精确搜索 2根据GroupName，模糊搜索； keyword 搜索关键词；count 群组个数；
	static void sonQueryGroupMember(unsigned int matchKey, int reason, const char *groupid, int count, ECGroupMember *member);//查询群组成员。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
	static void sonDeleteGroupMember(unsigned int matchKey, int reason, const char* groupid, const char* member);//删除群组成员。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员
	static void sonQueryGroupMemberCard(unsigned int matchKey, int reason, ECGroupMemberCard *card);//查询群组成员名片。matchKey 匹配主调键值；reason：200成功，其他报错；
	static void sonModifyGroupMemberCard(unsigned int matchKey, int reason, ECGroupMemberCard* pCard);//修改群组成员名片。matchKey 匹配主调键值；reason：200成功，其他报错；
	static void sonForbidMemberSpeakGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int isBan);//群组禁言。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员；isBan 1允许发言 2禁止发言
	static void sonSetGroupMessageRule(unsigned int matchKey, int reason, const char* groupid, bool notice);//设置群组消息规则。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；notice 是否通知
    static void sonSetGroupMemberRole(unsigned int matchKey, int reason);//设置群组成员权限
	/****************************服务器下发的群组相关通知******************************/
	static void sonReceiveGroupNoticeMessage(const ECGroupNoticeMessage * pMsg);

	/****************************音频播放回调接口******************************/
	static void sonRecordingTimeOut(int ms, const char* sessionId);//录音超时。ms 为录音时长，单位毫秒,sessionId 透传会话ID
	static void sonFinishedPlaying(const char* sessionId, int reason);//播放结束,sessionId透传会话ID,reason 200成功.其他失败
	static void sonRecordingAmplitude(double amplitude, const char* sessionId);//返回音量的振幅。录音时振幅范围0-1,sessionId 透传会话ID

	/********************************************会议回调接口***********************************************/
	static void sonMeetingIncoming(ECMeetingComingInfo *pCome);//会议接到邀请。
	static void sonReceiveInterphoneMeetingMessage(ECInterphoneMsg* pInterphoneMsg);// 收到实时对讲里面的通知消息。
	static void sonReceiveVoiceMeetingMessage(ECVoiceMeetingMsg* pVoiceMeetingMsg);// 收到语音会议里面的通知消息。
	static void sonReceiveVideoMeetingMessage(ECVideoMeetingMsg* pVideoMeetingMsg);// 收到视频会议里面的通知消息。
	static void sonCreateMultimediaMeeting(unsigned int matchKey, int reason, ECMeetingInfo *pInfo);//创建音频、视频会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
	static void sonJoinMeeting(int reason, const char* conferenceId);//主动加入会议的回调。//reason：200成功，其他报错；conferenceId 会议ID；
	static void sonExitMeeting(const char* conferenceId);//退出会议。conferenceId 会议ID；
	static void sonQueryMeetingMembers(unsigned int matchKey, int reason, const char* conferenceId, int count, ECMeetingMemberInfo* pMembers);//查询加入回忆的成员。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；count 成员数组个数，pMembers 成员数组
	static void sonDismissMultiMediaMeeting(unsigned int matchKey, int reason, const char*conferenceId);//解散实时对讲、音频、视频会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；
	static void sonQueryMultiMediaMeetings(unsigned int matchKey, int reason, int count, ECMeetingInfo* pMeetingInfo);//查询会议信息的回调。matchKey 匹配主调键值；reason：200成功，其他报错；count 会议数组个数，pMembers 会议数组
	static void sonDeleteMemberMultiMediaMeeting(unsigned int matchKey, int reason, ECMeetingDeleteMemberInfo *pInfo);//删除会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
	static void sonInviteJoinMultiMediaMeeting(unsigned int matchKey, int reason, ECMeetingInviteInfo *pInfo);//邀请加入会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
	static void sonCreateInterphoneMeeting(unsigned int matchKey, int reason, const char* interphoneId);//创建实时对讲会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；interphoneId 实时对讲ID
	static void sonExitInterphoneMeeting(const char* interphoneId);//退出实时对讲。interphoneId 实时对讲ID
	static void sonSetMeetingSpeakListen(unsigned int matchKey, int reason, ECMeetingMemberSpeakListenInfo* pInfo);//设置会议成员听讲状态的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
	static void sonControlInterphoneMic(unsigned int matchKey, int reason, const char* controller, const char* interphoneId, bool requestIsControl);//实时对讲抢麦放麦的回调。matchKey 匹配主调键值；reason：200成功，其他报错；controller 控麦成员；interphoneId 实时对讲ID；requestIsControl 是否发起控麦
	static void sonPublishVideo(unsigned int matchKey, int reason, const char* conferenceId);//发布视频的回调。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；
	static void sonUnpublishVideo(unsigned int matchKey, int reason, const char* conferenceId);//取消视频的回调。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；
	static void sonRequestConferenceMemberVideo(int reason, const char *conferenceId, const char *member, int type);//视频会议时，请求会议成员视频数据。reason 详见Video_Conference_status；conferenceId 会议ID；member成员；type详见ECVideoDataType；
	static void sonCancelConferenceMemberVideo(int reason, const char *conferenceId, const char *member, int type);//视频会议时，取消会议成员视频数据响应。reason 详见Video_Conference_status；conferenceId 会议ID；tmember成员；ype详见ECVideoDataType；


private:
	/****************************登录接口的回调******************************/
	virtual void onLogInfo(const char* loginfo) =0 ; // 用于接收底层的log信息,调试出现的问题.
	virtual void onLogOut(int reason) = 0;//登出回调。reason：200成功，其他报错；
	virtual void onConnectState(const ECConnectState state) = 0;//连接回调；
	virtual void onSetPersonInfo(unsigned int matchKey, int reason, unsigned long long version) = 0; //设置个人资料。reason：200成功，其他报错；version:个人资料版本号
	virtual void onGetPersonInfo(unsigned int matchKey, int reason, ECPersonInfo *pPersonInfo) = 0;//获取个人资料。reason：200成功，其他报错；
	virtual void onGetUserState(unsigned int matchKey, int reason, int count, ECPersonState *pStateArr) = 0;//获取用户状态，reason：200成功，其他报错;count 数组个数，pStateArr 用户状态数组首指针
	virtual void onPublishPresence(unsigned int matchKey, int reason) = 0;//发布用户状态，reason：200成功，其他报错
	virtual void onReceiveFriendsPublishPresence(int count, ECPersonState *pStateArr) = 0;//收到服务器推送的用户状态变化,count 数组个数，pStateArr 用户状态数组首指针
	virtual void onSoftVersion(const char* softVersion, int updateMode, const char* updateDesc) = 0;//应用软件最新版本号升级提示，供应用自己升级用。softVersion：软件最新版本号；updateMode:更新模式  1：手动更新 2：强制更新；updateDesc：软件更新说明 
	virtual void onGetOnlineMultiDevice(unsigned int matchKey, int reason, int count, ECMultiDeviceState *pMultiDeviceStateArr) = 0;//获得登录者多设备登录状态变化回调,reason：200成功，其他报错count 数组个数，pMultiDeviceStateArr 多设备登录状态数组首指针
	virtual void onReceiveMultiDeviceState(int count, ECMultiDeviceState *pMultiDeviceStateArr) = 0;//收到服务器推送的登录者多设备登录状态变化,count 数组个数，pMultiDeviceStateArr 多设备登录状态数组首指针

	/****************************VOIP回调接口******************************/
	virtual void onMakeCallBack(unsigned int matchKey, int reason, ECCallBackInfo *pInfo)=0;//回拨请求。reason：200成功，其他报错；
	virtual void onCallEvents(int reason, const ECVoIPCallMsg * call)=0;//事件状态
	virtual void onCallIncoming(ECVoIPComingInfo *pComing)=0;  //音视频呼叫接到邀请
	virtual void onDtmfReceived(const char *callId, char dtmf)=0;//收到DTMF按键时的回调。callId 通话ID；	
	virtual void onSwitchCallMediaTypeRequest(const char*callId, int video)=0; // 收到对方更新媒体请求。 callId 通话ID；video：1  请求增加视频（需要响应） 0:请求删除视频（可以不响应，底层自动去除视频）
	virtual void onSwitchCallMediaTypeResponse(const char*callId, int video)=0;  // 对方应答媒体状态结果。callId 通话ID； video 1 有视频 0 无视频
	virtual void onRemoteVideoRatio(const char *CallidOrConferenceId, int width, int height, int type, const char *member)=0;//远端视频媒体分辨率变化时上报。type : 0 点对点,1 视频会议，2 共享会议；type=0 CallidOrConferenceId为通话id，type>0 CallidOrConferenceId为会议id；member有效

	
	/****************************消息处理接口的回调******************************/
	virtual void onOfflineMessageCount(int count)=0;//离线条数；
	virtual int  onGetOfflineMessage()=0;//离线拉取条数。<0 全拉；0 不拉；大于0，只拉取最新的条数，
	virtual void onOfflineMessageComplete()=0;//拉取离线成功
	virtual void onReceiveOfflineMessage(ECMessage *pMsg)=0;//离线消息回调
	virtual void onReceiveMessage(ECMessage *pMsg)=0;//实时消息回调
	virtual void onReceiveFile(ECMessage* pFileInfo)=0;//用户收到发送者发送过来的文件等待用户决定是否下载文件的回调。该回调消息的消息类型(MsgType)默认是6
	virtual void onDownloadFileComplete(unsigned int matchKey, int reason, ECMessage *msg)=0;//下载文件回调，下载完成后被调用（下载失败也会被调）。reason:0成功，其他报错；offset 内容大小偏移量
	virtual void onRateOfProgressAttach(unsigned int matchKey, unsigned long long rateSize, unsigned long long fileSize, ECMessage *msg)=0;//上传下载文件进度。ratesize 已经完成进度大小（字节）；filezise 总文件大小（字节）
	virtual void onSendTextMessage(unsigned int matchKey, int reason, ECMessage *msg)=0;//发送消息回调。reason：200成功，其他报错；
	virtual void onSendMediaFile(unsigned int matchKey, int reason, ECMessage *msg)=0;//上传文件回调，上传完成后被调用（上传失败也会被调）。reason:0成功，其他报错；offset 内容大小偏移量
	virtual void onDeleteMessage(unsigned int matchKey, int reason, int type, const char* msgId)=0;//删除服务端消息。reason:0成功，其他报错；type:透传消息类型；msgId:
	virtual void onOperateMessage(unsigned int tcpMsgIdOut, int reason)=0;//删除消息
	virtual void onReceiveOpreateNoticeMessage(const ECOperateNoticeMessage *pMsg) = 0;
	virtual void onUploadVTMFileOrBuf(unsigned int matchKey, int reason, const char* fileUrl) = 0;//上传VTM文件回调, fileIdOut：文件id, fileUrl:文件服务器保存地址

	/****************************群组接口对应的回调******************************/
	virtual void onCreateGroup(unsigned int matchKey, int reason, ECGroupDetailInfo *pInfo)=0;//创建群组。matchKey 匹配主调键值；reason：200成功，其他报错；
	virtual void onDismissGroup(unsigned int matchKey, int reason, const char* groupid)=0;//解散群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
	virtual void onQuitGroup(unsigned int matchKey, int reason, const char* groupid)=0;//退出群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
	virtual void onJoinGroup(unsigned int matchKey, int reason, const char* groupid)=0;//加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
	virtual void onReplyRequestJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int confirm)=0;//回复请求加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员；confirm 1拒绝 2同意
	virtual void onInviteJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char** members, int membercount, int confirm)=0;//邀请加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；members 成员数组；membercount 数组个数 ；confirm 1拒绝 2同意
	virtual void onReplyInviteJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int confirm)=0;//回复邀请加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员；confirm 1拒绝 2同意
	virtual void onQueryOwnGroup(unsigned int matchKey, int reason, int count, ECGroupSimpleInfo *group)=0;//查询我的群组。matchKey 匹配主调键值；reason：200成功，其他报错；
	virtual void onQueryGroupDetail(unsigned int matchKey, int reason, ECGroupDetailInfo *detail)=0;//查询群组详情。matchKey 匹配主调键值；reason：200成功，其他报错；
	virtual void onModifyGroup(unsigned int matchKey, int reason, ECModifyGroupInfo *pInfo)=0;//修改群组信息。matchKey 匹配主调键值；reason：200成功，其他报错；
	virtual void onSearchPublicGroup(unsigned int matchKey, int reason, int searchType, const char* keyword, int count, ECGroupSearchInfo *group, int pageNo)=0;//搜索公共群组。matchKey 匹配主调键值；reason：200成功，其他报错；searchType 搜索类型 1根据GroupId，精确搜索 2根据GroupName，模糊搜索； keyword 搜索关键词；count 群组个数；int pageNo 搜索分页索引
	virtual void onQueryGroupMember(unsigned int matchKey, int reason, const char *groupid, int count, ECGroupMember *member)=0;//查询群组成员。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
	virtual void onDeleteGroupMember(unsigned int matchKey, int reason, const char* groupid, const char* member)=0;//删除群组成员。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员
	virtual void onQueryGroupMemberCard(unsigned int matchKey, int reason, ECGroupMemberCard *card)=0;//查询群组成员名片。matchKey 匹配主调键值；reason：200成功，其他报错；
	virtual void onModifyGroupMemberCard(unsigned int matchKey, int reason, ECGroupMemberCard* pCard)=0;//修改群组成员名片。matchKey 匹配主调键值；reason：200成功，其他报错；
	virtual void onForbidMemberSpeakGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int isBan)=0;//群组禁言。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员；isBan 1允许发言 2禁止发言
	virtual void onSetGroupMessageRule(unsigned int matchKey, int reason, const char* groupid, bool notice)=0;//设置群组消息规则。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；notice 是否通知
    virtual void onSetGroupMemberRole(unsigned int matchKey, int reason)=0;//设置群组成员权限
	/****************************服务器下发的群组相关通知******************************/
	virtual void onReceiveGroupNoticeMessage(const ECGroupNoticeMessage * pMsg)=0;

	/****************************音频播放回调接口******************************/
	virtual void onRecordingTimeOut(int ms, const char* sessionId)=0;//录音超时。ms 为录音时长，单位毫秒,sessionId 透传会话ID
	virtual void onFinishedPlaying(const char* sessionId, int reason)=0;//播放结束,sessionId透传会话ID,reason 200成功.其他失败
	virtual void onRecordingAmplitude(double amplitude, const char* sessionId)=0;//返回音量的振幅。录音时振幅范围0-1,sessionId 透传会话ID

	/********************************************会议回调接口***********************************************/
	virtual void onMeetingIncoming(ECMeetingComingInfo *pCome) = 0;//会议接到邀请。
	virtual void onReceiveInterphoneMeetingMessage(ECInterphoneMsg* pInterphoneMsg) = 0;// 收到实时对讲里面的通知消息。
	virtual void onReceiveVoiceMeetingMessage(ECVoiceMeetingMsg* pVoiceMeetingMsg) = 0;// 收到语音会议里面的通知消息。
	virtual void onReceiveVideoMeetingMessage(ECVideoMeetingMsg* pVideoMeetingMsg) = 0;// 收到视频会议里面的通知消息。
	virtual void onCreateMultimediaMeeting(unsigned int matchKey, int reason, ECMeetingInfo *pInfo) = 0;//创建音频、视频会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
	virtual void onJoinMeeting(int reason, const char* conferenceId) = 0;//主动加入会议的回调。//reason：200成功，其他报错；conferenceId 会议ID；
	virtual void onExitMeeting(const char* conferenceId) = 0;//退出会议。conferenceId 会议ID；
	virtual void onQueryMeetingMembers(unsigned int matchKey, int reason, const char* conferenceId, int count, ECMeetingMemberInfo* pMembers) = 0;//查询加入回忆的成员。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；count 成员数组个数，pMembers 成员数组
	virtual void onDismissMultiMediaMeeting(unsigned int matchKey, int reason, const char*conferenceId) = 0;//解散实时对讲、音频、视频会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；
	virtual void onQueryMultiMediaMeetings(unsigned int matchKey, int reason, int count, ECMeetingInfo* pMeetingInfo) = 0;//查询会议信息的回调。matchKey 匹配主调键值；reason：200成功，其他报错；count 会议数组个数，pMembers 会议数组
	virtual void onDeleteMemberMultiMediaMeeting(unsigned int matchKey, int reason, ECMeetingDeleteMemberInfo *pInfo) = 0;//删除会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
	virtual void onInviteJoinMultiMediaMeeting(unsigned int matchKey, int reason, ECMeetingInviteInfo *pInfo) = 0;//邀请加入会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
	virtual void onCreateInterphoneMeeting(unsigned int matchKey, int reason, const char* interphoneId) = 0;//创建实时对讲会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；interphoneId 实时对讲ID
	virtual void onExitInterphoneMeeting(const char* interphoneId) = 0;//退出实时对讲。interphoneId 实时对讲ID
	virtual void onSetMeetingSpeakListen(unsigned int matchKey, int reason, ECMeetingMemberSpeakListenInfo* pInfo) = 0;//设置会议成员听讲状态的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
	virtual void onControlInterphoneMic(unsigned int matchKey, int reason, const char* controller, const char* interphoneId, bool requestIsControl) = 0;//实时对讲抢麦放麦的回调。matchKey 匹配主调键值；reason：200成功，其他报错；controller 控麦成员；interphoneId 实时对讲ID；requestIsControl 是否发起控麦
	virtual void onPublishVideo(unsigned int matchKey, int reason, const char* conferenceId) = 0;//发布视频的回调。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；
	virtual void onUnpublishVideo(unsigned int matchKey, int reason, const char* conferenceId) = 0;//取消视频的回调。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；
	virtual void onRequestConferenceMemberVideo(int reason, const char *conferenceId, const char *member, int type) = 0;//视频会议时，请求会议成员视频数据。reason 详见Video_Conference_status；conferenceId 会议ID；member成员；type详见ECVideoDataType；
	virtual void onCancelConferenceMemberVideo(int reason, const char *conferenceId, const char *member, int type) = 0;//视频会议时，取消会议成员视频数据响应。reason 详见Video_Conference_status；conferenceId 会议ID；tmember成员；ype详见ECVideoDataType；


public:
	static const std::string & GetVersion();
	int Initialize();
	int UnInitialize();

	void SetTraceFlag(int level, const char *logFileName);
	int QueryErrorDescribe(const char** errCodeDescribeOut, int errCodeIn);

	int SetServerEnvType(int nEnvType, const char *ServerXmlFileName);
    int SetServerAddress(const char *protobuf_addr, int protobuf_port, const char *filehttp_addr, int filehttp_port);
	int SetInternalDNS(bool enable, const char *dns, int port);

	int Login(const ECLoginInfo *loginInfo);
	int Logout();

	int SetPersonInfo(unsigned int *matchKey, ECPersonInfo* pInfo);
	int GetPersonInfo(unsigned int *matchKey, const char* username);
	int GetUserState(unsigned int *matchKey, const char** members, int membercount);


	int MakeCallBack(unsigned int *matchKey, const char *caller, const char *called, const char *callerDisplay, const char *calledDisplay);
	int MakeCall(const char **OutCallid, int callType, const char *called);
	int AcceptCall(const char *callId);
	int ReleaseCall(const char *callId, int reason);
	int SendDTMF(const char *callId, const char dtmf);
	const char* GetCurrentCall();
	int SetMute(bool mute);
	bool GetMuteStatus();
	int SetSoftMute(const char *callid, bool on);
	int GetSoftMuteStatus(const char *callid, bool* bMute);
	int SetAudioConfigEnabled(int type, bool enabled, int mode);
	int GetAudioConfigEnabled(int type, bool *enabled, int *mode);
	int SetCodecEnabled(int type, bool enabled);
	bool GetCodecEnabled(int type);
	int SetCodecNack(bool bAudioNack, bool bVideoNack);
	int GetCodecNack(bool *bAudioNack, bool *bVideoNack);
	int GetCallStatistics(const char *callId, bool bVideo, ECMediaStatisticsInfo *statistics);
	int SetVideoView(void *view, void *localView);
	void SetVideoBitRates(int bitrates);
	int SetRing(const char* fileName);
	int SetRingback(const char*fileName);
	int RequestSwitchCallMediaType(const char *callId, int video);
	int ResponseSwitchCallMediaType(const char *callId, int video);
	int GetCameraInfo(ECCameraInfo **  info);
	int SelectCamera(int cameraIndex, int capabilityIndex, int fps, int rotate, bool force);
	int GetNetworkStatistic(const char *callId, long long *duration, long long *sendTotalWifi, long long *recvTotalWifi);
	int GetSpeakerVolume(unsigned int* volume);
	int SetSpeakerVolume(unsigned int volume);
	void SetDtxEnabled(bool enabled);
	void SetSelfPhoneNumber(const char * phoneNumber);
	void SetSelfName(const char * nickName);
	int EnableLoudsSpeaker(bool enable);
	bool GetLoudsSpeakerStatus();
	int ResetVideoView(const char *callid, void* remoteView, void *localView);

	int GetLocalVideoSnapshotEx(const char* callid, unsigned char **buf, unsigned int *size, unsigned int *width, unsigned int *height);
	int GetRemoteVideoSnapshotEx(const char* callid, unsigned char **buf, unsigned int *size, unsigned int *width, unsigned int *height);
	int StartRecordScreen(const char *callid, const char *filename, int bitrate, int fps, int type);
	int StartRecordScreenEx(const char *callid, const char *filename, int bitrate, int fps, int type, int left, int top, int width, int height);
	int StopRecordScreen(const char *callid);



	int SendTextMessage(unsigned int *matchKey, const char *receiver, const char *message, MsgType type, const char *userdata, char * msgId);
	int SendMediaMessage(unsigned int *matchKey, const char* receiver, const char* fileName, MsgType type, const char* userdata, char * msgId);
	int DownloadFileMessage(unsigned int *matchKey, const char* sender, const char* recv, const char* url, const char* fileName, const char* msgId, MsgType type);
	void CancelUploadOrDownloadNOW(unsigned int matchKey);
	int DeleteMessage(unsigned int *matchKey, int type, const char* msgId);
	int OperateMessage(unsigned int* matchKey, const char* version, const char* messageId, int type);
	int CreateGroup(unsigned int *matchKey, const char*groupName, int type, const char* province, const char* city, int scope, const char* declared, int permission, int isDismiss, const char* groupDomain, bool isDiscuss);
	int DismissGroup(unsigned int *matchKey, const char* groupid);
	int QuitGroup(unsigned int *matchKey, const char* groupid);
	int JoinGroup(unsigned int *matchKey, const char* groupid, const char* declared);
	int ReplyRequestJoinGroup(unsigned int *matchKey, const char* groupid, const char* member, int confirm);
	int InviteJoinGroup(unsigned int *matchKey, const char* groupId, const char* declard, const char** members, int membercount, int confirm);
	int ReplyInviteJoinGroup(unsigned int *matchKey, const char* groupid, const char* invitor, int confirm);
	int QueryOwnGroup(unsigned int *matchKey, const char* borderGroupid, int pageSize, int target);
	int QueryGroupDetail(unsigned int *matchKey, const char* groupId);
	int ModifyGroup(unsigned int *matchKey, ECModifyGroupInfo* pModifyGroupInfo);
	int SearchPublicGroups(unsigned int *matchKey, int searchType, const char* keyword, int pageNo, int pageSize);
	int QueryGroupMember(unsigned int *matchKey, const char* groupId, const char* borderMember, int pageSize);
	int DeleteGroupMember(unsigned int *matchKey, const char* groupid, const char* member);
	int QueryGroupMemberCard(unsigned int *matchKey, const char* groupid, const char* member);
	int ModifyMemberCard(unsigned int *matchKey, ECGroupMemberCard *card);
	int ForbidMemberSpeak(unsigned int *matchKey, const char* groupid, const char* member, int isBan);
	int SetGroupMessageRule(unsigned int *matchKey, const char* groupid, bool notice);
	int StartVoiceRecording(const char* fileName, const char* sessionId);
	void StopVoiceRecording();
	int PlayVoiceMsg(const char* fileName, const char* sessionId);
	void StopVoiceMsg();
	int GetVoiceDuration(const char* fileName, int *duration);
	int UploadVTMFile(unsigned int* matchKey, const char* companyId, const char* companyPwd, const char* fileName, const char* callbackUrl);
	int UploadVTMBuf(unsigned int* matchKey, const char* companyId, const char* companyPwd, const char* Name, const char* buf, unsigned int size, const char* callbackUrl);


	int RequestConferenceMemberVideo(const char *conferenceId, const char *conferencePasswd, const char *member, void *videoWindow, int type);
	int CancelMemberVideo(const char *conferenceId, const char *conferencePasswd, const char *member, int type);
	int ResetVideoConfWindow(const char *conferenceId, const char *member, void *newWindow, int type);
	int CreateMultimediaMeeting(unsigned int *matchKey, int  meetingType, const char* meetingName, const char* password,
		const char* keywords, int voiceMode, int square, bool bAutoJoin, bool autoClose, bool autoDelete);
	int JoinMeeting(int  meetingType, const char* conferenceId, const char* password);
	int ExitMeeting(const char* conferenceId);
	int QueryMeetingMembers(unsigned int *matchKey, int meetingType, const char* conferenceId);
	int DismissMultiMediaMeeting(unsigned int *matchKey, int meetingType, const char* conferenceId);
	int QueryMultiMediaMeetings(unsigned int *matchKey, int meetingType, const char* keywords);
	int InviteJoinMultiMediaMeeting(unsigned int *matchKey, const char* conferenceId, const char** members, int membercount, bool blanding,
		bool isSpeak, bool isListen, const char* disNumber, const char* userdata);
	int DeleteMemberMultiMediaMeeting(unsigned int *matchKey, int meetingType, const char* conferenceId, const char* member, bool isVoIP);
	int PublishVideo(unsigned int *matchKey, const char* conferenceId);
	int UnpublishVideo(unsigned int *matchKey, const char* conferenceId);
	int CreateInterphoneMeeting(unsigned int *matchKey, const char** members, int membercount);
	int ControlInterphoneMic(unsigned int *matchKey, bool isControl, const char* interphoneId);
};

