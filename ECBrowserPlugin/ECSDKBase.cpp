#include "stdafx.h"
#include "ECSDKBase.h"
#include <log4cplus/loggingmacros.h>
#include "codingHelper.h"
#include <json.h>

extern std::wstring utf16Path;
extern std::string utf8AppDataDir;

ECSDKBase::ECSDKBase() :m_hShareWnd(nullptr), m_utf8AppDataDir(utf8AppDataDir)
{
	log = log4cplus::Logger::getInstance("ECSDKBase");
	LOG4CPLUS_TRACE(log, "construction");
}


ECSDKBase::~ECSDKBase()
{
	LOG4CPLUS_TRACE(log, "destruction");
}

void ECSDKBase::onLogInfo(const char* loginfo) // 用于接收底层的log信息,调试出现的问题.
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	std::string * data = new std::string(loginfo);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ << ":" << *data);
	PostMessage(gethWnd(), WM_onLoginfo, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onLogOut(int reason)//登出回调。reason：200成功，其他报错；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ << ":" << reason);
	PostMessage(gethWnd(), WM_onLogOut, NULL, reason);
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onConnectState(const ECConnectState state)//连接回调；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	ECConnectState * data = new ECConnectState(state);
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ", code:" << data->code << ", reason:" << data->reason);
	PostMessage(gethWnd(), WM_onConnectState, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onSetPersonInfo(unsigned int matchKey, int reason, unsigned long long version)//设置个人资料。reason：200成功，其他报错；version:个人资料版本号
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	EConSetPersonInfo * data = new EConSetPersonInfo();
	data->matchKey = matchKey;
	data->reason = reason;
	data->version = version;
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matchKey:" << matchKey << ",reason:" << reason << ",version:" << version);
	PostMessage(gethWnd(), WM_onSetPersonInfo, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onGetPersonInfo(unsigned int matchKey, int reason, ECPersonInfo *pPersonInfo)//获取个人资料。reason：200成功，其他报错；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	EConGetPersonInfo * data = new EConGetPersonInfo();
	data->matchKey = matchKey;
	data->reason = reason;
	data->PersonInfo = *pPersonInfo;
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matchKey:" << matchKey << ", reason:" << reason
		<< ",birth:" << data->PersonInfo.birth
		<< ",nickName:" << data->PersonInfo.nickName
		<< ",sex:" << data->PersonInfo.sex
		<< ",sign:" << data->PersonInfo.sign
		<< ",version:" << data->PersonInfo.version);
	PostMessage(gethWnd(), WM_onGetPersonInfo, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onGetUserState(unsigned int matchKey, int reason, int count, ECPersonState *pStateArr)//获取用户状态，reason：200成功，其他报错;count 数组个数，pStateArr 用户状态数组首指针
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	EConGetUserState * data = new EConGetUserState();
	data->matchKey = matchKey;
	data->reason = reason;

	LOG4CPLUS_DEBUG(log, __FUNCTION__",matchKey:" << matchKey << ",reason:" << reason << ",count:" << count);
	for (int i = 0; i < count; i++)
	{
		ECPersonState * State = pStateArr + i;
		LOG4CPLUS_DEBUG(log,  ",accound:" << State->accound
			<< ",device:" << State->device
			<< ",netType:" << State->netType
			<< ",nickName:" << State->nickName
			<< ",state:" << State->state
			<< ",subState:" << State->subState
			<< ",timestamp:" << State->timestamp
			<< ",userData:" << State->userData);
		data->States.push_back(*State);
	}

	PostMessage(gethWnd(), WM_onGetUserState, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onPublishPresence(unsigned int matchKey, int reason)//发布用户状态，reason：200成功，其他报错
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__",matchKey:" << matchKey << ",reason:" << reason);
	PostMessage(gethWnd(), WM_onPublishPresence, matchKey, reason);
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onReceiveFriendsPublishPresence(int count, ECPersonState *pStateArr)//收到服务器推送的用户状态变化,count 数组个数，pStateArr 用户状态数组首指针
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	std::vector<ECPersonState> * data = new std::vector<ECPersonState>();
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",count:" << count);

	for (int i = 0; i < count; i++){
		ECPersonState * state = pStateArr + i;
		LOG4CPLUS_DEBUG(log, ",accound:" << state->accound
			<< ",device:" << state->device
			<< ",netType:" << state->netType
			<< ",nickName:" << state->nickName
			<< ",state:" << state->state
			<< ",subState:" << state->subState
			<< ",timestamp:" << state->timestamp
			<< ",userData:" << state->userData);
		data->push_back(*state);
	}
	PostMessage(gethWnd(), WM_onReceiveFriendsPublishPresence, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onSoftVersion(const char* softVersion, int updateMode, const char* updateDesc)//应用软件最新版本号升级提示，供应用自己升级用。softVersion：软件最新版本号；updateMode:更新模式  1：手动更新 2：强制更新；updateDesc：软件更新说明 
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	EConSoftVersion * data = new EConSoftVersion();
	data->softVersion = softVersion;
	data->updateMode = updateMode;
	data->updateDesc = updateDesc;
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",softVersion:" << softVersion << ",updateMode:" << updateMode << ",updateDesc:" << updateDesc);
	PostMessage(gethWnd(), WM_onSoftVersion, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onGetOnlineMultiDevice(unsigned int matchKey, int reason, int count, ECMultiDeviceState *pMultiDeviceStateArr)//获得登录者多设备登录状态变化回调,reason：200成功，其他报错count 数组个数，pMultiDeviceStateArr 多设备登录状态数组首指针
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	EConGetOnlineMultiDevice * data = new EConGetOnlineMultiDevice();
	data->matchKey = matchKey;
	data->reason = reason;

	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matchKey:" << matchKey << ",reason:" << reason << ",count:" << count);
	for (int i = 0; i < count;i++)
	{
		ECMultiDeviceState * state = pMultiDeviceStateArr + i;
		LOG4CPLUS_DEBUG(log, ",device:" << state->device
			<< ",state:" << state->state);
		data->MultiDeviceStates.push_back(*state);
	}
	PostMessage(gethWnd(), WM_onGetOnlineMultiDevice, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onReceiveMultiDeviceState(int count, ECMultiDeviceState *pMultiDeviceStateArr)//收到服务器推送的登录者多设备登录状态变化,count 数组个数，pMultiDeviceStateArr 多设备登录状态数组首指针
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	std::vector<ECMultiDeviceState> * data = new std::vector<ECMultiDeviceState>();
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",count:" << count);
	for (int i = 0; i < count;i++)
	{
		ECMultiDeviceState * state = pMultiDeviceStateArr + i;
		LOG4CPLUS_DEBUG(log, ",device:" << state->device
			<<",state:" << state->state);
		data->push_back(*state);
	}

	PostMessage(gethWnd(), WM_onReceiveMultiDeviceState, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onMakeCallBack(unsigned int matchKey, int reason, ECCallBackInfo *pInfo)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	EConMakeCallBack * data = new EConMakeCallBack();
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matchKey:" << matchKey << ",reason:" << reason 
		<< ",called:" << pInfo->called
		<< ",calledDisplay:" << pInfo->calledDisplay
		<< ",caller:" << pInfo->caller
		<< ",callerDisplay:" << pInfo->callerDisplay);

	data->matchKey = matchKey;
	data->reason = reason;
	data->CallBackInfo = *pInfo;
	PostMessage(gethWnd(), WM_onMakeCallBack, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onCallEvents(int reason, const ECVoIPCallMsg * call)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",reason:" << reason
		<< ",callee:" << call->callee
		<< ",caller:" << call->caller
		<< ",callid:" << call->callid
		<< ",callType:" << call->callType
		<< ",eCode:" << call->eCode);

	ECVoIPCallMsg * data = new ECVoIPCallMsg(*call);
	PostMessage(gethWnd(), WM_onCallEvents, reason, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onCallIncoming(ECVoIPComingInfo *pComing)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",caller:" << pComing->caller
		<< ",callid:" << pComing->callid
		<< ",callType:" << pComing->callType
		<< ",display:" << pComing->display
		<< ",nickname:" << pComing->nickname);

	ECVoIPComingInfo * data = new ECVoIPComingInfo(*pComing);
	PostMessage(gethWnd(),WM_onCallIncoming,NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onDtmfReceived(const char *callId, char dtmf)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__",callId:" << callId << ",dtmf:" << dtmf);
	std::string *strCallId = new std::string(callId);
	PostMessage(gethWnd(), WM_onDtmfReceived, reinterpret_cast<WPARAM>(strCallId), dtmf);
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onSwitchCallMediaTypeRequest(const char*callId, int video)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__",callId:" << callId << ",video:" << video);
	std::string *strCallId = new std::string(callId);
	PostMessage(gethWnd(), WM_onSwitchCallMediaTypeRequest, reinterpret_cast<WPARAM>(strCallId), video);
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onSwitchCallMediaTypeResponse(const char*callId, int video)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__",callId:" << callId << ",video:" << video);
	std::string *strCallId = new std::string(callId);
	PostMessage(gethWnd(), WM_onSwitchCallMediaTypeResponse, reinterpret_cast<WPARAM>(strCallId), video);
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onRemoteVideoRatio(const char *CallidOrConferenceId, int width, int height, int type, const char *member)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__",CallidOrConferenceId:" << CallidOrConferenceId
		<< ",width:" << width
		<< ",height:" << height
		<< ",type:" << type
		<< ",memeber:" << member);

	EConRemoteVideoRatio * data = new EConRemoteVideoRatio();
	data->CallidOrConferenceId = CallidOrConferenceId;
	data->width = width;
	data->height = height;
	data->type = type;
	data->member = member;
	PostMessage(gethWnd(), WM_onRemoteVideoRatio, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

/****************************消息处理接口的回调******************************/
void ECSDKBase::onOfflineMessageCount(int count)//离线条数；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__":" << count);
	PostMessage(gethWnd(), WM_onOfflineMessageCount, NULL, count);
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

int  ECSDKBase::onGetOfflineMessage()//离线拉取条数。<0 全拉；0 不拉；大于0，只拉取最新的条数，
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	std::unique_lock<std::mutex> lck(m_onGetOfflineMessage_mtx);
	PostMessage(gethWnd(), WM_onGetOfflineMessage, NULL, NULL);

	m_onGetOfflineMessage_cv.wait(lck);
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end, result:" << m_onGetOfflineMessage_Result);
	return m_onGetOfflineMessage_Result;
}

void ECSDKBase::onOfflineMessageComplete()//拉取离线成功
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	PostMessage(gethWnd(), WM_onOfflineMessageComplete, NULL, NULL);
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onReceiveOfflineMessage(ECMessage *pMsg)//离线消息回调
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__",dateCreated:" << pMsg->dateCreated
		<< ",fileName:" << pMsg->fileName
		<< ",fileSize:" << pMsg->fileSize
		<< ",fileUrl:" << pMsg->fileUrl
		<< ",msgContent:" << pMsg->msgContent
		<< ",msgId:" << pMsg->msgId
		<< ",msgType:" << pMsg->msgType
		<< ",offset:" << pMsg->offset
		<< ",receiver:" << pMsg->receiver
		<< ",sender:" << pMsg->sender
		<< ",senderNickName:" << pMsg->senderNickName
		<< ",sessionId:" << pMsg->sessionId
		<< ",sessionType:" << pMsg->sessionType
		<< ",userData:" << pMsg->userData
		<< ",isAt:" << pMsg->isAt
		<< ",type:" << pMsg->type);

	ECMessage * data = new ECMessage(*pMsg);
	PostMessage(gethWnd(), WM_onReceiveOfflineMessage, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onReceiveMessage(ECMessage *pMsg)//实时消息回调
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__",dateCreated:" << pMsg->dateCreated
		<< ",fileName:" << pMsg->fileName
		<< ",fileSize:" << pMsg->fileSize
		<< ",fileUrl:" << pMsg->fileUrl
		<< ",msgContent:" << pMsg->msgContent
		<< ",msgId:" << pMsg->msgId
		<< ",msgType:" << pMsg->msgType
		<< ",offset:" << pMsg->offset
		<< ",receiver:" << pMsg->receiver
		<< ",sender:" << pMsg->sender
		<< ",senderNickName:" << pMsg->senderNickName
		<< ",sessionId:" << pMsg->sessionId
		<< ",sessionType:" << pMsg->sessionType
		<< ",userData:" << pMsg->userData
		<< ",isAt:" << pMsg->isAt
		<< ",type:" << pMsg->type);

	ECMessage * data = new ECMessage(*pMsg);
	PostMessage(gethWnd(), WM_onReceiveMessage, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onReceiveFile(ECMessage* pFileInfo)//用户收到发送者发送过来的文件等待用户决定是否下载文件的回调。该回调消息的消息类型(MsgType)默认是6
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__",dateCreated:" << pFileInfo->dateCreated
		<< ",fileName:" << pFileInfo->fileName
		<< ",fileSize:" << pFileInfo->fileSize
		<< ",fileUrl:" << pFileInfo->fileUrl
		<< ",msgContent:" << pFileInfo->msgContent
		<< ",msgId:" << pFileInfo->msgId
		<< ",msgType:" << pFileInfo->msgType
		<< ",offset:" << pFileInfo->offset
		<< ",receiver:" << pFileInfo->receiver
		<< ",sender:" << pFileInfo->sender
		<< ",senderNickName:" << pFileInfo->senderNickName
		<< ",sessionId:" << pFileInfo->sessionId
		<< ",sessionType:" << pFileInfo->sessionType
		<< ",userData:" << pFileInfo->userData
		<< ",isAt:" << pFileInfo->isAt
		<< ",type:" << pFileInfo->type);

	ECMessage * data = new ECMessage(*pFileInfo);
	PostMessage(gethWnd(), WM_onReceiveFile, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onDownloadFileComplete(unsigned int matchKey, int reason, ECMessage *msg)//下载文件回调，下载完成后被调用（下载失败也会被调）。reason:0成功，其他报错；offset 内容大小偏移量
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",reason:" << reason
		<< ",dateCreated:" << msg->dateCreated
		<< ",fileName:" << msg->fileName
		<< ",fileSize:" << msg->fileSize
		<< ",fileUrl:" << msg->fileUrl
		<< ",msgContent:" << msg->msgContent
		<< ",msgId:" << msg->msgId
		<< ",msgType:" << msg->msgType
		<< ",offset:" << msg->offset
		<< ",receiver:" << msg->receiver
		<< ",sender:" << msg->sender
		<< ",senderNickName:" << msg->senderNickName
		<< ",sessionId:" << msg->sessionId
		<< ",sessionType:" << msg->sessionType
		<< ",userData:" << msg->userData
		<< ",isAt:" << msg->isAt
		<< ",type:" << msg->type);

	EConDownloadFileComplete * data = new EConDownloadFileComplete();
	data->matchKey = matchKey;
	data->reason = reason;
	data->msg = *msg;

	//由于msg.fileSize 值为0，暂时使用msg.offset值。
	if (data->msg.fileSize == 0) data->msg.fileSize = data->msg.offset;

	PostMessage(gethWnd(), WM_onDownloadFileComplete, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onRateOfProgressAttach(unsigned int matchKey, unsigned long long rateSize, unsigned long long fileSize, ECMessage *msg)//上传下载文件进度。ratesize 已经完成进度大小（字节）；filezise 总文件大小（字节）
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",rateSize:" << rateSize << ",fileSize:" << fileSize
		<< ",dateCreated:" << msg->dateCreated
		<< ",fileName:" << msg->fileName
		<< ",fileSize:" << msg->fileSize
		<< ",fileUrl:" << msg->fileUrl
		<< ",msgContent:" << msg->msgContent
		<< ",msgId:" << msg->msgId
		<< ",msgType:" << msg->msgType
		<< ",offset:" << msg->offset
		<< ",receiver:" << msg->receiver
		<< ",sender:" << msg->sender
		<< ",senderNickName:" << msg->senderNickName
		<< ",sessionId:" << msg->sessionId
		<< ",sessionType:" << msg->sessionType
		<< ",userData:" << msg->userData
		<< ",isAt:" << msg->isAt
		<< ",type:" << msg->type);

	EConRateOfProgressAttach * data = new EConRateOfProgressAttach();
	data->matchKey = matchKey;
	data->rateSize = rateSize;
	data->fileSize = fileSize;
	data->msg = *msg;
	//由于msg.fileSize 和msg.offset值为0，暂时使用fileSize值。
	if(data->msg.fileSize == 0) data->msg.fileSize = fileSize;
	if(data->msg.offset ==0) data->msg.offset = fileSize;

	PostMessage(gethWnd(), WM_onRateOfProgressAttach, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onSendTextMessage(unsigned int matchKey, int reason, ECMessage *msg)//发送消息回调。reason：200成功，其他报错；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",reason:" << reason
		<< ",dateCreated:" << msg->dateCreated
		<< ",fileName:" << msg->fileName
		<< ",fileSize:" << msg->fileSize
		<< ",fileUrl:" << msg->fileUrl
		<< ",msgContent:" << msg->msgContent
		<< ",msgId:" << msg->msgId
		<< ",msgType:" << msg->msgType
		<< ",offset:" << msg->offset
		<< ",receiver:" << msg->receiver
		<< ",sender:" << msg->sender
		<< ",senderNickName:" << msg->senderNickName
		<< ",sessionId:" << msg->sessionId
		<< ",sessionType:" << msg->sessionType
		<< ",userData:" << msg->userData 
		<< ",isAt:" << msg->isAt
		<< ",type:" << msg->type);


	std::string msgContent = msg->msgContent;
	if (msgContent.compare(0, 7, "VncCmd:") == 0){
	}
	else
	{
		EConSendTextMessage * data = new EConSendTextMessage();
		data->matchKey = matchKey;
		data->reason = reason;
		data->msg = *msg;
		PostMessage(gethWnd(), WM_onSendTextMessage, NULL, reinterpret_cast<LPARAM>(data));
	}

	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onSendMediaFile(unsigned int matchKey, int reason, ECMessage *msg)//上传文件回调，上传完成后被调用（上传失败也会被调）。reason:0成功，其他报错；offset 内容大小偏移量
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",reason:" << reason
		<< ",dateCreated:" << msg->dateCreated
		<< ",fileName:" << msg->fileName
		<< ",fileSize:" << msg->fileSize
		<< ",fileUrl:" << msg->fileUrl
		<< ",msgContent:" << msg->msgContent
		<< ",msgId:" << msg->msgId
		<< ",msgType:" << msg->msgType
		<< ",offset:" << msg->offset
		<< ",receiver:" << msg->receiver
		<< ",sender:" << msg->sender
		<< ",senderNickName:" << msg->senderNickName
		<< ",sessionId:" << msg->sessionId
		<< ",sessionType:" << msg->sessionType
		<< ",userData:" << msg->userData
		<< ",isAt:" << msg->isAt
		<< ",type:" << msg->type);

	EConSendMediaFile * data = new EConSendMediaFile();
	data->matchKey = matchKey;
	data->reason = reason;
	data->msg = *msg;
	//由于msg.fileSize 值为0，暂时使用msg.offset值。
	if(data->msg.fileSize == 0) data->msg.fileSize = data->msg.offset;

	PostMessage(gethWnd(), WM_onSendMediaFile, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onDeleteMessage(unsigned int matchKey, int reason, int type, const char* msgId)//删除服务端消息。reason:0成功，其他报错；type:透传消息类型；msgId:
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",reason:" << reason
		<< ",type:" << type
		<< ",msgId:" << msgId);

	EConDeleteMessage * data = new EConDeleteMessage();
	data->matchKey = matchKey;
	data->reason = reason;
	data->type = type;
	data->msgId;

	PostMessage(gethWnd(), WM_onDeleteMessage, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onOperateMessage(unsigned int tcpMsgIdOut, int reason)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",tcpMsgIdOut:" << tcpMsgIdOut << ",reason:" << reason);

	EConOperateMessage * data = new EConOperateMessage();
	data->tcpMsgIdOut = tcpMsgIdOut;
	data->reason = reason;

	PostMessage(gethWnd(), WM_onOperateMessage, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onReceiveOpreateNoticeMessage(const ECOperateNoticeMessage *pMsg)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	EConReceiveOpreateNoticeMessage * data = new EConReceiveOpreateNoticeMessage();
	data->notice = pMsg->notice;
	switch (pMsg->notice)
	{
	case NTDeleteMessage: data->DeleteMsg =  *(ECDeleteMessageNtyMsg *)pMsg->pNoticeMsg; break;
	case NTReadMessage: data->ReadMsg = *(ECReadMessageNtyMsg *)pMsg->pNoticeMsg; break;
	case NTWithdrawMessage: data->WithDrawMessage = *(ECWithDrawMessageNtyMsg*)pMsg->pNoticeMsg; break;
		default:
			delete data;
			LOG4CPLUS_WARN(log, "NTOperateUnknownNotice");
			LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
			return;
	}

	PostMessage(gethWnd(), WM_onReceiveOpreateNoticeMessage, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onUploadVTMFileOrBuf(unsigned int matchKey, int reason, const char* fileUrl)//上传VTM文件回调, fileIdOut：文件id, fileUrl:文件服务器保存地址
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",reason:" << reason
		<< ",fileUrl:" << fileUrl);

	EConUploadVTMFileOrBuf * data = new EConUploadVTMFileOrBuf();
	data->matchKey = matchKey;
	data->reason = reason;
	data->fileUrl = fileUrl;

	PostMessage(gethWnd(), WM_onUploadVTMFileOrBuf, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

/****************************群组接口对应的回调******************************/
void ECSDKBase::onCreateGroup(unsigned int matchKey, int reason, ECGroupDetailInfo *pInfo)//创建群组。matchKey 匹配主调键值；reason：200成功，其他报错；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",reason:" << reason
		<< ",city:" << pInfo->city
		<< ",dateCreated:" << pInfo->dateCreated
		<< ",declared:" << pInfo->declared
		<< ",discuss:" << pInfo->discuss
		<< ",groupDomain:" << pInfo->groupDomain
		<< ",groupId:" << pInfo->groupId
		<< ",isNotice:" << pInfo->isNotice
		<< ",memberCount:" << pInfo->memberCount
		<< ",name:" << pInfo->name
		<< ",owner:" << pInfo->owner
		<< ",permission:" << pInfo->permission
		<< ",province:" << pInfo->province
		<< ",scope:" << pInfo->scope
		<< ",type:" << pInfo->type);

	EConCreateGroup * data = new EConCreateGroup();
	data->matchKey = matchKey;
	data->reason = reason;
	data->info = *pInfo;

	PostMessage(gethWnd(), WM_onCreateGroup, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onDismissGroup(unsigned int matchKey, int reason, const char* groupid)//解散群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",reason:" << reason << ",groupid:" << groupid);

	EConDismissGroup * data = new EConDismissGroup();
	data->matchKey = matchKey;
	data->reason = reason;
	data->groupid = groupid;

	PostMessage(gethWnd(), WM_onDismissGroup, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onQuitGroup(unsigned int matchKey, int reason, const char* groupid)//退出群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",reason:" << reason << ",groupid:" << groupid);

	EConQuitGroup * data = new EConQuitGroup();
	data->matchKey = matchKey;
	data->reason = reason;
	data->groupid = groupid;

	PostMessage(gethWnd(), WM_onQuitGroup, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onJoinGroup(unsigned int matchKey, int reason, const char* groupid)//加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",reason:" << reason << ",groupid:" << groupid);

	EConJoinGroup * data = new EConJoinGroup();
	data->matchKey = matchKey;
	data->reason = reason;
	data->groupid = groupid;

	PostMessage(gethWnd(), WM_onJoinGroup, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onReplyRequestJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int confirm)//回复请求加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员；confirm 1拒绝 2同意
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",reason:" << reason << ",groupid:" << groupid
		<<",member:" << member << ",confirm:" << confirm);

	EConReplyRequestJoinGroup * data = new EConReplyRequestJoinGroup();
	data->matchKey = matchKey;
	data->reason = reason;
	data->groupid = groupid;
	data->member = member;
	data->confirm = confirm;

	PostMessage(gethWnd(), WM_onReplyRequestJoinGroup, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onInviteJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char** members, int membercount, int confirm)//邀请加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；members 成员数组；membercount 数组个数 ；confirm 1拒绝 2同意
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",reason:" << reason << ",groupid:" << groupid
		<< ",membercount:" << membercount << ",confirm:" << confirm);

	EConInviteJoinGroup * data = new EConInviteJoinGroup();

	for (int i = 0; i < membercount; i++)
	{
		LOG4CPLUS_DEBUG(log, __FUNCTION__"memeber:" << members[i]);
		data->members.push_back(members[i]);
	}

	data->matchKey = matchKey;
	data->reason = reason;
	data->groupid = groupid;
	data->confirm = confirm;

	PostMessage(gethWnd(), WM_onInviteJoinGroup, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onReplyInviteJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int confirm)//回复邀请加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员；confirm 1拒绝 2同意
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",reason:" << reason << ",groupid:" << groupid
		<< ",member:" << member << ",confirm:" << confirm);

	EConReplyInviteJoinGroup * data = new EConReplyInviteJoinGroup();
	data->matchKey = matchKey;
	data->reason = reason;
	data->groupid = groupid;
	data->member = member;
	data->confirm = confirm;

	PostMessage(gethWnd(), WM_onReplyInviteJoinGroup, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onQueryOwnGroup(unsigned int matchKey, int reason, int count, ECGroupSimpleInfo *group)//查询我的群组。matchKey 匹配主调键值；reason：200成功，其他报错；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",reason:" << reason 
		<< ",count:" << count);

	EConQueryOwnGroup * data = new EConQueryOwnGroup();
	for (int i = 0; i < count; i++)
	{
		data->group.push_back(group[i]);
		LOG4CPLUS_DEBUG(log, __FUNCTION__",discuss:" <<group[i].discuss
			<<",groupId:" << group[i].groupId
			<<",isNotice:"<<group[i].isNotice
			<<",memberCount:"<<group[i].memberCount
			<<",name:"<<group[i].name
			<<",owner:"<<group[i].owner
			<<",permission:"<<group[i].permission
			<<",scope:"<<group[i].scope);
	}
	data->matchKey = matchKey;
	data->reason = reason;

	PostMessage(gethWnd(), WM_onQueryOwnGroup, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onQueryGroupDetail(unsigned int matchKey, int reason, ECGroupDetailInfo *detail)//查询群组详情。matchKey 匹配主调键值；reason：200成功，其他报错；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",reason:" << reason
		<< ",city:" << detail->city
		<< ",dateCreated:" << detail->dateCreated
		<< ",declared:" << detail->declared
		<< ",discuss:" << detail->discuss
		<< ",groupDomain:" << detail->groupDomain
		<< ",groupId:" << detail->groupId
		<< ",isNotice:" << detail->isNotice
		<< ",memberCount:" << detail->memberCount
		<< ",name:" << detail->name
		<< ",owner:" << detail->owner
		<< ",permission:" << detail->permission
		<< ",province:" << detail->province
		<< ",scope:" << detail->scope
		<< ",type:" << detail->type);


	EConQueryGroupDetail * data = new EConQueryGroupDetail();
	data->matchKey = matchKey;
	data->reason = reason;
	data->detail = *detail;

	PostMessage(gethWnd(), WM_onQueryGroupDetail, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onModifyGroup(unsigned int matchKey, int reason, ECModifyGroupInfo *pInfo)//修改群组信息。matchKey 匹配主调键值；reason：200成功，其他报错；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",reason:" << reason
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


	EConModifyGroup * data = new EConModifyGroup();
	data->matchKey = matchKey;
	data->reason = reason;
	data->info = *pInfo;

	PostMessage(gethWnd(), WM_onModifyGroup, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onSearchPublicGroup(unsigned int matchKey, int reason, int searchType, const char* keyword, int count, ECGroupSearchInfo *group, int pageNo)//搜索公共群组。matchKey 匹配主调键值；reason：200成功，其他报错；searchType 搜索类型 1根据GroupId，精确搜索 2根据GroupName，模糊搜索； keyword 搜索关键词；count 群组个数；int pageNo 搜索分页索引
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",reason:" << reason << ",searchType:" << searchType
		<< ",keyword:" << keyword << ",count:" << count << "pageNo:" << pageNo);

	EConSearchPublicGroup * data = new EConSearchPublicGroup();
	data->matchKey = matchKey;
	data->reason = reason;
	data->searchType = searchType;
	data->keyword = keyword;

	for (int i = 0; i < count; i++)
	{
		LOG4CPLUS_DEBUG(log, __FUNCTION__ 
			<< ",declared:" << group[i].declared
			<< ",discuss:" << group[i].discuss
			<< ",groupId:" << group[i].groupId
			<< ",memberCount:" << group[i].memberCount
			<< ",name:" << group[i].name
			<< ",owner:" << group[i].owner
			<< ",permission:" << group[i].permission
			<< ",scope:" << group[i].scope);
		data->group.push_back(group[i]);
	}


	PostMessage(gethWnd(), WM_onSearchPublicGroup, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onQueryGroupMember(unsigned int matchKey, int reason, const char *groupid, int count, ECGroupMember *member)//查询群组成员。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",reason:" << reason << ",groupid:" << groupid
		<< ",count:" << count);

	EConQueryGroupMember * data = new EConQueryGroupMember();
	data->matchKey = matchKey;
	data->reason = reason;
	data->groupid = groupid;

	for (int i = 0; i < count; i++)
	{
		LOG4CPLUS_DEBUG(log, __FUNCTION__
			<< ",declared:" << member[i].member
			<< ",discuss:" << member[i].nickName
			<< ",groupId:" << member[i].role
			<< ",memberCount:" << member[i].sex
			<< ",name:" << member[i].speakState);
		data->members.push_back(member[i]);
	}


	PostMessage(gethWnd(), WM_onQueryGroupMember, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onDeleteGroupMember(unsigned int matchKey, int reason, const char* groupid, const char* member)//删除群组成员。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",reason:" << reason << ",groupid:" << groupid
		<< ",member:" << member);

	EConDeleteGroupMember * data = new EConDeleteGroupMember();
	data->matchKey = matchKey;
	data->reason = reason;
	data->groupid = groupid;
	data->member = member;

	PostMessage(gethWnd(), WM_onDeleteGroupMember, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onQueryGroupMemberCard(unsigned int matchKey, int reason, ECGroupMemberCard *card)//查询群组成员名片。matchKey 匹配主调键值；reason：200成功，其他报错；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",reason:" << reason 
		<< ",display:" << card->display
		<< ",groupId:" << card->groupId
		<< ",mail:" << card->mail
		<< ",member:" << card->member
		<< ",phone:" << card->phone
		<< ",remark:" << card->remark
		<< ",role:" << card->role
		<< ",sex:" << card->sex
		<< ",speakState:" << card->speakState);

	EConQueryGroupMemberCard * data = new EConQueryGroupMemberCard();
	data->matchKey = matchKey;
	data->reason = reason;
	data->card = *card;

	PostMessage(gethWnd(), WM_onQueryGroupMemberCard, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onModifyGroupMemberCard(unsigned int matchKey, int reason, ECGroupMemberCard* pCard)//修改群组成员名片。matchKey 匹配主调键值；reason：200成功，其他报错；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",reason:" << reason
		<< ",display:" << pCard->display
		<< ",groupId:" << pCard->groupId
		<< ",mail:" << pCard->mail
		<< ",member:" << pCard->member
		<< ",phone:" << pCard->phone
		<< ",remark:" << pCard->remark
		<< ",role:" << pCard->role
		<< ",sex:" << pCard->sex
		<< ",speakState:" << pCard->speakState);

	EConModifyGroupMemberCard * data = new EConModifyGroupMemberCard();
	data->matchKey = matchKey;
	data->reason = reason;
	data->card = *pCard;

	PostMessage(gethWnd(), WM_onModifyGroupMemberCard, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onForbidMemberSpeakGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int isBan)//群组禁言。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员；isBan 1允许发言 2禁止发言
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",reason:" << reason
		<< ",groupid:" << groupid
		<< ",member:" << member
		<< ",isBan:" << isBan);

	EConForbidMemberSpeakGroup * data = new EConForbidMemberSpeakGroup();
	data->matchKey = matchKey;
	data->reason = reason;
	data->groupid = groupid;
	data->member = member;
	data->isBan = isBan;

	PostMessage(gethWnd(), WM_onForbidMemberSpeakGroup, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onSetGroupMessageRule(unsigned int matchKey, int reason, const char* groupid, bool notice)//设置群组消息规则。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；notice 是否通知
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matcheKey:" << matchKey << ",reason:" << reason
		<< ",groupid:" << groupid
		<< ",notice:" << notice);

	EConSetGroupMessageRule * data = new EConSetGroupMessageRule();
	data->matchKey = matchKey;
	data->reason = reason;
	data->groupid = groupid;
	data->notice = notice;

	PostMessage(gethWnd(), WM_onSetGroupMessageRule, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onSetGroupMemberRole(unsigned int matchKey, int reason)//设置群组成员权限
{

}

/****************************服务器下发的群组相关通知******************************/
void ECSDKBase::onReceiveGroupNoticeMessage(const ECGroupNoticeMessage * pMsg)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",notice:" << pMsg->notice << ",pNoticeMsg:" << pMsg->pNoticeMsg);

	EConReceiveGroupNoticeMessage * data = new EConReceiveGroupNoticeMessage();
	data->notice = pMsg->notice;
	switch (pMsg->notice)
	{
	case NTRequestJoinGroup:
		data->requestMsg = *((ECRequestJoinGroupNtyMsg*)pMsg->pNoticeMsg);
		LOG4CPLUS_DEBUG(log, __FUNCTION__",confirm:" << data->requestMsg.confirm
			<<",dateCreated:" << data->requestMsg.dateCreated
			<<",declared:" << data->requestMsg.declared
			<<",groupId:" << data->requestMsg.groupId
			<<",groupName:" << data->requestMsg.groupName
			<<",nickName:" << data->requestMsg.nickName
			<<",proposer:" << data->requestMsg.proposer
			<<",sender:" << data->requestMsg.sender);
		break;
	case NTInviteJoinGroup:
		data->inviteMsg = *((ECInviteJoinGroupNtyMsg*)pMsg->pNoticeMsg);
		LOG4CPLUS_DEBUG(log, __FUNCTION__",admin:" << data->inviteMsg.admin
			<< ",confirm:" << data->inviteMsg.confirm
			<< ",dateCreated:" << data->inviteMsg.dateCreated
			<< ",declared:" << data->inviteMsg.declared
			<< ",groupId:" << data->inviteMsg.groupId
			<< ",groupName:" << data->inviteMsg.groupName
			<< ",nickName:" << data->inviteMsg.nickName
			<< ",sender:" << data->inviteMsg.sender);
		break;
	case NTMemberJoinedGroup:
		data->memberMsg = *((ECMemberJoinedGroupNtyMsg*)pMsg->pNoticeMsg);
		LOG4CPLUS_DEBUG(log, __FUNCTION__",dateCreated:" << data->memberMsg.dateCreated
			<< ",declared:" << data->memberMsg.declared
			<< ",groupId:" << data->memberMsg.groupId
			<< ",groupName:" << data->memberMsg.groupName
			<< ",member:" << data->memberMsg.member
			<< ",nickName:" << data->memberMsg.nickName
			<< ",sender:" << data->memberMsg.sender);
		break;
	case NTDismissGroup:
		data->dismissMsg = *((ECDismissGroupNtyMsg*)pMsg->pNoticeMsg);
		LOG4CPLUS_DEBUG(log, __FUNCTION__",dateCreated:" << data->dismissMsg.dateCreated
			<< ",groupId:" << data->dismissMsg.groupId
			<< ",groupName:" << data->dismissMsg.groupName
			<< ",sender:" << data->dismissMsg.sender);
		break;
	case NTQuitGroup:
		data->quitMsg = *((ECQuitGroupNtyMsg*)pMsg->pNoticeMsg);
		LOG4CPLUS_DEBUG(log, __FUNCTION__ << ",dateCreated:" << data->quitMsg.dateCreated
			<< ",groupId:" << data->quitMsg.groupId
			<< ",groupName:" << data->quitMsg.groupName
			<< ",member:" << data->quitMsg.member
			<< ",nickName:" << data->quitMsg.nickName
			<< ",sender:" << data->quitMsg.sender);
		break;
	case NTRemoveGroupMember:
		data->removeMsg = *((ECRemoveGroupMemberNtyMsg*)pMsg->pNoticeMsg);
		LOG4CPLUS_DEBUG(log, __FUNCTION__",dateCreated:" << data->removeMsg.dateCreated
			<< ",groupId:" << data->removeMsg.groupId
			<< ",groupName:" << data->removeMsg.groupName
			<< ",member:" << data->removeMsg.member
			<< ",nickName:" << data->removeMsg.nickName
			<< ",sender:" << data->removeMsg.sender);
		break;
	case NTReplyRequestJoinGroup:
		data->replyrequestMsg = *((ECReplyRequestJoinGroupNtyMsg*)pMsg->pNoticeMsg);
		LOG4CPLUS_DEBUG(log, __FUNCTION__",admin:" << data->replyrequestMsg.admin
			<< ",confirm:" << data->replyrequestMsg.confirm
			<< ",dateCreated:" << data->replyrequestMsg.dateCreated
			<< ",groupId:" << data->replyrequestMsg.groupId
			<< ",groupName:" << data->replyrequestMsg.groupName
			<< ",member:" << data->replyrequestMsg.member
			<< ",nickName:" << data->replyrequestMsg.nickName
			<< ",sender:" << data->replyrequestMsg.sender);
		break;
	case NTReplyInviteJoinGroup:
		data->replyinviteMsg = *((ECReplyInviteJoinGroupNtyMsg*)pMsg->pNoticeMsg);
		LOG4CPLUS_DEBUG(log, __FUNCTION__",admin:" << data->replyinviteMsg.admin
			<< ",confirm:" << data->replyinviteMsg.confirm
			<< ",dateCreated:" << data->replyinviteMsg.dateCreated
			<< ",groupId:" << data->replyinviteMsg.groupId
			<< ",groupName:" << data->replyinviteMsg.groupName
			<< ",member:" << data->replyinviteMsg.member
			<< ",nickName:" << data->replyinviteMsg.nickName
			<< ",sender:" << data->replyinviteMsg.sender);
		break;
	case NTModifyGroup:
		data->modifyMsg = *((ECModifyGroupNtyMsg*)pMsg->pNoticeMsg);
		LOG4CPLUS_DEBUG(log, __FUNCTION__",dateCreated:" << data->modifyMsg.dateCreated
			<< ",groupId:" << data->modifyMsg.groupId
			<< ",groupName:" << data->modifyMsg.groupName
			<< ",member:" << data->modifyMsg.member
			<< ",modifyDic:" << data->modifyMsg.modifyDic
			<< ",sender:" << data->modifyMsg.sender);
		break;
	default:
		break;
	}

	PostMessage(gethWnd(), WM_onReceiveGroupNoticeMessage, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

/****************************音频播放回调接口******************************/
void ECSDKBase::onRecordingTimeOut(int ms, const char* sessionId)//录音超时。ms 为录音时长，单位毫秒,sessionId 透传会话ID
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",ms:" << ms << ",sessionId:" << sessionId);
	EConRecordingTimeOut * data = new EConRecordingTimeOut();
	data->ms = ms;
	data->sessionId = sessionId;
	PostMessage(gethWnd(), WM_onRecordingTimeOut, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onFinishedPlaying(const char* sessionId, int reason)//播放结束,sessionId透传会话ID,reason 200成功.其他失败
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",sessionId:" << sessionId << ",reason:" << reason);
	EConFinishedPlaying * data = new EConFinishedPlaying();
	data->sessionId = sessionId;
	data->reason = reason;
	PostMessage(gethWnd(), WM_onFinishedPlaying, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onRecordingAmplitude(double amplitude, const char* sessionId)//返回音量的振幅。录音时振幅范围0-1,sessionId 透传会话ID
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__",amplitude:" << amplitude <<  ",sessionId:" << sessionId);
	EConRecordingAmplitude * data = new EConRecordingAmplitude();
	data->amplitude = amplitude;
	data->sessionId = sessionId;
	PostMessage(gethWnd(), WM_onRecordingAmplitude, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

/********************************************会议回调接口***********************************************/
void ECSDKBase::onMeetingIncoming(ECMeetingComingInfo *pCome)//会议接到邀请。
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",caller:" << pCome->caller << ",callid:" << pCome->callid
		<< ",display:" << pCome->display << ",Id:" << pCome->Id
		<< ",meetingType:" << pCome->meetingType << ",nickname:" << pCome->nickname);

	ECMeetingComingInfo * data = new ECMeetingComingInfo(*pCome);
	PostMessage(gethWnd(), WM_onMeetingIncoming, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}
void ECSDKBase::onReceiveInterphoneMeetingMessage(ECInterphoneMsg* pInterphoneMsg)// 收到实时对讲里面的通知消息。
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",datecreated:" << pInterphoneMsg->datecreated << ",from:" << pInterphoneMsg->from
		<< ",interphoneid:" << pInterphoneMsg->interphoneid << ",member:" << pInterphoneMsg->member
		<< ",receiver:" << pInterphoneMsg->receiver << ",state:" << pInterphoneMsg->state
		<< ",type:" << pInterphoneMsg->type << ",userdata:" << pInterphoneMsg->userdata
		<< ",var:" << pInterphoneMsg->var);

	ECInterphoneMsg * data = new ECInterphoneMsg(*pInterphoneMsg);
	PostMessage(gethWnd(), WM_onReceiveInterphoneMeetingMessage, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}
void ECSDKBase::onReceiveVoiceMeetingMessage(ECVoiceMeetingMsg* pVoiceMeetingMsg)// 收到语音会议里面的通知消息。
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",chatroomid:" << pVoiceMeetingMsg->chatroomid << ",forbid:" << pVoiceMeetingMsg->forbid
		<< ",isVoIP:" << pVoiceMeetingMsg->isVoIP << ",member:" << pVoiceMeetingMsg->member
		<< ",receiver:" << pVoiceMeetingMsg->receiver << ",sender:" << pVoiceMeetingMsg->sender
		<< ",type:" << pVoiceMeetingMsg->type << ",userdata:" << pVoiceMeetingMsg->userdata
		<< ",var:" << pVoiceMeetingMsg->var);

	ECVoiceMeetingMsg * data = new ECVoiceMeetingMsg(*pVoiceMeetingMsg);
	PostMessage(gethWnd(), WM_onReceiveVoiceMeetingMessage, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}
void ECSDKBase::onReceiveVideoMeetingMessage(ECVideoMeetingMsg* pVideoMeetingMsg)// 收到视频会议里面的通知消息。
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",dataState:" << pVideoMeetingMsg->dataState << ",forbid:" << pVideoMeetingMsg->forbid
		<< ",isVoIP:" << pVideoMeetingMsg->isVoIP << ",member:" << pVideoMeetingMsg->member
		<< ",receiver:" << pVideoMeetingMsg->receiver 
		<< ",roomid:" <<pVideoMeetingMsg->roomid
		<< ",sender:" << pVideoMeetingMsg->sender
		<< ",state:" << pVideoMeetingMsg->state
		<< ",type:" << pVideoMeetingMsg->type 
		<< ",userdata:" << pVideoMeetingMsg->userdata
		<< ",var:" << pVideoMeetingMsg->var
		<< ",videoState:" << pVideoMeetingMsg->videoState);

	ECVideoMeetingMsg * data = new ECVideoMeetingMsg(*pVideoMeetingMsg);
	PostMessage(gethWnd(), WM_onReceiveVideoMeetingMessage, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}
void ECSDKBase::onCreateMultimediaMeeting(unsigned int matchKey, int reason, ECMeetingInfo *pInfo)//创建音频、视频会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ",matchKey:" << matchKey << ",reason:" << reason
		<< ",confirm:" << pInfo->confirm << ",creator:" << pInfo->creator
		<< ",Id:" << pInfo->Id
		<< ",joinedCount:" << pInfo->joinedCount
		<< ",keywords:" << pInfo->keywords
		<< ",meetingType:" << pInfo->meetingType
		<< ",name:" << pInfo->name
		<< ",square:" << pInfo->square);

	EConCreateMultimediaMeeting * data = new EConCreateMultimediaMeeting();
	data->matchKey = matchKey;
	data->reason = reason;
	data->info = *pInfo;

	PostMessage(gethWnd(), WM_onCreateMultimediaMeeting, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}
void ECSDKBase::onJoinMeeting(int reason, const char* conferenceId)//主动加入会议的回调。//reason：200成功，其他报错；conferenceId 会议ID；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");

	LOG4CPLUS_DEBUG(log, __FUNCTION__",reason:" << reason << ",conferenceId:" << conferenceId);
	EConJoinMeeting * data = new EConJoinMeeting();
	data->reason = reason;
	data->conferenceId = conferenceId;

	PostMessage(gethWnd(), WM_onJoinMeeting, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");

}
void ECSDKBase::onExitMeeting(const char* conferenceId)//退出会议。conferenceId 会议ID；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");

	LOG4CPLUS_DEBUG(log, __FUNCTION__",conferenceId:" << conferenceId);
	std::string * data = new std::string(conferenceId);

	PostMessage(gethWnd(), WM_onExitMeeting, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}
void ECSDKBase::onQueryMeetingMembers(unsigned int matchKey, int reason, const char* conferenceId, int count, ECMeetingMemberInfo* pMembers)//查询加入回忆的成员。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；count 成员数组个数，pMembers 成员数组
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");

	LOG4CPLUS_DEBUG(log, __FUNCTION__",matchKey:"<< matchKey << ",reason:" << reason << ",conferenceId:" << conferenceId
		<< ",count:" << count);
	EConQueryMeetingMembers * data = new EConQueryMeetingMembers();

	for (int i = 0; i < count; i++)
	{
		LOG4CPLUS_DEBUG(log, __FUNCTION__ ",dataState:" << pMembers[i].dataState << ",forbid:" << pMembers[i].forbid
			<< ",isMic:" << pMembers[i].isMic << ",isOnline:" << pMembers[i].isOnline
			<< ",isVoIP:" << pMembers[i].isVoIP 
			<< ",meetingType:" << pMembers[i].meetingType
			<< ",member:" << pMembers[i].member
			<< ",state:" << pMembers[i].state
			<< ",type:" << pMembers[i].type
			<< ",userdata:" << pMembers[i].userdata
			<< ",videoState:" << pMembers[i].videoState);

		data->Members.push_back(pMembers[i]);
	}
	data->matchKey = matchKey;
	data->reason = reason;
	data->conferenceId = conferenceId;

	PostMessage(gethWnd(), WM_onQueryMeetingMembers, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}
void ECSDKBase::onDismissMultiMediaMeeting(unsigned int matchKey, int reason, const char*conferenceId)//解散实时对讲、音频、视频会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");

	LOG4CPLUS_DEBUG(log, __FUNCTION__",matchKey:" << matchKey << ",reason:" << reason << ",conferenceId:" << conferenceId);
	EConDismissMultiMediaMeeting * data = new EConDismissMultiMediaMeeting();
	data->matchKey = matchKey;
	data->reason = reason;
	data->conferenceId = conferenceId;

	PostMessage(gethWnd(), WM_onDismissMultiMediaMeeting, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}
void ECSDKBase::onQueryMultiMediaMeetings(unsigned int matchKey, int reason, int count, ECMeetingInfo* pMeetingInfo)//查询会议信息的回调。matchKey 匹配主调键值；reason：200成功，其他报错；count 会议数组个数，pMembers 会议数组
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");

	LOG4CPLUS_DEBUG(log, __FUNCTION__",matchKey:" << matchKey << ",reason:" << reason << ",count:" << count);
	EConQueryMultiMediaMeetings * data = new EConQueryMultiMediaMeetings();

	for (int i = 0; i < count; i++)
	{
		LOG4CPLUS_DEBUG(log, __FUNCTION__  ",confirm:" << pMeetingInfo[i].confirm << ",creator:" << pMeetingInfo[i].creator
			<< ",Id:" << pMeetingInfo[i].Id
			<< ",joinedCount:" << pMeetingInfo[i].joinedCount
			<< ",keywords:" << pMeetingInfo[i].keywords
			<< ",meetingType:" << pMeetingInfo[i].meetingType
			<< ",name:" << pMeetingInfo[i].name
			<< ",square:" << pMeetingInfo[i].square);

		data->infos.push_back(pMeetingInfo[i]);
	}
	data->matchKey = matchKey;
	data->reason = reason;

	PostMessage(gethWnd(), WM_onQueryMultiMediaMeetings, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}
void ECSDKBase::onDeleteMemberMultiMediaMeeting(unsigned int matchKey, int reason, ECMeetingDeleteMemberInfo *pInfo)//删除会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");

	LOG4CPLUS_DEBUG(log, __FUNCTION__",matchKey:" << matchKey << ",reason:" << reason << ",Id:"<<pInfo->Id
		<< ",isVoIP:" << pInfo->isVoIP << ",meetingType:" << pInfo->meetingType
		<< ",member:" << pInfo->member);

	EConDeleteMemberMultiMediaMeeting * data = new EConDeleteMemberMultiMediaMeeting();

	data->matchKey = matchKey;
	data->reason = reason;
	data->info = *pInfo;

	PostMessage(gethWnd(), WM_onDeleteMemberMultiMediaMeeting, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");

}
void ECSDKBase::onInviteJoinMultiMediaMeeting(unsigned int matchKey, int reason, ECMeetingInviteInfo *pInfo)//邀请加入会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");

	EConInviteJoinMultiMediaMeeting * data = new EConInviteJoinMultiMediaMeeting();

	LOG4CPLUS_DEBUG(log, __FUNCTION__",matchKey:" << matchKey << ",reason:" << reason << ",Id:" << pInfo->Id
		<< ",isListen:" << pInfo->isListen << ",isSpeak:" << pInfo->isSpeak
		<< ",userdata:" << pInfo->userdata
		<< ",memberCount:" << pInfo->memberCount);

	data->matchKey = matchKey;
	data->reason = reason;
	data->info.Id = pInfo->Id;
	data->info.isListen = pInfo->isListen;
	data->info.isSpeak = pInfo->isSpeak;
	data->info.userdata = pInfo->userdata;

	for (int i = 0; i < pInfo->memberCount; i++)
	{
		LOG4CPLUS_DEBUG(log, __FUNCTION__ << ",members:" << pInfo->members[i]);
		data->info.members.push_back(pInfo->members[i]);
	}


	PostMessage(gethWnd(), WM_onInviteJoinMultiMediaMeeting, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}
void ECSDKBase::onCreateInterphoneMeeting(unsigned int matchKey, int reason, const char* interphoneId)//创建实时对讲会议的回调。matchKey 匹配主调键值；reason：200成功，其他报错；interphoneId 实时对讲ID
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");

	LOG4CPLUS_DEBUG(log, __FUNCTION__",matchKey:" << matchKey << ",reason:" << reason << ",interphoneId:" << interphoneId);
	EConCreateInterphoneMeeting * data = new EConCreateInterphoneMeeting();
	data->matchKey = matchKey;
	data->reason = reason;
	data->interphoneId = interphoneId;

	PostMessage(gethWnd(), WM_onCreateInterphoneMeeting, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}
void ECSDKBase::onExitInterphoneMeeting(const char* interphoneId)//退出实时对讲。interphoneId 实时对讲ID
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");

	LOG4CPLUS_DEBUG(log, __FUNCTION__",interphoneId:" << interphoneId);
	std::string * data = new std::string(interphoneId);

	PostMessage(gethWnd(), WM_onExitInterphoneMeeting, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onSetMeetingSpeakListen(unsigned int matchKey, int reason, ECMeetingMemberSpeakListenInfo* pInfo)//设置会议成员听讲状态的回调。matchKey 匹配主调键值；reason：200成功，其他报错；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");

	LOG4CPLUS_DEBUG(log, __FUNCTION__",matchKey:" << matchKey << ",reason:" << reason << ",Id:" << pInfo->Id
		<< ",isVoIP:" << pInfo->isVoIP
		<< ",meetingType:" << pInfo->meetingType
		<< ",member:" << pInfo->member
		<< ",speaklisen:" << pInfo->speaklisen);
	EConSetMeetingSpeakListen * data = new EConSetMeetingSpeakListen();
	data->matchKey = matchKey;
	data->reason = reason;
	data->Info = *pInfo;

	PostMessage(gethWnd(), WM_onSetMeetingSpeakListen, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

void ECSDKBase::onControlInterphoneMic(unsigned int matchKey, int reason, const char* controller, const char* interphoneId, bool requestIsControl)//实时对讲抢麦放麦的回调。matchKey 匹配主调键值；reason：200成功，其他报错；controller 控麦成员；interphoneId 实时对讲ID；requestIsControl 是否发起控麦
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");

	LOG4CPLUS_DEBUG(log, __FUNCTION__",matchKey:" << matchKey << ",reason:" << reason << ",interphoneId:" << interphoneId
		<< ",requestIsControl:" << requestIsControl);
	EConControlInterphoneMic * data = new EConControlInterphoneMic();
	data->matchKey = matchKey;
	data->reason = reason;
	data->controller = controller;
	data->interphoneId = interphoneId;
	data->requestIsControl = requestIsControl;

	PostMessage(gethWnd(), WM_onControlInterphoneMic, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}
void ECSDKBase::onPublishVideo(unsigned int matchKey, int reason, const char* conferenceId)//发布视频的回调。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");

	LOG4CPLUS_DEBUG(log, __FUNCTION__",matchKey:" << matchKey << ",reason:" << reason << ",conferenceId:" << conferenceId);
	EConPublishVideo * data = new EConPublishVideo();
	data->matchKey = matchKey;
	data->reason = reason;
	data->conferenceId = conferenceId;

	PostMessage(gethWnd(), WM_onPublishVideo, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}
void ECSDKBase::onUnpublishVideo(unsigned int matchKey, int reason, const char* conferenceId)//取消视频的回调。matchKey 匹配主调键值；reason：200成功，其他报错；conferenceId 会议ID；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");

	LOG4CPLUS_DEBUG(log, __FUNCTION__",matchKey:" << matchKey << ",reason:" << reason << ",conferenceId:" << conferenceId);
	EConUnpublishVideo * data = new EConUnpublishVideo();
	data->matchKey = matchKey;
	data->reason = reason;
	data->conferenceId = conferenceId;

	PostMessage(gethWnd(), WM_onUnpublishVideo, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}
void ECSDKBase::onRequestConferenceMemberVideo(int reason, const char *conferenceId, const char *member, int type)//视频会议时，请求会议成员视频数据。reason 详见Video_Conference_status；conferenceId 会议ID；member成员；type详见ECVideoDataType；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");

	LOG4CPLUS_DEBUG(log, __FUNCTION__",reason:" << reason << ",conferenceId:" << conferenceId << ",member:" << member << ",type:" << type);
	EConRequestConferenceMemberVideo * data = new EConRequestConferenceMemberVideo();
	data->reason = reason;
	data->conferenceId = conferenceId;
	data->member = member;
	data->type = type;

	PostMessage(gethWnd(), WM_onRequestConferenceMemberVideo, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}
void ECSDKBase::onCancelConferenceMemberVideo(int reason, const char *conferenceId, const char *member, int type)//视频会议时，取消会议成员视频数据响应。reason 详见Video_Conference_status；conferenceId 会议ID；tmember成员；ype详见ECVideoDataType；
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", start");

	LOG4CPLUS_DEBUG(log, __FUNCTION__",reason:" << reason << ",conferenceId:" << conferenceId << ",member:" << member << ",type:" << type);
	EConCancelConferenceMemberVideo * data = new EConCancelConferenceMemberVideo();
	data->reason = reason;
	data->conferenceId = conferenceId;
	data->member = member;
	data->type = type;

	PostMessage(gethWnd(), WM_onCancelConferenceMemberVideo, NULL, reinterpret_cast<LPARAM>(data));
	LOG4CPLUS_TRACE(log, __FUNCTION__ ", end");
}

int ECSDKBase::Initialize()
{
	int ret = ECSDK::Initialize();
	//std::string utf8Path = Unicode2Utf8(utf16Path);
	//utf8Path = utf8Path.substr(0,utf8Path.rfind("\\"));
	//std::string xmlFileName = utf8Path + "\\sdk_server_config.xml";
	//ECSDK::SetServerEnvType(0, xmlFileName.c_str());
	return ret;
}

int ECSDKBase::Login(const ECLoginInfo *loginInfo)
{
	m_utf8VoipId = loginInfo->username;
	return ECSDK::Login(loginInfo);
}

int ECSDKBase::SetDesktopWnd(HANDLE hWnd)
{
	m_hShareWnd = hWnd;
	LOG4CPLUS_DEBUG(log, __FUNCTION__ << " hShareWnd:" << m_hShareWnd);
	return 0;
}

int ECSDKBase::sendVncCmdToRemote(int roomID, int msgType, int msgStatus, const std::string &desc)
{

	std::string cmdMsg = "VncCmd:";
	Json::Value root;
	root["roomID"] = roomID;
	root["type"] = msgType;
	root["status"] = msgStatus;
	root["desc"] = desc;
	root["account"] = m_utf8VoipId;
	root["svrIP"] = m_VncServerIP;
	root["svrPort"] = m_VncServerPort;

	cmdMsg.append(root.toStyledString().c_str());

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " sendto:" << m_VncRemoteId << " MsgBody:" << cmdMsg);
	char msgId[64];
	if (ECSDKBase::SendTextMessage(NULL, this->m_VncRemoteId.c_str(), cmdMsg.c_str(), Msg_Type_Text, NULL, msgId))
	{
		LOG4CPLUS_ERROR(log, __FUNCTION__ << " send VncCmd failed.");
		return -1;
	}
	return 0;
}

int ECSDKBase::RGB24Snapshot(BYTE* pData, int width, int height, unsigned char ** outData, unsigned int *outSize)
{
	int size = width*height * 3; // 每个像素点3个字节

	// 位图第一部分，文件信息
	BITMAPFILEHEADER bfh;
	bfh.bfType = 0x4d42; //bm
	bfh.bfSize = size // data size
		+ sizeof(BITMAPFILEHEADER) // first section size
		+ sizeof(BITMAPINFOHEADER) // second section size
		;
	bfh.bfReserved1 = 0; // reserved
	bfh.bfReserved2 = 0; // reserved
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// 位图第二部分，数据信息
	BITMAPINFOHEADER bih;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = width;
	bih.biHeight = height;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = size;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

	*outSize = bfh.bfSize;
	unsigned char * buf = (unsigned char *)malloc(*outSize);
	*outData = buf;

	memcpy(buf, &bfh, sizeof(bfh));
	buf += sizeof(bfh);
	memcpy(buf, &bih, sizeof(bih));
	buf += sizeof(bih);
	int linesize = width * 3;
	for (int line = height - 1; line >= 0; line--)
	{
		memcpy(buf, pData + linesize * line, linesize);
		buf += linesize;
	}
	return 0;
}

const std::string & ECSDKBase::getAppDataDir()const
{
	// TODO: Add your implementation code here
	LOG4CPLUS_DEBUG(log, __FUNCTION__ << " result:" << m_utf8AppDataDir);
	return m_utf8AppDataDir;
}