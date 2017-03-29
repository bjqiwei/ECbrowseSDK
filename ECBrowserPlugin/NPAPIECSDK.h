#pragma once
#include "ECSDKBase.h"
#include "NPAPIPluginBase.h"
#include <log4cplus/logger.h>

class NPAPIECSDK:
	public ECSDKBase,
	public NPAPIPluginBase
{
public:
	explicit NPAPIECSDK(NPP pNPInstance);
	virtual ~NPAPIECSDK();
private:
	log4cplus::Logger log;

	//function id
	NPIdentifier fInitialize_id;
	NPIdentifier fUnInitialize_id;
	NPIdentifier fLogin_id;
	NPIdentifier fSetTraceFlag_id;
	NPIdentifier fSetServerEnvType_id;
	NPIdentifier fSetInternalDNS_id;
	NPIdentifier fLogout_id;
	NPIdentifier fSetPersonInfo_id;
	NPIdentifier fGetPersonInfo_id;
	NPIdentifier fGetUserState_id;
	NPIdentifier fQueryErrorDescribe_id;
	NPIdentifier fMakeCallBack_id;
	NPIdentifier fMakeCall_id;
	NPIdentifier fAcceptCall_id;
	NPIdentifier fReleaseCall_id;
	NPIdentifier fSendDTMF_id;
	NPIdentifier fGetCurrentCall_id;
	NPIdentifier fSetMute_id;
	NPIdentifier fGetMuteStatus_id;
	NPIdentifier fSetSoftMute_id;
	NPIdentifier fGetSoftMuteStatus_id;
	NPIdentifier fSetAudioConfigEnabled_id;
	NPIdentifier fGetAudioConfigEnabled_id;
	NPIdentifier fSetCodecEnabled_id;
	NPIdentifier fGetCodecEnabled_id;
	NPIdentifier fSetCodecNack_id;
	NPIdentifier fGetCodecNack_id;
	NPIdentifier fGetCallStatistics_id;
	NPIdentifier fSetVideoView_id;
	NPIdentifier fSetVideoBitRates_id;
	NPIdentifier fSetRing_id;
	NPIdentifier fSetRingback_id;
	NPIdentifier fRequestSwitchCallMediaType_id;
	NPIdentifier fResponseSwitchCallMediaType_id;
	NPIdentifier fGetCameraInfo_id;
	NPIdentifier fSelectCamera_id;
	NPIdentifier fGetNetworkStatistic_id;
	NPIdentifier fGetSpeakerVolume_id;
	NPIdentifier fSetSpeakerVolume_id;
	NPIdentifier fSetDtxEnabled_id;
	NPIdentifier fSetSelfPhoneNumber_id;
	NPIdentifier fSetSelfName_id;
	NPIdentifier fEnableLoudsSpeaker_id;
	NPIdentifier fGetLoudsSpeakerStatus_id;
	NPIdentifier fResetVideoView_id;
	NPIdentifier fSendTextMessage_id;
	NPIdentifier fSendMediaMessage_id;
	NPIdentifier fDownloadFileMessage_id;
	NPIdentifier fCancelUploadOrDownloadNOW_id;
	NPIdentifier fDeleteMessage_id;
	NPIdentifier fOperateMessage_id;
	NPIdentifier fUploadVTMFile_id;
	NPIdentifier fUploadVTMBuf_id;
	NPIdentifier fCreateGroup_id;
	NPIdentifier fDismissGroup_id;
	NPIdentifier fQuitGroup_id;
	NPIdentifier fJoinGroup_id;
	NPIdentifier fReplyRequestJoinGroup_id;
	NPIdentifier fInviteJoinGroup_id;
	NPIdentifier fReplyInviteJoinGroup_id;
	NPIdentifier fQueryOwnGroup_id;
	NPIdentifier fQueryGroupDetail_id;
	NPIdentifier fModifyGroup_id;
	NPIdentifier fSearchPublicGroups_id;
	NPIdentifier fQueryGroupMember_id;
	NPIdentifier fDeleteGroupMember_id;
	NPIdentifier fQueryGroupMemberCard_id;
	NPIdentifier fModifyMemberCard_id;
	NPIdentifier fForbidMemberSpeak_id;
	NPIdentifier fSetGroupMessageRule_id;
	NPIdentifier fStartVoiceRecording_id;
	NPIdentifier fStopVoiceRecording_id;
	NPIdentifier fPlayVoiceMsg_id;
	NPIdentifier fStopVoiceMsg_id;
	NPIdentifier fGetVoiceDuration_id;
	NPIdentifier fRequestConferenceMemberVideo_id;
	NPIdentifier fCancelMemberVideo_id;
	NPIdentifier fResetVideoConfWindow_id;
	NPIdentifier fCreateMultimediaMeeting_id;
	NPIdentifier fJoinMeeting_id;
	NPIdentifier fExitMeeting_id;
	NPIdentifier fQueryMeetingMembers_id;
	NPIdentifier fDismissMultiMediaMeeting_id;
	NPIdentifier fQueryMultiMediaMeetings_id;
	NPIdentifier fInviteJoinMultiMediaMeeting_id;
	NPIdentifier fDeleteMemberMultiMediaMeeting_id;
	NPIdentifier fPublishVideo_id;
	NPIdentifier fUnpublishVideo_id;
	NPIdentifier fCreateInterphoneMeeting_id;
	NPIdentifier fControlInterphoneMic_id;
    NPIdentifier fSetServerAddress_id;

	//property id
	NPIdentifier pversion_id;
	NPIdentifier pappDataDir_id;


	//Event id
	NPIdentifier onLogInfo_id;
	NPIdentifier onLogOut_id;
	NPIdentifier onConnectState_id;
	NPIdentifier onSetPersonInfo_id;
	NPIdentifier onGetPersonInfo_id;
	NPIdentifier onGetUserState_id;
	NPIdentifier onPublishPresence_id;
	NPIdentifier onReceiveFriendsPublishPresence_id;
	NPIdentifier onSoftVersion_id;
	NPIdentifier onGetOnlineMultiDevice_id;
	NPIdentifier onReceiveMultiDeviceState_id;
	NPIdentifier onMakeCallBack_id;
	NPIdentifier onCallEvents_id;
	NPIdentifier onCallIncoming_id;
	NPIdentifier onDtmfReceived_id;
	NPIdentifier onSwitchCallMediaTypeRequest_id;
	NPIdentifier onSwitchCallMediaTypeResponse_id;
	NPIdentifier onRemoteVideoRatio_id;

	NPIdentifier onOfflineMessageCount_id;
	NPIdentifier onGetOfflineMessage_id;
	NPIdentifier onOfflineMessageComplete_id;
	NPIdentifier onReceiveOfflineMessage_id;
	NPIdentifier onReceiveMessage_id;
	NPIdentifier onReceiveFile_id;
	NPIdentifier onDownloadFileComplete_id;
	NPIdentifier onRateOfProgressAttach_id;
	NPIdentifier onSendTextMessage_id;
	NPIdentifier onSendMediaFile_id;
	NPIdentifier onDeleteMessage_id;
	NPIdentifier onOperateMessage_id;
	NPIdentifier onReceiveOpreateNoticeMessage_id;
	NPIdentifier onUploadVTMFileOrBuf_id;

	/****************************群组接口对应的回调******************************/
	NPIdentifier onCreateGroup_id;
	NPIdentifier onDismissGroup_id;
	NPIdentifier onQuitGroup_id;
	NPIdentifier onJoinGroup_id;
	NPIdentifier onReplyRequestJoinGroup_id;
	NPIdentifier onInviteJoinGroup_id;
	NPIdentifier onReplyInviteJoinGroup_id;
	NPIdentifier onQueryOwnGroup_id;
	NPIdentifier onQueryGroupDetail_id;
	NPIdentifier onModifyGroup_id;
	NPIdentifier onSearchPublicGroup_id;
	NPIdentifier onQueryGroupMember_id;
	NPIdentifier onDeleteGroupMember_id;
	NPIdentifier onQueryGroupMemberCard_id;
	NPIdentifier onModifyGroupMemberCard_id;
	NPIdentifier onForbidMemberSpeakGroup_id;
	NPIdentifier onSetGroupMessageRule_id;
	/****************************服务器下发的群组相关通知******************************/
	NPIdentifier onReceiveGroupNoticeMessage_id;

	/****************************音频播放回调接口******************************/
	NPIdentifier onRecordingTimeOut_id;
	NPIdentifier onFinishedPlaying_id;
	NPIdentifier onRecordingAmplitude_id;

	NPIdentifier onMeetingIncoming_id;
	NPIdentifier onReceiveInterphoneMeetingMessage_id;
	NPIdentifier onReceiveVoiceMeetingMessage_id;
	NPIdentifier onReceiveVideoMeetingMessage_id;
	NPIdentifier onCreateMultimediaMeeting_id;
	NPIdentifier onJoinMeeting_id;
	NPIdentifier onExitMeeting_id;
	NPIdentifier onQueryMeetingMembers_id;
	NPIdentifier onDismissMultiMediaMeeting_id;
	NPIdentifier onQueryMultiMediaMeetings_id;
	NPIdentifier onDeleteMemberMultiMediaMeeting_id;
	NPIdentifier onInviteJoinMultiMediaMeeting_id;
	NPIdentifier onCreateInterphoneMeeting_id;
	NPIdentifier onExitInterphoneMeeting_id;
	NPIdentifier onSetMeetingSpeakListen_id;
	NPIdentifier onControlInterphoneMic_id;
	NPIdentifier onPublishVideo_id;
	NPIdentifier onUnpublishVideo_id;
	NPIdentifier onRequestConferenceMemberVideo_id;
	NPIdentifier onCancelConferenceMemberVideo_id;
	
	
	//Object id
	NPObject* onLogInfo_obj;
	NPObject* onLogOut_obj;
	NPObject* onConnectState_obj;
	NPObject* onSetPersonInfo_obj;
	NPObject* onGetPersonInfo_obj;
	NPObject* onGetUserState_obj;
	NPObject* onPublishPresence_obj;
	NPObject* onReceiveFriendsPublishPresence_obj;
	NPObject* onSoftVersion_obj;
	NPObject* onGetOnlineMultiDevice_obj;
	NPObject* onReceiveMultiDeviceState_obj;
	NPObject* onMakeCallBack_obj;
	NPObject* onCallEvents_obj;
	NPObject* onCallIncoming_obj;
	NPObject* onDtmfReceived_obj;
	NPObject* onSwitchCallMediaTypeRequest_obj;
	NPObject* onSwitchCallMediaTypeResponse_obj;
	NPObject* onRemoteVideoRatio_obj;

	NPObject* onOfflineMessageCount_obj;
	NPObject* onGetOfflineMessage_obj;
	NPObject* onOfflineMessageComplete_obj;
	NPObject* onReceiveOfflineMessage_obj;
	NPObject* onReceiveMessage_obj;
	NPObject* onReceiveFile_obj;
	NPObject* onDownloadFileComplete_obj;
	NPObject* onRateOfProgressAttach_obj;
	NPObject* onSendTextMessage_obj;
	NPObject* onSendMediaFile_obj;
	NPObject* onDeleteMessage_obj;
	NPObject* onOperateMessage_obj;
	NPObject* onReceiveOpreateNoticeMessage_obj;
	NPObject* onUploadVTMFileOrBuf_obj;

	/****************************群组接口对应的回调******************************/
	NPObject* onCreateGroup_obj;
	NPObject* onDismissGroup_obj;
	NPObject* onQuitGroup_obj;
	NPObject* onJoinGroup_obj;
	NPObject* onReplyRequestJoinGroup_obj;
	NPObject* onInviteJoinGroup_obj;
	NPObject* onReplyInviteJoinGroup_obj;
	NPObject* onQueryOwnGroup_obj;
	NPObject* onQueryGroupDetail_obj;
	NPObject* onModifyGroup_obj;
	NPObject* onSearchPublicGroup_obj;
	NPObject* onQueryGroupMember_obj;
	NPObject* onDeleteGroupMember_obj;
	NPObject* onQueryGroupMemberCard_obj;
	NPObject* onModifyGroupMemberCard_obj;
	NPObject* onForbidMemberSpeakGroup_obj;
	NPObject* onSetGroupMessageRule_obj;
	/****************************服务器下发的群组相关通知******************************/
	NPObject* onReceiveGroupNoticeMessage_obj;

	/****************************音频播放回调接口******************************/
	NPObject* onRecordingTimeOut_obj;
	NPObject* onFinishedPlaying_obj;
	NPObject* onRecordingAmplitude_obj;

	NPObject* onMeetingIncoming_obj;
	NPObject* onReceiveInterphoneMeetingMessage_obj;
	NPObject* onReceiveVoiceMeetingMessage_obj;
	NPObject* onReceiveVideoMeetingMessage_obj;
	NPObject* onCreateMultimediaMeeting_obj;
	NPObject* onJoinMeeting_obj;
	NPObject* onExitMeeting_obj;
	NPObject* onQueryMeetingMembers_obj;
	NPObject* onDismissMultiMediaMeeting_obj;
	NPObject* onQueryMultiMediaMeetings_obj;
	NPObject* onDeleteMemberMultiMediaMeeting_obj;
	NPObject* onInviteJoinMultiMediaMeeting_obj;
	NPObject* onCreateInterphoneMeeting_obj;
	NPObject* onExitInterphoneMeeting_obj;
	NPObject* onSetMeetingSpeakListen_obj;
	NPObject* onControlInterphoneMic_obj;
	NPObject* onPublishVideo_obj;
	NPObject* onUnpublishVideo_obj;
	NPObject* onRequestConferenceMemberVideo_obj;
	NPObject* onCancelConferenceMemberVideo_obj;

public:
	virtual NPBool init(NPWindow* pNPWindow);
	virtual void shut();
	virtual HWND  gethWnd() const;
	virtual const char * getName()const
	{
		return "Cloopen Browser SDK.";
	}
	virtual const char * getDescription()const
	{
		return "Cloopen Browser SDK for FireFox or Chrome.";
	}

protected:
	virtual bool HasMethod(const NPIdentifier name) const;
	virtual bool HasProperty(const NPIdentifier name) const;
	virtual bool GetProperty(const NPIdentifier name, NPVariant *result) const;
	virtual bool SetProperty(const NPIdentifier name, const NPVariant *value);
	virtual bool RemoveProperty(const NPIdentifier name) const;
	virtual bool Invoke(const NPIdentifier name, const NPVariant *args,
		uint32_t argCount, NPVariant *result);

public:
	virtual LRESULT CALLBACK PluginWinProc(HWND, UINT, WPARAM, LPARAM);
};

