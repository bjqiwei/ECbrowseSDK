#include "stdafx.h"
#include "NPAPIECSDK.h"
#include <log4cplus/loggingmacros.h>
#include "codingHelper.h"
#include "ZBase64.h"


static bool JSGetProperty(NPObject * object, const std::string & name, NPVariant * value)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance("JSGetProperty");

	NPIdentifier nama_id = NPN_GetStringIdentifier(name.c_str());

	if (object->_class->hasProperty(object, nama_id)){
		return object->_class->getProperty(object, nama_id, value);
	}

	return false;
}

static bool JSSetProperty(NPObject * object, const std::string & name, const NPVariant & value)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance("JSSetProperty");

	NPIdentifier nama_id = NPN_GetStringIdentifier(name.c_str());
	return object->_class->setProperty(object, nama_id, &value);
}

static bool JSSetProperty(NPVariant & object, const std::string & name, const NPVariant & value)
{
	assert(NPVARIANT_IS_OBJECT(object));
	return JSSetProperty(NPVARIANT_TO_OBJECT(object), name, value);
}

static bool JSSetProperty(NPVariant & object, const std::string & name, const CNPVariant & value)
{
	assert(NPVARIANT_IS_OBJECT(object));
	return JSSetProperty(NPVARIANT_TO_OBJECT(object), name, value);
}

static bool JSSetValue(NPObject * object, const NPVariant & value)
{
	return JSSetProperty(object, "value", value);
}

static bool JSSetValue(NPVariant & object, const NPVariant & value)
{
	assert(NPVARIANT_IS_OBJECT(object));
	return JSSetValue(NPVARIANT_TO_OBJECT(object), value);
}

static bool JSAddArrayElement(NPObject* array, const NPVariant & value)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance("JSAddArrayElement");

	NPIdentifier push_id = NPN_GetStringIdentifier("push");

	if (array->_class->hasMethod(array, push_id)){
		NPVariant result;
		return array->_class->invoke(array, push_id, &value, 1, &result);
	}

	return false;
}

static bool JSAddArrayElement(const NPVariant &array, const NPVariant & value)
{
	assert(NPVARIANT_IS_OBJECT(array));
	return JSAddArrayElement(NPVARIANT_TO_OBJECT(array),value);
}

static bool JSGetArrayElementOfIndex(NPObject *array, int32_t index, NPVariant * value)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance("JSGetArrayElementOfIndex");

	NPIdentifier nama_id = NPN_GetIntIdentifier(index);

	if (array->_class->hasProperty(array, nama_id)){
		return array->_class->getProperty(array, nama_id, value);
	}

	return false;
}

NPAPIECSDK::NPAPIECSDK(NPP pNPInstance) :NPAPIPluginBase(pNPInstance),
	onLogInfo_obj(nullptr),
	onLogOut_obj(nullptr),
	onConnectState_obj(nullptr),
	onSetPersonInfo_obj(nullptr),
	onGetPersonInfo_obj(nullptr),
	onGetUserState_obj(nullptr),
	onPublishPresence_obj(nullptr),
	onReceiveFriendsPublishPresence_obj(nullptr),
	onSoftVersion_obj(nullptr),
	onGetOnlineMultiDevice_obj(nullptr),
	onReceiveMultiDeviceState_obj(nullptr),
	onMakeCallBack_obj(nullptr),
	onCallEvents_obj(nullptr),
	onCallIncoming_obj(nullptr),
	onDtmfReceived_obj(nullptr),
	onSwitchCallMediaTypeRequest_obj(nullptr),
	onSwitchCallMediaTypeResponse_obj(nullptr),
	onRemoteVideoRatio_obj(nullptr),
	onOfflineMessageCount_obj(nullptr),
	onGetOfflineMessage_obj(nullptr),
	onOfflineMessageComplete_obj(nullptr),
	onReceiveOfflineMessage_obj(nullptr),
	onReceiveMessage_obj(nullptr),
	onReceiveFile_obj(nullptr),
	onDownloadFileComplete_obj(nullptr),
	onRateOfProgressAttach_obj(nullptr),
	onSendTextMessage_obj(nullptr),
	onSendMediaFile_obj(nullptr),
	onDeleteMessage_obj(nullptr),
	onOperateMessage_obj(nullptr),
	onReceiveOpreateNoticeMessage_obj(nullptr),
	onUploadVTMFileOrBuf_obj(nullptr),
	onCreateGroup_obj(nullptr),
	onDismissGroup_obj(nullptr),
	onQuitGroup_obj(nullptr),
	onJoinGroup_obj(nullptr),
	onReplyRequestJoinGroup_obj(nullptr),
	onInviteJoinGroup_obj(nullptr),
	onReplyInviteJoinGroup_obj(nullptr),
	onQueryOwnGroup_obj(nullptr),
	onQueryGroupDetail_obj(nullptr),
	onModifyGroup_obj(nullptr),
	onSearchPublicGroup_obj(nullptr),
	onQueryGroupMember_obj(nullptr),
	onDeleteGroupMember_obj(nullptr),
	onQueryGroupMemberCard_obj(nullptr),
	onModifyGroupMemberCard_obj(nullptr),
	onForbidMemberSpeakGroup_obj(nullptr),
	onSetGroupMessageRule_obj(nullptr),
	onReceiveGroupNoticeMessage_obj(nullptr),
	onRecordingTimeOut_obj(nullptr),
	onFinishedPlaying_obj(nullptr),
	onRecordingAmplitude_obj(nullptr),
	onMeetingIncoming_obj(nullptr),
	onReceiveInterphoneMeetingMessage_obj(nullptr),
	onReceiveVoiceMeetingMessage_obj(nullptr),
	onReceiveVideoMeetingMessage_obj(nullptr),
	onCreateMultimediaMeeting_obj(nullptr),
	onJoinMeeting_obj(nullptr),
	onExitMeeting_obj(nullptr),
	onQueryMeetingMembers_obj(nullptr),
	onDismissMultiMediaMeeting_obj(nullptr),
	onQueryMultiMediaMeetings_obj(nullptr),
	onDeleteMemberMultiMediaMeeting_obj(nullptr),
	onInviteJoinMultiMediaMeeting_obj(nullptr),
	onCreateInterphoneMeeting_obj(nullptr),
	onExitInterphoneMeeting_obj(nullptr),
	onSetMeetingSpeakListen_obj(nullptr),
	onControlInterphoneMic_obj(nullptr),
	onPublishVideo_obj(nullptr),
	onUnpublishVideo_obj(nullptr),
	onRequestConferenceMemberVideo_obj(nullptr),
	onCancelConferenceMemberVideo_obj(nullptr)
{
	this->log = log4cplus::Logger::getInstance("NPAPIECSDK");

	//function id
	fInitialize_id = NPN_GetStringIdentifier("Initialize");
	fUnInitialize_id = NPN_GetStringIdentifier("UnInitialize");
	fLogin_id = NPN_GetStringIdentifier("Login");
	fSetTraceFlag_id = NPN_GetStringIdentifier("SetTraceFlag");
	fSetServerEnvType_id = NPN_GetStringIdentifier("SetServerEnvType");
	fSetInternalDNS_id = NPN_GetStringIdentifier("SetInternalDNS");
	fLogout_id = NPN_GetStringIdentifier("Logout");
	fSetPersonInfo_id = NPN_GetStringIdentifier("SetPersonInfo");
	fGetPersonInfo_id = NPN_GetStringIdentifier("GetPersonInfo");
	fGetUserState_id = NPN_GetStringIdentifier("GetUserState");
	fQueryErrorDescribe_id = NPN_GetStringIdentifier("QueryErrorDescribe");
	fMakeCallBack_id = NPN_GetStringIdentifier("MakeCallBack");
	fMakeCall_id = NPN_GetStringIdentifier("MakeCall");
	fAcceptCall_id = NPN_GetStringIdentifier("AcceptCall");
	fReleaseCall_id = NPN_GetStringIdentifier("ReleaseCall");
	fSendDTMF_id = NPN_GetStringIdentifier("SendDTMF");
	fGetCurrentCall_id = NPN_GetStringIdentifier("GetCurrentCall");
	fSetMute_id = NPN_GetStringIdentifier("SetMute");
	fGetMuteStatus_id = NPN_GetStringIdentifier("GetMuteStatus");
	fSetSoftMute_id = NPN_GetStringIdentifier("SetSoftMute");
	fGetSoftMuteStatus_id = NPN_GetStringIdentifier("GetSoftMuteStatus");
	fSetAudioConfigEnabled_id = NPN_GetStringIdentifier("SetAudioConfigEnabled");
	fGetAudioConfigEnabled_id = NPN_GetStringIdentifier("GetAudioConfigEnabled");
	fSetCodecEnabled_id = NPN_GetStringIdentifier("SetCodecEnabled");
	fGetCodecEnabled_id = NPN_GetStringIdentifier("GetCodecEnabled");
	fSetCodecNack_id = NPN_GetStringIdentifier("SetCodecNack");
	fGetCodecNack_id = NPN_GetStringIdentifier("GetCodecNack");
	fGetCallStatistics_id = NPN_GetStringIdentifier("GetCallStatistics");
	fSetVideoView_id = NPN_GetStringIdentifier("SetVideoView");
	fSetVideoBitRates_id = NPN_GetStringIdentifier("SetVideoBitRates");
	fSetRing_id = NPN_GetStringIdentifier("SetRing");
	fSetRingback_id = NPN_GetStringIdentifier("SetRingback");
	fRequestSwitchCallMediaType_id = NPN_GetStringIdentifier("RequestSwitchCallMediaType");
	fResponseSwitchCallMediaType_id = NPN_GetStringIdentifier("ResponseSwitchCallMediaType");
	fGetCameraInfo_id = NPN_GetStringIdentifier("GetCameraInfo");
	fSelectCamera_id = NPN_GetStringIdentifier("SelectCamera");
	fGetNetworkStatistic_id = NPN_GetStringIdentifier("GetNetworkStatistic");
	fGetSpeakerVolume_id = NPN_GetStringIdentifier("GetSpeakerVolume");
	fSetSpeakerVolume_id = NPN_GetStringIdentifier("SetSpeakerVolume");
	fSetDtxEnabled_id = NPN_GetStringIdentifier("SetDtxEnabled");
	fSetSelfPhoneNumber_id = NPN_GetStringIdentifier("SetSelfPhoneNumber");
	fSetSelfName_id = NPN_GetStringIdentifier("SetSelfName");
	fEnableLoudsSpeaker_id = NPN_GetStringIdentifier("EnableLoudsSpeaker");
	fGetLoudsSpeakerStatus_id = NPN_GetStringIdentifier("GetLoudsSpeakerStatus");
	fResetVideoView_id = NPN_GetStringIdentifier("ResetVideoView");
	fSendTextMessage_id = NPN_GetStringIdentifier("SendTextMessage");
	fSendMediaMessage_id = NPN_GetStringIdentifier("SendMediaMessage");
	fDownloadFileMessage_id = NPN_GetStringIdentifier("DownloadFileMessage");
	fCancelUploadOrDownloadNOW_id = NPN_GetStringIdentifier("CancelUploadOrDownloadNOW");
	fDeleteMessage_id = NPN_GetStringIdentifier("DeleteMessage");
	fOperateMessage_id = NPN_GetStringIdentifier("OperateMessage");
	fUploadVTMFile_id = NPN_GetStringIdentifier("UploadVTMFile");
	fUploadVTMBuf_id = NPN_GetStringIdentifier("UploadVTMBuf");
	fCreateGroup_id = NPN_GetStringIdentifier("CreateGroup");
	fDismissGroup_id = NPN_GetStringIdentifier("DismissGroup");
	fQuitGroup_id = NPN_GetStringIdentifier("QuitGroup");
	fJoinGroup_id = NPN_GetStringIdentifier("JoinGroup");
	fReplyRequestJoinGroup_id = NPN_GetStringIdentifier("ReplyRequestJoinGroup");
	fInviteJoinGroup_id = NPN_GetStringIdentifier("InviteJoinGroup");
	fReplyInviteJoinGroup_id = NPN_GetStringIdentifier("ReplyInviteJoinGroup");
	fQueryOwnGroup_id = NPN_GetStringIdentifier("QueryOwnGroup");
	fQueryGroupDetail_id = NPN_GetStringIdentifier("QueryGroupDetail");
	fModifyGroup_id = NPN_GetStringIdentifier("ModifyGroup");
	fSearchPublicGroups_id = NPN_GetStringIdentifier("SearchPublicGroups");
	fQueryGroupMember_id = NPN_GetStringIdentifier("QueryGroupMember");
	fDeleteGroupMember_id = NPN_GetStringIdentifier("DeleteGroupMember");
	fQueryGroupMemberCard_id = NPN_GetStringIdentifier("QueryGroupMemberCard");
	fModifyMemberCard_id = NPN_GetStringIdentifier("ModifyMemberCard");
	fForbidMemberSpeak_id = NPN_GetStringIdentifier("ForbidMemberSpeak");
	fSetGroupMessageRule_id = NPN_GetStringIdentifier("SetGroupMessageRule");
	fStartVoiceRecording_id = NPN_GetStringIdentifier("StartVoiceRecording");
	fStopVoiceRecording_id = NPN_GetStringIdentifier("StopVoiceRecording");
	fPlayVoiceMsg_id = NPN_GetStringIdentifier("PlayVoiceMsg");
	fStopVoiceMsg_id = NPN_GetStringIdentifier("StopVoiceMsg");
	fGetVoiceDuration_id = NPN_GetStringIdentifier("GetVoiceDuration");
	fRequestConferenceMemberVideo_id = NPN_GetStringIdentifier("RequestConferenceMemberVideo");
	fCancelMemberVideo_id = NPN_GetStringIdentifier("CancelMemberVideo");
	fResetVideoConfWindow_id = NPN_GetStringIdentifier("ResetVideoConfWindow");
	fCreateMultimediaMeeting_id = NPN_GetStringIdentifier("CreateMultimediaMeeting");
	fJoinMeeting_id = NPN_GetStringIdentifier("JoinMeeting");
	fExitMeeting_id = NPN_GetStringIdentifier("ExitMeeting");
	fQueryMeetingMembers_id = NPN_GetStringIdentifier("QueryMeetingMembers");
	fDismissMultiMediaMeeting_id = NPN_GetStringIdentifier("DismissMultiMediaMeeting");
	fQueryMultiMediaMeetings_id = NPN_GetStringIdentifier("QueryMultiMediaMeetings");
	fInviteJoinMultiMediaMeeting_id = NPN_GetStringIdentifier("InviteJoinMultiMediaMeeting");
	fDeleteMemberMultiMediaMeeting_id = NPN_GetStringIdentifier("DeleteMemberMultiMediaMeeting");
	fPublishVideo_id = NPN_GetStringIdentifier("PublishVideo");
	fUnpublishVideo_id = NPN_GetStringIdentifier("UnpublishVideo");
	fCreateInterphoneMeeting_id = NPN_GetStringIdentifier("CreateInterphoneMeeting");
	fControlInterphoneMic_id = NPN_GetStringIdentifier("ControlInterphoneMic");
    fSetServerAddress_id = NPN_GetStringIdentifier("SetServerAddress");

	//property id
	pversion_id = NPN_GetStringIdentifier("version");
	pappDataDir_id = NPN_GetStringIdentifier("appDataDir");

	//event id
	onLogInfo_id = NPN_GetStringIdentifier("onLogInfo");
	onLogOut_id = NPN_GetStringIdentifier("onLogOut");
	onConnectState_id = NPN_GetStringIdentifier("onConnectState");
	onSetPersonInfo_id = NPN_GetStringIdentifier("onSetPersonInfo");
	onGetPersonInfo_id = NPN_GetStringIdentifier("onGetPersonInfo");
	onGetUserState_id = NPN_GetStringIdentifier("onGetUserState");
	onPublishPresence_id = NPN_GetStringIdentifier("onPublishPresence");
	onReceiveFriendsPublishPresence_id = NPN_GetStringIdentifier("onReceiveFriendsPublishPresence");
	onSoftVersion_id = NPN_GetStringIdentifier("onSoftVersion");
	onGetOnlineMultiDevice_id = NPN_GetStringIdentifier("onGetOnlineMultiDevice");
	onReceiveMultiDeviceState_id = NPN_GetStringIdentifier("onReceiveMultiDeviceState");
	onMakeCallBack_id = NPN_GetStringIdentifier("onMakeCallBack");
	onCallEvents_id = NPN_GetStringIdentifier("onCallEvents");
	onCallIncoming_id = NPN_GetStringIdentifier("onCallIncoming");
	onDtmfReceived_id = NPN_GetStringIdentifier("onDtmfReceived");
	onSwitchCallMediaTypeRequest_id = NPN_GetStringIdentifier("onSwitchCallMediaTypeRequest");
	onSwitchCallMediaTypeResponse_id = NPN_GetStringIdentifier("onSwitchCallMediaTypeResponse");
	onRemoteVideoRatio_id = NPN_GetStringIdentifier("onRemoteVideoRatio");

	onOfflineMessageCount_id = NPN_GetStringIdentifier("onOfflineMessageCount");
	onGetOfflineMessage_id = NPN_GetStringIdentifier("onGetOfflineMessage");
	onOfflineMessageComplete_id = NPN_GetStringIdentifier("onOfflineMessageComplete");
	onReceiveOfflineMessage_id = NPN_GetStringIdentifier("onReceiveOfflineMessage");
	onReceiveMessage_id = NPN_GetStringIdentifier("onReceiveMessage");
	onReceiveFile_id = NPN_GetStringIdentifier("onReceiveFile");
	onDownloadFileComplete_id = NPN_GetStringIdentifier("onDownloadFileComplete");
	onRateOfProgressAttach_id = NPN_GetStringIdentifier("onRateOfProgressAttach");
	onSendTextMessage_id = NPN_GetStringIdentifier("onSendTextMessage");
	onSendMediaFile_id = NPN_GetStringIdentifier("onSendMediaFile");
	onDeleteMessage_id = NPN_GetStringIdentifier("onDeleteMessage");
	onOperateMessage_id = NPN_GetStringIdentifier("onOperateMessage");
	onReceiveOpreateNoticeMessage_id = NPN_GetStringIdentifier("onReceiveOpreateNoticeMessage");
	onUploadVTMFileOrBuf_id = NPN_GetStringIdentifier("onUploadVTMFileOrBuf");

	/****************************群组接口对应的回调******************************/
	onCreateGroup_id = NPN_GetStringIdentifier("onCreateGroup");
	onDismissGroup_id = NPN_GetStringIdentifier("onDismissGroup");
	onQuitGroup_id = NPN_GetStringIdentifier("onQuitGroup");
	onJoinGroup_id = NPN_GetStringIdentifier("onJoinGroup");
	onReplyRequestJoinGroup_id = NPN_GetStringIdentifier("onReplyRequestJoinGroup");
	onInviteJoinGroup_id = NPN_GetStringIdentifier("onInviteJoinGroup");
	onReplyInviteJoinGroup_id = NPN_GetStringIdentifier("onReplyInviteJoinGroup");
	onQueryOwnGroup_id = NPN_GetStringIdentifier("onQueryOwnGroup");
	onQueryGroupDetail_id = NPN_GetStringIdentifier("onQueryGroupDetail");
	onModifyGroup_id = NPN_GetStringIdentifier("onModifyGroup");
	onSearchPublicGroup_id = NPN_GetStringIdentifier("onSearchPublicGroup");
	onQueryGroupMember_id = NPN_GetStringIdentifier("onQueryGroupMember");
	onDeleteGroupMember_id = NPN_GetStringIdentifier("onDeleteGroupMember");
	onQueryGroupMemberCard_id = NPN_GetStringIdentifier("onQueryGroupMemberCard");
	onModifyGroupMemberCard_id = NPN_GetStringIdentifier("onModifyGroupMemberCard");
	onForbidMemberSpeakGroup_id = NPN_GetStringIdentifier("onForbidMemberSpeakGroup");
	onSetGroupMessageRule_id = NPN_GetStringIdentifier("onSetGroupMessageRule");
	/****************************服务器下发的群组相关通知******************************/
	onReceiveGroupNoticeMessage_id = NPN_GetStringIdentifier("onReceiveGroupNoticeMessage");

	/****************************音频播放回调接口******************************/
	onRecordingTimeOut_id = NPN_GetStringIdentifier("onRecordingTimeOut");
	onFinishedPlaying_id = NPN_GetStringIdentifier("onFinishedPlaying");
	onRecordingAmplitude_id = NPN_GetStringIdentifier("onRecordingAmplitude");

	onMeetingIncoming_id = NPN_GetStringIdentifier("onMeetingIncoming");
	onReceiveInterphoneMeetingMessage_id = NPN_GetStringIdentifier("onReceiveInterphoneMeetingMessage");
	onReceiveVoiceMeetingMessage_id = NPN_GetStringIdentifier("onReceiveVoiceMeetingMessage");
	onReceiveVideoMeetingMessage_id = NPN_GetStringIdentifier("onReceiveVideoMeetingMessage");
	onCreateMultimediaMeeting_id = NPN_GetStringIdentifier("onCreateMultimediaMeeting");
	onJoinMeeting_id = NPN_GetStringIdentifier("onJoinMeeting");
	onExitMeeting_id = NPN_GetStringIdentifier("onExitMeeting");
	onQueryMeetingMembers_id = NPN_GetStringIdentifier("onQueryMeetingMembers");
	onDismissMultiMediaMeeting_id = NPN_GetStringIdentifier("onDismissMultiMediaMeeting");
	onQueryMultiMediaMeetings_id = NPN_GetStringIdentifier("onQueryMultiMediaMeetings");
	onDeleteMemberMultiMediaMeeting_id = NPN_GetStringIdentifier("onDeleteMemberMultiMediaMeeting");
	onInviteJoinMultiMediaMeeting_id = NPN_GetStringIdentifier("onInviteJoinMultiMediaMeeting");
	onCreateInterphoneMeeting_id = NPN_GetStringIdentifier("onCreateInterphoneMeeting");
	onExitInterphoneMeeting_id = NPN_GetStringIdentifier("onExitInterphoneMeeting");
	onSetMeetingSpeakListen_id = NPN_GetStringIdentifier("onSetMeetingSpeakListen");
	onControlInterphoneMic_id = NPN_GetStringIdentifier("onControlInterphoneMic");
	onPublishVideo_id = NPN_GetStringIdentifier("onPublishVideo");
	onUnpublishVideo_id = NPN_GetStringIdentifier("onUnpublishVideo");
	onRequestConferenceMemberVideo_id = NPN_GetStringIdentifier("onRequestConferenceMemberVideo");
	onCancelConferenceMemberVideo_id = NPN_GetStringIdentifier("onCancelConferenceMemberVideo");

	const char *ua = NPN_UserAgent(m_pNPInstance);
	LOG4CPLUS_INFO(log, "UserAgent:" << ua);

}

bool NPAPIECSDK::HasMethod(const NPIdentifier name) const
{
	bool result = (name == fInitialize_id
		|| name == fUnInitialize_id
		|| name == fLogin_id
		|| name == fSetTraceFlag_id
		|| name == fSetServerEnvType_id
		|| name == fSetInternalDNS_id
		|| name == fLogout_id
		|| name == fSetPersonInfo_id
		|| name == fGetPersonInfo_id
		|| name == fGetUserState_id
		|| name == fQueryErrorDescribe_id
		|| name == fMakeCallBack_id
		|| name == fMakeCall_id
		|| name == fAcceptCall_id
		|| name == fReleaseCall_id
		|| name == fSendDTMF_id
		|| name == fGetCurrentCall_id
		|| name == fSetMute_id
		|| name == fGetMuteStatus_id
		|| name == fSetSoftMute_id
		|| name == fGetSoftMuteStatus_id
		|| name == fSetAudioConfigEnabled_id
		|| name == fGetAudioConfigEnabled_id
		|| name == fSetCodecEnabled_id
		|| name == fGetCodecEnabled_id
		|| name == fSetCodecNack_id
		|| name == fGetCodecNack_id
		|| name == fGetCallStatistics_id
		|| name == fSetVideoView_id
		|| name == fSetVideoBitRates_id
		|| name == fSetRing_id
		|| name == fSetRingback_id
		|| name == fRequestSwitchCallMediaType_id
		|| name == fResponseSwitchCallMediaType_id
		|| name == fGetCameraInfo_id
		|| name == fSelectCamera_id
		|| name == fGetNetworkStatistic_id
		|| name == fGetSpeakerVolume_id
		|| name == fSetSpeakerVolume_id
		|| name == fSetDtxEnabled_id
		|| name == fSetSelfPhoneNumber_id
		|| name == fSetSelfName_id
		|| name == fEnableLoudsSpeaker_id
		|| name == fGetLoudsSpeakerStatus_id
		|| name == fResetVideoView_id
		|| name == fSendTextMessage_id
		|| name == fSendMediaMessage_id
		|| name == fDownloadFileMessage_id
		|| name == fCancelUploadOrDownloadNOW_id
		|| name == fDeleteMessage_id
		|| name == fOperateMessage_id
		|| name == fUploadVTMFile_id
		|| name == fUploadVTMBuf_id
		|| name == fCreateGroup_id
		|| name == fDismissGroup_id
		|| name == fQuitGroup_id
		|| name == fJoinGroup_id
		|| name == fReplyRequestJoinGroup_id
		|| name == fInviteJoinGroup_id
		|| name == fReplyInviteJoinGroup_id
		|| name == fQueryOwnGroup_id
		|| name == fQueryGroupDetail_id
		|| name == fModifyGroup_id
		|| name == fSearchPublicGroups_id
		|| name == fQueryGroupMember_id
		|| name == fDeleteGroupMember_id
		|| name == fQueryGroupMemberCard_id
		|| name == fModifyMemberCard_id
		|| name == fForbidMemberSpeak_id
		|| name == fSetGroupMessageRule_id
		|| name == fStartVoiceRecording_id
		|| name == fStopVoiceRecording_id
		|| name == fPlayVoiceMsg_id
		|| name == fStopVoiceMsg_id
		|| name == fGetVoiceDuration_id
		|| name == fRequestConferenceMemberVideo_id
		|| name == fCancelMemberVideo_id
		|| name == fResetVideoConfWindow_id
		|| name == fCreateMultimediaMeeting_id
		|| name == fJoinMeeting_id
		|| name == fExitMeeting_id
		|| name == fQueryMeetingMembers_id
		|| name == fDismissMultiMediaMeeting_id
		|| name == fQueryMultiMediaMeetings_id
		|| name == fInviteJoinMultiMediaMeeting_id
		|| name == fDeleteMemberMultiMediaMeeting_id
		|| name == fPublishVideo_id
		|| name == fUnpublishVideo_id
		|| name == fCreateInterphoneMeeting_id
		|| name == fControlInterphoneMic_id
        || name == fSetServerAddress_id);

	const char * strName = NPN_UTF8FromIdentifier(name);
	if (result)
	{
		LOG4CPLUS_DEBUG(log, getName() << " HasMethod:" << strName);
	}
	else
	{
		LOG4CPLUS_WARN(log, getName() << " NoHasMethod:" << strName);
	}
	NPN_MemFree((void *)strName);

	return result;
}

bool NPAPIECSDK::HasProperty(const NPIdentifier name) const
{
	bool result = (name == this->pversion_id
		|| name == this->pappDataDir_id
		|| name == onLogInfo_id
		|| name == onLogOut_id
		|| name == onConnectState_id
		|| name == onSetPersonInfo_id
		|| name == onGetPersonInfo_id
		|| name == onGetUserState_id
		|| name == onPublishPresence_id
		|| name == onReceiveFriendsPublishPresence_id
		|| name == onSoftVersion_id
		|| name == onGetOnlineMultiDevice_id
		|| name == onReceiveMultiDeviceState_id
		|| name == onMakeCallBack_id
		|| name == onCallEvents_id
		|| name == onCallIncoming_id
		|| name == onDtmfReceived_id
		|| name == onSwitchCallMediaTypeRequest_id
		|| name == onSwitchCallMediaTypeResponse_id
		|| name == onRemoteVideoRatio_id
		|| name == onOfflineMessageCount_id
		|| name == onGetOfflineMessage_id
		|| name == onOfflineMessageComplete_id
		|| name == onReceiveOfflineMessage_id
		|| name == onReceiveMessage_id
		|| name == onReceiveFile_id
		|| name == onDownloadFileComplete_id
		|| name == onRateOfProgressAttach_id
		|| name == onSendTextMessage_id
		|| name == onSendMediaFile_id
		|| name == onDeleteMessage_id
		|| name == onOperateMessage_id
		|| name == onReceiveOpreateNoticeMessage_id
		|| name == onUploadVTMFileOrBuf_id
		|| name == onCreateGroup_id
		|| name == onDismissGroup_id
		|| name == onQuitGroup_id
		|| name == onJoinGroup_id
		|| name == onReplyRequestJoinGroup_id
		|| name == onInviteJoinGroup_id
		|| name == onReplyInviteJoinGroup_id
		|| name == onQueryOwnGroup_id
		|| name == onQueryGroupDetail_id
		|| name == onModifyGroup_id
		|| name == onSearchPublicGroup_id
		|| name == onQueryGroupMember_id
		|| name == onDeleteGroupMember_id
		|| name == onQueryGroupMemberCard_id
		|| name == onModifyGroupMemberCard_id
		|| name == onForbidMemberSpeakGroup_id
		|| name == onSetGroupMessageRule_id
		|| name == onReceiveGroupNoticeMessage_id
		|| name == onRecordingTimeOut_id
		|| name == onFinishedPlaying_id
		|| name == onRecordingAmplitude_id
		|| name == onMeetingIncoming_id
		|| name == onReceiveInterphoneMeetingMessage_id
		|| name == onReceiveVoiceMeetingMessage_id
		|| name == onReceiveVideoMeetingMessage_id
		|| name == onCreateMultimediaMeeting_id
		|| name == onJoinMeeting_id
		|| name == onExitMeeting_id
		|| name == onQueryMeetingMembers_id
		|| name == onDismissMultiMediaMeeting_id
		|| name == onQueryMultiMediaMeetings_id
		|| name == onDeleteMemberMultiMediaMeeting_id
		|| name == onInviteJoinMultiMediaMeeting_id
		|| name == onCreateInterphoneMeeting_id
		|| name == onExitInterphoneMeeting_id
		|| name == onSetMeetingSpeakListen_id
		|| name == onControlInterphoneMic_id
		|| name == onPublishVideo_id
		|| name == onUnpublishVideo_id
		|| name == onRequestConferenceMemberVideo_id
		|| name == onCancelConferenceMemberVideo_id);

	const char * strName = NPN_UTF8FromIdentifier(name);
	if (result)
	{
		LOG4CPLUS_DEBUG(log, getName() << " HasProperty:" << strName);
	}
	else
	{
		LOG4CPLUS_WARN(log, getName() << " NoHasProperty:" << strName);
	}
	NPN_MemFree((void *)strName);
	return result;
}

bool NPAPIECSDK::GetProperty(const NPIdentifier name, NPVariant *result) const
{
	VOID_TO_NPVARIANT(*result);
	bool res = true;
	const char * strName = NPN_UTF8FromIdentifier(name);
	if (name == pversion_id) {
		char* npOutString = (char *)NPN_MemAlloc(this->GetVersion().length() + 1);
		strcpy_s(npOutString,this->GetVersion().length()+1, this->GetVersion().c_str());
		STRINGZ_TO_NPVARIANT(npOutString, *result);
	}
	else if (name == this->pappDataDir_id) {
		char* npOutString = (char *)NPN_MemAlloc(this->getAppDataDir().length() + 1);
		strcpy_s(npOutString, this->getAppDataDir().size() + 1, this->getAppDataDir().c_str());
		STRINGZ_TO_NPVARIANT(npOutString, *result);
	}
	else
	{
		res = false;
		LOG4CPLUS_WARN(log, getName() << " No GetProperty:" << strName);
	}


	NPN_MemFree((void *)strName);
	//LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return res;
}

bool NPAPIECSDK::SetProperty(const NPIdentifier name, const NPVariant *value)
{
	bool res = true;
	if (name == onLogInfo_id){
		if (this->onLogInfo_obj){
			NPN_ReleaseObject(this->onLogInfo_obj);
		}
		this->onLogInfo_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onLogOut_id){
		if (this->onLogOut_obj){
			NPN_ReleaseObject(this->onLogOut_obj);
		}
		this->onLogOut_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onConnectState_id){
		if (this->onConnectState_obj){
			NPN_ReleaseObject(this->onConnectState_obj);
		}
		this->onConnectState_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onSetPersonInfo_id){
		if (this->onSetPersonInfo_obj){
			NPN_ReleaseObject(this->onSetPersonInfo_obj);
		}
		this->onSetPersonInfo_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onGetPersonInfo_id){
		if (this->onGetPersonInfo_obj){
			NPN_ReleaseObject(this->onGetPersonInfo_obj);
		}
		this->onGetPersonInfo_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onGetUserState_id){
		if (this->onGetUserState_obj){
			NPN_ReleaseObject(this->onGetUserState_obj);
		}
		this->onGetUserState_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onPublishPresence_id){
		if (this->onPublishPresence_obj){
			NPN_ReleaseObject(this->onPublishPresence_obj);
		}
		this->onPublishPresence_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onReceiveFriendsPublishPresence_id){
		if (this->onReceiveFriendsPublishPresence_obj){
			NPN_ReleaseObject(this->onReceiveFriendsPublishPresence_obj);
		}
		this->onReceiveFriendsPublishPresence_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onSoftVersion_id){
		if (this->onSoftVersion_obj){
			NPN_ReleaseObject(this->onSoftVersion_obj);
		}
		this->onSoftVersion_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onGetOnlineMultiDevice_id){
		if (this->onGetOnlineMultiDevice_obj){
			NPN_ReleaseObject(this->onGetOnlineMultiDevice_obj);
		}
		this->onGetOnlineMultiDevice_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onReceiveMultiDeviceState_id){
		if (this->onReceiveMultiDeviceState_obj){
			NPN_ReleaseObject(this->onReceiveMultiDeviceState_obj);
		}
		this->onReceiveMultiDeviceState_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onMakeCallBack_id){
		if (this->onMakeCallBack_obj){
			NPN_ReleaseObject(this->onMakeCallBack_obj);
		}
		this->onMakeCallBack_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onCallEvents_id){
		if (this->onCallEvents_obj){
			NPN_ReleaseObject(this->onCallEvents_obj);
		}
		this->onCallEvents_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onCallIncoming_id){
		if (this->onCallIncoming_obj){
			NPN_ReleaseObject(this->onCallIncoming_obj);
		}
		this->onCallIncoming_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onDtmfReceived_id){
		if (this->onDtmfReceived_obj){
			NPN_ReleaseObject(this->onDtmfReceived_obj);
		}
		this->onDtmfReceived_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onSwitchCallMediaTypeRequest_id){
		if (this->onSwitchCallMediaTypeRequest_obj){
			NPN_ReleaseObject(this->onSwitchCallMediaTypeRequest_obj);
		}
		this->onSwitchCallMediaTypeRequest_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onSwitchCallMediaTypeResponse_id){
		if (this->onSwitchCallMediaTypeResponse_obj){
			NPN_ReleaseObject(this->onSwitchCallMediaTypeResponse_obj);
		}
		this->onSwitchCallMediaTypeResponse_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onRemoteVideoRatio_id){
		if (this->onRemoteVideoRatio_obj){
			NPN_ReleaseObject(this->onRemoteVideoRatio_obj);
		}
		this->onRemoteVideoRatio_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onOfflineMessageCount_id){
		if (this->onGetOfflineMessage_obj){
			NPN_ReleaseObject(this->onGetOfflineMessage_obj);
		}
		this->onGetOfflineMessage_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onGetOfflineMessage_id){
		if (this->onGetOfflineMessage_obj){
			NPN_ReleaseObject(this->onGetOfflineMessage_obj);
		}
		this->onGetOfflineMessage_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onOfflineMessageComplete_id){
		if (this->onOfflineMessageComplete_obj){
			NPN_ReleaseObject(this->onOfflineMessageComplete_obj);
		}
		this->onOfflineMessageComplete_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onReceiveOfflineMessage_id){
		if (this->onReceiveOfflineMessage_obj){
			NPN_ReleaseObject(this->onReceiveOfflineMessage_obj);
		}
		this->onReceiveOfflineMessage_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onReceiveMessage_id){
		if (this->onReceiveMessage_obj){
			NPN_ReleaseObject(this->onReceiveMessage_obj);
		}
		this->onReceiveMessage_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onReceiveFile_id){
		if (this->onReceiveFile_obj){
			NPN_ReleaseObject(this->onReceiveFile_obj);
		}
		this->onReceiveFile_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onDownloadFileComplete_id){
		if (this->onDownloadFileComplete_obj){
			NPN_ReleaseObject(this->onDownloadFileComplete_obj);
		}
		this->onDownloadFileComplete_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onRateOfProgressAttach_id){
		if (this->onRateOfProgressAttach_obj){
			NPN_ReleaseObject(this->onRateOfProgressAttach_obj);
		}
		this->onRateOfProgressAttach_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onSendTextMessage_id){
		if (this->onSendTextMessage_obj){
			NPN_ReleaseObject(this->onSendTextMessage_obj);
		}
		this->onSendTextMessage_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onSendMediaFile_id){
		if (this->onSendMediaFile_obj){
			NPN_ReleaseObject(this->onSendMediaFile_obj);
		}
		this->onSendMediaFile_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onDeleteMessage_id){
		if (this->onDeleteMessage_obj){
			NPN_ReleaseObject(this->onDeleteMessage_obj);
		}
		this->onDeleteMessage_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onOperateMessage_id){
		if (this->onOperateMessage_obj){
			NPN_ReleaseObject(this->onOperateMessage_obj);
		}
		this->onOperateMessage_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onReceiveOpreateNoticeMessage_id){
		if (this->onReceiveOpreateNoticeMessage_obj){
			NPN_ReleaseObject(this->onReceiveOpreateNoticeMessage_obj);
		}
		this->onReceiveOpreateNoticeMessage_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onUploadVTMFileOrBuf_id){
		if (this->onUploadVTMFileOrBuf_obj){
			NPN_ReleaseObject(this->onUploadVTMFileOrBuf_obj);
		}
		this->onUploadVTMFileOrBuf_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onCreateGroup_id){
		if (this->onCreateGroup_obj){
			NPN_ReleaseObject(this->onCreateGroup_obj);
		}
		this->onCreateGroup_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onDismissGroup_id){
		if (this->onDismissGroup_obj){
			NPN_ReleaseObject(this->onDismissGroup_obj);
		}
		this->onDismissGroup_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onQuitGroup_id){
		if (this->onQuitGroup_obj){
			NPN_ReleaseObject(this->onQuitGroup_obj);
		}
		this->onQuitGroup_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onJoinGroup_id){
		if (this->onJoinGroup_obj){
			NPN_ReleaseObject(this->onJoinGroup_obj);
		}
		this->onJoinGroup_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onReplyRequestJoinGroup_id){
		if (this->onReplyRequestJoinGroup_obj){
			NPN_ReleaseObject(this->onReplyRequestJoinGroup_obj);
		}
		this->onReplyRequestJoinGroup_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onInviteJoinGroup_id){
		if (this->onInviteJoinGroup_obj){
			NPN_ReleaseObject(this->onInviteJoinGroup_obj);
		}
		this->onInviteJoinGroup_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onReplyInviteJoinGroup_id){
		if (this->onReplyInviteJoinGroup_obj){
			NPN_ReleaseObject(this->onReplyInviteJoinGroup_obj);
		}
		this->onReplyInviteJoinGroup_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onQueryOwnGroup_id){
		if (this->onQueryOwnGroup_obj){
			NPN_ReleaseObject(this->onQueryOwnGroup_obj);
		}
		this->onQueryOwnGroup_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onQueryGroupDetail_id){
		if (this->onQueryGroupDetail_obj){
			NPN_ReleaseObject(this->onQueryGroupDetail_obj);
		}
		this->onQueryGroupDetail_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onModifyGroup_id){
		if (this->onModifyGroup_obj){
			NPN_ReleaseObject(this->onModifyGroup_obj);
		}
		this->onModifyGroup_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onSearchPublicGroup_id){
		if (this->onSearchPublicGroup_obj){
			NPN_ReleaseObject(this->onSearchPublicGroup_obj);
		}
		this->onSearchPublicGroup_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onQueryGroupMember_id){
		if (this->onQueryGroupMember_obj){
			NPN_ReleaseObject(this->onQueryGroupMember_obj);
		}
		this->onQueryGroupMember_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onDeleteGroupMember_id){
		if (this->onDeleteGroupMember_obj){
			NPN_ReleaseObject(this->onDeleteGroupMember_obj);
		}
		this->onDeleteGroupMember_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onQueryGroupMemberCard_id){
		if (this->onQueryGroupMember_obj){
			NPN_ReleaseObject(this->onQueryGroupMember_obj);
		}
		this->onQueryGroupMember_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onModifyGroupMemberCard_id){
		if (this->onModifyGroupMemberCard_obj){
			NPN_ReleaseObject(this->onModifyGroupMemberCard_obj);
		}
		this->onModifyGroupMemberCard_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onForbidMemberSpeakGroup_id){
		if (this->onForbidMemberSpeakGroup_obj){
			NPN_ReleaseObject(this->onForbidMemberSpeakGroup_obj);
		}
		this->onForbidMemberSpeakGroup_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onSetGroupMessageRule_id){
		if (this->onSetGroupMessageRule_obj){
			NPN_ReleaseObject(this->onSetGroupMessageRule_obj);
		}
		this->onSetGroupMessageRule_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onReceiveGroupNoticeMessage_id){
		if (this->onReceiveGroupNoticeMessage_obj){
			NPN_ReleaseObject(this->onReceiveGroupNoticeMessage_obj);
		}
		this->onReceiveGroupNoticeMessage_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onRecordingTimeOut_id){
		if (this->onRecordingTimeOut_obj){
			NPN_ReleaseObject(this->onRecordingTimeOut_obj);
		}
		this->onRecordingTimeOut_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onFinishedPlaying_id){
		if (this->onFinishedPlaying_obj){
			NPN_ReleaseObject(this->onFinishedPlaying_obj);
		}
		this->onFinishedPlaying_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onRecordingAmplitude_id){
		if (this->onRecordingAmplitude_obj){
			NPN_ReleaseObject(this->onRecordingAmplitude_obj);
		}
		this->onRecordingAmplitude_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onMeetingIncoming_id){
		if (this->onMeetingIncoming_obj){
			NPN_ReleaseObject(this->onMeetingIncoming_obj);
		}
		this->onMeetingIncoming_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onReceiveInterphoneMeetingMessage_id){
		if (this->onReceiveInterphoneMeetingMessage_obj){
			NPN_ReleaseObject(this->onReceiveInterphoneMeetingMessage_obj);
		}
		this->onReceiveInterphoneMeetingMessage_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onReceiveVoiceMeetingMessage_id){
		if (this->onReceiveVoiceMeetingMessage_obj){
			NPN_ReleaseObject(this->onReceiveVoiceMeetingMessage_obj);
		}
		this->onReceiveVoiceMeetingMessage_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onReceiveVideoMeetingMessage_id){
		if (this->onReceiveVideoMeetingMessage_obj){
			NPN_ReleaseObject(this->onReceiveVideoMeetingMessage_obj);
		}
		this->onReceiveVideoMeetingMessage_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onCreateMultimediaMeeting_id){
		if (this->onCreateMultimediaMeeting_obj){
			NPN_ReleaseObject(this->onCreateMultimediaMeeting_obj);
		}
		this->onCreateMultimediaMeeting_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onJoinMeeting_id){
		if (this->onJoinMeeting_obj){
			NPN_ReleaseObject(this->onJoinMeeting_obj);
		}
		this->onJoinMeeting_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onExitMeeting_id){
		if (this->onExitMeeting_obj){
			NPN_ReleaseObject(this->onExitMeeting_obj);
		}
		this->onExitMeeting_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onQueryMeetingMembers_id){
		if (this->onQueryMeetingMembers_obj){
			NPN_ReleaseObject(this->onQueryMeetingMembers_obj);
		}
		this->onQueryMeetingMembers_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onDismissMultiMediaMeeting_id){
		if (this->onDismissMultiMediaMeeting_obj){
			NPN_ReleaseObject(this->onDismissMultiMediaMeeting_obj);
		}
		this->onDismissMultiMediaMeeting_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onQueryMultiMediaMeetings_id){
		if (this->onQueryMultiMediaMeetings_obj){
			NPN_ReleaseObject(this->onQueryMultiMediaMeetings_obj);
		}
		this->onQueryMultiMediaMeetings_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onDeleteMemberMultiMediaMeeting_id){
		if (this->onDeleteMemberMultiMediaMeeting_obj){
			NPN_ReleaseObject(this->onDeleteMemberMultiMediaMeeting_obj);
		}
		this->onDeleteMemberMultiMediaMeeting_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onInviteJoinMultiMediaMeeting_id){
		if (this->onInviteJoinMultiMediaMeeting_obj){
			NPN_ReleaseObject(this->onInviteJoinMultiMediaMeeting_obj);
		}
		this->onInviteJoinMultiMediaMeeting_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onCreateInterphoneMeeting_id){
		if (this->onCreateInterphoneMeeting_obj){
			NPN_ReleaseObject(this->onCreateInterphoneMeeting_obj);
		}
		this->onCreateInterphoneMeeting_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onExitInterphoneMeeting_id){
		if (this->onExitInterphoneMeeting_obj){
			NPN_ReleaseObject(this->onExitInterphoneMeeting_obj);
		}
		this->onExitInterphoneMeeting_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onSetMeetingSpeakListen_id){
		if (this->onSetMeetingSpeakListen_obj){
			NPN_ReleaseObject(this->onSetMeetingSpeakListen_obj);
		}
		this->onSetMeetingSpeakListen_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onControlInterphoneMic_id){
		if (this->onControlInterphoneMic_obj){
			NPN_ReleaseObject(this->onControlInterphoneMic_obj);
		}
		this->onControlInterphoneMic_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onPublishVideo_id){
		if (this->onPublishVideo_obj){
			NPN_ReleaseObject(this->onPublishVideo_obj);
		}
		this->onPublishVideo_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onUnpublishVideo_id){
		if (this->onUnpublishVideo_obj){
			NPN_ReleaseObject(this->onUnpublishVideo_obj);
		}
		this->onUnpublishVideo_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onRequestConferenceMemberVideo_id){
		if (this->onRequestConferenceMemberVideo_obj){
			NPN_ReleaseObject(this->onRequestConferenceMemberVideo_obj);
		}
		this->onRequestConferenceMemberVideo_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == onCancelConferenceMemberVideo_id){
		if (this->onCancelConferenceMemberVideo_obj){
			NPN_ReleaseObject(this->onCancelConferenceMemberVideo_obj);
		}
		this->onCancelConferenceMemberVideo_obj = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));
	}
	else if (name == this->pappDataDir_id) {
		res = false;
		LOG4CPLUS_WARN(log, __FUNCTION__ << " appDataDir is readonly property.");
	}
	else{
		res = false;
	}
	return res;
}

bool NPAPIECSDK::RemoveProperty(const NPIdentifier name) const
{
	return false;
}

bool NPAPIECSDK::Invoke(const NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	if (name == fInitialize_id){
		int r = ECSDKBase::Initialize();
		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fUnInitialize_id){
		int r = ECSDKBase::UnInitialize();
		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fLogin_id){
		if (argCount < 1)
		{
			LOG4CPLUS_ERROR(log, "Login missing parameter.");
			return false;
		}
		if (args[0].type != NPVariantType_Object)
		{
			LOG4CPLUS_ERROR(log, "Login parameter Wrong format");
			return false;
		}
		NPVariant varValue;
		char userName[256];
		char userPassword[256];
		char appKey[256];
		char appToken[256];
		char timestamp[256];
		char MD5Token[256];

		ECLoginInfo info;
		info.authType = LoginAuthType_NormalAuth;

		if (JSGetProperty(args[0].value.objectValue, "authType", &varValue))
		{
			int32_t autType;
			NPVARIANT_TO_INT32_SAFE(varValue, autType);
			info.authType = (LoginAuthType)autType;
		}
		if (JSGetProperty(args[0].value.objectValue, "username", &varValue)){
			std::string utf8UserName;
			if (NPVARIANT_IS_STRING(varValue))
				NPVARIANT_TO_STRING_SAFE(varValue, utf8UserName);
			strcpy_s(userName, utf8UserName.c_str());

			info.username = userName;
		}
		if (JSGetProperty(args[0].value.objectValue, "userPassword", &varValue)){
			std::string utf8Password;
			if (NPVARIANT_IS_STRING(varValue))
				NPVARIANT_TO_STRING_SAFE(varValue, utf8Password);
			strcpy_s(userPassword, utf8Password.c_str());

			info.userPassword = userPassword;
		}
		if (JSGetProperty(args[0].value.objectValue, "appKey", &varValue)){
			std::string utf8AppKey;
			if (NPVARIANT_IS_STRING(varValue))
				NPVARIANT_TO_STRING_SAFE(varValue, utf8AppKey);
			strcpy_s(appKey, utf8AppKey.c_str());

			info.appKey = appKey;
		}
		if (JSGetProperty(args[0].value.objectValue, "appToken", &varValue)){
			std::string utf8AppToken;
			if (NPVARIANT_IS_STRING(varValue))
				NPVARIANT_TO_STRING_SAFE(varValue, utf8AppToken);
			strcpy_s(appToken, utf8AppToken.c_str());

			info.appToken = appToken;
		}
		if (JSGetProperty(args[0].value.objectValue, "timestamp", &varValue)){
			std::string utf8Timestamp;
			if (NPVARIANT_IS_STRING(varValue))
				NPVARIANT_TO_STRING_SAFE(varValue, utf8Timestamp);
			strcpy_s(timestamp, utf8Timestamp.c_str());

			info.timestamp = timestamp;
		}
		if (JSGetProperty(args[0].value.objectValue, "MD5Token", &varValue)){
			std::string utf8MD5Token;
			if (NPVARIANT_IS_STRING(varValue))
				NPVARIANT_TO_STRING_SAFE(varValue, utf8MD5Token);
			strcpy_s(MD5Token, utf8MD5Token.c_str());

			info.MD5Token = MD5Token;
		}
		int r = ECSDKBase::Login(&info);
		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fSetTraceFlag_id){
		int32_t level = 0;
		std::string utf8FileName;
		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "SetTraceFlag missing parameter.");
			return false;
		}

		NPVARIANT_TO_INT32_SAFE(args[0], level);
		if (NPVARIANT_IS_STRING(args[1]))
			NPVARIANT_TO_STRING_SAFE(args[1], utf8FileName);

		ECSDKBase::SetTraceFlag(level, utf8FileName.c_str());
		VOID_TO_NPVARIANT(*result);

		return true;
	}
	else if (name == fSetServerEnvType_id){
		int32_t nEnvType;
		std::string utf8FileName;
		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "SetServerEnvType missing parameter.");
			return false;
		}

		NPVARIANT_TO_INT32_SAFE(args[0], nEnvType);
		if (NPVARIANT_IS_STRING(args[1]))
			NPVARIANT_TO_STRING_SAFE(args[1], utf8FileName);

		int r = ECSDKBase::SetServerEnvType(nEnvType, utf8FileName.c_str());
		INT32_TO_NPVARIANT(r,*result);

		return true;
	}
	else if (name == fSetInternalDNS_id){
		bool enabled = false;
		std::string utf8DNS;
		int32_t port = 0;

		if (argCount < 3){
			LOG4CPLUS_ERROR(log, "SetInternalDNS missing parameter.");
			return false;
		}

		NPVARIANT_TO_BOOLEAN_SAFE(args[0], enabled);
		if (NPVARIANT_IS_STRING(args[1]))
			NPVARIANT_TO_STRING_SAFE(args[1], utf8DNS);
		NPVARIANT_TO_INT32_SAFE(args[2], port);

		int r = ECSDKBase::SetInternalDNS(enabled, utf8DNS.c_str(), port);
		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fLogout_id){
		int r = ECSDKBase::Logout();
		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fSetPersonInfo_id){
		if (argCount < 2)
		{
			LOG4CPLUS_ERROR(log, "SetPersonInfo missing parameter.");
			return false;
		}
		if (!NPVARIANT_IS_OBJECT(args[0]) || !NPVARIANT_IS_OBJECT(args[1]))
		{
			LOG4CPLUS_ERROR(log, "SetPersonInfo parameter Wrong format");
			return false;
		}
		ECPersonInfo info;
		unsigned int rmatchKey = 0;
		NPVariant varValue;

		if (JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "birth", &varValue)){
			std::string birth;
			if (NPVARIANT_IS_STRING(varValue))
				NPVARIANT_TO_STRING_SAFE(varValue, birth);
			strcpy_s(info.birth, birth.c_str());
		}

		if (JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "nickName", &varValue)){
			std::string nickName;
			if (NPVARIANT_IS_STRING(varValue))
				NPVARIANT_TO_STRING_SAFE(varValue, nickName);
			strcpy_s(info.nickName, nickName.c_str());
		}

		if (JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "sex", &varValue)){
			int32_t sex = -1;
			NPVARIANT_TO_INT32_SAFE(varValue, sex);
			info.sex = sex;
		}

		if (JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "sign", &varValue)){
			std::string sign;
			if (NPVARIANT_IS_STRING(varValue))
				NPVARIANT_TO_STRING_SAFE(varValue, sign);
			strcpy_s(info.sign, sign.c_str());
		}

		if (JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "version", &varValue)){
			int32_t version = -1;
			NPVARIANT_TO_INT32_SAFE(varValue, version);
			info.version = version;
		}

		int r = ECSDKBase::SetPersonInfo(&rmatchKey, &info);
		NPVariant matchKey;
		INT32_TO_NPVARIANT(rmatchKey, matchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), matchKey);
		INT32_TO_NPVARIANT(r, *result);

		return true;
	}
	else if (name == fGetPersonInfo_id){
		if (argCount < 2)
		{
			LOG4CPLUS_ERROR(log, "GetPersonInfo missing parameter.");
			return false;
		}
		if (!NPVARIANT_IS_OBJECT(args[0]))
		{
			LOG4CPLUS_ERROR(log, "GetPersonInfo parameter Wrong format");
			return false;
		}
		unsigned int rmatchKey;
		std::string utf8Username;
		if (NPVARIANT_IS_STRING(args[1]))
			NPVARIANT_TO_STRING_SAFE(args[1], utf8Username);

		int32_t r = ECSDKBase::GetPersonInfo(&rmatchKey, utf8Username.c_str());

		NPVariant matchKey;
		INT32_TO_NPVARIANT(rmatchKey, matchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), matchKey);
		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fGetUserState_id){
		if (argCount < 2)
		{
			LOG4CPLUS_ERROR(log, "GetUserState missing parameter.");
			return false;
		}
		if (!NPVARIANT_IS_OBJECT(args[0]) || !NPVARIANT_IS_OBJECT(args[1]))
		{
			LOG4CPLUS_ERROR(log, "GetUserState parameter Wrong format");
			return false;
		}
		unsigned int rmatchKey;
		NPVariant varValue;

		if (JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "length", &varValue)){
			int32_t length = 0;
			NPVARIANT_TO_INT32_SAFE(varValue, length);
			std::vector<std::string>strArray(length);
			std::vector<const char *> arr(length);

			for (int i = 0; i < length; i++)
			{
				NPVariant element;
				if (JSGetArrayElementOfIndex(NPVARIANT_TO_OBJECT(args[1]), i, &element))
				{
					NPVARIANT_TO_STRING_SAFE(element, strArray[i]);
					arr[i] = strArray[i].c_str();
				}
			}

			int r = ECSDKBase::GetUserState(&rmatchKey, arr.data(), length);
			NPVariant matchKey;
			INT32_TO_NPVARIANT(rmatchKey, matchKey);
			JSSetValue(NPVARIANT_TO_OBJECT(args[0]), matchKey);
			INT32_TO_NPVARIANT(r, *result);
		}
		else{
			LOG4CPLUS_ERROR(log, __FUNCTION__ " Parameter not array.");
			int r = ECSDKBase::GetUserState(&rmatchKey, nullptr, 0);
			NPVariant matchKey;
			INT32_TO_NPVARIANT(rmatchKey, matchKey);
			JSSetValue(NPVARIANT_TO_OBJECT(args[0]), matchKey);
			INT32_TO_NPVARIANT(r, *result);
		}
		return true;
	}
	else if (name == fQueryErrorDescribe_id){
		if (argCount < 2)
		{
			LOG4CPLUS_ERROR(log, "QueryErrorDescribe missing parameter.");
			return false;
		}
		if (!NPVARIANT_IS_OBJECT(args[0]))
		{
			LOG4CPLUS_ERROR(log, "QueryErrorDescribe parameter Wrong format");
			return false;
		}

		const char * errBuf = nullptr;
		int32_t errCode = 0;
		NPVARIANT_TO_INT32_SAFE(args[1], errCode);

		int r = ECSDKBase::QueryErrorDescribe(&errBuf, errCode);
		NPVariant errDes;
		char * npErrBuf = (char*)NPN_MemAlloc(strlen(errBuf) + 1);
		strncpy_s(npErrBuf, strlen(errBuf) + 1, errBuf, strlen(errBuf));
		STRINGZ_TO_NPVARIANT(npErrBuf, errDes);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), errDes);

		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fMakeCallBack_id){
		if (argCount < 5)
		{
			LOG4CPLUS_ERROR(log, "MakeCallBack missing parameter.");
			return false;
		}
		if (!NPVARIANT_IS_OBJECT(args[0]))
		{
			LOG4CPLUS_ERROR(log, "MakeCallBack parameter Wrong format");
			return false;
		}
		unsigned int rmatchKey;
		std::string utf8Caller;
		std::string utf8Called;
		std::string utf8CallerDisplay;
		std::string utf8CalledDisplay;

		if (NPVARIANT_IS_STRING(args[1]))
			NPVARIANT_TO_STRING_SAFE(args[1], utf8Caller);
		if (NPVARIANT_IS_STRING(args[2]))
			NPVARIANT_TO_STRING_SAFE(args[2], utf8Called);
		if (NPVARIANT_IS_STRING(args[3]))
			NPVARIANT_TO_STRING_SAFE(args[3], utf8CallerDisplay);
		if (NPVARIANT_IS_STRING(args[4]))
			NPVARIANT_TO_STRING_SAFE(args[4], utf8CalledDisplay);
	

		int r = ECSDKBase::MakeCallBack(&rmatchKey, utf8Caller.c_str(), utf8Called.c_str(),
			utf8CallerDisplay.c_str(), utf8CalledDisplay.c_str());

		NPVariant matchKey;
		INT32_TO_NPVARIANT(rmatchKey, matchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), matchKey);
		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fMakeCall_id){
		if (argCount < 3)
		{
			LOG4CPLUS_ERROR(log, "MakeCall missing parameter.");
			return false;
		}
		if (!NPVARIANT_IS_OBJECT(args[0]))
		{
			LOG4CPLUS_ERROR(log, "MakeCall parameter Wrong format");
			return false;
		}
		const char * rOutCallid = nullptr;
		int callType = 0;
		std::string utf8Called;

		NPVARIANT_TO_INT32_SAFE(args[1], callType);
		if (NPVARIANT_IS_STRING(args[2]))
			NPVARIANT_TO_STRING_SAFE(args[2], utf8Called);

		int r = ECSDKBase::MakeCall(&rOutCallid, callType, utf8Called.c_str());
		if (rOutCallid == nullptr){
			rOutCallid = "";
		}

		NPVariant callid;
		char * npCallid = (char*)NPN_MemAlloc(strlen(rOutCallid) + 1);
		strncpy_s(npCallid, strlen(rOutCallid) + 1, rOutCallid, strlen(rOutCallid));
		STRINGZ_TO_NPVARIANT(npCallid, callid);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), callid);

		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fAcceptCall_id){
		if (argCount < 1){
			LOG4CPLUS_ERROR(log, "AcceptCall missing parameter.");
			return false;
		}
		
		std::string utf8Callid;

		if (NPVARIANT_IS_STRING(args[0]))
			NPVARIANT_TO_STRING_SAFE(args[0], utf8Callid);

		int r = ECSDKBase::AcceptCall(utf8Callid.c_str());

		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fReleaseCall_id){
		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "ReleaseCall missing parameter.");
			return false;
		}

		std::string utf8Callid;
		int32_t reason = 0;

		if (NPVARIANT_IS_STRING(args[0]))
			NPVARIANT_TO_STRING_SAFE(args[0], utf8Callid);
		NPVARIANT_TO_INT32_SAFE(args[1], reason);

		int r = ECSDKBase::ReleaseCall(utf8Callid.c_str(), reason);

		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fSendDTMF_id){
		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "SendDTMF missing parameter.");
			return false;
		}

		std::string utf8Callid;
		std::string utf8DTMF;

		if (NPVARIANT_IS_STRING(args[0]))
			NPVARIANT_TO_STRING_SAFE(args[0], utf8Callid);

		if (NPVARIANT_IS_STRING(args[1]))
			NPVARIANT_TO_STRING_SAFE(args[1],utf8DTMF);

		int r = ECSDKBase::SendDTMF(utf8Callid.c_str(), utf8DTMF.at(0));

		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fGetCurrentCall_id){
		const char * callid = ECSDKBase::GetCurrentCall();
		if (callid == nullptr){
			callid = "";
		}

		char* npOutString = (char *)NPN_MemAlloc(strlen(callid)+1);
		strcpy_s(npOutString, strlen(callid)+1, callid);
		STRINGZ_TO_NPVARIANT(npOutString, *result);
		return true;
	}
	else if (name == fSetMute_id){
		bool mute = true;

		if (argCount < 1){
			LOG4CPLUS_ERROR(log, "SetMute missing parameter.");
			return false;
		}

		NPVARIANT_TO_BOOLEAN_SAFE(args[0], mute);

		int r = ECSDKBase::SetMute(mute);
		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fGetMuteStatus_id){
		bool mute = ECSDKBase::GetMuteStatus();
		BOOLEAN_TO_NPVARIANT(mute, *result);
		return true;
	}
	else if (name == fSetSoftMute_id)
	{
		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "SetSoftMute missing parameter.");
			return false;
		}

		std::string utf8Callid;
		bool mute;

		if (NPVARIANT_IS_STRING(args[0]))
			NPVARIANT_TO_STRING_SAFE(args[0], utf8Callid);

		NPVARIANT_TO_BOOLEAN_SAFE(args[1], mute);

		int r = ECSDKBase::SetSoftMute(utf8Callid.c_str(), mute);

		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fGetSoftMuteStatus_id)
	{
		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "GetSoftMuteStatus missing parameter.");
			return false;
		}

		std::string utf8Callid;
		bool mute = true;

		if (NPVARIANT_IS_STRING(args[0]))
			NPVARIANT_TO_STRING_SAFE(args[0], utf8Callid);

		if (!NPVARIANT_IS_OBJECT(args[1]))
		{
			LOG4CPLUS_ERROR(log, "GetSoftMuteStatus parameter Wrong format");
			return false;
		}

		int r = ECSDKBase::GetSoftMuteStatus(utf8Callid.c_str(), &mute);
		NPVariant varMute;
		BOOLEAN_TO_NPVARIANT(mute, varMute);
		JSSetValue(NPVARIANT_TO_OBJECT(args[1]), varMute);

		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fSetAudioConfigEnabled_id){

		if (argCount < 3){
			LOG4CPLUS_ERROR(log, "SetAudioConfigEnabled missing parameter.");
			return false;
		}

		int32_t type = 0;
		bool enabled = true;
		int32_t mode = 0;

		NPVARIANT_TO_INT32_SAFE(args[0], type);
		NPVARIANT_TO_BOOLEAN_SAFE(args[1], enabled);
		NPVARIANT_TO_INT32_SAFE(args[2], mode);

		int r = ECSDKBase::SetAudioConfigEnabled(type, enabled, mode);
		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fGetAudioConfigEnabled_id){

		if (argCount < 3){
			LOG4CPLUS_ERROR(log, "GetAudioConfigEnabled missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[1]) || !NPVARIANT_IS_OBJECT(args[2]))
		{
			LOG4CPLUS_ERROR(log, "GetAudioConfigEnabled parameter Wrong format");
			return false;
		}

		int32_t type = 0;
		bool enabled;
		int mode;

		NPVARIANT_TO_INT32_SAFE(args[0], type);

		int r = ECSDKBase::GetAudioConfigEnabled(type, &enabled, &mode);

		NPVariant varEnabled;
		BOOLEAN_TO_NPVARIANT(enabled, varEnabled);
		JSSetValue(NPVARIANT_TO_OBJECT(args[1]), varEnabled);

		NPVariant varMode;
		INT32_TO_NPVARIANT(mode, varMode);
		JSSetValue(NPVARIANT_TO_OBJECT(args[2]), varMode);

		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fSetCodecEnabled_id){

		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "SetCodecEnabled missing parameter.");
			return false;
		}

		int32_t type = 0;
		bool enabled = true;

		NPVARIANT_TO_INT32_SAFE(args[0], type);
		NPVARIANT_TO_BOOLEAN_SAFE(args[1], enabled);

		int r = ECSDKBase::SetCodecEnabled(type, enabled);
		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fGetCodecEnabled_id){

		if (argCount < 1){
			LOG4CPLUS_ERROR(log, "GetCodecEnabled missing parameter.");
			return false;
		}

		int32_t type = 0;

		NPVARIANT_TO_INT32_SAFE(args[0], type);

		bool r = ECSDKBase::GetCodecEnabled(type);
		BOOLEAN_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fSetCodecNack_id){
		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "SetCodecNack missing parameter.");
			return false;
		}

		bool bAudioNack = false;
		bool bVideoNack = false;

		NPVARIANT_TO_BOOLEAN_SAFE(args[0], bAudioNack);
		NPVARIANT_TO_BOOLEAN_SAFE(args[1], bVideoNack);

		int32_t r = ECSDKBase::SetCodecNack(bAudioNack, bVideoNack);
		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fGetCodecNack_id){
		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "GetCodecNack missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0]) || !NPVARIANT_IS_OBJECT(args[1]))
		{
			LOG4CPLUS_ERROR(log, "GetCodecNack parameter Wrong format");
			return false;
		}


		bool bAudioNack = false;
		bool bVideoNack = false;


		int32_t r = ECSDKBase::GetCodecNack(&bAudioNack, &bVideoNack);
		NPVariant varAudioNack;
		BOOLEAN_TO_NPVARIANT(bAudioNack, varAudioNack);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varAudioNack);

		NPVariant varVideoNack;
		BOOLEAN_TO_NPVARIANT(bVideoNack, varVideoNack);
		JSSetValue(NPVARIANT_TO_OBJECT(args[1]), varVideoNack);

		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fGetCallStatistics_id){

		if (argCount < 3){
			LOG4CPLUS_ERROR(log, "GetCallStatistics missing parameter.");
			return false;
		}

		std::string callid;
		bool bVideo = false;

		if (NPVARIANT_IS_STRING(args[0])){
			NPVARIANT_TO_STRING_SAFE(args[0], callid);
		}

		NPVARIANT_TO_BOOLEAN_SAFE(args[1], bVideo);

		ECMediaStatisticsInfo info;
		int32_t r = ECSDKBase::GetCallStatistics(callid.c_str(), bVideo, &info);

		NPVariant varByteReceived;
		INT32_TO_NPVARIANT(info.bytesReceived, varByteReceived);
		JSSetProperty(NPVARIANT_TO_OBJECT(args[2]), "bytesReceived", varByteReceived);

		NPVariant varByteSent;
		INT32_TO_NPVARIANT(info.bytesSent, varByteSent);
		JSSetProperty(NPVARIANT_TO_OBJECT(args[2]), "bytesSent", varByteSent);

		NPVariant varCumulativeLost;
		INT32_TO_NPVARIANT(info.cumulativeLost, varCumulativeLost);
		JSSetProperty(NPVARIANT_TO_OBJECT(args[2]), "cumulativeLost", varCumulativeLost);

		NPVariant varExtendeMax;
		INT32_TO_NPVARIANT(info.extendedMax, varExtendeMax);
		JSSetProperty(NPVARIANT_TO_OBJECT(args[2]), "extendedMax", varExtendeMax);

		NPVariant varFractionLost;
		INT32_TO_NPVARIANT(info.fractionLost, varFractionLost);
		JSSetProperty(NPVARIANT_TO_OBJECT(args[2]), "fractionLost", varFractionLost);

		NPVariant varJitterSamples;
		INT32_TO_NPVARIANT(info.jitterSamples, varJitterSamples);
		JSSetProperty(NPVARIANT_TO_OBJECT(args[2]), "jitterSamples", varJitterSamples);

		NPVariant varPacketRecevived;
		INT32_TO_NPVARIANT(info.packetsReceived, varPacketRecevived);
		JSSetProperty(NPVARIANT_TO_OBJECT(args[2]), "packetsReceived", varPacketRecevived);

		NPVariant varPacketSent;
		INT32_TO_NPVARIANT(info.packetsSent, varPacketSent);
		JSSetProperty(NPVARIANT_TO_OBJECT(args[2]), "packetsSent", varPacketSent);

		NPVariant varRttMs;
		INT32_TO_NPVARIANT(info.rttMs, varRttMs);
		JSSetProperty(NPVARIANT_TO_OBJECT(args[2]), "rttMs", varRttMs);

		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fSetVideoView_id){

		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "SetVideoView missing parameter.");
			return false;
		}
		int32_t remoteView = 0;
		int32_t localView = 0;

		NPVARIANT_TO_INT32_SAFE(args[0], remoteView);
		NPVARIANT_TO_INT32_SAFE(args[1], localView);

		int32_t r = ECSDKBase::SetVideoView((void*)remoteView, (void*)localView);
		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fSetVideoBitRates_id){
		if (argCount < 1){
			LOG4CPLUS_ERROR(log, "SetVideoBitRates missing parameter.");
			return false;
		}
		int32_t bitrates = 0;

		NPVARIANT_TO_INT32_SAFE(args[0], bitrates);
		ECSDKBase::SetVideoBitRates(bitrates);
		return true;
	}
	else if (name == fSetRing_id){

		if (argCount < 1){
			LOG4CPLUS_ERROR(log, "SetRing missing parameter.");
			return false;
		}
		std::string fileName;
		if (NPVARIANT_IS_STRING(args[0])){
			NPVARIANT_TO_STRING_SAFE(args[0], fileName);
		}

		int r = ECSDKBase::SetRing(fileName.c_str());
		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fSetRingback_id){

		if (argCount < 1){
			LOG4CPLUS_ERROR(log, "SetRingback missing parameter.");
			return false;
		}
		std::string fileName;
		if (NPVARIANT_IS_STRING(args[0])){
			NPVARIANT_TO_STRING_SAFE(args[0], fileName);
		}

		int r = ECSDKBase::SetRingback(fileName.c_str());
		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fRequestSwitchCallMediaType_id){

		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "RequestSwitchCallMediaType missing parameter.");
			return false;
		}
		std::string callId;
		int32_t video = 0;
		if (NPVARIANT_IS_STRING(args[0])){
			NPVARIANT_TO_STRING_SAFE(args[0], callId);
		}
		NPVARIANT_TO_INT32_SAFE(args[1], video);

		int32_t r = ECSDKBase::RequestSwitchCallMediaType(callId.c_str(), video);
		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fResponseSwitchCallMediaType_id){

		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "ResponseSwitchCallMediaType missing parameter.");
			return false;
		}
		std::string callId;
		int32_t video = 0;
		if (NPVARIANT_IS_STRING(args[0])){
			NPVARIANT_TO_STRING_SAFE(args[0], callId);
		}
		NPVARIANT_TO_INT32_SAFE(args[1], video);

		int r = ECSDKBase::ResponseSwitchCallMediaType(callId.c_str(), video);
		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fGetCameraInfo_id){

		if (argCount < 1){
			LOG4CPLUS_ERROR(log, "GetCameraInfo missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "GetCameraInfo parameter Wrong format");
			return false;
		}

		NPObject * cameraInfo = NPVARIANT_TO_OBJECT(args[0]);

		ECCameraInfo * info = nullptr;
		int32_t r = ECSDKBase::GetCameraInfo(&info);
		INT32_TO_NPVARIANT(r, *result);

		//JSNewArray(pScript, & cameraInfo);

		for (int nIndex = 0; nIndex < r; nIndex++)
		{
			NPVariant cameraObj;
			bool result = JSNewObject(&cameraObj);
			
			NPVariant varIndex;
			INT32_TO_NPVARIANT(info[nIndex].index, varIndex);
			JSSetProperty(NPVARIANT_TO_OBJECT(cameraObj), "index", varIndex);

			NPVariant varId;
			char * npId = (char*)NPN_MemAlloc(strlen(info[nIndex].id) + 1);
			strncpy_s(npId, strlen(info[nIndex].id) + 1, info[nIndex].id, strlen(info[nIndex].id));
			STRINGZ_TO_NPVARIANT(npId, varId);
			JSSetProperty(NPVARIANT_TO_OBJECT(cameraObj), "id", varId);

			NPVariant varName;
			char * npName = (char*)NPN_MemAlloc(strlen(info[nIndex].name) + 1);
			strncpy_s(npName, strlen(info[nIndex].name) + 1, info[nIndex].name, strlen(info[nIndex].name));
			STRINGZ_TO_NPVARIANT(npName, varName);
			JSSetProperty(NPVARIANT_TO_OBJECT(cameraObj), "name", varName);

			NPVariant varCapabilityCount;
			INT32_TO_NPVARIANT(info[nIndex].capabilityCount, varCapabilityCount);
			JSSetProperty(NPVARIANT_TO_OBJECT(cameraObj), "capabilityCount", varCapabilityCount);

			NPVariant  capabilityArray;
			JSNewArray(&capabilityArray);

			ECCameraCapability * pCapability = info[nIndex].capability;
			for (int CapabilityIndex = 0; CapabilityIndex < info[nIndex].capabilityCount; CapabilityIndex++)
			{
				NPVariant capabilityObj;
				JSNewObject(&capabilityObj);

				NPVariant varWidth;
				INT32_TO_NPVARIANT(pCapability[CapabilityIndex].width, varWidth);
				JSSetProperty(NPVARIANT_TO_OBJECT(capabilityObj), "width", varWidth);

				NPVariant varHeight;
				INT32_TO_NPVARIANT(pCapability[CapabilityIndex].height, varHeight);
				JSSetProperty(NPVARIANT_TO_OBJECT(capabilityObj), "height", varHeight);

				NPVariant varMaxfps;
				INT32_TO_NPVARIANT(pCapability[CapabilityIndex].maxfps, varMaxfps);
				JSSetProperty(NPVARIANT_TO_OBJECT(capabilityObj), "maxfps", varMaxfps);

				JSAddArrayElement(NPVARIANT_TO_OBJECT(capabilityArray), capabilityObj);
			}

			JSSetProperty(NPVARIANT_TO_OBJECT(cameraObj), "capability", capabilityArray);

			JSAddArrayElement(cameraInfo, cameraObj);
		}
		return true;
	}
	else if (name == fSelectCamera_id){
		if (argCount < 5){
			LOG4CPLUS_ERROR(log, "SelectCamera missing parameter.");
			return false;
		}
		int32_t cameraIndex = 0;
		int32_t capabilityIndex = 0;
		int32_t fps = 0;
		int32_t rotate = 0;
		bool force = false;

		NPVARIANT_TO_INT32_SAFE(args[0], cameraIndex);
		NPVARIANT_TO_INT32_SAFE(args[1], capabilityIndex);
		NPVARIANT_TO_INT32_SAFE(args[2], fps);
		NPVARIANT_TO_INT32_SAFE(args[3], rotate);
		NPVARIANT_TO_BOOLEAN_SAFE(args[4], force);

		int32_t r = ECSDKBase::SelectCamera(cameraIndex, capabilityIndex, fps, rotate, force);
		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fGetNetworkStatistic_id){

		if (argCount < 4){
			LOG4CPLUS_ERROR(log, "GetNetworkStatistic missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[1]) || !NPVARIANT_IS_OBJECT(args[2]) || !NPVARIANT_IS_OBJECT(args[3])){
			LOG4CPLUS_ERROR(log, "GetNetworkStatistic parameter Wrong format");
			return false;
		}

		std::string callId;
		long long duration;
		long long sendTotalWifi;
		long long recvTotalWifi;
		int32_t r = ECSDKBase::GetNetworkStatistic(callId.c_str(), &duration, &sendTotalWifi, &recvTotalWifi);
		INT32_TO_NPVARIANT(r, *result);

		NPVariant varDuration;
		INT32_TO_NPVARIANT(duration, varDuration);
		JSSetValue(NPVARIANT_TO_OBJECT(args[1]), varDuration);

		NPVariant varSendTotalWifi;
		INT32_TO_NPVARIANT(sendTotalWifi, varSendTotalWifi);
		JSSetValue(NPVARIANT_TO_OBJECT(args[2]), varSendTotalWifi);

		NPVariant varRecvTotalWifi;
		INT32_TO_NPVARIANT(recvTotalWifi, varRecvTotalWifi);
		JSSetValue(NPVARIANT_TO_OBJECT(args[3]), varRecvTotalWifi);

		return true;
	}
	else if (name == fGetSpeakerVolume_id){

		if (argCount < 1){
			LOG4CPLUS_ERROR(log, "GetSpeakerVolume missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "GetSpeakerVolume parameter Wrong format");
			return false;
		}
		unsigned int volume;
		int32_t r = ECSDKBase::GetSpeakerVolume(&volume);
		INT32_TO_NPVARIANT(r, *result);

		NPVariant varVolume;
		INT32_TO_NPVARIANT(volume, varVolume);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varVolume);

		return true;
	}
	else if (name == fSetSpeakerVolume_id){

		if (argCount < 1){
			LOG4CPLUS_ERROR(log, "SetSpeakerVolume missing parameter.");
			return false;
		}

		int32_t volume;
		NPVARIANT_TO_INT32_SAFE(args[0], volume);

		int32_t r = ECSDKBase::SetSpeakerVolume(volume);
		INT32_TO_NPVARIANT(r, *result);

		return true;
	}
	else if (name == fSetDtxEnabled_id){

		if (argCount < 1){
			LOG4CPLUS_ERROR(log, "SetDtxEnabled missing parameter.");
			return false;
		}

		bool enabled = false;
		NPVARIANT_TO_BOOLEAN_SAFE(args[0], enabled);

		ECSDKBase::SetDtxEnabled(enabled);

		return true;
	}
	else if (name == fSetSelfPhoneNumber_id){

		if (argCount < 1){
			LOG4CPLUS_ERROR(log, "SetSelfPhoneNumber missing parameter.");
			return false;
		}

		std::string  phoneNumber;
		if (NPVARIANT_IS_STRING(args[0])){
			NPVARIANT_TO_STRING_SAFE(args[0], phoneNumber);
		}

		ECSDKBase::SetSelfPhoneNumber(phoneNumber.c_str());

		return true;
	}
	else if (name == fSetSelfName_id){

		if (argCount < 1){
			LOG4CPLUS_ERROR(log, "SetSelfName missing parameter.");
			return false;
		}

		std::string  name;
		if (NPVARIANT_IS_STRING(args[0])){
			NPVARIANT_TO_STRING_SAFE(args[0], name);
		}

		ECSDKBase::SetSelfName(name.c_str());

		return true;
	}
	else if (name == fEnableLoudsSpeaker_id){

		if (argCount < 1){
			LOG4CPLUS_ERROR(log, "EnableLoudsSpeaker missing parameter.");
			return false;
		}

		bool enabled = false;
		NPVARIANT_TO_BOOLEAN_SAFE(args[0], enabled);
	
		int32_t r = ECSDKBase::EnableLoudsSpeaker(enabled);
		INT32_TO_NPVARIANT(r, *result);

		return true;
	}
	else if (name == fGetLoudsSpeakerStatus_id)
	{
		bool status = false;
		status = ECSDKBase::GetLoudsSpeakerStatus();
		BOOLEAN_TO_NPVARIANT(status, *result);
		return true;
	}
	else if (name == fResetVideoView_id)
	{
		if (argCount < 3){
			LOG4CPLUS_ERROR(log, "ResetVideoView missing parameter.");
			return false;
		}

		std::string callid;
		int32_t remoteView = 0;
		int32_t localView = 0;

		if (NPVARIANT_IS_STRING(args[0])){
			NPVARIANT_TO_STRING_SAFE(args[0], callid);
		}
		NPVARIANT_TO_INT32_SAFE(args[1], remoteView);
		NPVARIANT_TO_INT32_SAFE(args[2], localView);

		int r = ECSDKBase::ResetVideoView(callid.c_str(),
			(void*)remoteView, (void*)localView);
		INT32_TO_NPVARIANT(r, *result);
		return true;
	}
	else if (name == fSendTextMessage_id){

		if (argCount < 6){
			LOG4CPLUS_ERROR(log, "SendTextMessage missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0]) || !NPVARIANT_IS_OBJECT(args[5])){
			LOG4CPLUS_ERROR(log, "SendTextMessage parameter Wrong format");
			return false;
		}
		std::string receiver;
		std::string message;
		int32_t type = 0;
		std::string userdata;
		unsigned int matchKey;
		char msgId[64];
		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], receiver);
		}

		if (NPVARIANT_IS_STRING(args[2])){
			NPVARIANT_TO_STRING_SAFE(args[2], message);
		}

		NPVARIANT_TO_INT32_SAFE(args[3], type);

		if (NPVARIANT_IS_STRING(args[4])){
			NPVARIANT_TO_STRING_SAFE(args[4], userdata);
		}

		int r = ECSDKBase::SendTextMessage(&matchKey,receiver.c_str(), message.c_str(),
			(MsgType)type, userdata.c_str(), msgId);

		INT32_TO_NPVARIANT(r, *result);

		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		NPVariant varMsgId;
		char * npMsgId = (char*)NPN_MemAlloc(strlen(msgId) + 1);
		strncpy_s(npMsgId, strlen(msgId) + 1, msgId, strlen(msgId));
		STRINGZ_TO_NPVARIANT(npMsgId, varMsgId);
		JSSetValue(NPVARIANT_TO_OBJECT(args[5]),varMsgId);

		return true;
	}
	else if (name == fSendMediaMessage_id){

		if (argCount < 6){
			LOG4CPLUS_ERROR(log, "SendMediaMessage missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0]) || !NPVARIANT_IS_OBJECT(args[5])){
			LOG4CPLUS_ERROR(log, "SendMediaMessage parameter Wrong format");
			return false;
		}

		std::string receiver;
		std::string fileName;
		int32_t type = 0;
		std::string userdata;
		unsigned int matchKey;
		char msgId[64];
		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], receiver);
		}

		if (NPVARIANT_IS_STRING(args[2])){
			NPVARIANT_TO_STRING_SAFE(args[2], fileName);
		}

		NPVARIANT_TO_INT32_SAFE(args[3], type);

		if (NPVARIANT_IS_STRING(args[4])){
			NPVARIANT_TO_STRING_SAFE(args[4], userdata);
		}

		int r = ECSDKBase::SendMediaMessage(&matchKey, receiver.c_str(), fileName.c_str(),
			(MsgType)type, userdata.c_str(), msgId);

		INT32_TO_NPVARIANT(r, *result);

		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		NPVariant varMsgId;
		char * npMsgId = (char*)NPN_MemAlloc(strlen(msgId) + 1);
		strncpy_s(npMsgId, strlen(msgId) + 1, msgId, strlen(msgId));
		STRINGZ_TO_NPVARIANT(npMsgId, varMsgId);
		JSSetValue(NPVARIANT_TO_OBJECT(args[5]), varMsgId);

		return true;
	}
	else if (name == fDownloadFileMessage_id){

		if (argCount < 7){
			LOG4CPLUS_ERROR(log, "DownloadFileMessage missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "DownloadFileMessage parameter Wrong format");
			return false;
		}

		std::string sender;
		std::string reciver;
		std::string url;
		std::string fileName;
		std::string msgId;
		int32_t type = 0;

		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], sender);
		}

		if (NPVARIANT_IS_STRING(args[2])){
			NPVARIANT_TO_STRING_SAFE(args[2], reciver);
		}

		if (NPVARIANT_IS_STRING(args[3])){
			NPVARIANT_TO_STRING_SAFE(args[3], url);
		}

		if (NPVARIANT_IS_STRING(args[4])){
			NPVARIANT_TO_STRING_SAFE(args[4], fileName);
		}

		if (NPVARIANT_IS_STRING(args[5])){
			NPVARIANT_TO_STRING_SAFE(args[5], msgId);
		}

		NPVARIANT_TO_INT32_SAFE(args[6], type);

		unsigned int matchKey = 0;
		int32_t r = ECSDKBase::DownloadFileMessage(&matchKey,sender.c_str(), reciver.c_str(),
			url.c_str(), fileName.c_str(), msgId.c_str(),(MsgType)type);

		INT32_TO_NPVARIANT(r, *result);

		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fCancelUploadOrDownloadNOW_id){

		if (argCount < 1){
			LOG4CPLUS_ERROR(log, "CancelUploadOrDownloadNOW missing parameter.");
			return false;
		}
		int32_t matchKey = 0;
		NPVARIANT_TO_INT32_SAFE(args[0], matchKey);

		ECSDKBase::CancelUploadOrDownloadNOW(matchKey);
		return true;
	}
	else if (name == fDeleteMessage_id)
	{
		if (argCount < 3){
			LOG4CPLUS_ERROR(log, "DeleteMessage missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "DeleteMessage parameter Wrong format");
			return false;
		}

		unsigned int matchKey;
		int32_t type = 0;
		std::string msgId;

		NPVARIANT_TO_INT32_SAFE(args[1], type);
		if (NPVARIANT_IS_STRING(args[2])){
			NPVARIANT_TO_STRING_SAFE(args[2], msgId);
		}

		int32_t r = ECSDKBase::DeleteMessage(&matchKey, type, msgId.c_str());
		INT32_TO_NPVARIANT(r, *result);

		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);
		return true;
	}
	else if (name == fOperateMessage_id)
	{
		if (argCount < 4){
			LOG4CPLUS_ERROR(log, "OperateMessage missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "OperateMessage parameter Wrong format");
			return false;
		}

		unsigned int matchKey;
		int32_t type = 0;
		std::string version;
		std::string msgId;

		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], version);
		}
		if (NPVARIANT_IS_STRING(args[2])){
			NPVARIANT_TO_STRING_SAFE(args[2], msgId);
		}
		NPVARIANT_TO_INT32_SAFE(args[3], type);

		int32_t r = ECSDKBase::OperateMessage(&matchKey, version.c_str(), msgId.c_str(), type);
		INT32_TO_NPVARIANT(r, *result);

		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);
		return true;
	}
	else if (name == fUploadVTMFile_id)
	{
		if (argCount < 5){
			LOG4CPLUS_ERROR(log, "UploadVTMFile missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "UploadVTMFile parameter Wrong format");
			return false;
		}

		unsigned int matchKey;
		std::string companyId;
		std::string companyPwd;
		std::string fileName;
		std::string callbackUrl;

		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], companyId);
		}
		if (NPVARIANT_IS_STRING(args[2])){
			NPVARIANT_TO_STRING_SAFE(args[2], companyPwd);
		}
		if (NPVARIANT_IS_STRING(args[3])){
			NPVARIANT_TO_STRING_SAFE(args[3], fileName);
		}
		if (NPVARIANT_IS_STRING(args[4])){
			NPVARIANT_TO_STRING_SAFE(args[4], callbackUrl);
		}

		int32_t r = ECSDKBase::UploadVTMFile(&matchKey, companyId.c_str(), companyPwd.c_str(), fileName.c_str(),
			callbackUrl.c_str());
		INT32_TO_NPVARIANT(r, *result);

		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);
		return true;
	}
	else if (name == fUploadVTMBuf_id)
	{
		if (argCount < 6){
			LOG4CPLUS_ERROR(log, "UploadVTMBu missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "UploadVTMBu parameter Wrong format");
			return false;
		}

		unsigned int matchKey;
		std::string companyId;
		std::string companyPwd;
		std::string Name;
		std::string Data;
		std::string callbackUrl;

		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], companyId);
		}
		if (NPVARIANT_IS_STRING(args[2])){
			NPVARIANT_TO_STRING_SAFE(args[2], companyPwd);
		}
		if (NPVARIANT_IS_STRING(args[3])){
			NPVARIANT_TO_STRING_SAFE(args[3], Name);
		}

		if (NPVARIANT_IS_STRING(args[4])){
			NPVARIANT_TO_STRING_SAFE(args[4], Data);
			int len;
			Data = ZBase64::Decode(Data.c_str(), Data.size(), len);
		}

		if (NPVARIANT_IS_STRING(args[5])){
			NPVARIANT_TO_STRING_SAFE(args[5], callbackUrl);
		}

		int32_t r = ECSDKBase::UploadVTMBuf(&matchKey, companyId.c_str(), companyPwd.c_str(),Name.c_str(),
			Data.c_str(), Data.length(), callbackUrl.c_str());
		INT32_TO_NPVARIANT(r, *result);

		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);
		return true;
	}
	else if (name == fCreateGroup_id){

		if (argCount < 11){
			LOG4CPLUS_ERROR(log, "CreateGroup missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "CreateGroup parameter Wrong format");
			return false;
		}

		unsigned int matchKey;
		std::string groupName;
		int32_t type = 0;
		std::string province;
		std::string city;
		int32_t scope = 0;
		std::string declared;
		int32_t permission = 0;
		int32_t isDismiss = 0;
		std::string groupDomain;
		bool isDiscuss = false;

		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], groupName);
		}
		NPVARIANT_TO_INT32_SAFE(args[2], type);

		if (NPVARIANT_IS_STRING(args[3])){
			NPVARIANT_TO_STRING_SAFE(args[3], province);
		}

		if (NPVARIANT_IS_STRING(args[4])){
			NPVARIANT_TO_STRING_SAFE(args[4], city);
		}

		NPVARIANT_TO_INT32_SAFE(args[5], scope);

		if (NPVARIANT_IS_STRING(args[6])){
			NPVARIANT_TO_STRING_SAFE(args[6], declared);
		}

		NPVARIANT_TO_INT32_SAFE(args[7], permission);
		NPVARIANT_TO_INT32_SAFE(args[8], isDismiss);

		if (NPVARIANT_IS_STRING(args[9])){
			NPVARIANT_TO_STRING_SAFE(args[9], groupDomain);
		}

		NPVARIANT_TO_BOOLEAN_SAFE(args[10], isDiscuss);

		int32_t r = ECSDKBase::CreateGroup(&matchKey, groupName.c_str(), type, province.c_str(),
			city.c_str(), scope, declared.c_str(), permission, isDismiss,
			groupDomain.c_str(), isDiscuss);
		INT32_TO_NPVARIANT(r, *result);

		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fDismissGroup_id){

		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "DismissGroup missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "DismissGroup parameter Wrong format");
			return false;
		}
		unsigned int matchKey;
		std::string groupid;
		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], groupid);
		}

		int r = ECSDKBase::DismissGroup(&matchKey, groupid.c_str());
		INT32_TO_NPVARIANT(r, *result);
		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);
		return true;
	}
	else if (name == fQuitGroup_id){

		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "QuitGroup missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "QuitGroup parameter Wrong format");
			return false;
		}
		unsigned int matchKey;
		std::string groupid;
		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], groupid);
		}

		int r = ECSDKBase::QuitGroup(&matchKey, groupid.c_str());
		INT32_TO_NPVARIANT(r, *result);
		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fJoinGroup_id){

		if (argCount < 3){
			LOG4CPLUS_ERROR(log, "JoinGroup missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "JoinGroup parameter Wrong format");
			return false;
		}
		unsigned int matchKey;
		std::string groupid;
		std::string declared;

		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], groupid);
		}

		if (NPVARIANT_IS_STRING(args[2])){
			NPVARIANT_TO_STRING_SAFE(args[2], declared);
		}

		int r = ECSDKBase::JoinGroup(&matchKey, groupid.c_str(), declared.c_str());
		INT32_TO_NPVARIANT(r, *result);
		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);
		return true;
	}
	else if (name == fReplyRequestJoinGroup_id){

		if (argCount < 4){
			LOG4CPLUS_ERROR(log, "ReplyRequestJoinGroup missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "ReplyRequestJoinGroup parameter Wrong format");
			return false;
		}
		unsigned int matchKey;
		std::string groupid;
		std::string member;
		int32_t confirm = 0;

		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], groupid);
		}

		if (NPVARIANT_IS_STRING(args[2])){
			NPVARIANT_TO_STRING_SAFE(args[2], member);
		}
		NPVARIANT_TO_INT32_SAFE(args[3], confirm);

		int r = ECSDKBase::ReplyRequestJoinGroup(&matchKey, groupid.c_str(), member.c_str(), confirm);
		INT32_TO_NPVARIANT(r, *result);
		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fInviteJoinGroup_id){

		if (argCount < 5){
			LOG4CPLUS_ERROR(log, "InviteJoinGroup missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0]) || !NPVARIANT_IS_OBJECT(args[3])){
			LOG4CPLUS_ERROR(log, "InviteJoinGroup parameter Wrong format");
			return false;
		}
		unsigned int matchKey;
		std::string groupid;
		std::string declared;

		int32_t confirm = 0;

		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], groupid);
		}

		if (NPVARIANT_IS_STRING(args[2])){
			NPVARIANT_TO_STRING_SAFE(args[2], declared);
		}

		NPVariant varLength;
		std::vector<const char *> arr;
		std::vector<std::string> strArr;
		if (JSGetProperty(NPVARIANT_TO_OBJECT(args[3]), "length", &varLength)){
			int32_t length = 0;
			NPVARIANT_TO_INT32_SAFE(varLength, length);
			arr.resize(length);
			strArr.resize(length);

			for (int i = 0; i < length; i++)
			{
				NPVariant varElement;
				if (JSGetArrayElementOfIndex(NPVARIANT_TO_OBJECT(args[3]), i, &varElement))
				{
					NPVARIANT_TO_STRING_SAFE(varElement, strArr[i]);
					arr[i] = strArr[i].c_str();
				}
			}
		}

		NPVARIANT_TO_INT32_SAFE(args[4], confirm);

		int r = ECSDKBase::InviteJoinGroup(&matchKey, groupid.c_str(), declared.c_str(), arr.data(), arr.size(), confirm);
		INT32_TO_NPVARIANT(r, *result);
		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fReplyInviteJoinGroup_id){

		if (argCount < 4){
			LOG4CPLUS_ERROR(log, "ReplyInviteJoinGroup missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "ReplyInviteJoinGroup parameter Wrong format");
			return false;
		}

		unsigned int matchKey;
		std::string groupid;
		std::string invitor;
		int32_t confirm = 0;

		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], groupid);
		}

		if (NPVARIANT_IS_STRING(args[2])){
			NPVARIANT_TO_STRING_SAFE(args[2], invitor);
		}
		NPVARIANT_TO_INT32_SAFE(args[3], confirm);

		int r = ECSDKBase::ReplyInviteJoinGroup(&matchKey, groupid.c_str(), invitor.c_str(), confirm);
		INT32_TO_NPVARIANT(r, *result);
		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fQueryOwnGroup_id){

		if (argCount < 4){
			LOG4CPLUS_ERROR(log, "QueryOwnGroup missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "QueryOwnGroup parameter Wrong format");
			return false;
		}

		unsigned int matchKey;
		std::string borderGroupid;
		int32_t pageSize = 0;
		int32_t target = 0;

		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], borderGroupid);
		}
		
		NPVARIANT_TO_INT32_SAFE(args[2], pageSize);
		NPVARIANT_TO_INT32_SAFE(args[3], target);

		int r = ECSDKBase::QueryOwnGroup(&matchKey, borderGroupid.c_str(), pageSize, target);
		INT32_TO_NPVARIANT(r, *result);
		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fQueryGroupDetail_id){

		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "QueryGroupDetail missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "QueryGroupDetail parameter Wrong format");
			return false;
		}

		unsigned int matchKey;
		std::string groupid;

		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], groupid);
		}

		int r = ECSDKBase::QueryGroupDetail(&matchKey, groupid.c_str());
		INT32_TO_NPVARIANT(r, *result);
		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fModifyGroup_id){

		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "ModifyGroup missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0]) || !NPVARIANT_IS_OBJECT(args[1])){
			LOG4CPLUS_ERROR(log, "ModifyGroup parameter Wrong format");
			return false;
		}


		ECModifyGroupInfo groupInfo;
		unsigned int matchKey;
		NPVariant varValue;

		if (JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "city", &varValue)){
			std::string var;
			if (NPVARIANT_IS_STRING(varValue)){
				NPVARIANT_TO_STRING_SAFE(varValue, var);
			}
			strncpy_s(groupInfo.city, var.c_str(), sizeof(groupInfo.city) - 1);
		}

		if (SUCCEEDED(JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "declared", &varValue))){
			std::string var;
			if (NPVARIANT_IS_STRING(varValue)){
				NPVARIANT_TO_STRING_SAFE(varValue, var);
			}
			strncpy_s(groupInfo.declared, var.c_str(), sizeof(groupInfo.declared) - 1);
		}

		if (SUCCEEDED(JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "discuss", &varValue))){
			int32_t discuss = 0;
			NPVARIANT_TO_INT32_SAFE(varValue, discuss);
			groupInfo.discuss = discuss;
		}

		if (SUCCEEDED(JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "groupDomain", &varValue))){
			std::string var;
			if (NPVARIANT_IS_STRING(varValue)){
				NPVARIANT_TO_STRING_SAFE(varValue, var);
			}
			strncpy_s(groupInfo.groupDomain, var.c_str(), sizeof(groupInfo.groupDomain) - 1);
		}

		if (SUCCEEDED(JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "groupId", &varValue))){
			std::string var;
			if (NPVARIANT_IS_STRING(varValue)){
				NPVARIANT_TO_STRING_SAFE(varValue, var);
			}
			strncpy_s(groupInfo.groupId, var.c_str(), sizeof(groupInfo.groupId) - 1);
		}

		if (SUCCEEDED(JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "name", &varValue))){
			std::string var;
			if (NPVARIANT_IS_STRING(varValue)){
				NPVARIANT_TO_STRING_SAFE(varValue, var);
			}
			strncpy_s(groupInfo.name, var.c_str(), sizeof(groupInfo.name) - 1);
		}

		if (SUCCEEDED(JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "permission", &varValue))){
			int32_t permission = 0;
			NPVARIANT_TO_INT32_SAFE(varValue, permission);
			groupInfo.permission = permission;
		}

		if (SUCCEEDED(JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "province", &varValue))){
			std::string var;
			if (NPVARIANT_IS_STRING(varValue)){
				NPVARIANT_TO_STRING_SAFE(varValue, var);
			}
			strncpy_s(groupInfo.province, var.c_str(), sizeof(groupInfo.province) - 1);
		}

		if (SUCCEEDED(JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "scope", &varValue))){
			int32_t scope = 0;
			NPVARIANT_TO_INT32_SAFE(varValue, scope);
			groupInfo.scope = scope;
		}

		if (SUCCEEDED(JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "type", &varValue))){
			int32_t type = 0;
			NPVARIANT_TO_INT32_SAFE(varValue, type);
			groupInfo.type = type;
		}

		int32_t r = ECSDKBase::ModifyGroup(&matchKey, &groupInfo);
		INT32_TO_NPVARIANT(r, *result);
		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fSearchPublicGroups_id){

		if (argCount < 3){
			LOG4CPLUS_ERROR(log, "SearchPublicGroups missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "SearchPublicGroups parameter Wrong format");
			return false;
		}

		unsigned int matchKey;
		int32_t searchType = 0;
		std::string keyword;
		int32_t pageNo = 1;
		int32_t pageSize = 10;

		NPVARIANT_TO_INT32_SAFE(args[1], searchType);
		if (NPVARIANT_IS_STRING(args[2])){
			NPVARIANT_TO_STRING_SAFE(args[2], keyword);
		}

		if (argCount>3)
			NPVARIANT_TO_INT32_SAFE(args[3], pageNo);

		if (argCount>4)
			NPVARIANT_TO_INT32_SAFE(args[4], pageSize);

		int r = ECSDKBase::SearchPublicGroups(&matchKey, searchType, keyword.c_str(), pageNo, pageSize);
		INT32_TO_NPVARIANT(r, *result);
		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fQueryGroupMember_id){

		if (argCount < 4){
			LOG4CPLUS_ERROR(log, "QueryGroupMember missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "QueryGroupMember parameter Wrong format");
			return false;
		}

		unsigned int matchKey;
		std::string groupid;
		std::string borderMember;
		int32_t pageSize = 0;

		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], groupid);
		}

		if (NPVARIANT_IS_STRING(args[2])){
			NPVARIANT_TO_STRING_SAFE(args[2], borderMember);
		}

		NPVARIANT_TO_INT32_SAFE(args[3], pageSize);

		int r = ECSDKBase::QueryGroupMember(&matchKey, groupid.c_str(), borderMember.c_str(), pageSize);
		INT32_TO_NPVARIANT(r, *result);
		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fDeleteGroupMember_id){

		if (argCount < 3){
			LOG4CPLUS_ERROR(log, "DeleteGroupMember missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "DeleteGroupMember parameter Wrong format");
			return false;
		}

		std::string groupid;
		std::string member;

		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], groupid);
		}

		if (NPVARIANT_IS_STRING(args[2])){
			NPVARIANT_TO_STRING_SAFE(args[2], member);
		}

		unsigned int matchKey;
		int r = ECSDKBase::DeleteGroupMember(&matchKey, groupid.c_str(), member.c_str());
		INT32_TO_NPVARIANT(r, *result);
		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fQueryGroupMemberCard_id){

		if (argCount < 3){
			LOG4CPLUS_ERROR(log, "QueryGroupMemberCard missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "QueryGroupMemberCard parameter Wrong format");
			return false;
		}

		std::string groupid;
		std::string member;

		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], groupid);
		}

		if (NPVARIANT_IS_STRING(args[2])){
			NPVARIANT_TO_STRING_SAFE(args[2], member);
		}

		unsigned int matchKey;
		int r = ECSDKBase::QueryGroupMemberCard(&matchKey, groupid.c_str(), member.c_str());
		INT32_TO_NPVARIANT(r, *result);
		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fModifyMemberCard_id){

		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "ModifyMemberCard missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0]) || !NPVARIANT_IS_OBJECT(args[1])){
			LOG4CPLUS_ERROR(log, "ModifyMemberCard parameter Wrong format");
			return false;
		}

		ECGroupMemberCard card;
		unsigned int matchKey;
		NPVariant varValue;

		if (JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "display", &varValue)){
			std::string var;
			if (NPVARIANT_IS_STRING(varValue)){
				NPVARIANT_TO_STRING_SAFE(varValue, var);
			}
			strncpy_s(card.display, var.c_str(), sizeof(card.display) - 1);
		}

		if (JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "groupId", &varValue)){
			std::string var;
			if (NPVARIANT_IS_STRING(varValue)){
				NPVARIANT_TO_STRING_SAFE(varValue, var);
			}
			strncpy_s(card.groupId, var.c_str(), sizeof(card.groupId) - 1);
		}

		if (JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "mail", &varValue)){
			std::string var;
			if (NPVARIANT_IS_STRING(varValue)){
				NPVARIANT_TO_STRING_SAFE(varValue, var);
			}
			strncpy_s(card.mail, var.c_str(), sizeof(card.mail) - 1);
		}

		if (JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "member", &varValue)){
			std::string var;
			if (NPVARIANT_IS_STRING(varValue)){
				NPVARIANT_TO_STRING_SAFE(varValue, var);
			}
			strncpy_s(card.member, var.c_str(), sizeof(card.member) - 1);
		}

		if (JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "phone", &varValue)){
			std::string var;
			if (NPVARIANT_IS_STRING(varValue)){
				NPVARIANT_TO_STRING_SAFE(varValue, var);
			}
			strncpy_s(card.phone, var.c_str(), sizeof(card.phone) - 1);
		}

		if (JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "remark", &varValue)){
			std::string var;
			if (NPVARIANT_IS_STRING(varValue)){
				NPVARIANT_TO_STRING_SAFE(varValue, var);
			}
			strncpy_s(card.remark, var.c_str(), sizeof(card.remark) - 1);
		}

		if (JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "role", &varValue)){
			int32_t role = 0;
			NPVARIANT_TO_INT32_SAFE(varValue, role);
			card.role = role;
		}

		if (JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "sex", &varValue)){
			int32_t sex = 0;
			NPVARIANT_TO_INT32_SAFE(varValue, sex);
			card.sex = sex;
		}

		if (JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "speakState", &varValue)){
			int32_t speakState = 0;
			NPVARIANT_TO_INT32_SAFE(varValue, speakState);
			card.speakState = speakState;
		}

		int r = ECSDKBase::ModifyMemberCard(&matchKey, &card);
		INT32_TO_NPVARIANT(r, *result);
		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fForbidMemberSpeak_id){

		if (argCount < 4){
			LOG4CPLUS_ERROR(log, "ForbidMemberSpeak missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "ForbidMemberSpeak parameter Wrong format");
			return false;
		}

		std::string groupid;
		std::string member;
		int32_t isBan = 0;

		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], groupid);
		}

		if (NPVARIANT_IS_STRING(args[2])){
			NPVARIANT_TO_STRING_SAFE(args[2], member);
		}

		NPVARIANT_TO_INT32_SAFE(args[3], isBan);

		unsigned int matchKey;
		int r = ECSDKBase::ForbidMemberSpeak(&matchKey, groupid.c_str(), member.c_str(), isBan);
		INT32_TO_NPVARIANT(r, *result);
		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fSetGroupMessageRule_id){

		if (argCount < 3){
			LOG4CPLUS_ERROR(log, "SetGroupMessageRule missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "SetGroupMessageRule parameter Wrong format");
			return false;
		}

		std::string groupid;
		std::string member;
		bool notice = false;

		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], groupid);
		}

		NPVARIANT_TO_BOOLEAN_SAFE(args[2], notice);

		unsigned int matchKey;
		int r = ECSDKBase::SetGroupMessageRule(&matchKey, groupid.c_str(), notice);
		INT32_TO_NPVARIANT(r, *result);
		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fStartVoiceRecording_id){

		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "StartVoiceRecording missing parameter.");
			return false;
		}
		std::string fileName;
		std::string sessionId;

		if (NPVARIANT_IS_STRING(args[0])){
			NPVARIANT_TO_STRING_SAFE(args[0], fileName);
		}

		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], sessionId);
		}

		int r = ECSDKBase::StartVoiceRecording(fileName.c_str(), sessionId.c_str());
		INT32_TO_NPVARIANT(r, *result);

		return true;
	}
	else if (name == fStopVoiceRecording_id){

		ECSDKBase::StopVoiceRecording();
		return true;
	}
	else if (name == fPlayVoiceMsg_id){

		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "PlayVoiceMsg missing parameter.");
			return false;
		}
		std::string fileName;
		std::string sessionId;

		if (NPVARIANT_IS_STRING(args[0])){
			NPVARIANT_TO_STRING_SAFE(args[0], fileName);
		}

		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], sessionId);
		}

		int r = ECSDKBase::PlayVoiceMsg(fileName.c_str(), sessionId.c_str());
		INT32_TO_NPVARIANT(r, *result);

		return true;
	}
	else if (name == fStopVoiceMsg_id){

		ECSDKBase::StopVoiceMsg();
		return true;
	}
	else if (name == fGetVoiceDuration_id){

		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "GetVoiceDuration missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[1])){
			LOG4CPLUS_ERROR(log, "GetVoiceDuration parameter Wrong format");
			return false;
		}

		std::string fileName;
		int duration = 0;

		if (NPVARIANT_IS_STRING(args[0])){
			NPVARIANT_TO_STRING_SAFE(args[0], fileName);
		}

		int r = ECSDKBase::GetVoiceDuration(fileName.c_str(), &duration);
		INT32_TO_NPVARIANT(r, *result);

		NPVariant varDuration;
		INT32_TO_NPVARIANT(duration, varDuration);
		JSSetValue(NPVARIANT_TO_OBJECT(args[1]), varDuration);

		return true;
	}
	else if (name == fRequestConferenceMemberVideo_id){

		if (argCount < 5){
			LOG4CPLUS_ERROR(log, "RequestConferenceMemberVideo missing parameter.");
			return false;
		}

		std::string conferenceId;
		std::string conferencePassword;
		std::string member;
		int32_t videoWindow = 0;
		int32_t type = 0;

		if (NPVARIANT_IS_STRING(args[0]))
		{
			NPVARIANT_TO_STRING_SAFE(args[0], conferenceId);
		}
		if (NPVARIANT_IS_STRING(args[1]))
		{
			NPVARIANT_TO_STRING_SAFE(args[1], conferencePassword);
		}
		if (NPVARIANT_IS_STRING(args[2]))
		{
			NPVARIANT_TO_STRING_SAFE(args[2], member);
		}
		NPVARIANT_TO_INT32_SAFE(args[3], videoWindow);
		NPVARIANT_TO_INT32_SAFE(args[4], type);

		int r = ECSDKBase::RequestConferenceMemberVideo(conferenceId.c_str(), 
			conferencePassword.c_str(), member.c_str(), (void *)videoWindow, type);
		INT32_TO_NPVARIANT(r, *result);

		return true;
	}
	else if (name == fCancelMemberVideo_id){

		if (argCount < 4){
			LOG4CPLUS_ERROR(log, "CancelMemberVideo missing parameter.");
			return false;
		}

		std::string conferenceId;
		std::string conferencePassword;
		std::string member;
		int32_t type = 0;

		if (NPVARIANT_IS_STRING(args[0]))
		{
			NPVARIANT_TO_STRING_SAFE(args[0], conferenceId);
		}
		if (NPVARIANT_IS_STRING(args[1]))
		{
			NPVARIANT_TO_STRING_SAFE(args[1], conferencePassword);
		}
		if (NPVARIANT_IS_STRING(args[2]))
		{
			NPVARIANT_TO_STRING_SAFE(args[2], member);
		}
		NPVARIANT_TO_INT32_SAFE(args[3], type);

		int r = ECSDKBase::CancelMemberVideo(conferenceId.c_str(),
			conferencePassword.c_str(), member.c_str(), type);
		INT32_TO_NPVARIANT(r, *result);

		return true;
	}
	else if (name == fResetVideoConfWindow_id){

		if (argCount < 4){
			LOG4CPLUS_ERROR(log, "ResetVideoConfWindow missing parameter.");
			return false;
		}

		std::string conferenceId;
		std::string member;
		int32_t newWindow = 0;
		int32_t type = 0;

		if (NPVARIANT_IS_STRING(args[0]))
		{
			NPVARIANT_TO_STRING_SAFE(args[0], conferenceId);
		}
		if (NPVARIANT_IS_STRING(args[1]))
		{
			NPVARIANT_TO_STRING_SAFE(args[1], member);
		}
		NPVARIANT_TO_INT32_SAFE(args[2], newWindow);
		NPVARIANT_TO_INT32_SAFE(args[3], type);

		int r = ECSDKBase::ResetVideoConfWindow(conferenceId.c_str()
			, member.c_str(), (void *)newWindow, type);

		INT32_TO_NPVARIANT(r, *result);

		return true;
	}
	else if (name == fCreateMultimediaMeeting_id){

		if (argCount < 10){
			LOG4CPLUS_ERROR(log, "CreateMultimediaMeeting missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "CreateMultimediaMeeting parameter Wrong format");
			return false;
		}

		unsigned int matchKey = 0;
		int32_t meetingType = 0;
		std::string meetingName;
		std::string password;
		std::string keywords;
		int32_t voiceMode = 0;
		int32_t square = 0;
		bool bAutoJoin = false;
		bool autoClose = false;
		bool autoDelete = false;

		NPVARIANT_TO_INT32_SAFE(args[1], meetingType);

		if (NPVARIANT_IS_STRING(args[2]))
		{
			NPVARIANT_TO_STRING_SAFE(args[2], meetingName);
		}
		if (NPVARIANT_IS_STRING(args[3]))
		{
			NPVARIANT_TO_STRING_SAFE(args[3], password);
		}
		if (NPVARIANT_IS_STRING(args[4]))
		{
			NPVARIANT_TO_STRING_SAFE(args[4],keywords);
		}
		NPVARIANT_TO_INT32_SAFE(args[5], voiceMode);
		NPVARIANT_TO_INT32_SAFE(args[6], square);
		NPVARIANT_TO_BOOLEAN_SAFE(args[7], bAutoJoin);
		NPVARIANT_TO_BOOLEAN_SAFE(args[8], autoClose);
		NPVARIANT_TO_BOOLEAN_SAFE(args[9], autoDelete);

		int r = ECSDKBase::CreateMultimediaMeeting(&matchKey,meetingType,meetingName.c_str()
			, password.c_str(), keywords.c_str(), voiceMode,square,bAutoJoin,autoClose,autoDelete);

		INT32_TO_NPVARIANT(r, *result);

		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fJoinMeeting_id){

		if (argCount < 3){
			LOG4CPLUS_ERROR(log, "JoinMeeting missing parameter.");
			return false;
		}

		int32_t meetingType = 0;
		std::string conferenceId;
		std::string password;


		NPVARIANT_TO_INT32_SAFE(args[0], meetingType);

		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], conferenceId);
		}
		if (NPVARIANT_IS_STRING(args[2])){
			NPVARIANT_TO_STRING_SAFE(args[2], password);
		}
		
		int r = ECSDKBase::JoinMeeting(meetingType, conferenceId.c_str(), password.c_str());

		INT32_TO_NPVARIANT(r, *result);

		return true;
	}
	else if (name == fExitMeeting_id){

		if (argCount < 1){
			LOG4CPLUS_ERROR(log, "ExitMeeting missing parameter.");
			return false;
		}

		std::string conferenceId;

		if (NPVARIANT_IS_STRING(args[0])){
			NPVARIANT_TO_STRING_SAFE(args[0], conferenceId);
		}

		int r = ECSDKBase::ExitMeeting(conferenceId.c_str());

		INT32_TO_NPVARIANT(r, *result);

		return true;
	}
	else if (name == fQueryMeetingMembers_id){

		if (argCount < 3){
			LOG4CPLUS_ERROR(log, "QueryMeetingMembers missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "QueryMeetingMembers parameter Wrong format");
			return false;
		}

		unsigned int matchKey = 0;
		int32_t meetingType = 0;
		std::string conferenceId;

		NPVARIANT_TO_INT32_SAFE(args[1], meetingType);

		if (NPVARIANT_IS_STRING(args[2])){
			NPVARIANT_TO_STRING_SAFE(args[2], conferenceId);
		}

		int r = ECSDKBase::QueryMeetingMembers(&matchKey, meetingType, conferenceId.c_str());

		INT32_TO_NPVARIANT(r, *result);

		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fDismissMultiMediaMeeting_id){

		if (argCount < 3){
			LOG4CPLUS_ERROR(log, "DismissMultiMediaMeeting missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "DismissMultiMediaMeeting parameter Wrong format");
			return false;
		}

		unsigned int matchKey = 0;
		int32_t meetingType = 0;
		std::string conferenceId;

		NPVARIANT_TO_INT32_SAFE(args[1], meetingType);

		if (NPVARIANT_IS_STRING(args[2])){
			NPVARIANT_TO_STRING_SAFE(args[2], conferenceId);
		}

		int r = ECSDKBase::DismissMultiMediaMeeting(&matchKey, meetingType, conferenceId.c_str());

		INT32_TO_NPVARIANT(r, *result);

		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fQueryMultiMediaMeetings_id){

		if (argCount < 3){
			LOG4CPLUS_ERROR(log, "QueryMultiMediaMeetings missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "QueryMultiMediaMeetings parameter Wrong format");
			return false;
		}

		unsigned int matchKey = 0;
		int32_t meetingType = 0;
		std::string keywords;

		NPVARIANT_TO_INT32_SAFE(args[1], meetingType);

		if (NPVARIANT_IS_STRING(args[2])){
			NPVARIANT_TO_STRING_SAFE(args[2], keywords);
		}

		int r = ECSDKBase::QueryMultiMediaMeetings(&matchKey, meetingType, keywords.c_str());

		INT32_TO_NPVARIANT(r, *result);

		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fInviteJoinMultiMediaMeeting_id){

		if (argCount < 8){
			LOG4CPLUS_ERROR(log, "InviteJoinMultiMediaMeeting missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0]) || !NPVARIANT_IS_OBJECT(args[2])){
			LOG4CPLUS_ERROR(log, "InviteJoinMultiMediaMeeting parameter Wrong format");
			return false;
		}

		unsigned int matchKey = 0;
		std::string conferenceId;
		NPObject * members = nullptr;
		bool blanding = false;
		bool isSpeak = false;
		bool isListen = false;
		std::string disNumber;
		std::string userData;

		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], conferenceId);
		}
		members = NPVARIANT_TO_OBJECT(args[2]);
		NPVARIANT_TO_BOOLEAN_SAFE(args[3], blanding);
		NPVARIANT_TO_BOOLEAN_SAFE(args[4], isSpeak);
		NPVARIANT_TO_BOOLEAN_SAFE(args[5], isListen);
		if (NPVARIANT_IS_STRING(args[6])){
			NPVARIANT_TO_STRING_SAFE(args[6], disNumber);
		}
		if (NPVARIANT_IS_STRING(args[7])){
			NPVARIANT_TO_STRING_SAFE(args[7], userData);
		}

		NPVariant varLength;
		VOID_TO_NPVARIANT(varLength);
		std::vector<const char *> arr;
		std::vector<std::string> strArr;

		if (JSGetProperty(members, "length", &varLength) && NPVARIANT_IS_INT32(varLength)){
			int32_t length = 0;
			NPVARIANT_TO_INT32_SAFE(varLength, length);
			arr.resize(length);
			strArr.resize(length);

			for (int i = 0; i < length; i++)
			{
				NPVariant element;
				if (JSGetArrayElementOfIndex(members, i, &element))
				{
					NPVARIANT_TO_STRING_SAFE(element, strArr[i]);
					arr[i] = strArr[i].c_str();
				}
			}
		}

		int r = ECSDKBase::InviteJoinMultiMediaMeeting(&matchKey, conferenceId.c_str(), arr.data(), arr.size(),
			blanding, isSpeak, isListen, disNumber.c_str(), userData.c_str());
		INT32_TO_NPVARIANT(r, *result);

		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fDeleteMemberMultiMediaMeeting_id){

		if (argCount < 5){
			LOG4CPLUS_ERROR(log, "DeleteMemberMultiMediaMeeting missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "DeleteMemberMultiMediaMeeting parameter Wrong format");
			return false;
		}

		unsigned int matchKey = 0;
		int32_t meetingType = 0;
		std::string conferenceId;
		std::string  members;
		bool isVoIP = false;

		NPVARIANT_TO_INT32_SAFE(args[1], meetingType);
		if (NPVARIANT_IS_STRING(args[2])){
			NPVARIANT_TO_STRING_SAFE(args[2], conferenceId);
		}
		if (NPVARIANT_IS_STRING(args[3])){
			NPVARIANT_TO_STRING_SAFE(args[3], members);
		}
		NPVARIANT_TO_BOOLEAN_SAFE(args[4], isVoIP);


		int r = ECSDKBase::DeleteMemberMultiMediaMeeting(&matchKey, meetingType, conferenceId.c_str(),
			members.c_str(),isVoIP);

		INT32_TO_NPVARIANT(r, *result);

		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fPublishVideo_id){

		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "PublishVideo missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "PublishVideo parameter Wrong format");
			return false;
		}

		unsigned int matchKey = 0;
		std::string conferenceId;

		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], conferenceId);
		}

		int r = ECSDKBase::PublishVideo(&matchKey, conferenceId.c_str());

		INT32_TO_NPVARIANT(r, *result);

		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fUnpublishVideo_id){

		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "UnpublishVideo missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "UnpublishVideo parameter Wrong format");
			return false;
		}

		unsigned int matchKey = 0;
		std::string conferenceId;

		if (NPVARIANT_IS_STRING(args[1])){
			NPVARIANT_TO_STRING_SAFE(args[1], conferenceId);
		}

		int r = ECSDKBase::UnpublishVideo(&matchKey, conferenceId.c_str());

		INT32_TO_NPVARIANT(r, *result);

		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fCreateInterphoneMeeting_id){

		if (argCount < 2){
			LOG4CPLUS_ERROR(log, "CreateInterphoneMeeting missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0]) || !NPVARIANT_IS_OBJECT(args[1])){
			LOG4CPLUS_ERROR(log, "CreateInterphoneMeeting parameter Wrong format");
			return false;
		}

		unsigned int matchKey =0;
		NPVariant varLength;
		VOID_TO_NPVARIANT(varLength);
		std::vector<const char *> arr;
		std::vector<std::string> strArr;

		if (JSGetProperty(NPVARIANT_TO_OBJECT(args[1]), "length", &varLength) && NPVARIANT_IS_INT32(varLength)){
			int32_t length = 0;
			NPVARIANT_TO_INT32_SAFE(varLength, length);
			arr.resize(length);
			strArr.resize(length);

			for (int i = 0; i < length; i++)
			{
				NPVariant element;
				if (JSGetArrayElementOfIndex(NPVARIANT_TO_OBJECT(args[1]), i, &element))
				{
					NPVARIANT_TO_STRING_SAFE(element, strArr[i]);
					arr[i] = strArr[i].c_str();
				}
			}
		}

		int r = ECSDKBase::CreateInterphoneMeeting(&matchKey, arr.data(), arr.size());
		INT32_TO_NPVARIANT(r, *result);

		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
	else if (name == fControlInterphoneMic_id){

		if (argCount < 3){
			LOG4CPLUS_ERROR(log, "ControlInterphoneMic missing parameter.");
			return false;
		}

		if (!NPVARIANT_IS_OBJECT(args[0])){
			LOG4CPLUS_ERROR(log, "ControlInterphoneMic parameter Wrong format");
			return false;
		}

		unsigned int matchKey = 0;
		bool isControl = false;
		std::string interphoneId;

		NPVARIANT_TO_BOOLEAN_SAFE(args[1], isControl);
		if (NPVARIANT_IS_STRING(args[2])){
			NPVARIANT_TO_STRING_SAFE(args[2], interphoneId);
		}

		int r = ECSDKBase::ControlInterphoneMic(&matchKey, isControl, interphoneId.c_str());
		INT32_TO_NPVARIANT(r, *result);

		NPVariant varMatchKey;
		INT32_TO_NPVARIANT(matchKey, varMatchKey);
		JSSetValue(NPVARIANT_TO_OBJECT(args[0]), varMatchKey);

		return true;
	}
    else if (name == fSetServerAddress_id){

        if (argCount < 4){
            LOG4CPLUS_ERROR(log, "SetServerAddress missing parameter.");
            return false;
        }

        std::string utf8Connector;
        int32_t connectorPort = 0;
        std::string utf8FileServer;
        int32_t fileServerPort = 0;

        if (NPVARIANT_IS_STRING(args[0])){
            NPVARIANT_TO_STRING_SAFE(args[0], utf8Connector);
        }
        NPVARIANT_TO_INT32_SAFE(args[1], connectorPort);
 
        if (NPVARIANT_IS_STRING(args[2])){
            NPVARIANT_TO_STRING_SAFE(args[2], utf8FileServer);
        }
        NPVARIANT_TO_INT32_SAFE(args[3], fileServerPort);

        int r = ECSDKBase::SetServerAddress(utf8Connector.c_str(), connectorPort, 
            utf8FileServer.c_str(), fileServerPort);
        INT32_TO_NPVARIANT(r, *result);

        return true;
    }
	else{
		return false;
	}
}


NPBool NPAPIECSDK::init(NPWindow* pNPWindow)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " m_hWnd:" << m_hWnd);
	NPAPIPluginBase::init(pNPWindow);
	const char *ua = NPN_UserAgent(m_pNPInstance);
	LOG4CPLUS_INFO(log, "UserAgent:" << ua);
	return TRUE;
}

void NPAPIECSDK::shut()
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " m_hWnd:" << m_hWnd);
	ECSDKBase::UnInitialize();
	NPAPIPluginBase::shut();
}

HWND  NPAPIECSDK::gethWnd() const
{
	return m_hWnd;
}

LRESULT CALLBACK NPAPIECSDK::PluginWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_onLoginfo:{
		if (this->onLogInfo_obj){
			NPVariant result;
			std::string * strmsg = reinterpret_cast<std::string *>(lParam);
			NPVariant params[1];
			char* npOutString = (char *)NPN_MemAlloc(strmsg->length() + 1);
			strcpy_s(npOutString, strmsg->length(), strmsg->c_str());
			STRINGZ_TO_NPVARIANT(npOutString, params[0]);
			LOG4CPLUS_INFO(log, "Fire_onLogInfo:");
			delete strmsg;
			NPN_InvokeDefault(this->m_pNPInstance, this->onLogInfo_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onLogOut:{
		if (this->onLogOut_obj){
			NPVariant result;
			NPVariant params[1];
			
			INT32_TO_NPVARIANT(lParam, params[0]);
			LOG4CPLUS_INFO(log, "Fire_onLogOut:");

			NPN_InvokeDefault(this->m_pNPInstance, this->onLogOut_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onConnectState:{
		if (this->onConnectState_obj){
			NPVariant result;
			ECConnectState * data = reinterpret_cast<ECConnectState *>(lParam);

			NPVariant params[1];

			JSNewObject(&params[0]);
			NPVariant varCode;
			INT32_TO_NPVARIANT(data->code, varCode);
			JSSetProperty(NPVARIANT_TO_OBJECT(params[0]),"code", varCode);

			NPVariant varReason;
			INT32_TO_NPVARIANT(data->reason, varReason);
			JSSetProperty(NPVARIANT_TO_OBJECT(params[0]), "reason", varReason);

			LOG4CPLUS_INFO(log, "Fire_onConnectState:");
			delete data;
			NPN_InvokeDefault(this->m_pNPInstance, this->onConnectState_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onSetPersonInfo:{
		if (this->onSetPersonInfo_obj)
		{
			NPVariant result;
			EConSetPersonInfo * data = reinterpret_cast<EConSetPersonInfo *>(lParam);

			NPVariant params[3];
			INT32_TO_NPVARIANT(data->matchKey, params[0]);
			INT32_TO_NPVARIANT(data->reason, params[1]);
			INT32_TO_NPVARIANT(data->version, params[2]);
	
			LOG4CPLUS_INFO(log, "Fire_onSetPersonInfo:");
			delete data;
			NPN_InvokeDefault(this->m_pNPInstance, this->onSetPersonInfo_obj, params, sizeof(params)/sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onGetPersonInfo:{
		if (this->onGetPersonInfo_obj)
		{
			NPVariant result;
			EConGetPersonInfo * data = reinterpret_cast<EConGetPersonInfo *>(lParam);

			NPVariant params[3];
			INT32_TO_NPVARIANT(data->matchKey, params[0]);
			INT32_TO_NPVARIANT(data->reason, params[1]);

			JSNewObject(&params[2]);
			JSSetProperty(params[2], "version", CNPVariant(data->PersonInfo.version));
			JSSetProperty(params[2], "sex", CNPVariant(data->PersonInfo.sex));
			JSSetProperty(params[2], "nickName", CNPVariant(data->PersonInfo.nickName));
			JSSetProperty(params[2], "birth", CNPVariant(data->PersonInfo.birth));
			JSSetProperty(params[2], "sign", CNPVariant(data->PersonInfo.sign));

			LOG4CPLUS_INFO(log, "Fire_onGetPersonInfo:");
			delete data;
			NPN_InvokeDefault(this->m_pNPInstance, this->onGetPersonInfo_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onGetUserState:
	{
		if (this->onGetUserState_obj)
		{
			NPVariant result;
			EConGetUserState * data = reinterpret_cast<EConGetUserState *>(lParam);

			NPVariant params[3];
			INT32_TO_NPVARIANT(data->matchKey, params[0]);
			INT32_TO_NPVARIANT(data->reason, params[1]);
			JSNewArray(&params[2]);
			for (auto it : data->States)
			{
				NPVariant  psersonState;
				JSNewObject(&psersonState);
				JSSetProperty(psersonState, "accound", it.accound);
				JSSetProperty(psersonState, "nickName", it.nickName);
				JSSetProperty(psersonState, "device", it.device);
				JSSetProperty(psersonState, "netType", it.netType);
				JSSetProperty(psersonState, "state", it.state);
				JSSetProperty(psersonState, "subState", it.subState);
				JSSetProperty(psersonState, "timestamp", it.timestamp);
				JSSetProperty(psersonState, "userData", it.userData);

				JSAddArrayElement(params[2], psersonState);
			}

			LOG4CPLUS_INFO(log, "Fire_onGetUserState:");
			delete data;
			NPN_InvokeDefault(this->m_pNPInstance, this->onGetUserState_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onPublishPresence:
	{
		if (this->onPublishPresence_obj)
		{
			NPVariant result;

			NPVariant params[2];
			INT32_TO_NPVARIANT(wParam, params[0]);
			INT32_TO_NPVARIANT(lParam, params[1]);

			LOG4CPLUS_INFO(log, "Fire_onPublishPresence:");

			NPN_InvokeDefault(this->m_pNPInstance, this->onPublishPresence_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onReceiveFriendsPublishPresence:
	{
		if (this->onReceiveFriendsPublishPresence_obj)
		{
			NPVariant result;

			std::vector<ECPersonState> * data = reinterpret_cast<std::vector<ECPersonState> *>(lParam);

			NPVariant params[1];
			JSNewArray(&params[1]);
			for (auto it : *data)
			{
				NPVariant personState;
				JSNewObject(&personState);
				JSSetProperty(personState, "accound", it.accound);
				JSSetProperty(personState, "device", it.device);
				JSSetProperty(personState, "netType", it.netType);
				JSSetProperty(personState, "nickName", it.nickName);
				JSSetProperty(personState, "state", it.state);
				JSSetProperty(personState, "subState", it.subState);
				JSSetProperty(personState, "timestamp", it.timestamp);
				JSSetProperty(personState, "userData", it.userData);

				JSAddArrayElement(params[1], personState);
			}
			LOG4CPLUS_INFO(log, "Fire_onReceiveFriendsPublishPresence:");
			delete data;
			NPN_InvokeDefault(this->m_pNPInstance, this->onReceiveFriendsPublishPresence_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onSoftVersion:
	{
		if (onSoftVersion_obj)
		{
			NPVariant result;
			EConSoftVersion * data = reinterpret_cast<EConSoftVersion *>(lParam);

			NPVariant params[3];
			params[0] = CNPVariant(data->softVersion);
			params[1] = CNPVariant(data->updateMode);
			params[2] = CNPVariant(data->updateDesc);

			LOG4CPLUS_INFO(log, "Fire_onSoftVersion:");
			delete data;
			NPN_InvokeDefault(this->m_pNPInstance, this->onSoftVersion_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onGetOnlineMultiDevice:
	{
		if (this->onGetOnlineMultiDevice_obj)
		{
			NPVariant result;
			EConGetOnlineMultiDevice * data = reinterpret_cast<EConGetOnlineMultiDevice *>(lParam);

			NPVariant params[3];
			INT32_TO_NPVARIANT(data->matchKey, params[0]);
			INT32_TO_NPVARIANT(data->reason, params[1]);
			JSNewArray(&params[2]);
			for (auto it : data->MultiDeviceStates)
			{
				NPVariant  State;
				JSNewObject(&State);
				JSSetProperty(State, "device", it.device);
				JSSetProperty(State, "state", it.state);

				JSAddArrayElement(params[2], State);
			}

			LOG4CPLUS_INFO(log, "Fire_onGetOnlineMultiDevice:");
			delete data;
			NPN_InvokeDefault(this->m_pNPInstance, this->onGetOnlineMultiDevice_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onReceiveMultiDeviceState:
	{
		if (this->onReceiveMultiDeviceState_obj)
		{
			NPVariant result;

			std::vector<ECMultiDeviceState> * data = reinterpret_cast<std::vector<ECMultiDeviceState> *>(lParam);

			NPVariant params[1];
			JSNewArray(&params[1]);
			for (auto it : *data)
			{
				NPVariant State;
				JSNewObject(&State);
				JSSetProperty(State, "device", it.device);
				JSSetProperty(State, "state", it.state);

				JSAddArrayElement(params[1], State);
			}
			LOG4CPLUS_INFO(log, "Fire_onReceiveMultiDeviceState:");
			delete data;
			NPN_InvokeDefault(this->m_pNPInstance, this->onReceiveMultiDeviceState_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onMakeCallBack:
	{
		if (this->onMakeCallBack_obj)
		{
			NPVariant result;
			EConMakeCallBack * data = reinterpret_cast<EConMakeCallBack *>(lParam);
			NPVariant params[1];

			JSNewObject(&params[0]);
			JSSetProperty(params[0], "called", data->CallBackInfo.called);
			JSSetProperty(params[0], "calledDisplay", data->CallBackInfo.calledDisplay);
			JSSetProperty(params[0], "caller", data->CallBackInfo.caller);
			JSSetProperty(params[0], "callerDisplay", data->CallBackInfo.callerDisplay);

			LOG4CPLUS_TRACE(log, "Fire_onMakeCallBack");
			delete data;
			NPN_InvokeDefault(this->m_pNPInstance, this->onMakeCallBack_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onCallEvents:
	{
		if (this->onCallEvents_obj)
		{
			NPVariant result;
			ECVoIPCallMsg * data = reinterpret_cast<ECVoIPCallMsg *>(lParam);
			NPVariant params[2];

			params[0] = CNPVariant(wParam);
			JSNewObject(&params[1]);
			JSSetProperty(params[1], "callee", data->callee);
			JSSetProperty(params[1], "caller", data->caller);
			JSSetProperty(params[1], "callid", data->callid);
			JSSetProperty(params[1], "callType", data->callType);
			JSSetProperty(params[1], "eCode", data->eCode);

			LOG4CPLUS_TRACE(log, "Fire_onCallEvents");
			delete data;
			NPN_InvokeDefault(this->m_pNPInstance, this->onCallEvents_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onCallIncoming:
	{
		if (this->onCallIncoming_obj)
		{
			NPVariant result;
			ECVoIPComingInfo * data = reinterpret_cast<ECVoIPComingInfo *>(lParam);
			NPVariant params[1];

			JSNewObject(&params[0]);
			JSSetProperty(params[0], "caller", data->caller);
			JSSetProperty(params[0], "callid", data->callid);
			JSSetProperty(params[0], "callType", data->callType);
			JSSetProperty(params[0], "display", data->display);
			JSSetProperty(params[0], "nickname", data->nickname);

			LOG4CPLUS_TRACE(log, "Fire_onCallIncoming");
			delete data;
			NPN_InvokeDefault(this->m_pNPInstance, this->onCallIncoming_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onDtmfReceived:
	{
		if (this->onDtmfReceived_obj)
		{
			NPVariant result;
			std::string * data = reinterpret_cast<std::string *>(wParam);
			NPVariant params[2];

			params[0] = CNPVariant(*data);
			std::string dtmf;
			dtmf.assign(1,(char)lParam);
			params[1] = CNPVariant(dtmf);

			LOG4CPLUS_TRACE(log, "Fire_onDtmfReceived");
			delete data;
			NPN_InvokeDefault(this->m_pNPInstance, this->onDtmfReceived_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onSwitchCallMediaTypeRequest:
	{
		if (this->onSwitchCallMediaTypeRequest_obj)
		{
			NPVariant result;
			std::string * data = reinterpret_cast<std::string *>(wParam);
			NPVariant params[2];

			params[0] = CNPVariant(*data);
			params[1] = CNPVariant(lParam);

			LOG4CPLUS_TRACE(log, "Fire_onSwitchCallMediaTypeRequest");
			delete data;
			NPN_InvokeDefault(this->m_pNPInstance, this->onSwitchCallMediaTypeRequest_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onSwitchCallMediaTypeResponse:
	{
		if (this->onSwitchCallMediaTypeResponse_obj)
		{
			NPVariant result;
			std::string * data = reinterpret_cast<std::string *>(wParam);
			NPVariant params[2];

			params[0] = CNPVariant(*data);
			params[1] = CNPVariant(lParam);

			LOG4CPLUS_TRACE(log, "Fire_onSwitchCallMediaTypeResponse");
			delete data;
			NPN_InvokeDefault(this->m_pNPInstance, this->onSwitchCallMediaTypeResponse_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onRemoteVideoRatio:
	{
		if (this->onRemoteVideoRatio_obj)
		{
			NPVariant result;
			EConRemoteVideoRatio * data = reinterpret_cast<EConRemoteVideoRatio *>(lParam);
			NPVariant params[5];

			params[0] = CNPVariant(data->CallidOrConferenceId);
			params[1] = CNPVariant(data->width);
			params[2] = CNPVariant(data->height);
			params[3] = CNPVariant(data->type);
			params[4] = CNPVariant(data->member);

			LOG4CPLUS_TRACE(log, "Fire_onRemoteVideoRatio");
			delete data;
			NPN_InvokeDefault(this->m_pNPInstance, this->onRemoteVideoRatio_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onOfflineMessageCount:
	{
		if (this->onOfflineMessageCount_obj)
		{
			NPVariant result;
	
			NPVariant params[1];
			params[0] = CNPVariant(lParam);

			LOG4CPLUS_TRACE(log, "Fire_onOfflineMessageCount");
			NPN_InvokeDefault(this->m_pNPInstance, this->onOfflineMessageCount_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onGetOfflineMessage:
	{
		if (this->onGetOfflineMessage_obj)
		{
			NPVariant result;
	
			NPN_InvokeDefault(this->m_pNPInstance, this->onGetOfflineMessage_obj, nullptr, 0, &result);
			LOG4CPLUS_TRACE(log, "Fire_onGetOfflineMessage:" << NPVARIANT_TO_INT32(result));
			int32_t int32Result;
			NPVARIANT_TO_INT32_SAFE(result, int32Result);
			m_onGetOfflineMessage_Result = int32Result;
			std::unique_lock<std::mutex> lck(m_onGetOfflineMessage_mtx);
			m_onGetOfflineMessage_cv.notify_one();
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onOfflineMessageComplete:
	{
		if (this->onOfflineMessageComplete_obj)
		{
			NPVariant result;
			LOG4CPLUS_TRACE(log, "Fire_onOfflineMessageComplete");
			NPN_InvokeDefault(this->m_pNPInstance, this->onOfflineMessageComplete_obj, nullptr, 0, &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onReceiveOfflineMessage:
	{
		if (this->onReceiveOfflineMessage_obj)
		{
			NPVariant result;
			ECMessage * data = reinterpret_cast<ECMessage *>(lParam);
			NPVariant params[1];

			JSNewObject(&params[0]);

			JSSetProperty(params[0], "dateCreated", data->dateCreated);
			JSSetProperty(params[0], "fileName", data->fileName);
			JSSetProperty(params[0], "fileSize", data->fileSize);
			JSSetProperty(params[0], "fileUrl", data->fileUrl);
			JSSetProperty(params[0], "msgContent", data->msgContent);
			JSSetProperty(params[0], "msgId", data->msgId);
			JSSetProperty(params[0], "msgType", data->msgType);
			JSSetProperty(params[0], "offset", data->offset);
			JSSetProperty(params[0], "receiver", data->receiver);
			JSSetProperty(params[0], "sender", data->sender);
			JSSetProperty(params[0], "senderNickName", data->senderNickName);
			JSSetProperty(params[0], "sessionId", data->sessionId);
			JSSetProperty(params[0], "sessionType", data->sessionType);
			JSSetProperty(params[0], "userData", data->userData);
			JSSetProperty(params[0], "isAt", data->isAt);
			JSSetProperty(params[0], "type", data->type);

			LOG4CPLUS_TRACE(log, "Fire_onReceiveOfflineMessage");
			delete data;
			NPN_InvokeDefault(this->m_pNPInstance, this->onReceiveOfflineMessage_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onReceiveMessage:
	{
		if (this->onReceiveMessage_obj)
		{
			NPVariant result;
			ECMessage * data = reinterpret_cast<ECMessage *>(lParam);
			NPVariant params[1];

			JSNewObject(&params[0]);

			JSSetProperty(params[0], "dateCreated", data->dateCreated);
			JSSetProperty(params[0], "fileName", data->fileName);
			JSSetProperty(params[0], "fileSize", data->fileSize);
			JSSetProperty(params[0], "fileUrl", data->fileUrl);
			JSSetProperty(params[0], "msgContent", data->msgContent);
			JSSetProperty(params[0], "msgId", data->msgId);
			JSSetProperty(params[0], "msgType", data->msgType);
			JSSetProperty(params[0], "offset", data->offset);
			JSSetProperty(params[0], "receiver", data->receiver);
			JSSetProperty(params[0], "sender", data->sender);
			JSSetProperty(params[0], "senderNickName", data->senderNickName);
			JSSetProperty(params[0], "sessionId", data->sessionId);
			JSSetProperty(params[0], "sessionType", data->sessionType);
			JSSetProperty(params[0], "userData", data->userData);
			JSSetProperty(params[0], "isAt", data->isAt);
			JSSetProperty(params[0], "type", data->type);

			LOG4CPLUS_TRACE(log, "Fire_onReceiveMessage");
			delete data;
			NPN_InvokeDefault(this->m_pNPInstance, this->onReceiveMessage_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onReceiveFile:
	{
		if (this->onReceiveFile_obj)
		{
			NPVariant result;
			ECMessage * data = reinterpret_cast<ECMessage *>(lParam);
			NPVariant params[1];

			JSNewObject(&params[0]);

			JSSetProperty(params[0], "dateCreated", data->dateCreated);
			JSSetProperty(params[0], "fileName", data->fileName);
			JSSetProperty(params[0], "fileSize", data->fileSize);
			JSSetProperty(params[0], "fileUrl", data->fileUrl);
			JSSetProperty(params[0], "msgContent", data->msgContent);
			JSSetProperty(params[0], "msgId", data->msgId);
			JSSetProperty(params[0], "msgType", data->msgType);
			JSSetProperty(params[0], "offset", data->offset);
			JSSetProperty(params[0], "receiver", data->receiver);
			JSSetProperty(params[0], "sender", data->sender);
			JSSetProperty(params[0], "senderNickName", data->senderNickName);
			JSSetProperty(params[0], "sessionId", data->sessionId);
			JSSetProperty(params[0], "sessionType", data->sessionType);
			JSSetProperty(params[0], "userData", data->userData);
			JSSetProperty(params[0], "isAt", data->isAt);
			JSSetProperty(params[0], "type", data->type);

			LOG4CPLUS_TRACE(log, "Fire_onReceiveFile");
			delete data;
			NPN_InvokeDefault(this->m_pNPInstance, this->onReceiveFile_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onDownloadFileComplete:
	{
		if (this->onDownloadFileComplete_obj)
		{
			NPVariant result;
			EConDownloadFileComplete * data = reinterpret_cast<EConDownloadFileComplete *>(lParam);

			NPVariant params[3];
			params[0] = CNPVariant(data->matchKey);
			params[1] = CNPVariant(data->reason);

			NPVariant msg;
			JSNewObject(&msg);
			JSSetProperty(msg, "dateCreated", data->msg.dateCreated);
			JSSetProperty(msg, "fileName", data->msg.fileName);
			JSSetProperty(msg, "fileSize", data->msg.fileSize);
			JSSetProperty(msg, "fileUrl", data->msg.fileUrl);
			JSSetProperty(msg, "msgContent", data->msg.msgContent);
			JSSetProperty(msg, "msgId", data->msg.msgId);
			JSSetProperty(msg, "msgType", data->msg.msgType);
			JSSetProperty(msg, "offset", data->msg.offset);
			JSSetProperty(msg, "receiver", data->msg.receiver);
			JSSetProperty(msg, "sender", data->msg.sender);
			JSSetProperty(msg, "senderNickName", data->msg.senderNickName);
			JSSetProperty(msg, "sessionId", data->msg.sessionId);
			JSSetProperty(msg, "sessionType", data->msg.sessionType);
			JSSetProperty(msg, "userData", data->msg.userData);
			params[2] = msg;

			LOG4CPLUS_TRACE(log, "Fire_onDownloadFileComplete");
			delete data;
			NPN_InvokeDefault(this->m_pNPInstance, this->onDownloadFileComplete_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onRateOfProgressAttach:
	{
		if (this->onRateOfProgressAttach_obj)
		{
			NPVariant result;
			EConRateOfProgressAttach * data = reinterpret_cast<EConRateOfProgressAttach *>(lParam);

			NPVariant params[4];
			params[0] = CNPVariant(data->matchKey);
			params[1] = CNPVariant(data->rateSize);
			params[2] = CNPVariant(data->fileSize);

			NPVariant msg;
			JSNewObject(&msg);
			JSSetProperty(msg, "dateCreated", data->msg.dateCreated);
			JSSetProperty(msg, "fileName", data->msg.fileName);
			JSSetProperty(msg, "fileSize", data->msg.fileSize);
			JSSetProperty(msg, "fileUrl", data->msg.fileUrl);
			JSSetProperty(msg, "msgContent", data->msg.msgContent);
			JSSetProperty(msg, "msgId", data->msg.msgId);
			JSSetProperty(msg, "msgType", data->msg.msgType);
			JSSetProperty(msg, "offset", data->msg.offset);
			JSSetProperty(msg, "receiver", data->msg.receiver);
			JSSetProperty(msg, "sender", data->msg.sender);
			JSSetProperty(msg, "senderNickName", data->msg.senderNickName);
			JSSetProperty(msg, "sessionId", data->msg.sessionId);
			JSSetProperty(msg, "sessionType", data->msg.sessionType);
			JSSetProperty(msg, "userData", data->msg.userData);
			params[3] = msg;

			LOG4CPLUS_TRACE(log, "Fire_onRateOfProgressAttach");
			delete data;
			NPN_InvokeDefault(this->m_pNPInstance, this->onRateOfProgressAttach_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onSendTextMessage:
	{
		if (this->onSendTextMessage_obj)
		{
			NPVariant result;
			EConSendTextMessage * param = reinterpret_cast<EConSendTextMessage *>(lParam);

			NPVariant params[3];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);

			NPVariant msg;
			JSNewObject(&msg);
			JSSetProperty(msg, "dateCreated", param->msg.dateCreated);
			JSSetProperty(msg, "fileName", param->msg.fileName);
			JSSetProperty(msg, "fileSize", param->msg.fileSize);
			JSSetProperty(msg, "fileUrl", param->msg.fileUrl);
			JSSetProperty(msg, "msgContent", param->msg.msgContent);
			JSSetProperty(msg, "msgId", param->msg.msgId);
			JSSetProperty(msg, "msgType", param->msg.msgType);
			JSSetProperty(msg, "offset", param->msg.offset);
			JSSetProperty(msg, "receiver", param->msg.receiver);
			JSSetProperty(msg, "sender", param->msg.sender);
			JSSetProperty(msg, "senderNickName", param->msg.senderNickName);
			JSSetProperty(msg, "sessionId", param->msg.sessionId);
			JSSetProperty(msg, "sessionType", param->msg.sessionType);
			JSSetProperty(msg, "userData", param->msg.userData);
			params[2] = msg;

			LOG4CPLUS_TRACE(log, "Fire_onSendTextMessage");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onSendTextMessage_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onSendMediaFile:
	{
		if (this->onSendMediaFile_obj)
		{
			NPVariant result;
			EConSendMediaFile * param = reinterpret_cast<EConSendMediaFile *>(lParam);

			NPVariant params[3];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);

			NPVariant msg;
			JSNewObject(&msg);
			JSSetProperty(msg, "dateCreated", param->msg.dateCreated);
			JSSetProperty(msg, "fileName", param->msg.fileName);
			JSSetProperty(msg, "fileSize", param->msg.fileSize);
			JSSetProperty(msg, "fileUrl", param->msg.fileUrl);
			JSSetProperty(msg, "msgContent", param->msg.msgContent);
			JSSetProperty(msg, "msgId", param->msg.msgId);
			JSSetProperty(msg, "msgType", param->msg.msgType);
			JSSetProperty(msg, "offset", param->msg.offset);
			JSSetProperty(msg, "receiver", param->msg.receiver);
			JSSetProperty(msg, "sender", param->msg.sender);
			JSSetProperty(msg, "senderNickName", param->msg.senderNickName);
			JSSetProperty(msg, "sessionId", param->msg.sessionId);
			JSSetProperty(msg, "sessionType", param->msg.sessionType);
			JSSetProperty(msg, "userData", param->msg.userData);
			params[2] = msg;

			LOG4CPLUS_TRACE(log, "Fire_onSendMediaFile");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onSendMediaFile_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onDeleteMessage:
	{
		if (this->onDeleteMessage_obj)
		{
			NPVariant result;
			EConDeleteMessage * param = reinterpret_cast<EConDeleteMessage *>(lParam);

			NPVariant params[4];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);
			params[2] = CNPVariant(param->type);
			params[3] = CNPVariant(param->msgId);

			LOG4CPLUS_TRACE(log, "Fire_onDeleteMessage");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onDeleteMessage_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onOperateMessage:
	{
		if (this->onOperateMessage_obj)
		{
			NPVariant result;
			EConOperateMessage * param = reinterpret_cast<EConOperateMessage *>(lParam);

			NPVariant params[2];
			params[0] = CNPVariant(param->tcpMsgIdOut);
			params[1] = CNPVariant(param->reason);

			LOG4CPLUS_TRACE(log, "Fire_onOperateMessage");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onOperateMessage_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onReceiveOpreateNoticeMessage:
	{
		if (this->onReceiveOpreateNoticeMessage_obj)
		{
			NPVariant result;
			EConReceiveOpreateNoticeMessage * param = reinterpret_cast<EConReceiveOpreateNoticeMessage *>(lParam);

			NPVariant params[1];
			JSNewObject(&params[0]);
			JSSetProperty(params[0], "notice", param->notice);

			NPVariant msg;
			JSNewObject(&msg);
			switch (param->notice)
			{
			case NTDeleteMessage:
				JSSetProperty(msg, "msgContent", param->DeleteMsg.msgContent);
				JSSetProperty(msg, "msgId", param->DeleteMsg.msgId);
				JSSetProperty(msg, "receiver", param->DeleteMsg.receiver);
				JSSetProperty(msg, "sender", param->DeleteMsg.sender);
				JSSetProperty(msg, "senderNickName", param->DeleteMsg.senderNickName);
				JSSetProperty(msg, "sessionId", param->DeleteMsg.sessionId);
				JSSetProperty(msg, "userData", param->DeleteMsg.userData);
				break;
			case NTReadMessage:
				JSSetProperty(msg, "msgContent", param->ReadMsg.msgContent);
				JSSetProperty(msg, "msgId", param->ReadMsg.msgId);
				JSSetProperty(msg, "receiver", param->ReadMsg.receiver);
				JSSetProperty(msg, "sender", param->ReadMsg.sender);
				JSSetProperty(msg, "senderNickName", param->ReadMsg.senderNickName);
				JSSetProperty(msg, "sessionId", param->ReadMsg.sessionId);
				JSSetProperty(msg, "userData", param->ReadMsg.userData);
				break;
			case NTWithdrawMessage:
				JSSetProperty(msg, "msgContent", param->WithDrawMessage.msgContent);
				JSSetProperty(msg, "msgId", param->WithDrawMessage.msgId);
				JSSetProperty(msg, "receiver", param->WithDrawMessage.receiver);
				JSSetProperty(msg, "sender", param->WithDrawMessage.sender);
				JSSetProperty(msg, "senderNickName", param->WithDrawMessage.senderNickName);
				JSSetProperty(msg, "sessionId", param->WithDrawMessage.sessionId);
				JSSetProperty(msg, "userData", param->WithDrawMessage.userData);
				break;
			default:
				break;
			}
			JSSetProperty(params[0], "msg", msg);

			LOG4CPLUS_TRACE(log, "Fire_onReceiveOpreateNoticeMessage");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onReceiveOpreateNoticeMessage_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onUploadVTMFileOrBuf:
	{
		if (this->onUploadVTMFileOrBuf_obj)
		{
			NPVariant result;
			EConUploadVTMFileOrBuf * param = reinterpret_cast<EConUploadVTMFileOrBuf *>(lParam);

			NPVariant params[4];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);
			params[3] = CNPVariant(param->fileUrl);

			LOG4CPLUS_TRACE(log, "Fire_onUploadVTMFileOrBuf");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onUploadVTMFileOrBuf_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onCreateGroup:
	{
		if (this->onCreateGroup_obj)
		{
			NPVariant result;
			EConCreateGroup * param = reinterpret_cast<EConCreateGroup *>(lParam);

			NPVariant params[3];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);

			NPVariant msg;
			JSNewObject(&msg);
			JSSetProperty(msg, "city", param->info.city);
			JSSetProperty(msg, "dateCreated", param->info.dateCreated);
			JSSetProperty(msg, "declared", param->info.declared);
			JSSetProperty(msg, "discuss", param->info.discuss);
			JSSetProperty(msg, "groupDomain", param->info.groupDomain);
			JSSetProperty(msg, "groupId", param->info.groupId);
			JSSetProperty(msg, "isNotice", param->info.isNotice);
			JSSetProperty(msg, "memberCount", param->info.memberCount);
			JSSetProperty(msg, "name", param->info.name);
			JSSetProperty(msg, "owner", param->info.owner);
			JSSetProperty(msg, "permission", param->info.permission);
			JSSetProperty(msg, "province", param->info.province);
			JSSetProperty(msg, "scope", param->info.scope);
			JSSetProperty(msg, "type", param->info.type);
			params[2] = msg;

			LOG4CPLUS_TRACE(log, "Fire_onCreateGroup");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onCreateGroup_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onDismissGroup:
	{
		if (this->onDismissGroup_obj)
		{
			NPVariant result;
			EConDismissGroup * param = reinterpret_cast<EConDismissGroup *>(lParam);

			NPVariant params[3];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);
			params[2] = CNPVariant(param->groupid);

			LOG4CPLUS_TRACE(log, "Fire_onDismissGroup");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onDismissGroup_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onQuitGroup:
	{
		if (this->onQuitGroup_obj)
		{
			NPVariant result;
			EConQuitGroup * param = reinterpret_cast<EConQuitGroup *>(lParam);

			NPVariant params[3];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);
			params[2] = CNPVariant(param->groupid);

			LOG4CPLUS_TRACE(log, "Fire_onQuitGroup");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onQuitGroup_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onJoinGroup:
	{
		if (this->onJoinGroup_obj)
		{
			NPVariant result;
			EConJoinGroup * param = reinterpret_cast<EConJoinGroup *>(lParam);

			NPVariant params[3];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);
			params[2] = CNPVariant(param->groupid);

			LOG4CPLUS_TRACE(log, "Fire_onJoinGroup");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onJoinGroup_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onReplyRequestJoinGroup:
	{
		if (this->onReplyRequestJoinGroup_obj)
		{
			NPVariant result;
			EConReplyRequestJoinGroup * param = reinterpret_cast<EConReplyRequestJoinGroup *>(lParam);

			NPVariant params[5];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);
			params[2] = CNPVariant(param->groupid);
			params[3] = CNPVariant(param->member);
			params[4] = CNPVariant(param->confirm);

			LOG4CPLUS_TRACE(log, "Fire_onReplyRequestJoinGroup");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onReplyRequestJoinGroup_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onInviteJoinGroup:
	{
		if (this->onInviteJoinGroup_obj)
		{
			NPVariant result;
			EConInviteJoinGroup * param = reinterpret_cast<EConInviteJoinGroup *>(lParam);

			NPVariant params[5];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);
			params[2] = CNPVariant(param->groupid);

			JSNewArray(&params[3]);
			for (auto it : param->members)
			{
				JSAddArrayElement(params[3], CNPVariant(it));
			}

			params[4] = CNPVariant(param->confirm);

			LOG4CPLUS_TRACE(log, "Fire_onInviteJoinGroup");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onInviteJoinGroup_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onReplyInviteJoinGroup:
	{
		if (this->onReplyInviteJoinGroup_obj)
		{
			NPVariant result;
			EConReplyInviteJoinGroup * param = reinterpret_cast<EConReplyInviteJoinGroup *>(lParam);

			NPVariant params[5];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);
			params[2] = CNPVariant(param->groupid);
			params[3] = CNPVariant(param->member);
			params[4] = CNPVariant(param->confirm);

			LOG4CPLUS_TRACE(log, "Fire_onReplyInviteJoinGroup");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onReplyInviteJoinGroup_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onQueryOwnGroup:
	{
		if (this->onQueryOwnGroup_obj)
		{
			NPVariant result;
			EConQueryOwnGroup * param = reinterpret_cast<EConQueryOwnGroup *>(lParam);

			NPVariant params[3];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);

			JSNewArray(&params[2]);
			for (auto it : param->group)
			{
				NPVariant  group;
				JSNewObject(&group);
				JSSetProperty(group, "discuss", it.discuss);
				JSSetProperty(group, "groupId", it.groupId);
				JSSetProperty(group, "isNotice", it.isNotice);
				JSSetProperty(group, "memberCount", it.memberCount);
				JSSetProperty(group, "name", it.name);
				JSSetProperty(group, "owner", it.owner);
				JSSetProperty(group, "permission", it.permission);
				JSSetProperty(group, "scope", it.scope);

				JSAddArrayElement(params[2], group);
			}

			LOG4CPLUS_TRACE(log, "Fire_onQueryOwnGroup");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onQueryOwnGroup_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onQueryGroupDetail:
	{
		if (this->onQueryGroupDetail_obj)
		{
			NPVariant result;
			EConQueryGroupDetail * param = reinterpret_cast<EConQueryGroupDetail *>(lParam);

			NPVariant params[3];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);

			NPVariant detail;
			JSNewObject(&detail);
			JSSetProperty(detail, "city", param->detail.city);
			JSSetProperty(detail, "dateCreated", param->detail.dateCreated);
			JSSetProperty(detail, "declared", param->detail.declared);
			JSSetProperty(detail, "discuss", param->detail.discuss);
			JSSetProperty(detail, "groupDomain", param->detail.groupDomain);
			JSSetProperty(detail, "groupId", param->detail.groupId);
			JSSetProperty(detail, "isNotice", param->detail.isNotice);
			JSSetProperty(detail, "memberCount", param->detail.memberCount);
			JSSetProperty(detail, "name", param->detail.name);
			JSSetProperty(detail, "owner", param->detail.owner);
			JSSetProperty(detail, "permission", param->detail.permission);
			JSSetProperty(detail, "province", param->detail.province);
			JSSetProperty(detail, "scope", param->detail.scope);
			JSSetProperty(detail, "type", param->detail.type);
			params[2] = detail;

			LOG4CPLUS_TRACE(log, "Fire_onQueryGroupDetail");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onQueryGroupDetail_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onModifyGroup:
	{
		if (this->onModifyGroup_obj)
		{
			NPVariant result;
			EConModifyGroup * param = reinterpret_cast<EConModifyGroup *>(lParam);

			NPVariant params[3];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);

			NPVariant detail;
			JSNewObject(&detail);
			JSSetProperty(detail, "city", param->info.city);
			JSSetProperty(detail, "declared", param->info.declared);
			JSSetProperty(detail, "discuss", param->info.discuss);
			JSSetProperty(detail, "groupDomain", param->info.groupDomain);
			JSSetProperty(detail, "groupId", param->info.groupId);
			JSSetProperty(detail, "name", param->info.name);
			JSSetProperty(detail, "permission", param->info.permission);
			JSSetProperty(detail, "province", param->info.province);
			JSSetProperty(detail, "scope", param->info.scope);
			JSSetProperty(detail, "type", param->info.type);
			params[2] = detail;

			LOG4CPLUS_TRACE(log, "Fire_onModifyGroup");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onModifyGroup_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onSearchPublicGroup:
	{
		if (this->onSearchPublicGroup_obj)
		{
			NPVariant result;
			EConSearchPublicGroup * param = reinterpret_cast<EConSearchPublicGroup *>(lParam);

			NPVariant params[6];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);
			params[2] = CNPVariant(param->searchType);
			params[3] = CNPVariant(param->keyword);

			JSNewArray(&params[4]);

			for (auto it : param->group)
			{
				NPVariant group;
				JSNewObject(&group);
				JSSetProperty(group, "declared", it.declared);
				JSSetProperty(group, "discuss", it.discuss);
				JSSetProperty(group, "groupId", it.groupId);
				JSSetProperty(group, "memberCount", it.memberCount);
				JSSetProperty(group, "name", it.name);
				JSSetProperty(group, "owner", it.owner);
				JSSetProperty(group, "permission", it.permission);
				JSSetProperty(group, "scope", it.scope);

				JSAddArrayElement(params[4], group);
			}
			params[5] = CNPVariant(param->pageNo);

			LOG4CPLUS_TRACE(log, "Fire_onSearchPublicGroup");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onSearchPublicGroup_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onQueryGroupMember:
	{
		if (this->onQueryGroupMember_obj)
		{
			NPVariant result;
			EConQueryGroupMember * param = reinterpret_cast<EConQueryGroupMember *>(lParam);

			NPVariant params[4];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);
			params[2] = CNPVariant(param->groupid);

			JSNewArray(&params[3]);

			for (auto it : param->members)
			{
				NPVariant group;
				JSNewObject(&group);
				JSSetProperty(group, "member", it.member);
				JSSetProperty(group, "nickName", it.nickName);
				JSSetProperty(group, "role", it.role);
				JSSetProperty(group, "sex", it.sex);
				JSSetProperty(group, "speakState", it.speakState);

				JSAddArrayElement(params[3], group);
			}

			LOG4CPLUS_TRACE(log, "Fire_onQueryGroupMember");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onQueryGroupMember_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onDeleteGroupMember:
	{
		if (this->onDeleteGroupMember_obj)
		{
			NPVariant result;
			EConDeleteGroupMember * param = reinterpret_cast<EConDeleteGroupMember *>(lParam);

			NPVariant params[4];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);
			params[2] = CNPVariant(param->groupid);
			params[3] = CNPVariant(param->member);

			LOG4CPLUS_TRACE(log, "Fire_onDeleteGroupMember");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onDeleteGroupMember_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onQueryGroupMemberCard:
	{
		if (this->onQueryGroupMemberCard_obj)
		{
			NPVariant result;
			EConQueryGroupMemberCard * param = reinterpret_cast<EConQueryGroupMemberCard *>(lParam);

			NPVariant params[3];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);

			NPVariant card;
			JSNewObject(&card);
			JSSetProperty(card, "city", param->card.display);
			JSSetProperty(card, "declared", param->card.groupId);
			JSSetProperty(card, "discuss", param->card.mail);
			JSSetProperty(card, "groupDomain", param->card.member);
			JSSetProperty(card, "groupId", param->card.phone);
			JSSetProperty(card, "name", param->card.remark);
			JSSetProperty(card, "permission", param->card.role);
			JSSetProperty(card, "province", param->card.sex);
			JSSetProperty(card, "scope", param->card.speakState);
			params[2] = card;

			LOG4CPLUS_TRACE(log, "Fire_onQueryGroupMemberCard");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onQueryGroupMemberCard_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onModifyGroupMemberCard:
	{
		if (this->onModifyGroupMemberCard_obj)
		{
			NPVariant result;
			EConModifyGroupMemberCard * param = reinterpret_cast<EConModifyGroupMemberCard *>(lParam);

			NPVariant params[3];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);

			NPVariant card;
			JSNewObject(&card);
			JSSetProperty(card, "city", param->card.display);
			JSSetProperty(card, "declared", param->card.groupId);
			JSSetProperty(card, "discuss", param->card.mail);
			JSSetProperty(card, "groupDomain", param->card.member);
			JSSetProperty(card, "groupId", param->card.phone);
			JSSetProperty(card, "name", param->card.remark);
			JSSetProperty(card, "permission", param->card.role);
			JSSetProperty(card, "province", param->card.sex);
			JSSetProperty(card, "scope", param->card.speakState);
			params[2] = card;

			LOG4CPLUS_TRACE(log, "Fire_onModifyGroupMemberCard");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onModifyGroupMemberCard_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onForbidMemberSpeakGroup:
	{
		if (this->onForbidMemberSpeakGroup_obj)
		{
			NPVariant result;
			EConForbidMemberSpeakGroup * param = reinterpret_cast<EConForbidMemberSpeakGroup *>(lParam);

			NPVariant params[5];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);
			params[2] = CNPVariant(param->groupid);
			params[3] = CNPVariant(param->member);
			params[4] = CNPVariant(param->isBan);

			LOG4CPLUS_TRACE(log, "Fire_onForbidMemberSpeakGroup");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onForbidMemberSpeakGroup_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onSetGroupMessageRule:
	{
		if (this->onSetGroupMessageRule_obj)
		{
			NPVariant result;
			EConSetGroupMessageRule * param = reinterpret_cast<EConSetGroupMessageRule *>(lParam);

			NPVariant params[4];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);
			params[2] = CNPVariant(param->groupid);
			params[3] = CNPVariant(param->notice);

			LOG4CPLUS_TRACE(log, "Fire_onSetGroupMessageRule");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onSetGroupMessageRule_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onReceiveGroupNoticeMessage:
	{
		if (this->onReceiveGroupNoticeMessage_obj)
		{
			NPVariant result;
			EConReceiveGroupNoticeMessage * param = reinterpret_cast<EConReceiveGroupNoticeMessage *>(lParam);

			NPVariant params[1];
			NPVariant msg;
			JSNewObject(&msg);

			switch (param->notice)
			{
			case NTRequestJoinGroup:
				JSSetProperty(msg, "confirm:", param->requestMsg.confirm);
				JSSetProperty(msg, "dateCreated:", param->requestMsg.dateCreated);
				JSSetProperty(msg, "declared:", param->requestMsg.declared);
				JSSetProperty(msg, "groupId:", param->requestMsg.groupId);
				JSSetProperty(msg, "groupName:", param->requestMsg.groupName);
				JSSetProperty(msg, "nickName:", param->requestMsg.nickName);
				JSSetProperty(msg, "proposer:", param->requestMsg.proposer);
				JSSetProperty(msg, "sender:", param->requestMsg.sender);
				break;
			case NTInviteJoinGroup:
				JSSetProperty(msg, "admin:", param->inviteMsg.admin);
				JSSetProperty(msg, "confirm:", param->inviteMsg.confirm);
				JSSetProperty(msg, "dateCreated:", param->inviteMsg.dateCreated);
				JSSetProperty(msg, "declared:", param->inviteMsg.declared);
				JSSetProperty(msg, "groupId:", param->inviteMsg.groupId);
				JSSetProperty(msg, "groupName:", param->inviteMsg.groupName);
				JSSetProperty(msg, "nickName:", param->inviteMsg.nickName);
				JSSetProperty(msg, "sender:", param->inviteMsg.sender);
				break;
			case NTMemberJoinedGroup:
				JSSetProperty(msg, "dateCreated:", param->memberMsg.dateCreated);
				JSSetProperty(msg, "declared:", param->memberMsg.declared);
				JSSetProperty(msg, "groupId:", param->memberMsg.groupId);
				JSSetProperty(msg, "groupName:", param->memberMsg.groupName);
				JSSetProperty(msg, "member:", param->memberMsg.member);
				JSSetProperty(msg, "nickName:", param->memberMsg.nickName);
				JSSetProperty(msg, "sender:", param->memberMsg.sender);
				break;
			case NTDismissGroup:
				JSSetProperty(msg, "dateCreated:", param->dismissMsg.dateCreated);
				JSSetProperty(msg, "groupId:", param->dismissMsg.groupId);
				JSSetProperty(msg, "groupName:", param->dismissMsg.groupName);
				JSSetProperty(msg, "sender:", param->dismissMsg.sender);
				break;
			case NTQuitGroup:
				JSSetProperty(msg, "dateCreated:", param->quitMsg.dateCreated);
				JSSetProperty(msg, "groupId:", param->quitMsg.groupId);
				JSSetProperty(msg, "groupName:", param->quitMsg.groupName);
				JSSetProperty(msg, "member:", param->quitMsg.member);
				JSSetProperty(msg, "nickName:", param->quitMsg.nickName);
				JSSetProperty(msg, "sender:", param->quitMsg.sender);
				break;
			case NTRemoveGroupMember:
				JSSetProperty(msg, "dateCreated:", param->removeMsg.dateCreated);
				JSSetProperty(msg, "groupId:", param->removeMsg.groupId);
				JSSetProperty(msg, "groupName:", param->removeMsg.groupName);
				JSSetProperty(msg, "member:", param->removeMsg.member);
				JSSetProperty(msg, "nickName:", param->removeMsg.nickName);
				JSSetProperty(msg, "sender:", param->removeMsg.sender);
				break;
			case NTReplyRequestJoinGroup:
				JSSetProperty(msg, "admin:", param->replyrequestMsg.admin);
				JSSetProperty(msg, "confirm:", param->replyrequestMsg.confirm);
				JSSetProperty(msg, "dateCreated:", param->replyrequestMsg.dateCreated);
				JSSetProperty(msg, "groupId:", param->replyrequestMsg.groupId);
				JSSetProperty(msg, "groupName:", param->replyrequestMsg.groupName);
				JSSetProperty(msg, "member:", param->replyrequestMsg.member);
				JSSetProperty(msg, "nickName:", param->replyrequestMsg.nickName);
				JSSetProperty(msg, "sender:", param->replyrequestMsg.sender);
				break;
			case NTReplyInviteJoinGroup:
				JSSetProperty(msg, "admin:", param->replyinviteMsg.admin);
				JSSetProperty(msg, "confirm:", param->replyinviteMsg.confirm);
				JSSetProperty(msg, "dateCreated:", param->replyinviteMsg.dateCreated);
				JSSetProperty(msg, "groupId:", param->replyinviteMsg.groupId);
				JSSetProperty(msg, "groupName:", param->replyinviteMsg.groupName);
				JSSetProperty(msg, "member:", param->replyinviteMsg.member);
				JSSetProperty(msg, "nickName:", param->replyinviteMsg.nickName);
				JSSetProperty(msg, "sender:", param->replyinviteMsg.sender);
				break;
			case NTModifyGroup:
				JSSetProperty(msg, "dateCreated:", param->modifyMsg.dateCreated);
				JSSetProperty(msg, "groupId:", param->modifyMsg.groupId);
				JSSetProperty(msg, "groupName:", param->modifyMsg.groupName);
				JSSetProperty(msg, "member:", param->modifyMsg.member);
				JSSetProperty(msg, "modifyDic:", param->modifyMsg.modifyDic);
				JSSetProperty(msg, "sender:", param->modifyMsg.sender);
				break;
			default:
				break;
			}
			NPVariant notice;
			JSNewObject(&notice);
			JSSetProperty(notice, "NoticeMsg", msg);
			params[0] = notice;

			LOG4CPLUS_TRACE(log, "Fire_onReceiveGroupNoticeMessage");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onReceiveGroupNoticeMessage_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onRecordingTimeOut:
	{
		if (this->onRecordingTimeOut_obj)
		{
			NPVariant result;
			EConRecordingTimeOut * param = reinterpret_cast<EConRecordingTimeOut *>(lParam);

			NPVariant params[2];
			params[0] = CNPVariant(param->ms);
			params[1] = CNPVariant(param->sessionId);

			LOG4CPLUS_TRACE(log, "Fire_onRecordingTimeOut");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onRecordingTimeOut_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onFinishedPlaying:
	{
		if (this->onFinishedPlaying_obj)
		{
			NPVariant result;
			EConFinishedPlaying * param = reinterpret_cast<EConFinishedPlaying *>(lParam);

			NPVariant params[2];
			params[0] = CNPVariant(param->sessionId);
			params[1] = CNPVariant(param->reason);

			LOG4CPLUS_TRACE(log, "Fire_onFinishedPlaying");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onFinishedPlaying_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onRecordingAmplitude:
	{
		if (this->onRecordingAmplitude_obj)
		{
			NPVariant result;
			EConRecordingAmplitude * param = reinterpret_cast<EConRecordingAmplitude *>(lParam);

			NPVariant params[2];
			params[0] = CNPVariant(param->amplitude);
			params[1] = CNPVariant(param->sessionId);

			LOG4CPLUS_TRACE(log, "Fire_onRecordingAmplitude");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onRecordingAmplitude_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onMeetingIncoming:
	{
		if (this->onMeetingIncoming_obj)
		{
			NPVariant result;
			ECMeetingComingInfo * param = reinterpret_cast<ECMeetingComingInfo *>(lParam);

			NPVariant params[1];

			NPVariant come;
			JSNewObject(&come);
			JSSetProperty(come, "caller", param->caller);
			JSSetProperty(come, "callid", param->callid);
			JSSetProperty(come, "display", param->display);
			JSSetProperty(come, "Id", param->Id);
			JSSetProperty(come, "meetingType", param->meetingType);
			JSSetProperty(come, "nickname", param->nickname);
            JSSetProperty(come, "sdkUserData", param->sdkUserData);
			params[0] = come;

			LOG4CPLUS_TRACE(log, "Fire_onMeetingIncoming");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onMeetingIncoming_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onReceiveInterphoneMeetingMessage:
	{
		if (this->onReceiveInterphoneMeetingMessage_obj)
		{
			NPVariant result;
			ECInterphoneMsg * param = reinterpret_cast<ECInterphoneMsg *>(lParam);

			NPVariant params[1];

			NPVariant msg;
			JSNewObject(&msg);
			JSSetProperty(msg, "datecreated", param->datecreated);
			JSSetProperty(msg, "from", param->from);
			JSSetProperty(msg, "interphoneid", param->interphoneid);
			JSSetProperty(msg, "member", param->member);
			JSSetProperty(msg, "receiver", param->receiver);
			JSSetProperty(msg, "state", param->state);
			JSSetProperty(msg, "type", param->type);
			JSSetProperty(msg, "userdata", param->userdata);
			JSSetProperty(msg, "var", param->var);
			params[0] = msg;

			LOG4CPLUS_TRACE(log, "Fire_onReceiveInterphoneMeetingMessage");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onReceiveInterphoneMeetingMessage_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onReceiveVoiceMeetingMessage:
	{
		if (this->onReceiveVoiceMeetingMessage_obj)
		{
			NPVariant result;
			ECVoiceMeetingMsg * param = reinterpret_cast<ECVoiceMeetingMsg *>(lParam);

			NPVariant params[1];

			NPVariant msg;
			JSNewObject(&msg);
			JSSetProperty(msg, "chatroomid", param->chatroomid);
			JSSetProperty(msg, "forbid", param->forbid);
			JSSetProperty(msg, "isVoIP", param->isVoIP);
			JSSetProperty(msg, "member", param->member);
			JSSetProperty(msg, "receiver", param->receiver);
			JSSetProperty(msg, "sender", param->sender);
			JSSetProperty(msg, "state", param->state);
			JSSetProperty(msg, "type", param->type);
			JSSetProperty(msg, "userdata", param->userdata);
			JSSetProperty(msg, "var", param->var);
			params[0] = msg;

			LOG4CPLUS_TRACE(log, "Fire_onReceiveVoiceMeetingMessage");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onReceiveVoiceMeetingMessage_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onReceiveVideoMeetingMessage:
	{
		if (this->onReceiveVideoMeetingMessage_obj)
		{
			NPVariant result;
			ECVideoMeetingMsg * param = reinterpret_cast<ECVideoMeetingMsg *>(lParam);

			NPVariant params[1];

			NPVariant msg;
			JSNewObject(&msg);
			JSSetProperty(msg, "dataState", param->dataState);
			JSSetProperty(msg, "forbid", param->forbid);
			JSSetProperty(msg, "isVoIP", param->isVoIP);
			JSSetProperty(msg, "member", param->member);
			JSSetProperty(msg, "receiver", param->receiver);
			JSSetProperty(msg, "roomid", param->roomid);
			JSSetProperty(msg, "sender", param->sender);
			JSSetProperty(msg, "state", param->state);
			JSSetProperty(msg, "type", param->type);
			JSSetProperty(msg, "userdata", param->userdata);
			JSSetProperty(msg, "var", param->var);
			JSSetProperty(msg, "videoState", param->videoState);
			params[0] = msg;

			LOG4CPLUS_TRACE(log, "Fire_onReceiveVideoMeetingMessage");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onReceiveVideoMeetingMessage_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onCreateMultimediaMeeting:
	{
		if (this->onCreateMultimediaMeeting_obj)
		{
			NPVariant result;
			EConCreateMultimediaMeeting * param = reinterpret_cast<EConCreateMultimediaMeeting *>(lParam);

			NPVariant params[1];

			NPVariant info;
			JSNewObject(&info);
			JSSetProperty(info, "confirm", param->info.confirm);
			JSSetProperty(info, "creator", param->info.creator);
			JSSetProperty(info, "Id", param->info.Id);
			JSSetProperty(info, "joinedCount", param->info.joinedCount);
			JSSetProperty(info, "keywords", param->info.keywords);
			JSSetProperty(info, "meetingType", param->info.meetingType);
			JSSetProperty(info, "name", param->info.name);
			JSSetProperty(info, "square", param->info.square);
			params[0] = info;

			LOG4CPLUS_TRACE(log, "Fire_onCreateMultimediaMeeting");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onCreateMultimediaMeeting_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onJoinMeeting:
	{
		if (this->onJoinMeeting_obj)
		{
			NPVariant result;
			EConJoinMeeting * param = reinterpret_cast<EConJoinMeeting *>(lParam);

			NPVariant params[2];
			params[0] = CNPVariant(param->reason);
			params[1] = CNPVariant(param->conferenceId);

			LOG4CPLUS_TRACE(log, "Fire_onJoinMeeting");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onJoinMeeting_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onExitMeeting:
	{
		if (this->onExitMeeting_obj)
		{
			NPVariant result;
			std::string * param = reinterpret_cast<std::string *>(lParam);

			NPVariant params[1];
			params[1] = CNPVariant(*param);

			LOG4CPLUS_TRACE(log, "Fire_onExitMeeting");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onExitMeeting_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onQueryMeetingMembers:
	{
		if (this->onQueryMeetingMembers_obj)
		{
			NPVariant result;
			EConQueryMeetingMembers * param = reinterpret_cast<EConQueryMeetingMembers *>(lParam);

			NPVariant params[4];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);
			params[2] = CNPVariant(param->conferenceId);

			JSNewArray(&params[3]);

			for (auto it : param->Members)
			{
				NPVariant info;
				JSNewObject(&info);
				JSSetProperty(info, "dataState", it.dataState);
				JSSetProperty(info, "forbid", it.forbid);
				JSSetProperty(info, "isMic", it.isMic);
				JSSetProperty(info, "isOnline", it.isOnline);
				JSSetProperty(info, "isVoIP", it.isVoIP);
				JSSetProperty(info, "meetingType", it.meetingType);
				JSSetProperty(info, "member", it.member);
				JSSetProperty(info, "state", it.state);
				JSSetProperty(info, "type", it.type);
				JSSetProperty(info, "userdata", it.userdata);
				JSSetProperty(info, "videoState", it.videoState);

				JSAddArrayElement(params[3], info);
			}

			LOG4CPLUS_TRACE(log, "Fire_onQueryMeetingMembers");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onQueryMeetingMembers_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onDismissMultiMediaMeeting:
	{
		if (this->onDismissMultiMediaMeeting_obj)
		{
			NPVariant result;
			EConDismissMultiMediaMeeting * param = reinterpret_cast<EConDismissMultiMediaMeeting *>(lParam);

			NPVariant params[3];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);
			params[2] = CNPVariant(param->conferenceId);

			LOG4CPLUS_TRACE(log, "Fire_onDismissMultiMediaMeeting");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onDismissMultiMediaMeeting_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onQueryMultiMediaMeetings:
	{
		if (this->onQueryMultiMediaMeetings_obj)
		{
			NPVariant result;
			EConQueryMultiMediaMeetings * param = reinterpret_cast<EConQueryMultiMediaMeetings *>(lParam);

			NPVariant params[3];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);

			JSNewArray(&params[2]);

			for (auto it : param->infos)
			{
				NPVariant info;
				JSNewObject(&info);
				JSSetProperty(info, "confirm", it.confirm);
				JSSetProperty(info, "creator", it.creator);
				JSSetProperty(info, "Id", it.Id);
				JSSetProperty(info, "joinedCount", it.joinedCount);
				JSSetProperty(info, "keywords", it.keywords);
				JSSetProperty(info, "meetingType", it.meetingType);
				JSSetProperty(info, "name", it.name);
				JSSetProperty(info, "square", it.square);

				JSAddArrayElement(params[2], info);
			}

			LOG4CPLUS_TRACE(log, "Fire_onQueryMultiMediaMeetings");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onQueryMultiMediaMeetings_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onDeleteMemberMultiMediaMeeting:
	{
		if (this->onDeleteMemberMultiMediaMeeting_obj)
		{
			NPVariant result;
			EConDeleteMemberMultiMediaMeeting * param = reinterpret_cast<EConDeleteMemberMultiMediaMeeting *>(lParam);

			NPVariant params[3];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);

			NPVariant info;
			JSNewObject(&info);
			JSSetProperty(info, "Id", param->info.Id);
			JSSetProperty(info, "isVoIP", param->info.isVoIP);
			JSSetProperty(info, "meetingType", param->info.meetingType);
			JSSetProperty(info, "member", param->info.member);
			params[2] = info;

			LOG4CPLUS_TRACE(log, "Fire_onDeleteMemberMultiMediaMeeting");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onDeleteMemberMultiMediaMeeting_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onInviteJoinMultiMediaMeeting:
	{
		if (this->onInviteJoinMultiMediaMeeting_obj)
		{
			NPVariant result;
			EConInviteJoinMultiMediaMeeting * param = reinterpret_cast<EConInviteJoinMultiMediaMeeting *>(lParam);

			NPVariant params[3];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);

			NPVariant info;
			JSNewObject(&info);
			JSSetProperty(info, "Id", param->info.Id.c_str());
			JSSetProperty(info, "isListen", param->info.isListen);
			JSSetProperty(info, "isSpeak", param->info.isSpeak);
			JSSetProperty(info, "userdata", param->info.userdata.c_str());

			NPVariant members;
			JSNewArray(&members);
			for (auto it : param->info.members)
			{
				JSAddArrayElement(members, CNPVariant(it));
			}
			JSSetProperty(info, "members", members);
			params[2] = info;
			LOG4CPLUS_TRACE(log, "Fire_onInviteJoinMultiMediaMeeting");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onInviteJoinMultiMediaMeeting_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onCreateInterphoneMeeting:
	{
		if (this->onCreateInterphoneMeeting_obj)
		{
			NPVariant result;
			EConCreateInterphoneMeeting * param = reinterpret_cast<EConCreateInterphoneMeeting *>(lParam);

			NPVariant params[3];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);
			params[2] = CNPVariant(param->interphoneId);

			LOG4CPLUS_TRACE(log, "Fire_onCreateInterphoneMeeting");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onCreateInterphoneMeeting_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onExitInterphoneMeeting:
	{
		if (this->onExitInterphoneMeeting_obj)
		{
			NPVariant result;
			std::string * param = reinterpret_cast<std::string *>(lParam);

			NPVariant params[1];
			params[0] = CNPVariant(*param);

			LOG4CPLUS_TRACE(log, "Fire_onExitInterphoneMeeting");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onExitInterphoneMeeting_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onSetMeetingSpeakListen:
	{
		if (this->onSetMeetingSpeakListen_obj)
		{
			NPVariant result;
			EConSetMeetingSpeakListen * param = reinterpret_cast<EConSetMeetingSpeakListen *>(lParam);

			NPVariant params[3];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);

			NPVariant info;
			JSNewObject(&info);
			JSSetProperty(info, "Id", param->Info.Id);
			JSSetProperty(info, "isVoIP", param->Info.isVoIP);
			JSSetProperty(info, "meetingType", param->Info.meetingType);
			JSSetProperty(info, "member", param->Info.member);
			JSSetProperty(info, "speaklisen", param->Info.speaklisen);

			params[2] = info;

			LOG4CPLUS_TRACE(log, "Fire_onSetMeetingSpeakListen");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onSetMeetingSpeakListen_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onControlInterphoneMic:
	{
		if (this->onControlInterphoneMic_obj)
		{
			NPVariant result;
			EConControlInterphoneMic * param = reinterpret_cast<EConControlInterphoneMic *>(lParam);

			NPVariant params[5];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);
			params[2] = CNPVariant(param->controller);
			params[3] = CNPVariant(param->interphoneId);
			params[4] = CNPVariant(param->requestIsControl);

			LOG4CPLUS_TRACE(log, "Fire_onControlInterphoneMic");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onControlInterphoneMic_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onPublishVideo:
	{
		if (this->onPublishVideo_obj)
		{
			NPVariant result;
			EConPublishVideo * param = reinterpret_cast<EConPublishVideo *>(lParam);

			NPVariant params[3];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);
			params[2] = CNPVariant(param->conferenceId);

			LOG4CPLUS_TRACE(log, "Fire_onPublishVideo");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onPublishVideo_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onUnpublishVideo:
	{
		if (this->onUnpublishVideo_obj)
		{
			NPVariant result;
			EConUnpublishVideo * param = reinterpret_cast<EConUnpublishVideo *>(lParam);

			NPVariant params[3];
			params[0] = CNPVariant(param->matchKey);
			params[1] = CNPVariant(param->reason);
			params[2] = CNPVariant(param->conferenceId);

			LOG4CPLUS_TRACE(log, "Fire_onUnpublishVideo");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onUnpublishVideo_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onRequestConferenceMemberVideo:
	{
		if (this->onRequestConferenceMemberVideo_obj)
		{
			NPVariant result;
			EConRequestConferenceMemberVideo * param = reinterpret_cast<EConRequestConferenceMemberVideo *>(lParam);

			NPVariant params[4];
			params[0] = CNPVariant(param->reason);
			params[1] = CNPVariant(param->conferenceId);
			params[2] = CNPVariant(param->member);
			params[3] = CNPVariant(param->type);

			LOG4CPLUS_TRACE(log, "Fire_onRequestConferenceMemberVideo");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onRequestConferenceMemberVideo_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onCancelConferenceMemberVideo:
	{
		if (this->onCancelConferenceMemberVideo_obj)
		{
			NPVariant result;
			EConCancelConferenceMemberVideo * param = reinterpret_cast<EConCancelConferenceMemberVideo *>(lParam);

			NPVariant params[4];
			params[0] = CNPVariant(param->reason);
			params[1] = CNPVariant(param->conferenceId);
			params[2] = CNPVariant(param->member);
			params[3] = CNPVariant(param->type);

			LOG4CPLUS_TRACE(log, "Fire_onCancelConferenceMemberVideo");
			delete param;
			NPN_InvokeDefault(this->m_pNPInstance, this->onCancelConferenceMemberVideo_obj, params, sizeof(params) / sizeof(params[0]), &result);
			NPN_ReleaseVariantValue(&result);
		}
	}
	break;
	case WM_onVncViewerCallBack:
	{

	}
	break;
	default:
		break;
	}
	return NPAPIPluginBase::PluginWinProc(hWnd, msg, wParam, lParam);
}

NPAPIECSDK::~NPAPIECSDK()
{
	NPN_ReleaseObject(onLogInfo_obj);
	NPN_ReleaseObject(onLogOut_obj);
	NPN_ReleaseObject(onConnectState_obj);
	NPN_ReleaseObject(onSetPersonInfo_obj);
	NPN_ReleaseObject(onGetPersonInfo_obj);
	NPN_ReleaseObject(onGetUserState_obj);
	NPN_ReleaseObject(onPublishPresence_obj);
	NPN_ReleaseObject(onReceiveFriendsPublishPresence_obj);
	NPN_ReleaseObject(onSoftVersion_obj);
	NPN_ReleaseObject(onGetOnlineMultiDevice_obj);
	NPN_ReleaseObject(onReceiveMultiDeviceState_obj);
	NPN_ReleaseObject(onMakeCallBack_obj);
	NPN_ReleaseObject(onCallEvents_obj);
	NPN_ReleaseObject(onCallIncoming_obj);
	NPN_ReleaseObject(onDtmfReceived_obj);
	NPN_ReleaseObject(onSwitchCallMediaTypeRequest_obj);
	NPN_ReleaseObject(onSwitchCallMediaTypeResponse_obj);
	NPN_ReleaseObject(onRemoteVideoRatio_obj);

	NPN_ReleaseObject(onOfflineMessageCount_obj);
	NPN_ReleaseObject(onGetOfflineMessage_obj);
	NPN_ReleaseObject(onOfflineMessageComplete_obj);
	NPN_ReleaseObject(onReceiveOfflineMessage_obj);
	NPN_ReleaseObject(onReceiveMessage_obj);
	NPN_ReleaseObject(onReceiveFile_obj);
	NPN_ReleaseObject(onDownloadFileComplete_obj);
	NPN_ReleaseObject(onRateOfProgressAttach_obj);
	NPN_ReleaseObject(onSendTextMessage_obj);
	NPN_ReleaseObject(onSendMediaFile_obj);
	NPN_ReleaseObject(onDeleteMessage_obj);
	NPN_ReleaseObject(onOperateMessage_obj);
	NPN_ReleaseObject(onReceiveOpreateNoticeMessage_obj);
	NPN_ReleaseObject(onUploadVTMFileOrBuf_obj);

	/****************************群组接口对应的回调******************************/
	NPN_ReleaseObject(onCreateGroup_obj);
	NPN_ReleaseObject(onDismissGroup_obj);
	NPN_ReleaseObject(onQuitGroup_obj);
	NPN_ReleaseObject(onJoinGroup_obj);
	NPN_ReleaseObject(onReplyRequestJoinGroup_obj);
	NPN_ReleaseObject(onInviteJoinGroup_obj);
	NPN_ReleaseObject(onReplyInviteJoinGroup_obj);
	NPN_ReleaseObject(onQueryOwnGroup_obj);
	NPN_ReleaseObject(onQueryGroupDetail_obj);
	NPN_ReleaseObject(onModifyGroup_obj);
	NPN_ReleaseObject(onSearchPublicGroup_obj);
	NPN_ReleaseObject(onQueryGroupMember_obj);
	NPN_ReleaseObject(onDeleteGroupMember_obj);
	NPN_ReleaseObject(onQueryGroupMemberCard_obj);
	NPN_ReleaseObject(onModifyGroupMemberCard_obj);
	NPN_ReleaseObject(onForbidMemberSpeakGroup_obj);
	NPN_ReleaseObject(onSetGroupMessageRule_obj);
	/****************************服务器下发的群组相关通知******************************/
	NPN_ReleaseObject(onReceiveGroupNoticeMessage_obj);

	/****************************音频播放回调接口******************************/
	NPN_ReleaseObject(onRecordingTimeOut_obj);
	NPN_ReleaseObject(onFinishedPlaying_obj);
	NPN_ReleaseObject(onRecordingAmplitude_obj);

	NPN_ReleaseObject(onMeetingIncoming_obj);
	NPN_ReleaseObject(onReceiveInterphoneMeetingMessage_obj);
	NPN_ReleaseObject(onReceiveVoiceMeetingMessage_obj);
	NPN_ReleaseObject(onReceiveVideoMeetingMessage_obj);
	NPN_ReleaseObject(onCreateMultimediaMeeting_obj);
	NPN_ReleaseObject(onJoinMeeting_obj);
	NPN_ReleaseObject(onExitMeeting_obj);
	NPN_ReleaseObject(onQueryMeetingMembers_obj);
	NPN_ReleaseObject(onDismissMultiMediaMeeting_obj);
	NPN_ReleaseObject(onQueryMultiMediaMeetings_obj);
	NPN_ReleaseObject(onDeleteMemberMultiMediaMeeting_obj);
	NPN_ReleaseObject(onInviteJoinMultiMediaMeeting_obj);
	NPN_ReleaseObject(onCreateInterphoneMeeting_obj);
	NPN_ReleaseObject(onExitInterphoneMeeting_obj);
	NPN_ReleaseObject(onSetMeetingSpeakListen_obj);
	NPN_ReleaseObject(onControlInterphoneMic_obj);
	NPN_ReleaseObject(onPublishVideo_obj);
	NPN_ReleaseObject(onUnpublishVideo_obj);
	NPN_ReleaseObject(onRequestConferenceMemberVideo_obj);
	NPN_ReleaseObject(onCancelConferenceMemberVideo_obj);
}
