#pragma once
#include "ECSDK.h"
#include <log4cplus/logger.h>
#include <mutex>
#include <condition_variable>

enum{
	WM_onLoginfo = WM_USER + 111,
	WM_onLogOut,
	WM_onConnectState,
	WM_onSetPersonInfo,
	WM_onGetPersonInfo,
	WM_onGetUserState,
	WM_onPublishPresence,
	WM_onReceiveFriendsPublishPresence,
	WM_onSoftVersion,
	WM_onGetOnlineMultiDevice,
	WM_onReceiveMultiDeviceState,

	WM_onMakeCallBack,
	WM_onCallEvents,
	WM_onCallIncoming,
	WM_onDtmfReceived,
	WM_onSwitchCallMediaTypeRequest,
	WM_onSwitchCallMediaTypeResponse,
	WM_onRemoteVideoRatio,
	WM_onOfflineMessageCount,
	WM_onGetOfflineMessage,
	WM_onOfflineMessageComplete,
	WM_onReceiveOfflineMessage,
	WM_onReceiveMessage,
	WM_onReceiveFile,
	WM_onDownloadFileComplete,
	WM_onRateOfProgressAttach,
	WM_onSendTextMessage,
	WM_onSendMediaFile,
	WM_onDeleteMessage,
	WM_onOperateMessage,
	WM_onReceiveOpreateNoticeMessage,
	WM_onUploadVTMFileOrBuf,
	WM_onCreateGroup,
	WM_onDismissGroup,
	WM_onQuitGroup,
	WM_onJoinGroup,
	WM_onReplyRequestJoinGroup,
	WM_onInviteJoinGroup,
	WM_onReplyInviteJoinGroup,
	WM_onQueryOwnGroup,
	WM_onQueryGroupDetail,
	WM_onModifyGroup,
	WM_onSearchPublicGroup,
	WM_onQueryGroupMember,
	WM_onDeleteGroupMember,
	WM_onQueryGroupMemberCard,
	WM_onModifyGroupMemberCard,
	WM_onForbidMemberSpeakGroup,
	WM_onSetGroupMessageRule,
	WM_onReceiveGroupNoticeMessage,
	WM_onRecordingTimeOut,
	WM_onFinishedPlaying,
	WM_onRecordingAmplitude,
	WM_onMeetingIncoming,
	WM_onReceiveInterphoneMeetingMessage,
	WM_onReceiveVoiceMeetingMessage,
	WM_onReceiveVideoMeetingMessage,
	WM_onCreateMultimediaMeeting,
	WM_onJoinMeeting,
	WM_onExitMeeting,
	WM_onQueryMeetingMembers,
	WM_onDismissMultiMediaMeeting,
	WM_onQueryMultiMediaMeetings,
	WM_onDeleteMemberMultiMediaMeeting,
	WM_onInviteJoinMultiMediaMeeting,
	WM_onCreateInterphoneMeeting,
	WM_onExitInterphoneMeeting,
	WM_onSetMeetingSpeakListen,
	WM_onControlInterphoneMic,
	WM_onPublishVideo,
	WM_onUnpublishVideo,
	WM_onRequestConferenceMemberVideo,
	WM_onCancelConferenceMemberVideo,
	WM_onVncViewerCallBack
};

class ECSDKBase : public ECSDK
{
public:
	ECSDKBase();
	~ECSDKBase();

	std::string m_utf8VoipId;
	std::string m_utf8AppDataDir;
	HANDLE m_hShareWnd;
	std::string m_VncRemoteId;
	std::string m_VncServerIP;
	LONG m_VncServerPort;

	virtual HWND  gethWnd() const = 0;
	int Initialize();
	int Login(const ECLoginInfo *loginInfo);
	int SetDesktopWnd(HANDLE hWnd);
	int sendVncCmdToRemote(int roomID, int msgType, int msgStatus, const std::string &desc);
	const std::string & getAppDataDir()const;

private:
	/****************************登录接口的回调******************************/
	virtual void onLogInfo(const char* loginfo); // 用于接收底层的log信息,调试出现的问题.
	virtual void onLogOut(int reason);//登出回调。reason：200成功，其他报错；
	virtual void onConnectState(const ECConnectState state);//连接回调；
	virtual void onSetPersonInfo(unsigned int matchKey, int reason, unsigned long long version);//设置个人资料。reason：200成功，其他报错；version:个人资料版本号
	virtual void onGetPersonInfo(unsigned int matchKey, int reason, ECPersonInfo *pPersonInfo);//获取个人资料。reason：200成功，其他报错；
	virtual void onGetUserState(unsigned int matchKey, int reason, int count, ECPersonState *pStateArr);//获取用户状态，reason：200成功，其他报错;count 数组个数，pStateArr 用户状态数组首指针
	virtual void onPublishPresence(unsigned int matchKey, int reason);//发布用户状态，reason：200成功，其他报错
	virtual void onReceiveFriendsPublishPresence(int count, ECPersonState *pStateArr);//收到服务器推送的用户状态变化,count 数组个数，pStateArr 用户状态数组首指针
	virtual void onSoftVersion(const char* softVersion, int updateMode, const char* updateDesc);//应用软件最新版本号升级提示，供应用自己升级用。softVersion：软件最新版本号；updateMode:更新模式  1：手动更新 2：强制更新；updateDesc：软件更新说明 
	virtual void onGetOnlineMultiDevice(unsigned int matchKey, int reason, int count, ECMultiDeviceState *pMultiDeviceStateArr);//获得登录者多设备登录状态变化回调,reason：200成功，其他报错count 数组个数，pMultiDeviceStateArr 多设备登录状态数组首指针
	virtual void onReceiveMultiDeviceState(int count, ECMultiDeviceState *pMultiDeviceStateArr);//收到服务器推送的登录者多设备登录状态变化,count 数组个数，pMultiDeviceStateArr 多设备登录状态数组首指针

	/****************************VOIP回调接口******************************/
	virtual void onMakeCallBack(unsigned int matchKey, int reason, ECCallBackInfo *pInfo);//回拨请求。reason：200成功，其他报错；
	virtual void onCallEvents(int reason, const ECVoIPCallMsg * call);//事件状态
	virtual void onCallIncoming(ECVoIPComingInfo *pComing);  //音视频呼叫接到邀请
	virtual void onDtmfReceived(const char *callId, char dtmf);//收到DTMF按键时的回调。callId 通话ID；	
	virtual void onSwitchCallMediaTypeRequest(const char*callId, int video); // 收到对方更新媒体请求。 callId 通话ID；video：1  请求增加视频（需要响应） 0:请求删除视频（可以不响应，底层自动去除视频）
	virtual void onSwitchCallMediaTypeResponse(const char*callId, int video);  // 对方应答媒体状态结果。callId 通话ID； video 1 有视频 0 无视频
	virtual void onRemoteVideoRatio(const char *CallidOrConferenceId, int width, int height, int type, const char *member);//远端视频媒体分辨率变化时上报。type : 0 点对点,1 视频会议，2 共享会议；type=0 CallidOrConferenceId为通话id，type>0 CallidOrConferenceId为会议id；member有效

	/****************************消息处理接口的回调******************************/
	virtual void onOfflineMessageCount(int count);//离线条数；
protected:
	int m_onGetOfflineMessage_Result;
	std::mutex m_onGetOfflineMessage_mtx;
	std::condition_variable m_onGetOfflineMessage_cv;
private:
	virtual int  onGetOfflineMessage() override;//离线拉取条数。<0 全拉；0 不拉；大于0，只拉取最新的条数，
	virtual void onOfflineMessageComplete() override;//拉取离线成功
	virtual void onReceiveOfflineMessage(ECMessage *pMsg) override;//离线消息回调
	virtual void onReceiveMessage(ECMessage *pMsg) override;//实时消息回调
	virtual void onReceiveFile(ECMessage* pFileInfo) override;//用户收到发送者发送过来的文件等待用户决定是否下载文件的回调。该回调消息的消息类型(MsgType)默认是6
	virtual void onDownloadFileComplete(unsigned int matchKey, int reason, ECMessage *msg) override;//下载文件回调，下载完成后被调用（下载失败也会被调）。reason:0成功，其他报错；offset 内容大小偏移量
	virtual void onRateOfProgressAttach(unsigned int matchKey, unsigned long long rateSize, unsigned long long fileSize, ECMessage *msg);//上传下载文件进度。ratesize 已经完成进度大小（字节）；filezise 总文件大小（字节）
	virtual void onSendTextMessage(unsigned int matchKey, int reason, ECMessage *msg);//发送消息回调。reason：200成功，其他报错；
	virtual void onSendMediaFile(unsigned int matchKey, int reason, ECMessage *msg);//上传文件回调，上传完成后被调用（上传失败也会被调）。reason:0成功，其他报错；offset 内容大小偏移量
	virtual void onDeleteMessage(unsigned int matchKey, int reason, int type, const char* msgId);//删除服务端消息。reason:0成功，其他报错；type:透传消息类型；msgId:
	virtual void onOperateMessage(unsigned int tcpMsgIdOut, int reason) override;//删除消息
	virtual void onReceiveOpreateNoticeMessage(const ECOperateNoticeMessage *pMsg) override;
	virtual void onUploadVTMFileOrBuf(unsigned int matchKey, int reason, const char* fileUrl);//上传VTM文件回调, fileIdOut：文件id, fileUrl:文件服务器保存地址

	/****************************群组接口对应的回调******************************/
	virtual void onCreateGroup(unsigned int matchKey, int reason, ECGroupDetailInfo *pInfo);//创建群组。matchKey 匹配主调键值；reason：200成功，其他报错；
	virtual void onDismissGroup(unsigned int matchKey, int reason, const char* groupid);//解散群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
	virtual void onQuitGroup(unsigned int matchKey, int reason, const char* groupid);//退出群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
	virtual void onJoinGroup(unsigned int matchKey, int reason, const char* groupid);//加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
	virtual void onReplyRequestJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int confirm);//回复请求加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员；confirm 1拒绝 2同意
	virtual void onInviteJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char** members, int membercount, int confirm);//邀请加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；members 成员数组；membercount 数组个数 ；confirm 1拒绝 2同意
	virtual void onReplyInviteJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int confirm);//回复邀请加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员；confirm 1拒绝 2同意
	virtual void onQueryOwnGroup(unsigned int matchKey, int reason, int count, ECGroupSimpleInfo *group);//查询我的群组。matchKey 匹配主调键值；reason：200成功，其他报错；
	virtual void onQueryGroupDetail(unsigned int matchKey, int reason, ECGroupDetailInfo *detail);//查询群组详情。matchKey 匹配主调键值；reason：200成功，其他报错；
	virtual void onModifyGroup(unsigned int matchKey, int reason, ECModifyGroupInfo *pInfo);//修改群组信息。matchKey 匹配主调键值；reason：200成功，其他报错；
	virtual void onSearchPublicGroup(unsigned int matchKey, int reason, int searchType, const char* keyword, int count, ECGroupSearchInfo *group, int pageNo);//搜索公共群组。matchKey 匹配主调键值；reason：200成功，其他报错；searchType 搜索类型 1根据GroupId，精确搜索 2根据GroupName，模糊搜索； keyword 搜索关键词；count 群组个数；int pageNo 搜索分页索引
	virtual void onQueryGroupMember(unsigned int matchKey, int reason, const char *groupid, int count, ECGroupMember *member);//查询群组成员。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
	virtual void onDeleteGroupMember(unsigned int matchKey, int reason, const char* groupid, const char* member);//删除群组成员。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员
	virtual void onQueryGroupMemberCard(unsigned int matchKey, int reason, ECGroupMemberCard *card);//查询群组成员名片。matchKey 匹配主调键值；reason：200成功，其他报错；
	virtual void onModifyGroupMemberCard(unsigned int matchKey, int reason, ECGroupMemberCard* pCard);//修改群组成员名片。matchKey 匹配主调键值；reason：200成功，其他报错；
	virtual void onForbidMemberSpeakGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int isBan);//群组禁言。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员；isBan 1允许发言 2禁止发言
	virtual void onSetGroupMessageRule(unsigned int matchKey, int reason, const char* groupid, bool notice);//设置群组消息规则。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；notice 是否通知
    virtual void onSetGroupMemberRole(unsigned int matchKey, int reason);//设置群组成员权限
	/****************************服务器下发的群组相关通知******************************/
	virtual void onReceiveGroupNoticeMessage(const ECGroupNoticeMessage * pMsg);

	/****************************音频播放回调接口******************************/
	virtual void onRecordingTimeOut(int ms, const char* sessionId);//录音超时。ms 为录音时长，单位毫秒,sessionId 透传会话ID
	virtual void onFinishedPlaying(const char* sessionId, int reason);//播放结束,sessionId透传会话ID,reason 200成功.其他失败
	virtual void onRecordingAmplitude(double amplitude, const char* sessionId);//返回音量的振幅。录音时振幅范围0-1,sessionId 透传会话ID

	/********************************************会议回调接口***********************************************/
	virtual void onMeetingIncoming(ECMeetingComingInfo *pCome);//会议接到邀请。
	virtual void onReceiveInterphoneMeetingMessage(ECInterphoneMsg* pInterphoneMsg);// 收到实时对讲里面的通知消息。
	virtual void onReceiveVoiceMeetingMessage(ECVoiceMeetingMsg* pVoiceMeetingMsg);// 收到语音会议里面的通知消息。
	virtual void onReceiveVideoMeetingMessage(ECVideoMeetingMsg* pVideoMeetingMsg);// 收到视频会议里面的通知消息。
	virtual void onCreateMultimediaMeeting(unsigned int matchKey, int reason, ECMeetingInfo *pInfo);//创建音频、视频会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
	virtual void onJoinMeeting(int reason, const char* conferenceId);//主动加入会议的回调。//reason：200成功，其他报错；conferenceId 会议ID；
	virtual void onExitMeeting(const char* conferenceId);//退出会议。conferenceId 会议ID；
	virtual void onQueryMeetingMembers(unsigned int matchKey, int reason, const char* conferenceId, int count, ECMeetingMemberInfo* pMembers);//查询加入回忆的成员。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；count 成员数组个数，pMembers 成员数组
	virtual void onDismissMultiMediaMeeting(unsigned int matchKey, int reason, const char*conferenceId);//解散实时对讲、音频、视频会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；
	virtual void onQueryMultiMediaMeetings(unsigned int matchKey, int reason, int count, ECMeetingInfo* pMeetingInfo);//查询会议信息的回调。matchKey 匹配主调键值；reason：200成功，其他报错；count 会议数组个数，pMembers 会议数组
	virtual void onDeleteMemberMultiMediaMeeting(unsigned int matchKey, int reason, ECMeetingDeleteMemberInfo *pInfo);//删除会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
	virtual void onInviteJoinMultiMediaMeeting(unsigned int matchKey, int reason, ECMeetingInviteInfo *pInfo);//邀请加入会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
	virtual void onCreateInterphoneMeeting(unsigned int matchKey, int reason, const char* interphoneId);//创建实时对讲会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；interphoneId 实时对讲ID
	virtual void onExitInterphoneMeeting(const char* interphoneId);//退出实时对讲。interphoneId 实时对讲ID
	virtual void onSetMeetingSpeakListen(unsigned int matchKey, int reason, ECMeetingMemberSpeakListenInfo* pInfo) override;//设置会议成员听讲状态的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
	virtual void onControlInterphoneMic(unsigned int matchKey, int reason, const char* controller, const char* interphoneId, bool requestIsControl);//实时对讲抢麦放麦的回调。matchKey 匹配主调键值；reason：200成功，其他报错；controller 控麦成员；interphoneId 实时对讲ID；requestIsControl 是否发起控麦
	virtual void onPublishVideo(unsigned int matchKey, int reason, const char* conferenceId);//发布视频的回调。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；
	virtual void onUnpublishVideo(unsigned int matchKey, int reason, const char* conferenceId);//取消视频的回调。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；
	virtual void onRequestConferenceMemberVideo(int reason, const char *conferenceId, const char *member, int type);//视频会议时，请求会议成员视频数据。reason 详见Video_Conference_status；conferenceId 会议ID；member成员；type详见ECVideoDataType；
	virtual void onCancelConferenceMemberVideo(int reason, const char *conferenceId, const char *member, int type);//视频会议时，取消会议成员视频数据响应。reason 详见Video_Conference_status；conferenceId 会议ID；tmember成员；ype详见ECVideoDataType；

private:
	log4cplus::Logger log;
public:
	int RGB24Snapshot(BYTE* pData, int width, int height, unsigned char ** outData, unsigned int *outSize);
};

