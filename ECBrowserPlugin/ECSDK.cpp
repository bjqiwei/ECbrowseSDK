#include "stdafx.h"
#include "ECSDK.h"
#include "ECInterface.h"
#include "ECImInterface.h"
#include "ECMeetingInterface.h"
#include "ECVoipInterface.h"

#include <log4cplus/loggingmacros.h>

extern std::wstring utf16Path;
#pragma comment(lib,"version.lib")

static ECGeneralCallBackInterface GeneralCB;
static ECVoipCallBackInterface VoipCB;
static ECMessageCallBackInterface MessageCB;
static ECMeetingCallBackInterface MeetingCB;
static ECSDK * g_Instance = nullptr;

void ECSDK::sonLogInfo(const char* loginfo) // 用于接收底层的log信息,调试出现的问题.
{
    static log4cplus::Logger log = log4cplus::Logger::getInstance("mediaDebug");
    LOG4CPLUS_TRACE(log, loginfo);
	if(g_Instance)g_Instance->onLogInfo(loginfo);
}

void ECSDK::sonLogOut(int reason)//登出回调。reason：200成功，其他报错；
{
	if(g_Instance)g_Instance->onLogOut(reason);
}

void ECSDK::sonConnectState(const ECConnectState state)//连接回调；
{
	if(g_Instance)g_Instance->onConnectState(state);
}

void ECSDK::sonSetPersonInfo(unsigned int matchKey, int reason, unsigned long long version)//设置个人资料。reason：200成功，其他报错；version:个人资料版本号
{
	if(g_Instance)g_Instance->onSetPersonInfo(matchKey, reason, version);
}

void ECSDK::sonGetPersonInfo(unsigned int matchKey, int reason, ECPersonInfo *pPersonInfo)//获取个人资料。reason：200成功，其他报错；
{
	if(g_Instance)g_Instance->onGetPersonInfo(matchKey, reason, pPersonInfo);
}

void ECSDK::sonGetUserState(unsigned int matchKey, int reason, int count, ECPersonState *pStateArr)//获取用户状态，reason：200成功，其他报错;count 数组个数，pStateArr 用户状态数组首指针
{
	if(g_Instance)g_Instance->onGetUserState(matchKey, reason, count, pStateArr);
}

void ECSDK::sonPublishPresence(unsigned int matchKey, int reason)//发布用户状态，reason：200成功，其他报错
{
	if(g_Instance)g_Instance->onPublishPresence(matchKey, reason);
}

void ECSDK::sonReceiveFriendsPublishPresence(int count, ECPersonState *pStateArr)//收到服务器推送的用户状态变化,count 数组个数，pStateArr 用户状态数组首指针
{
	if(g_Instance)g_Instance->onReceiveFriendsPublishPresence(count, pStateArr);
}

void ECSDK::sonSoftVersion(const char* softVersion, int updateMode, const char* updateDesc)//应用软件最新版本号升级提示，供应用自己升级用。softVersion：软件最新版本号；updateMode:更新模式  1：手动更新 2：强制更新；updateDesc：软件更新说明 
{
	if(g_Instance)g_Instance->onSoftVersion(softVersion, updateMode, updateDesc);
}

void ECSDK::sonGetOnlineMultiDevice(unsigned int matchKey, int reason, int count, ECMultiDeviceState *pMultiDeviceStateArr)//获得登录者多设备登录状态变化回调,reason：200成功，其他报错count 数组个数，pMultiDeviceStateArr 多设备登录状态数组首指针
{
	if(g_Instance)g_Instance->onGetOnlineMultiDevice(matchKey, reason, count, pMultiDeviceStateArr);
}

void ECSDK::sonReceiveMultiDeviceState(int count, ECMultiDeviceState *pMultiDeviceStateArr)//收到服务器推送的登录者多设备登录状态变化,count 数组个数，pMultiDeviceStateArr 多设备登录状态数组首指针
{
	if(g_Instance)g_Instance->onReceiveMultiDeviceState(count, pMultiDeviceStateArr);
}

/****************************VOIP回调接口******************************/
void ECSDK::sonMakeCallBack(unsigned int matchKey, int reason, ECCallBackInfo *pInfo)//回拨请求。reason：200成功，其他报错；
{
	if(g_Instance)g_Instance->onMakeCallBack(matchKey, reason, pInfo);
}
void ECSDK::sonCallEvents(int reason, const ECVoIPCallMsg * call)//事件状态
{
	if(g_Instance)g_Instance->onCallEvents(reason, call);
}
void ECSDK::sonCallIncoming(ECVoIPComingInfo *pComing)  //音视频呼叫接到邀请
{
	if(g_Instance)g_Instance->onCallIncoming(pComing);
}
void ECSDK::sonDtmfReceived(const char *callId, char dtmf)//收到DTMF按键时的回调。callId 通话ID；	
{
	if(g_Instance)g_Instance->onDtmfReceived(callId, dtmf);
}
void ECSDK::sonSwitchCallMediaTypeRequest(const char*callId, int video) // 收到对方更新媒体请求。 callId 通话ID；video：1  请求增加视频（需要响应） 0:请求删除视频（可以不响应，底层自动去除视频）
{
	if(g_Instance)g_Instance->onSwitchCallMediaTypeRequest(callId, video);
}
void ECSDK::sonSwitchCallMediaTypeResponse(const char*callId, int video)  // 对方应答媒体状态结果。callId 通话ID； video 1 有视频 0 无视频
{
	if(g_Instance)g_Instance->onSwitchCallMediaTypeResponse(callId, video);
}
void ECSDK::sonRemoteVideoRatio(const char *CallidOrConferenceId, int width, int height, int type, const char *member)//远端视频媒体分辨率变化时上报。type : 0 点对点,1 视频会议，2 共享会议；type=0 CallidOrConferenceId为通话id，type>0 CallidOrConferenceId为会议id；member有效
{
	if(g_Instance)g_Instance->onRemoteVideoRatio(CallidOrConferenceId, width, height, type, member);
}

/****************************消息处理接口的回调******************************/
void ECSDK::sonOfflineMessageCount(int count)//离线条数；
{
	if(g_Instance)g_Instance->onOfflineMessageCount(count);
}
int  ECSDK::sonGetOfflineMessage()//离线拉取条数。<0 全拉；0 不拉；大于0，只拉取最新的条数，
{
	if(g_Instance)
		return g_Instance->onGetOfflineMessage();
    return 0;
}
void ECSDK::sonOfflineMessageComplete()//拉取离线成功
{
	if(g_Instance)g_Instance->onOfflineMessageComplete();
}
void ECSDK::sonReceiveOfflineMessage(ECMessage *pMsg)//离线消息回调
{
	if(g_Instance)g_Instance->onReceiveOfflineMessage(pMsg);
}
void ECSDK::sonReceiveMessage(ECMessage *pMsg)//实时消息回调
{
	if(g_Instance)g_Instance->onReceiveMessage(pMsg);
}
void ECSDK::sonReceiveFile(ECMessage* pFileInfo)//用户收到发送者发送过来的文件等待用户决定是否下载文件的回调。该回调消息的消息类型(MsgType)默认是6
{
	if(g_Instance)g_Instance->onReceiveFile(pFileInfo);
}
void ECSDK::sonDownloadFileComplete(unsigned int matchKey, int reason, ECMessage *msg)//下载文件回调，下载完成后被调用（下载失败也会被调）。reason:0成功，其他报错；offset 内容大小偏移量
{
	if(g_Instance)g_Instance->onDownloadFileComplete(matchKey, reason, msg);
}
void ECSDK::sonRateOfProgressAttach(unsigned int matchKey, unsigned long long rateSize, unsigned long long fileSize, ECMessage *msg)//上传下载文件进度。ratesize 已经完成进度大小（字节）；filezise 总文件大小（字节）
{
	if(g_Instance)g_Instance->onRateOfProgressAttach(matchKey, rateSize, fileSize, msg);
}
void ECSDK::sonSendTextMessage(unsigned int matchKey, int reason, ECMessage *msg)//发送消息回调。reason：200成功，其他报错；
{
	if(g_Instance)g_Instance->onSendTextMessage(matchKey, reason, msg);
}
void ECSDK::sonSendMediaFile(unsigned int matchKey, int reason, ECMessage *msg)//上传文件回调，上传完成后被调用（上传失败也会被调）。reason:0成功，其他报错；offset 内容大小偏移量
{
	if(g_Instance)g_Instance->onSendMediaFile(matchKey, reason, msg);
}

void ECSDK::sonDeleteMessage(unsigned int matchKey, int reason, int type, const char* msgId)//删除服务端消息。reason:0成功，其他报错；type:透传消息类型；msgId:
{
	if(g_Instance)g_Instance->onDeleteMessage(matchKey, reason, type, msgId);
}

void ECSDK::sonOperateMessage(unsigned int tcpMsgIdOut, int reason)//删除消息
{
	if (g_Instance) g_Instance->onOperateMessage(tcpMsgIdOut, reason);
}

void ECSDK::sonReceiveOpreateNoticeMessage(const ECOperateNoticeMessage *pMsg)
{
	if (g_Instance) g_Instance->onReceiveOpreateNoticeMessage(pMsg);
}

void ECSDK::sonUploadVTMFileOrBuf(unsigned int matchKey, int reason, const char* fileUrl)
{
	if (g_Instance)g_Instance->onUploadVTMFileOrBuf(matchKey, reason, fileUrl);
}

/****************************群组接口对应的回调******************************/
void ECSDK::sonCreateGroup(unsigned int matchKey, int reason, ECGroupDetailInfo *pInfo)//创建群组。matchKey 匹配主调键值；reason：200成功，其他报错；
{
	if(g_Instance)g_Instance->onCreateGroup(matchKey, reason, pInfo);
}
void ECSDK::sonDismissGroup(unsigned int matchKey, int reason, const char* groupid)//解散群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
{
	if(g_Instance)g_Instance->onDismissGroup(matchKey, reason, groupid);
}
void ECSDK::sonQuitGroup(unsigned int matchKey, int reason, const char* groupid)//退出群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
{
	if(g_Instance)g_Instance->onQuitGroup(matchKey, reason, groupid);
}
void ECSDK::sonJoinGroup(unsigned int matchKey, int reason, const char* groupid)//加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
{
	if(g_Instance)g_Instance->onJoinGroup(matchKey, reason, groupid);
}
void ECSDK::sonReplyRequestJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int confirm)//回复请求加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员；confirm 1拒绝 2同意
{
	if(g_Instance)g_Instance->onReplyRequestJoinGroup(matchKey, reason, groupid, member, confirm);
}
void ECSDK::sonInviteJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char** members, int membercount, int confirm)//邀请加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；members 成员数组；membercount 数组个数 ；confirm 1拒绝 2同意
{
	if(g_Instance)g_Instance->onInviteJoinGroup(matchKey, reason, groupid, members, membercount, confirm);
}
void ECSDK::sonReplyInviteJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int confirm)//回复邀请加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员；confirm 1拒绝 2同意
{
	if(g_Instance)g_Instance->onReplyInviteJoinGroup(matchKey, reason, groupid, member, confirm);
}
void ECSDK::sonQueryOwnGroup(unsigned int matchKey, int reason, int count, ECGroupSimpleInfo *group)//查询我的群组。matchKey 匹配主调键值；reason：200成功，其他报错；
{
	if(g_Instance)g_Instance->onQueryOwnGroup(matchKey, reason, count, group);
}
void ECSDK::sonQueryGroupDetail(unsigned int matchKey, int reason, ECGroupDetailInfo *detail)//查询群组详情。matchKey 匹配主调键值；reason：200成功，其他报错；
{
	if(g_Instance)g_Instance->onQueryGroupDetail(matchKey, reason, detail);
}
void ECSDK::sonModifyGroup(unsigned int matchKey, int reason, ECModifyGroupInfo *pInfo)//修改群组信息。matchKey 匹配主调键值；reason：200成功，其他报错；
{
	if(g_Instance)g_Instance->onModifyGroup(matchKey, reason, pInfo);
}
void ECSDK::sonSearchPublicGroup(unsigned int matchKey, int reason, int searchType, const char* keyword, int count, ECGroupSearchInfo *group)//搜索公共群组。matchKey 匹配主调键值；reason：200成功，其他报错；searchType 搜索类型 1根据GroupId，精确搜索 2根据GroupName，模糊搜索； keyword 搜索关键词；count 群组个数；int pageNo 搜索分页索引
{
	if(g_Instance)g_Instance->onSearchPublicGroup(matchKey, reason, searchType, keyword, count, group, 1);
}
void ECSDK::sonQueryGroupMember(unsigned int matchKey, int reason, const char *groupid, int count, ECGroupMember *member)//查询群组成员。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
{
	if(g_Instance)g_Instance->onQueryGroupMember(matchKey, reason, groupid, count, member);
}
void ECSDK::sonDeleteGroupMember(unsigned int matchKey, int reason, const char* groupid, const char* member)//删除群组成员。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员
{
	if(g_Instance)g_Instance->onDeleteGroupMember(matchKey, reason, groupid, member);
}
void ECSDK::sonQueryGroupMemberCard(unsigned int matchKey, int reason, ECGroupMemberCard *card)//查询群组成员名片。matchKey 匹配主调键值；reason：200成功，其他报错；
{
	if(g_Instance)g_Instance->onQueryGroupMemberCard(matchKey, reason, card);
}
void ECSDK::sonModifyGroupMemberCard(unsigned int matchKey, int reason, ECGroupMemberCard* pCard)//修改群组成员名片。matchKey 匹配主调键值；reason：200成功，其他报错；
{
	if(g_Instance)g_Instance->onModifyGroupMemberCard(matchKey, reason, pCard);
}
void ECSDK::sonForbidMemberSpeakGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int isBan)//群组禁言。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员；isBan 1允许发言 2禁止发言
{
	if(g_Instance)g_Instance->onForbidMemberSpeakGroup(matchKey, reason, groupid, member, isBan);
}
void ECSDK::sonSetGroupMessageRule(unsigned int matchKey, int reason, const char* groupid, bool notice)//设置群组消息规则。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；notice 是否通知
{
	if(g_Instance)g_Instance->onSetGroupMessageRule(matchKey, reason, groupid, notice);
}

void ECSDK::sonSetGroupMemberRole(unsigned int matchKey, int reason)//设置群组成员权限
{
    if(g_Instance)g_Instance->onSetGroupMemberRole(matchKey, reason);
}
/****************************服务器下发的群组相关通知******************************/
void ECSDK::sonReceiveGroupNoticeMessage(const ECGroupNoticeMessage * pMsg)
{
	if(g_Instance)g_Instance->onReceiveGroupNoticeMessage(pMsg);
}

/****************************音频播放回调接口******************************/
void ECSDK::sonRecordingTimeOut(int ms, const char* sessionId)//录音超时。ms 为录音时长，单位毫秒,sessionId 透传会话ID
{
	if(g_Instance)g_Instance->onRecordingTimeOut(ms, sessionId);
}
void ECSDK::sonFinishedPlaying(const char* sessionId, int reason)//播放结束,sessionId透传会话ID,reason 200成功.其他失败
{
	if(g_Instance)g_Instance->onFinishedPlaying(sessionId, reason);
}
void ECSDK::sonRecordingAmplitude(double amplitude, const char* sessionId)//返回音量的振幅。录音时振幅范围0-1,sessionId 透传会话ID
{
	if(g_Instance)g_Instance->onRecordingAmplitude(amplitude, sessionId);
}

/********************************************会议回调接口***********************************************/
void ECSDK::sonMeetingIncoming(ECMeetingComingInfo *pCome)//会议接到邀请。
{
	if(g_Instance)g_Instance->onMeetingIncoming(pCome);
}
void ECSDK::sonReceiveInterphoneMeetingMessage(ECInterphoneMsg* pInterphoneMsg)// 收到实时对讲里面的通知消息。
{
	if(g_Instance)g_Instance->onReceiveInterphoneMeetingMessage(pInterphoneMsg);
}
void ECSDK::sonReceiveVoiceMeetingMessage(ECVoiceMeetingMsg* pVoiceMeetingMsg)// 收到语音会议里面的通知消息。
{
	if(g_Instance)g_Instance->onReceiveVoiceMeetingMessage(pVoiceMeetingMsg);
}
void ECSDK::sonReceiveVideoMeetingMessage(ECVideoMeetingMsg* pVideoMeetingMsg)// 收到视频会议里面的通知消息。
{
	if(g_Instance)g_Instance->onReceiveVideoMeetingMessage(pVideoMeetingMsg);
}
void ECSDK::sonCreateMultimediaMeeting(unsigned int matchKey, int reason, ECMeetingInfo *pInfo)//创建音频、视频会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
{
	if(g_Instance)g_Instance->onCreateMultimediaMeeting(matchKey, reason, pInfo);
}
void ECSDK::sonJoinMeeting(int reason, const char* conferenceId)//主动加入会议的回调。//reason：200成功，其他报错；conferenceId 会议ID；
{
	if(g_Instance)g_Instance->onJoinMeeting(reason, conferenceId);
}
void ECSDK::sonExitMeeting(const char* conferenceId)//退出会议。conferenceId 会议ID；
{
	if(g_Instance)g_Instance->onExitMeeting(conferenceId);
}
void ECSDK::sonQueryMeetingMembers(unsigned int matchKey, int reason, const char* conferenceId, int count, ECMeetingMemberInfo* pMembers)//查询加入回忆的成员。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；count 成员数组个数，pMembers 成员数组
{
	if(g_Instance)g_Instance->onQueryMeetingMembers(matchKey, reason, conferenceId, count, pMembers);
}
void ECSDK::sonDismissMultiMediaMeeting(unsigned int matchKey, int reason, const char*conferenceId)//解散实时对讲、音频、视频会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；
{
	if(g_Instance)g_Instance->onDismissMultiMediaMeeting(matchKey, reason, conferenceId);
}
void ECSDK::sonQueryMultiMediaMeetings(unsigned int matchKey, int reason, int count, ECMeetingInfo* pMeetingInfo)//查询会议信息的回调。matchKey 匹配主调键值；reason：200成功，其他报错；count 会议数组个数，pMembers 会议数组
{
	if(g_Instance)g_Instance->onQueryMultiMediaMeetings(matchKey, reason, count, pMeetingInfo);
}
void ECSDK::sonDeleteMemberMultiMediaMeeting(unsigned int matchKey, int reason, ECMeetingDeleteMemberInfo *pInfo)//删除会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
{
	if(g_Instance)g_Instance->onDeleteMemberMultiMediaMeeting(matchKey, reason, pInfo);
}
void ECSDK::sonInviteJoinMultiMediaMeeting(unsigned int matchKey, int reason, ECMeetingInviteInfo *pInfo)//邀请加入会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
{
	if(g_Instance)g_Instance->onInviteJoinMultiMediaMeeting(matchKey, reason, pInfo);
}
void ECSDK::sonCreateInterphoneMeeting(unsigned int matchKey, int reason, const char* interphoneId)//创建实时对讲会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；interphoneId 实时对讲ID
{
	if(g_Instance)g_Instance->onCreateInterphoneMeeting(matchKey, reason, interphoneId);
}
void ECSDK::sonExitInterphoneMeeting(const char* interphoneId)//退出实时对讲。interphoneId 实时对讲ID
{
	if(g_Instance)g_Instance->onExitInterphoneMeeting(interphoneId);
}
void ECSDK::sonSetMeetingSpeakListen(unsigned int matchKey, int reason, ECMeetingMemberSpeakListenInfo* pInfo)//设置会议成员听讲状态的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
{
	if (g_Instance) g_Instance->onSetMeetingSpeakListen(matchKey, reason, pInfo);
}
void ECSDK::sonControlInterphoneMic(unsigned int matchKey, int reason, const char* controller, const char* interphoneId, bool requestIsControl)//实时对讲抢麦放麦的回调。matchKey 匹配主调键值；reason：200成功，其他报错；controller 控麦成员；interphoneId 实时对讲ID；requestIsControl 是否发起控麦
{
	if(g_Instance)g_Instance->onControlInterphoneMic(matchKey, reason, controller, interphoneId, requestIsControl);
}
void ECSDK::sonPublishVideo(unsigned int matchKey, int reason, const char* conferenceId)//发布视频的回调。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；
{
	if(g_Instance)g_Instance->onPublishVideo(matchKey, reason, conferenceId);
}
void ECSDK::sonUnpublishVideo(unsigned int matchKey, int reason, const char* conferenceId)//取消视频的回调。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；
{
	if(g_Instance)g_Instance->onUnpublishVideo(matchKey, reason, conferenceId);
}
void ECSDK::sonRequestConferenceMemberVideo(int reason, const char *conferenceId, const char *member, int type)//视频会议时，请求会议成员视频数据。reason 详见Video_Conference_status；conferenceId 会议ID；member成员；type详见ECVideoDataType；
{
	if(g_Instance)g_Instance->onRequestConferenceMemberVideo(reason, conferenceId, member, type);
}
void ECSDK::sonCancelConferenceMemberVideo(int reason, const char *conferenceId, const char *member, int type)//视频会议时，取消会议成员视频数据响应。reason 详见Video_Conference_status；conferenceId 会议ID；tmember成员；ype详见ECVideoDataType；
{
	if(g_Instance)g_Instance->onCancelConferenceMemberVideo(reason, conferenceId, member, type);
}


const std::string & ECSDK::GetVersion()
{
	static std::string result;
	if (!result.empty())
		return result;

	result = "0.0.0.0";
	VS_FIXEDFILEINFO *pVerInfo = NULL;
	DWORD dwTemp, dwSize, dwHandle = 0;
	BYTE *pData = NULL;
	UINT uLen;
	dwSize = GetFileVersionInfoSizeW(utf16Path.c_str(), &dwTemp);

	pData = new BYTE[dwSize];

	GetFileVersionInfoW(utf16Path.c_str(), dwHandle, dwSize, pData);
	VerQueryValue(pData, "\\", (void **)&pVerInfo, &uLen);


	DWORD verMS = pVerInfo->dwFileVersionMS;
	DWORD verLS = pVerInfo->dwFileVersionLS;

	int ver[4];
	ver[0] = HIWORD(verMS);
	ver[1] = LOWORD(verMS);
	ver[2] = HIWORD(verLS);
	ver[3] = LOWORD(verLS);

	std::stringstream ss;
	ss << ver[0] << "." << ver[1] << "." << ver[2] << "." << ver[3];
	result = ss.str();
	//LOG4CPLUS_INFO(log, "result:" << result);

	delete[] pData;
	return result;
}

ECSDK::ECSDK()
{
	log = log4cplus::Logger::getInstance("ECSDK");
	LOG4CPLUS_INFO(log, "SDK:" << log4cplus::helpers::tostring(utf16Path));
	LOG4CPLUS_INFO(log, "SDK Version:" << GetVersion());

	LOG4CPLUS_TRACE(log, "construction");
}


ECSDK::~ECSDK()
{
	LOG4CPLUS_TRACE(log, "destruction");
}

int ECSDK::Initialize()
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__);

	g_Instance = this;
	GeneralCB.pfonLogInfo = sonLogInfo;
	GeneralCB.pfonLogOut = sonLogOut;
	GeneralCB.pfonConnectState = sonConnectState;
	GeneralCB.pfonSetPersonInfo = sonSetPersonInfo;
	GeneralCB.pfonGetPersonInfo = sonGetPersonInfo;
	GeneralCB.pfonGetUserState = sonGetUserState;
	GeneralCB.pfonPublishPresence = sonPublishPresence;
	GeneralCB.pfonReceiveFriendsPublishPresence = sonReceiveFriendsPublishPresence;
	GeneralCB.pfonSoftVersion = sonSoftVersion;
	GeneralCB.pfonGetOnlineMultiDevice = sonGetOnlineMultiDevice;
	GeneralCB.pfonReceiveMultiDeviceState = sonReceiveMultiDeviceState;

	SetGeneralCallBackFuction(&GeneralCB);
	LOG4CPLUS_TRACE(log, "SetGeneralCallBackFuction");

	VoipCB.pfnonCallEvents = sonCallEvents;
	VoipCB.pfonCallIncoming = sonCallIncoming;
	VoipCB.pfonDtmfReceived = sonDtmfReceived;
	VoipCB.pfonMakeCallBack = sonMakeCallBack;
	VoipCB.pfonRemoteVideoRatio = sonRemoteVideoRatio;
	VoipCB.pfonSwitchCallMediaTypeRequest = sonSwitchCallMediaTypeResponse;
	VoipCB.pfonSwitchCallMediaTypeResponse = sonSwitchCallMediaTypeResponse;

	SetVoIPCallBackFuction(&VoipCB);
	LOG4CPLUS_TRACE(log, "SetVoIPCallBackFuction");

	MessageCB.pfonFinishedPlaying = sonFinishedPlaying;
	MessageCB.pfonReceiveGroupNoticeMessage = sonReceiveGroupNoticeMessage;
	MessageCB.pfonRecordingAmplitude = sonRecordingAmplitude;
	MessageCB.pfonRecordingTimeOut = sonRecordingTimeOut;
	MessageCB.pfonCreateGroup = sonCreateGroup;
	MessageCB.pfonDeleteGroupMember = sonDeleteGroupMember;
	MessageCB.pfonDismissGroup = sonDismissGroup;
	MessageCB.pfonDownloadFileComplete = sonDownloadFileComplete;
	MessageCB.pfonForbidMemberSpeakGroup = sonForbidMemberSpeakGroup;
	MessageCB.pfonGetOfflineMessage = sonGetOfflineMessage;
	MessageCB.pfonInviteJoinGroup = sonInviteJoinGroup;
	MessageCB.pfonJoinGroup = sonJoinGroup;
	MessageCB.pfonModifyGroup = sonModifyGroup;
	MessageCB.pfonModifyGroupMemberCard = sonModifyGroupMemberCard;
	MessageCB.pfonOfflineMessageComplete = sonOfflineMessageComplete;
	MessageCB.pfonOfflineMessageCount = sonOfflineMessageCount;
	MessageCB.pfonQueryGroupDetail = sonQueryGroupDetail;
	MessageCB.pfonQueryGroupMember = sonQueryGroupMember;
	MessageCB.pfonQueryGroupMemberCard = sonQueryGroupMemberCard;
	MessageCB.pfonQueryOwnGroup = sonQueryOwnGroup;
	MessageCB.pfonQuitGroup = sonQuitGroup;
	MessageCB.pfonRateOfProgressAttach = sonRateOfProgressAttach;
	MessageCB.pfonReceiveFile = sonReceiveFile;
	MessageCB.pfonReceiveMessage = sonReceiveMessage;
	MessageCB.pfonReceiveOfflineMessage = sonReceiveOfflineMessage;
	MessageCB.pfonReplyInviteJoinGroup = sonReplyInviteJoinGroup;
	MessageCB.pfonReplyRequestJoinGroup = sonReplyRequestJoinGroup;
	MessageCB.pfonSearchPublicGroup = sonSearchPublicGroup;
	MessageCB.pfonSendMediaFile = sonSendMediaFile;
	MessageCB.pfonDeleteMessage = sonDeleteMessage;
	MessageCB.pfonOperateMessage = sonOperateMessage;
	MessageCB.pfonOnReceiveOperateNoticeMessage = sonReceiveOpreateNoticeMessage;
	MessageCB.pfonUploadVTMFileOrBuf = sonUploadVTMFileOrBuf;
	MessageCB.pfonSendTextMessage = sonSendTextMessage;
	MessageCB.pfonSetGroupMessageRule = sonSetGroupMessageRule;
    MessageCB.pfonSetGroupMemberRole = sonSetGroupMemberRole;

	SetMessageCallBackFunction(&MessageCB);
	LOG4CPLUS_TRACE(log, "SetMessageCallBackFunction");

	MeetingCB.pfonCancelConferenceMemberVideo = sonCancelConferenceMemberVideo;
	MeetingCB.pfonControlInterphoneMic = sonControlInterphoneMic;
	MeetingCB.pfonCreateInterphoneMeeting = sonCreateInterphoneMeeting;
	MeetingCB.pfonCreateMultimediaMeeting = sonCreateMultimediaMeeting;
	MeetingCB.pfonDeleteMemberMultiMediaMeeting = sonDeleteMemberMultiMediaMeeting;
	MeetingCB.pfonDismissMultiMediaMeeting = sonDismissMultiMediaMeeting;
	MeetingCB.pfonExitInterphoneMeeting = sonExitInterphoneMeeting;
	MeetingCB.pfonSetMeetingSpeakListen = sonSetMeetingSpeakListen;
	MeetingCB.pfonExitMeeting = sonExitMeeting;
	MeetingCB.pfonInviteJoinMultiMediaMeeting = sonInviteJoinMultiMediaMeeting;
	MeetingCB.pfonJoinMeeting = sonJoinMeeting;
	MeetingCB.pfonMeetingIncoming = sonMeetingIncoming;
	MeetingCB.pfonPublishVideo = sonPublishVideo;
	MeetingCB.pfonQueryMeetingMembers = sonQueryMeetingMembers;
	MeetingCB.pfonQueryMultiMediaMeetings = sonQueryMultiMediaMeetings;
	MeetingCB.pfonReceiveInterphoneMeetingMessage = sonReceiveInterphoneMeetingMessage;
	MeetingCB.pfonReceiveVideoMeetingMessage = sonReceiveVideoMeetingMessage;
	MeetingCB.pfonReceiveVoiceMeetingMessage = sonReceiveVoiceMeetingMessage;
	MeetingCB.pfonRequestConferenceMemberVideo = sonRequestConferenceMemberVideo;
	MeetingCB.pfonUnpublishVideo = sonUnpublishVideo;

	SetMeetingCallBackFuction(&MeetingCB);
	LOG4CPLUS_DEBUG(log, "SetMeetingCallBackFuction");

	int ret = ::ECSDK_Initialize();
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::UnInitialize()
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__);
	g_Instance = nullptr;
	int ret = ::ECSDK_UnInitialize();
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

void ECSDK::SetTraceFlag(int level, const char *logFileName)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", level:" << level << ", fileName:" << logFileName);
	return ::SetTraceFlag(level, logFileName);
}

int ECSDK::QueryErrorDescribe(const char** errCodeDescribeOut, int errCodeIn)
{
	int ret = ::QueryErrorDescribe(errCodeDescribeOut, errCodeIn);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", errCode:" << errCodeIn << ", description:" << ((*errCodeDescribeOut)?(*errCodeDescribeOut):""));
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::SetServerEnvType(int nEnvType, const char *ServerXmlFileName)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",EnvType:" << nEnvType << ", xmlFileName:" << ServerXmlFileName);
	int ret = ::SetServerEnvType(nEnvType, ServerXmlFileName);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::SetServerAddress(const char *protobuf_addr, int protobuf_port, const char *filehttp_addr, int filehttp_port)
{
    LOG4CPLUS_DEBUG(log, __FUNCTION__ ",Connecter " << protobuf_addr << ":" << protobuf_port
        << ", FileServer " << filehttp_addr << ":" << filehttp_port);
    int ret = ::SetServerAddress(protobuf_addr, protobuf_port, filehttp_addr, filehttp_port);
    LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
    return ret;
}


int ECSDK::SetInternalDNS(bool enable, const char *dns, int port)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", enable:" << enable << ", dns:" << dns << ",port:" << port);
	int ret = ::SetInternalDNS(enable, dns, port);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::Login(const ECLoginInfo *loginInfo)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__", authType:" << loginInfo->authType
		<< ", username:" << (loginInfo->username?loginInfo->username:"")
		<< ", userPassword:" << (loginInfo->userPassword?loginInfo->userPassword:"")
		<< ", appKey:" << (loginInfo->appKey?loginInfo->appKey:"")
		<< ", appToken:" << (loginInfo->appToken?loginInfo->appToken:"")
		<< ", timestamp:" << (loginInfo->timestamp?loginInfo->timestamp:"")
		<< ", MD5Token:" <<(loginInfo->MD5Token?loginInfo->MD5Token:""));

	/*
	this->onLogInfo("onloginfo");
	this->onLogOut(00000);

	ECConnectState cstate;
	cstate.code = ECConnectStateCode::State_Connecting;
	cstate.reason = 0;
	this->onConnectState(cstate);
	this->onSetPersonInfo(0, 0, 1001);

	ECPersonInfo pInfo;
	strncpy_s(pInfo.birth, "129212", sizeof(pInfo.birth)-1);
	strncpy_s(pInfo.nickName,"qiwei", sizeof(pInfo.nickName)-1);
	pInfo.sex = 1;
	strncpy_s(pInfo.sign,"sign",sizeof(pInfo.sign)-1);
	pInfo.version = 1001;
	this->onGetPersonInfo(1, 1, &pInfo);

	ECPersonState pstate;
	this->onGetUserState(2, 2, 1, &pstate);
	this->onPublishPresence(3, 3);

	ECPersonState personState[2];
	strncpy_s(personState[0].accound, "qiwei", sizeof(personState[0].accound)-1);
	personState[0].device = 4;
	personState[0].netType = 4;
	strncpy_s(personState[0].nickName, "qiwei", sizeof(personState[0].nickName)-1);
	personState[0].state = 4;
	personState[0].subState = 4;
	strncpy_s(personState[0].timestamp, "2016年3月18日14:14:57", sizeof(personState[0].timestamp)-1);
	strncpy_s(personState[0].userData, "qiwei", sizeof(personState[0].userData)-1);
	strncpy_s(personState[1].accound, "qiwei", sizeof(personState[1].accound)-1);
	personState[1].device = 5;
	personState[1].netType = 5;
	strncpy_s(personState[1].nickName, "qiwei", sizeof(personState[1].nickName)-1);
	personState[1].state = 5;
	personState[1].subState = 5;
	strncpy_s(personState[1].timestamp, "2016年3月18日14:14:57", sizeof(personState[1].timestamp)-1);
	strncpy_s(personState[1].userData, "qiwei", sizeof(personState[1].userData)-1);
	this->onReceiveFriendsPublishPresence(2, personState);

	this->onSoftVersion("qiwei", 6, "qiweiqiwei");

	ECMultiDeviceState mdState[2];
	mdState[0].device = 7;
	mdState[0].state = 7;
	mdState[1].device = 8;
	mdState[1].state = 8;
	this->onGetOnlineMultiDevice(7, 7, 2, mdState);

	mdState[0].device = 9;
	mdState[0].state = 9;
	mdState[1].device = 10;
	mdState[1].state = 10;
	this->sonReceiveMultiDeviceState(2, mdState);
	this->onGetOfflineMessage();
	*/
	int ret = ::Login(loginInfo);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::Logout()
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__);
	int ret = ::Logout();
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::SetPersonInfo(unsigned int *matchKey, ECPersonInfo* pInfo)
{
	int ret = ::SetPersonInfo(matchKey, pInfo);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << *matchKey
		<< ", version:" << pInfo->version
		<< ", sex:" << pInfo->sex
		<< ",nickName:" << pInfo->nickName
		<< ",birth:" << pInfo->birth
		<< ",sigh:" << pInfo->sign );
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::GetPersonInfo(unsigned int *matchKey, const char* username)
{
	int ret = ::GetPersonInfo(matchKey, username);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matchKey:" << *matchKey << ",username:" << username);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::GetUserState(unsigned int *matchKey, const char** members, int membercount)
{
	std::string strmember;
	for (int i = 0; i < membercount; i++){
		strmember.append((*members));
		strmember.append(";");
	}
	int ret = ::GetUserState(matchKey, members, membercount);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matchKey:" << *matchKey
		<< ",members:" << strmember);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::MakeCallBack(unsigned int *matchKey, const char *caller, const char *called, const char *callerDisplay, const char *calledDisplay)
{
	int ret = ::MakeCallBack(matchKey, caller, called, callerDisplay, calledDisplay);
	LOG4CPLUS_DEBUG(log, __FUNCTION__",matchKey:" << *matchKey
		<< ",caller:" << caller
		<< ",called:" << called
		<< ",callerDisplay:" << callerDisplay
		<< ",calledDisplay:" << calledDisplay);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::MakeCall(const char **OutCallid, int callType, const char *called)
{
	int ret = ::MakeCall(OutCallid, callType, called);
	LOG4CPLUS_DEBUG(log, __FUNCTION__",OutCallid:" << (*OutCallid ? *OutCallid:"")
		<< ",callType:" << callType
		<< ",called:" << called);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::AcceptCall(const char *callId)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" callid:" << callId);
	int ret = ::AcceptCall(callId);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::ReleaseCall(const char *callId, int reason)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" callid:" << callId << ", reason:" << reason);
	int ret = ::ReleaseCall(callId, reason);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::SendDTMF(const char *callId, const char dtmf)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" callid:" << callId << ", dtmf:" << dtmf);
	int ret = ::SendDTMF(callId, dtmf);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

const char* ECSDK::GetCurrentCall()
{
	const char * ret = ::GetCurrentCall();
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << (ret?ret:""));
	return ret;
}

int ECSDK::SetMute(bool mute)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" mute:" << mute);
	int ret = ::SetMute(mute);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

bool ECSDK::GetMuteStatus()
{
	bool ret = ::GetMuteStatus();
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::SetSoftMute(const char *callid, bool on)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__",callid:" << callid << ",on:" << on);
	int ret = ::SetSoftMute(callid, on);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::GetSoftMuteStatus(const char *callid, bool* bMute)
{
	int ret = ::GetSoftMuteStatus(callid, bMute);
	LOG4CPLUS_DEBUG(log, __FUNCTION__",callid:" << callid << ",bMute:" << *bMute);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::SetAudioConfigEnabled(int type, bool enabled, int mode)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" type:" << type << ",enabled:" << enabled << ",mode:" << mode);
	int ret = ::SetAudioConfigEnabled(type, enabled, mode);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::GetAudioConfigEnabled(int type, bool *enabled, int *mode)
{
	int ret = ::GetAudioConfigEnabled(type, enabled, mode);
	LOG4CPLUS_DEBUG(log, __FUNCTION__" type:" << type << ",enabled:" << *enabled << ",mode:" << *mode);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::SetCodecEnabled(int type, bool enabled)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" type:" << type << ",enabled:" << enabled);
	int ret = ::SetCodecEnabled(type, enabled);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

bool ECSDK::GetCodecEnabled(int type)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" type:" << type);
	bool ret = ::GetCodecEnabled(type);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::SetCodecNack(bool bAudioNack, bool bVideoNack)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" bAudioNack:" << bAudioNack << ",bVideoNack:" << bVideoNack);
	int ret = ::SetCodecEnabled(bAudioNack, bVideoNack);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::GetCodecNack(bool *bAudioNack, bool *bVideoNack)
{
	int ret = ::GetCodecNack(bAudioNack, bVideoNack);
	LOG4CPLUS_DEBUG(log, __FUNCTION__" bAudioNack:" << *bAudioNack << ",bVideoNack:" << *bVideoNack);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::GetCallStatistics(const char *callId, bool bVideo, ECMediaStatisticsInfo *statistics)
{
	int ret = ::GetCallStatistics(callId, bVideo, statistics);
	LOG4CPLUS_DEBUG(log, __FUNCTION__" callId:" << callId << ",bVideo:" << bVideo
		<< ",bytesReceived:" << statistics->bytesReceived
		<< ",bytesSent:" << statistics->bytesSent
		<< ",cumulativeLost:" << statistics->cumulativeLost
		<< ",extendedMax:" << statistics->extendedMax
		<< ",fractionLost:" << statistics->fractionLost
		<< ",jitterSamples:" << statistics->jitterSamples
		<< ",packetsReceived:" << statistics->packetsReceived
		<< ",packetsSent:" << statistics->packetsSent
		<< ",rttMs:" << statistics->rttMs);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::SetVideoView(void *view, void *localView)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" view:" << view << ",localView:" << localView);
	int ret = ::SetVideoView(view, localView);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

void ECSDK::SetVideoBitRates(int bitrates)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" bitrates:" << bitrates);
	return ::SetVideoBitRates(bitrates);
}

int ECSDK::SetRing(const char* fileName)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" fileName:" << fileName);
	int ret = ::SetRing(fileName);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::SetRingback(const char*fileName)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" fileName:" << fileName);
	int ret = ::SetRingback(fileName);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::RequestSwitchCallMediaType(const char *callId, int video)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" callId:" << callId << ",video:" << video);
	int ret = ::RequestSwitchCallMediaType(callId, video);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::ResponseSwitchCallMediaType(const char *callId, int video)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" callId:" << callId << ",video:" << video);
	int ret = ::ResponseSwitchCallMediaType(callId, video);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::GetCameraInfo(ECCameraInfo **  info)
{
	int ret = ::GetCameraInfo(info);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " CameraCount:" << ret);
	for (int nIndex = 0; nIndex < ret; nIndex++)
	{
		LOG4CPLUS_DEBUG(log, __FUNCTION__ ",index:" << (*info)[nIndex].index
			<< ",id:" <<(*info)[nIndex].id
			<< ",name:" <<(*info)[nIndex].name
			<< ",capabilityCount:" <<(*info)[nIndex].capabilityCount);
		
		ECCameraCapability * pCapability = (*info)[nIndex].capability;
		for (int CapabilityIndex = 0; CapabilityIndex < (*info)[nIndex].capabilityCount; CapabilityIndex++)
		{
			LOG4CPLUS_DEBUG(log, __FUNCTION__",width:" << pCapability[CapabilityIndex].width
				<<",height:" << pCapability[CapabilityIndex].height
				<<",maxfps:" << pCapability[CapabilityIndex].maxfps);

		}
	}
	return ret;
}

int ECSDK::SelectCamera(int cameraIndex, int capabilityIndex, int fps, int rotate, bool force)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" cameraIndex:" << cameraIndex << ",capabilityIndex:" << capabilityIndex
		<< ",fps:" << fps << ",rotate:" << rotate << ",force:" << force);
	int ret = ::SelectCamera(cameraIndex, capabilityIndex, fps, rotate, force);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::GetNetworkStatistic(const char *callId, long long *duration, long long *sendTotalWifi, long long *recvTotalWifi)
{
	int ret = ::GetNetworkStatistic(callId, duration, sendTotalWifi, recvTotalWifi);
	LOG4CPLUS_DEBUG(log, __FUNCTION__" callId:" << callId << ",duration:" << *duration
		<< ",sendTotalWifi:" << *sendTotalWifi << ",recvTotalWifi:" << *recvTotalWifi);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::GetSpeakerVolume(unsigned int* volume)
{
	int ret = ::GetSpeakerVolume(volume);
	LOG4CPLUS_DEBUG(log, __FUNCTION__" volume:" << volume);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::SetSpeakerVolume(unsigned int volume)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" volume:" << volume);
	int ret = ::SetSpeakerVolume(volume);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

void ECSDK::SetDtxEnabled(bool enabled)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" enabled:" << enabled);
	return ::SetDtxEnabled(enabled);
}

void ECSDK::SetSelfPhoneNumber(const char * phoneNumber)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" phoneNumber:" << phoneNumber);
	return ::SetSelfPhoneNumber(phoneNumber);
}

void ECSDK::SetSelfName(const char * nickName)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" nickName:" << nickName);
	return ::SetSelfName(nickName);
}

int ECSDK::EnableLoudsSpeaker(bool enable)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" enable:" << enable);
	int ret = ::EnableLoudsSpeaker(enable);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

bool ECSDK::GetLoudsSpeakerStatus()
{
	bool ret = ::GetLoudsSpeakerStatus();
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::ResetVideoView(const char *callid, void* remoteView, void *localView)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" callid:" << callid
		<<",remoteView:" << remoteView
		<<",localView:" << localView);

	int ret = ::ResetVideoView(callid, remoteView, localView);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::GetLocalVideoSnapshotEx(const char* callid, unsigned char **buf, unsigned int *size, unsigned int *width, unsigned int *height)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" callid:" << callid);
	int ret = ::GetLocalVideoSnapshotEx(callid, buf, size, width, height);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",size:" << *size << ",width:" << *width << ",height:" << *height);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::GetRemoteVideoSnapshotEx(const char* callid, unsigned char **buf, unsigned int *size, unsigned int *width, unsigned int *height)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__" callid:" << callid);
	int ret = ::GetRemoteVideoSnapshotEx(callid, buf, size, width, height);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",size:" << *size << ",width:" << *width << ",height:" << *height);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::StartRecordScreen(const char *callid, const char *filename, int bitrate, int fps, int type)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__", callid:" << callid << ",filename:" << filename << ",bitrate:" << bitrate 
		<< ",fps:" << fps << ",type:" << type);

	int ret = ::StartRecordScreen(callid, filename, bitrate, fps, type);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::StartRecordScreenEx(const char *callid, const char *filename, int bitrate, int fps, int type, int left, int top, int width, int height)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__", callid:" << callid << ",filename:" << filename << ",bitrate:" << bitrate
		<< ",fps:" << fps << ",type:" << type << ",letf:" << left << ",top:" << top
		<< ",width:" << width << ",height:" << height);

	int ret = ::StartRecordScreenEx(callid, filename, bitrate, fps, type, left, top, width, height);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::StopRecordScreen(const char *callid)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__", callid:" << callid );

	int ret = ::StopRecordScreen(callid);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::SendTextMessage(unsigned int *matchKey, const char *receiver, const char *message, MsgType type, const char *userdata, char * msgId)
{
	int ret = ::SendTextMessage(matchKey, receiver, message, type, userdata, msgId);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << (matchKey ? *matchKey : -1)
		<< ", receiver:" << receiver
		<< ", message:" << message
		<< ",type:" << type
		<< ",userdata:" << (userdata ? userdata : "")
		<< ",msgId:" << msgId);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::SendMediaMessage(unsigned int *matchKey, const char* receiver, const char* fileName, MsgType type, const char* userdata, char * msgId)
{
	int ret = ::SendMediaMessage(matchKey, receiver, fileName, type, userdata, msgId);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << *matchKey
		<< ", receiver:" << receiver
		<< ", fileName:" << fileName
		<< ",type:" << type
		<< ",userdata:" << userdata
		<< ",msgId:" << msgId);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::DownloadFileMessage(unsigned int *matchKey, const char* sender, const char* recv, const char* url, const char* fileName, const char* msgId, MsgType type)
{
	int ret = ::DownloadFileMessage(matchKey, sender, recv, url, fileName, msgId, type);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << *matchKey
		<< ", sender:" << sender
		<< ", recv:" << recv
		<< ",url:" << url
		<< ",fileName:" << fileName
		<< ",msgId:" << msgId
		<< ",type:" << type);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
void ECSDK::CancelUploadOrDownloadNOW(unsigned int matchKey)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__",matchKey:" << matchKey);
	return ::CancelUploadOrDownloadNOW(matchKey);
}

int ECSDK::DeleteMessage(unsigned int *matchKey, int type, const char* msgId)
{
	int ret = ::DeleteMessage(matchKey, type, msgId);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << *matchKey
		<< ",type:" << type
		<< ",msgId:" << msgId);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::OperateMessage(unsigned int* matchKey, const char* version, const char* messageId, int type)
{
	int ret = ::OperateMessage(matchKey, version, messageId, type);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << *matchKey
		<< ",version:" << (version ? version : "")
		<< ",messageId:" << (messageId ? messageId : "")
		<< ",type:" << type);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::CreateGroup(unsigned int *matchKey, const char*groupName, int type, const char* province, const char* city, int scope, const char* declared, int permission, int isDismiss, const char* groupDomain, bool isDiscuss)
{
	int ret = ::CreateGroup(matchKey, groupName, type, province, city,scope,declared,permission,isDismiss,groupDomain,isDiscuss);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << *matchKey
		<< ", groupName:" << groupName
		<< ",type:" << type
		<< ",province:" << province
		<< ",city:" << city
		<< ",scope:" << scope
		<< ",declared:" << declared
		<< ",permission:" << permission
		<< ",isDismiss:" << isDismiss
		<< ",groupDomain:" << groupDomain
		<< ",isDiscuss:" << isDiscuss);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::DismissGroup(unsigned int *matchKey, const char* groupid)
{
	int ret = ::DismissGroup(matchKey, groupid);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << *matchKey
		<< ", groupid:" << groupid);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::QuitGroup(unsigned int *matchKey, const char* groupid)
{
	int ret = ::QuitGroup(matchKey, groupid);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << *matchKey
		<< ", groupid:" << groupid);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::JoinGroup(unsigned int *matchKey, const char* groupid, const char* declared)
{
	int ret = ::JoinGroup(matchKey, groupid,declared);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << *matchKey
		<< ", groupid:" << groupid
		<< ",declared:" << declared);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::ReplyRequestJoinGroup(unsigned int *matchKey, const char* groupid, const char* member, int confirm)
{
	int ret = ::ReplyRequestJoinGroup(matchKey, groupid, member, confirm);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << *matchKey
		<< ", groupid:" << groupid
		<< ",member:" << member
		<< ",confirm:" << confirm);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::InviteJoinGroup(unsigned int *matchKey, const char* groupId, const char* declard, const char** members, int membercount, int confirm)
{
	int ret = ::InviteJoinGroup(matchKey, groupId, declard, members, membercount, confirm);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << *matchKey
		<< ", groupid:" << groupId
		<< ",membercount:" << membercount
		<< ",confirm:" << confirm);
	for (int i = 0; i < membercount; i++)
	{
		LOG4CPLUS_DEBUG(log, __FUNCTION__",member:" << members[i]);
	}

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::ReplyInviteJoinGroup(unsigned int *matchKey, const char* groupid, const char* invitor, int confirm)
{
	int ret = ::ReplyInviteJoinGroup(matchKey, groupid, invitor, confirm);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << *matchKey
		<< ", groupid:" << groupid
		<< ",invitor:" << invitor
		<< ",confirm:" << confirm);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::QueryOwnGroup(unsigned int *matchKey, const char* borderGroupid, int pageSize, int target)
{
	int ret = ::QueryOwnGroup(matchKey, borderGroupid, pageSize, target);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << *matchKey
		<< ", borderGroupid:" << borderGroupid
		<< ",pageSize:" << pageSize
		<< ",target:" << target);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::QueryGroupDetail(unsigned int *matchKey, const char* groupId)
{
	int ret = ::QueryGroupDetail(matchKey, groupId);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << *matchKey
		<< ", groupid:" << groupId);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::ModifyGroup(unsigned int *matchKey, ECModifyGroupInfo* pInfo)
{
	int ret = ::ModifyGroup(matchKey, pInfo);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey
		<< ",city:" << pInfo->city
		<< ",declared:" << pInfo->declared
		<< ",discuss:" << pInfo->discuss
		<< ",groupDomain:" << pInfo->groupDomain
		<< ",groupId:" << pInfo->groupId
		<< ",name:" << pInfo->name
		<< ",permission:" << pInfo->permission
		<< ",province:" << pInfo->province
		<< ",scope:" << pInfo->scope
		<< ",type:" << pInfo->type);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::SearchPublicGroups(unsigned int *matchKey, int searchType, const char* keyword,int pageNo,int pageSize)
{
	int ret = ::SearchPublicGroups(matchKey, searchType, keyword);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << *matchKey
		<< ", searchType:" << searchType
		<< ",keyword:" << keyword
		<< ",pageNo:" << pageNo
		<< ",pageSize:"<< pageSize);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::QueryGroupMember(unsigned int *matchKey, const char* groupId, const char* borderMember, int pageSize)
{
	int ret = ::QueryGroupMember(matchKey, groupId, borderMember, pageSize);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << *matchKey
		<< ", groupId:" << groupId
		<< ",borderMember:" << borderMember
		<< ",pageSize:" << pageSize);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::DeleteGroupMember(unsigned int *matchKey, const char* groupid, const char* member)
{
	int ret = ::DeleteGroupMember(matchKey, groupid, member);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << *matchKey
		<< ", groupId:" << groupid
		<< ",member:" << member);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::QueryGroupMemberCard(unsigned int *matchKey, const char* groupid, const char* member)
{
	int ret = ::QueryGroupMemberCard(matchKey, groupid, member);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << *matchKey
		<< ", groupId:" << groupid
		<< ",member:" << member);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::ModifyMemberCard(unsigned int *matchKey, ECGroupMemberCard *card)
{
	int ret = ::ModifyMemberCard(matchKey, card);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey
		<< ",display:" << card->display
		<< ",groupId:" << card->groupId
		<< ",mail:" << card->mail
		<< ",member:" << card->member
		<< ",phone:" << card->phone
		<< ",remark:" << card->remark
		<< ",role:" << card->role
		<< ",sex:" << card->sex
		<< ",speakState:" << card->speakState);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::ForbidMemberSpeak(unsigned int *matchKey, const char* groupid, const char* member, int isBan)
{
	int ret = ::ForbidMemberSpeak(matchKey, groupid, member, isBan);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << *matchKey
		<< ", groupId:" << groupid
		<< ",member:" << member
		<< ",isBan:" << isBan);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::SetGroupMessageRule(unsigned int *matchKey, const char* groupid, bool notice)
{
	int ret = ::SetGroupMessageRule(matchKey, groupid, notice);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << *matchKey
		<< ", groupId:" << groupid
		<< ",notice:" << notice);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::StartVoiceRecording(const char* fileName, const char* sessionId)
{
	int ret = ::StartVoiceRecording(fileName, sessionId);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", fileName:" << fileName
		<< ", sessionId:" << sessionId);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
void ECSDK::StopVoiceRecording()
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__);
	::StopVoiceRecording();
}
int ECSDK::PlayVoiceMsg(const char* fileName, const char* sessionId)
{
	int ret = ::PlayVoiceMsg(fileName, sessionId);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", fileName:" << fileName
		<< ", sessionId:" << sessionId);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
void ECSDK::StopVoiceMsg()
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__);
	::StopVoiceMsg();
}
int ECSDK::GetVoiceDuration(const char* fileName, int *duration)
{
	int ret = ::GetVoiceDuration(fileName, duration);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", fileName:" << fileName
		<< ", duration:" << *duration);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::UploadVTMFile(unsigned int* matchKey, const char* companyId, const char* companyPwd, const char* fileName, const char* callbackUrl)
{
	
	int ret = ::UploadVTMFile(matchKey, companyId, companyPwd, fileName, callbackUrl);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << *matchKey
		<< ", companyId:" << (companyId ? companyId : "")
		<< ",companyPwd:" << (companyPwd ? companyPwd : "")
		<< ",fileName:" << (fileName?fileName:"")
		<< ",callbackUrl:" << (callbackUrl?callbackUrl:""));

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::UploadVTMBuf(unsigned int* matchKey, const char* companyId, const char* companyPwd, const char* Name, const char* buf, unsigned int size, const char* callbackUrl)
{
	int ret = ::UploadVTMBuf(matchKey, companyId, companyPwd, Name, (unsigned char *)buf, size, callbackUrl);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", matchKey:" << *matchKey
		<< ", companyId:" << (companyId ? companyId : "")
		<< ",companyPwd:" << (companyPwd ? companyPwd : "")
		<< ",Name:" << (Name ? Name : "")
		<< ",buf:" << std::string(buf, size)
		<< ",callbackUrl:" << (callbackUrl ? callbackUrl : ""));

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}

int ECSDK::RequestConferenceMemberVideo(const char *conferenceId, const char *conferencePasswd, const char *member, void *videoWindow, int type)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", conferenceId:" << conferenceId
		<< ", conferencePasswd:" << conferencePasswd
		<< ",member:" << member
		<< ",videoWindow:" << videoWindow
		<< ",type:" <<type);

	int ret = ::RequestConferenceMemberVideo(conferenceId, conferencePasswd,member,videoWindow,type);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::CancelMemberVideo(const char *conferenceId, const char *conferencePasswd, const char *member, int type)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", conferenceId:" << conferenceId
		<< ", conferencePasswd:" << conferencePasswd
		<< ",member:" << member
		<< ",type:" << type);

	int ret = ::CancelMemberVideo(conferenceId, conferencePasswd, member, type);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::ResetVideoConfWindow(const char *conferenceId, const char *member, void *newWindow, int type)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", conferenceId:" << conferenceId
		<< ",member:" << member
		<< ",newWindow:" << newWindow
		<< ",type:" << type);

	int ret = ::ResetVideoConfWindow(conferenceId, member, newWindow, type);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::CreateMultimediaMeeting(unsigned int *matchKey, int  meetingType, const char* meetingName, const char* password,
	const char* keywords, int voiceMode, int square, bool bAutoJoin, bool autoClose, bool autoDelete)
{
	int ret = ::CreateMultimediaMeeting(matchKey, meetingType, meetingName, password, keywords, voiceMode, square, bAutoJoin, autoClose, autoDelete);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matchKey:" << *matchKey
		<< ", meetingType:" << meetingType
		<< ", meetingName:" << meetingName
		<< ",password:" << password
		<< ",keywords:" << keywords
		<< ",voiceMode:" << voiceMode
		<< ",square:" << square
		<< ",bAutoJoin:" << bAutoJoin
		<< ",autoClose:" << autoClose
		<< ",autoDelete:" << autoDelete);


	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::JoinMeeting(int  meetingType, const char* conferenceId, const char* password)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",meetingType:" << meetingType
		<< ", conferenceId:" << conferenceId
		<< ", password:" << password);

	int ret = ::JoinMeeting(meetingType, conferenceId, password);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::ExitMeeting(const char* conferenceId)
{
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", conferenceId:" << conferenceId);

	int ret = ::ExitMeeting(conferenceId);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::QueryMeetingMembers(unsigned int *matchKey, int meetingType, const char* conferenceId)
{
	int ret = ::QueryMeetingMembers(matchKey, meetingType, conferenceId);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matchKey:" << *matchKey
		<<",meetingType:" << meetingType
		<< ", conferenceId:" << conferenceId);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);

	return ret;
}
int ECSDK::DismissMultiMediaMeeting(unsigned int *matchKey, int meetingType, const char* conferenceId)
{
	int ret = ::DismissMultiMediaMeeting(matchKey, meetingType, conferenceId);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matchKey:" << *matchKey
		<< ",meetingType:" << meetingType
		<< ", conferenceId:" << conferenceId);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);

	return ret;
}
int ECSDK::QueryMultiMediaMeetings(unsigned int *matchKey, int meetingType, const char* keywords)
{
	int ret = ::QueryMultiMediaMeetings(matchKey, meetingType, keywords);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matchKey:" << *matchKey
		<< ",meetingType:" << meetingType
		<< ", keywords:" << keywords);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);

	return ret;
}
int ECSDK::InviteJoinMultiMediaMeeting(unsigned int *matchKey, const char* conferenceId, const char** members, int membercount, bool blanding,
	bool isSpeak, bool isListen, const char* disNumber, const char* userdata)
{
	int ret = ::InviteJoinMultiMediaMeeting(matchKey, conferenceId, members, membercount, blanding, isSpeak, isListen, disNumber, userdata);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matchKey:" << *matchKey
		<< ",conferenceId:" << conferenceId
		<< ", membercount:" << membercount
		<<",blanding:" << blanding
		<< ",isSpeak:" << isSpeak
		<< ",isListen:" << isListen
		<< ",disNumber:" << disNumber
		<<",userdata:" << userdata);

	for (int i = 0; i < membercount; i++)
	{
		LOG4CPLUS_DEBUG(log, __FUNCTION__".member:" << members[i]);
	}

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret ;
}
int ECSDK::DeleteMemberMultiMediaMeeting(unsigned int *matchKey, int meetingType, const char* conferenceId, const char* member, bool isVoIP)
{
	int ret = ::DeleteMemberMultiMediaMeeting(matchKey, meetingType, conferenceId, member, isVoIP);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matchKey:" << *matchKey
		<< ", meetingType:" << meetingType
		<< ",conferenceId:" << conferenceId
		<< ",member:" << member
		<< ",isVoIP:" << isVoIP);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::PublishVideo(unsigned int *matchKey, const char* conferenceId)
{
	int ret = ::PublishVideo(matchKey, conferenceId);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matchKey:" << *matchKey
		<< ",conferenceId:" << conferenceId);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::UnpublishVideo(unsigned int *matchKey, const char* conferenceId)
{
	int ret = ::UnpublishVideo(matchKey, conferenceId);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matchKey:" << *matchKey
		<< ",conferenceId:" << conferenceId);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::CreateInterphoneMeeting(unsigned int *matchKey, const char** members, int membercount)
{
	int ret = ::CreateInterphoneMeeting(matchKey, members, membercount);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matchKey:" << *matchKey
		<< ",membercount:" << membercount);

	for (int i = 0; i < membercount; i++)
	{
		LOG4CPLUS_DEBUG(log, __FUNCTION__".member:" << members[i]);
	}

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}
int ECSDK::ControlInterphoneMic(unsigned int *matchKey, bool isControl, const char* interphoneId)
{
	int ret = ::ControlInterphoneMic(matchKey, isControl, interphoneId);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matchKey:" << *matchKey
		<< ",isControl:" << isControl
		<< ",interphoneId:" << interphoneId);

	LOG4CPLUS_DEBUG(log, __FUNCTION__ " result:" << ret);
	return ret;
}