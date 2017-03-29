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
	/****************************��¼�ӿڵĻص�******************************/
	virtual void onLogInfo(const char* loginfo); // ���ڽ��յײ��log��Ϣ,���Գ��ֵ�����.
	virtual void onLogOut(int reason);//�ǳ��ص���reason��200�ɹ�����������
	virtual void onConnectState(const ECConnectState state);//���ӻص���
	virtual void onSetPersonInfo(unsigned int matchKey, int reason, unsigned long long version);//���ø������ϡ�reason��200�ɹ�����������version:�������ϰ汾��
	virtual void onGetPersonInfo(unsigned int matchKey, int reason, ECPersonInfo *pPersonInfo);//��ȡ�������ϡ�reason��200�ɹ�����������
	virtual void onGetUserState(unsigned int matchKey, int reason, int count, ECPersonState *pStateArr);//��ȡ�û�״̬��reason��200�ɹ�����������;count ���������pStateArr �û�״̬������ָ��
	virtual void onPublishPresence(unsigned int matchKey, int reason);//�����û�״̬��reason��200�ɹ�����������
	virtual void onReceiveFriendsPublishPresence(int count, ECPersonState *pStateArr);//�յ����������͵��û�״̬�仯,count ���������pStateArr �û�״̬������ָ��
	virtual void onSoftVersion(const char* softVersion, int updateMode, const char* updateDesc);//Ӧ��������°汾��������ʾ����Ӧ���Լ������á�softVersion��������°汾�ţ�updateMode:����ģʽ  1���ֶ����� 2��ǿ�Ƹ��£�updateDesc���������˵�� 
	virtual void onGetOnlineMultiDevice(unsigned int matchKey, int reason, int count, ECMultiDeviceState *pMultiDeviceStateArr);//��õ�¼�߶��豸��¼״̬�仯�ص�,reason��200�ɹ�����������count ���������pMultiDeviceStateArr ���豸��¼״̬������ָ��
	virtual void onReceiveMultiDeviceState(int count, ECMultiDeviceState *pMultiDeviceStateArr);//�յ����������͵ĵ�¼�߶��豸��¼״̬�仯,count ���������pMultiDeviceStateArr ���豸��¼״̬������ָ��

	/****************************VOIP�ص��ӿ�******************************/
	virtual void onMakeCallBack(unsigned int matchKey, int reason, ECCallBackInfo *pInfo);//�ز�����reason��200�ɹ�����������
	virtual void onCallEvents(int reason, const ECVoIPCallMsg * call);//�¼�״̬
	virtual void onCallIncoming(ECVoIPComingInfo *pComing);  //����Ƶ���нӵ�����
	virtual void onDtmfReceived(const char *callId, char dtmf);//�յ�DTMF����ʱ�Ļص���callId ͨ��ID��	
	virtual void onSwitchCallMediaTypeRequest(const char*callId, int video); // �յ��Է�����ý������ callId ͨ��ID��video��1  ����������Ƶ����Ҫ��Ӧ�� 0:����ɾ����Ƶ�����Բ���Ӧ���ײ��Զ�ȥ����Ƶ��
	virtual void onSwitchCallMediaTypeResponse(const char*callId, int video);  // �Է�Ӧ��ý��״̬�����callId ͨ��ID�� video 1 ����Ƶ 0 ����Ƶ
	virtual void onRemoteVideoRatio(const char *CallidOrConferenceId, int width, int height, int type, const char *member);//Զ����Ƶý��ֱ��ʱ仯ʱ�ϱ���type : 0 ��Ե�,1 ��Ƶ���飬2 ������飻type=0 CallidOrConferenceIdΪͨ��id��type>0 CallidOrConferenceIdΪ����id��member��Ч

	/****************************��Ϣ����ӿڵĻص�******************************/
	virtual void onOfflineMessageCount(int count);//����������
protected:
	int m_onGetOfflineMessage_Result;
	std::mutex m_onGetOfflineMessage_mtx;
	std::condition_variable m_onGetOfflineMessage_cv;
private:
	virtual int  onGetOfflineMessage() override;//������ȡ������<0 ȫ����0 ����������0��ֻ��ȡ���µ�������
	virtual void onOfflineMessageComplete() override;//��ȡ���߳ɹ�
	virtual void onReceiveOfflineMessage(ECMessage *pMsg) override;//������Ϣ�ص�
	virtual void onReceiveMessage(ECMessage *pMsg) override;//ʵʱ��Ϣ�ص�
	virtual void onReceiveFile(ECMessage* pFileInfo) override;//�û��յ������߷��͹������ļ��ȴ��û������Ƿ������ļ��Ļص����ûص���Ϣ����Ϣ����(MsgType)Ĭ����6
	virtual void onDownloadFileComplete(unsigned int matchKey, int reason, ECMessage *msg) override;//�����ļ��ص���������ɺ󱻵��ã�����ʧ��Ҳ�ᱻ������reason:0�ɹ�����������offset ���ݴ�Сƫ����
	virtual void onRateOfProgressAttach(unsigned int matchKey, unsigned long long rateSize, unsigned long long fileSize, ECMessage *msg);//�ϴ������ļ����ȡ�ratesize �Ѿ���ɽ��ȴ�С���ֽڣ���filezise ���ļ���С���ֽڣ�
	virtual void onSendTextMessage(unsigned int matchKey, int reason, ECMessage *msg);//������Ϣ�ص���reason��200�ɹ�����������
	virtual void onSendMediaFile(unsigned int matchKey, int reason, ECMessage *msg);//�ϴ��ļ��ص����ϴ���ɺ󱻵��ã��ϴ�ʧ��Ҳ�ᱻ������reason:0�ɹ�����������offset ���ݴ�Сƫ����
	virtual void onDeleteMessage(unsigned int matchKey, int reason, int type, const char* msgId);//ɾ���������Ϣ��reason:0�ɹ�����������type:͸����Ϣ���ͣ�msgId:
	virtual void onOperateMessage(unsigned int tcpMsgIdOut, int reason) override;//ɾ����Ϣ
	virtual void onReceiveOpreateNoticeMessage(const ECOperateNoticeMessage *pMsg) override;
	virtual void onUploadVTMFileOrBuf(unsigned int matchKey, int reason, const char* fileUrl);//�ϴ�VTM�ļ��ص�, fileIdOut���ļ�id, fileUrl:�ļ������������ַ

	/****************************Ⱥ��ӿڶ�Ӧ�Ļص�******************************/
	virtual void onCreateGroup(unsigned int matchKey, int reason, ECGroupDetailInfo *pInfo);//����Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������
	virtual void onDismissGroup(unsigned int matchKey, int reason, const char* groupid);//��ɢȺ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��
	virtual void onQuitGroup(unsigned int matchKey, int reason, const char* groupid);//�˳�Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��
	virtual void onJoinGroup(unsigned int matchKey, int reason, const char* groupid);//����Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��
	virtual void onReplyRequestJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int confirm);//�ظ��������Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��member ��Ա��confirm 1�ܾ� 2ͬ��
	virtual void onInviteJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char** members, int membercount, int confirm);//�������Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��members ��Ա���飻membercount ������� ��confirm 1�ܾ� 2ͬ��
	virtual void onReplyInviteJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int confirm);//�ظ��������Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��member ��Ա��confirm 1�ܾ� 2ͬ��
	virtual void onQueryOwnGroup(unsigned int matchKey, int reason, int count, ECGroupSimpleInfo *group);//��ѯ�ҵ�Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������
	virtual void onQueryGroupDetail(unsigned int matchKey, int reason, ECGroupDetailInfo *detail);//��ѯȺ�����顣matchKey ƥ��������ֵ��reason��200�ɹ�����������
	virtual void onModifyGroup(unsigned int matchKey, int reason, ECModifyGroupInfo *pInfo);//�޸�Ⱥ����Ϣ��matchKey ƥ��������ֵ��reason��200�ɹ�����������
	virtual void onSearchPublicGroup(unsigned int matchKey, int reason, int searchType, const char* keyword, int count, ECGroupSearchInfo *group, int pageNo);//��������Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������searchType �������� 1����GroupId����ȷ���� 2����GroupName��ģ�������� keyword �����ؼ��ʣ�count Ⱥ�������int pageNo ������ҳ����
	virtual void onQueryGroupMember(unsigned int matchKey, int reason, const char *groupid, int count, ECGroupMember *member);//��ѯȺ���Ա��matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��
	virtual void onDeleteGroupMember(unsigned int matchKey, int reason, const char* groupid, const char* member);//ɾ��Ⱥ���Ա��matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��member ��Ա
	virtual void onQueryGroupMemberCard(unsigned int matchKey, int reason, ECGroupMemberCard *card);//��ѯȺ���Ա��Ƭ��matchKey ƥ��������ֵ��reason��200�ɹ�����������
	virtual void onModifyGroupMemberCard(unsigned int matchKey, int reason, ECGroupMemberCard* pCard);//�޸�Ⱥ���Ա��Ƭ��matchKey ƥ��������ֵ��reason��200�ɹ�����������
	virtual void onForbidMemberSpeakGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int isBan);//Ⱥ����ԡ�matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��member ��Ա��isBan 1������ 2��ֹ����
	virtual void onSetGroupMessageRule(unsigned int matchKey, int reason, const char* groupid, bool notice);//����Ⱥ����Ϣ����matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��notice �Ƿ�֪ͨ
    virtual void onSetGroupMemberRole(unsigned int matchKey, int reason);//����Ⱥ���ԱȨ��
	/****************************�������·���Ⱥ�����֪ͨ******************************/
	virtual void onReceiveGroupNoticeMessage(const ECGroupNoticeMessage * pMsg);

	/****************************��Ƶ���Żص��ӿ�******************************/
	virtual void onRecordingTimeOut(int ms, const char* sessionId);//¼����ʱ��ms Ϊ¼��ʱ������λ����,sessionId ͸���ỰID
	virtual void onFinishedPlaying(const char* sessionId, int reason);//���Ž���,sessionId͸���ỰID,reason 200�ɹ�.����ʧ��
	virtual void onRecordingAmplitude(double amplitude, const char* sessionId);//���������������¼��ʱ�����Χ0-1,sessionId ͸���ỰID

	/********************************************����ص��ӿ�***********************************************/
	virtual void onMeetingIncoming(ECMeetingComingInfo *pCome);//����ӵ����롣
	virtual void onReceiveInterphoneMeetingMessage(ECInterphoneMsg* pInterphoneMsg);// �յ�ʵʱ�Խ������֪ͨ��Ϣ��
	virtual void onReceiveVoiceMeetingMessage(ECVoiceMeetingMsg* pVoiceMeetingMsg);// �յ��������������֪ͨ��Ϣ��
	virtual void onReceiveVideoMeetingMessage(ECVideoMeetingMsg* pVideoMeetingMsg);// �յ���Ƶ���������֪ͨ��Ϣ��
	virtual void onCreateMultimediaMeeting(unsigned int matchKey, int reason, ECMeetingInfo *pInfo);//������Ƶ����Ƶ����Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������
	virtual void onJoinMeeting(int reason, const char* conferenceId);//�����������Ļص���//reason��200�ɹ�����������conferenceId ����ID��
	virtual void onExitMeeting(const char* conferenceId);//�˳����顣conferenceId ����ID��
	virtual void onQueryMeetingMembers(unsigned int matchKey, int reason, const char* conferenceId, int count, ECMeetingMemberInfo* pMembers);//��ѯ�������ĳ�Ա��matchKey ƥ��������ֵ��reason��200�ɹ�����������conferenceId ����ID��count ��Ա���������pMembers ��Ա����
	virtual void onDismissMultiMediaMeeting(unsigned int matchKey, int reason, const char*conferenceId);//��ɢʵʱ�Խ�����Ƶ����Ƶ����Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������conferenceId ����ID��
	virtual void onQueryMultiMediaMeetings(unsigned int matchKey, int reason, int count, ECMeetingInfo* pMeetingInfo);//��ѯ������Ϣ�Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������count �������������pMembers ��������
	virtual void onDeleteMemberMultiMediaMeeting(unsigned int matchKey, int reason, ECMeetingDeleteMemberInfo *pInfo);//ɾ������Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������
	virtual void onInviteJoinMultiMediaMeeting(unsigned int matchKey, int reason, ECMeetingInviteInfo *pInfo);//����������Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������
	virtual void onCreateInterphoneMeeting(unsigned int matchKey, int reason, const char* interphoneId);//����ʵʱ�Խ�����Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������interphoneId ʵʱ�Խ�ID
	virtual void onExitInterphoneMeeting(const char* interphoneId);//�˳�ʵʱ�Խ���interphoneId ʵʱ�Խ�ID
	virtual void onSetMeetingSpeakListen(unsigned int matchKey, int reason, ECMeetingMemberSpeakListenInfo* pInfo) override;//���û����Ա����״̬�Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������
	virtual void onControlInterphoneMic(unsigned int matchKey, int reason, const char* controller, const char* interphoneId, bool requestIsControl);//ʵʱ�Խ��������Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������controller �����Ա��interphoneId ʵʱ�Խ�ID��requestIsControl �Ƿ������
	virtual void onPublishVideo(unsigned int matchKey, int reason, const char* conferenceId);//������Ƶ�Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������conferenceId ����ID��
	virtual void onUnpublishVideo(unsigned int matchKey, int reason, const char* conferenceId);//ȡ����Ƶ�Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������conferenceId ����ID��
	virtual void onRequestConferenceMemberVideo(int reason, const char *conferenceId, const char *member, int type);//��Ƶ����ʱ����������Ա��Ƶ���ݡ�reason ���Video_Conference_status��conferenceId ����ID��member��Ա��type���ECVideoDataType��
	virtual void onCancelConferenceMemberVideo(int reason, const char *conferenceId, const char *member, int type);//��Ƶ����ʱ��ȡ�������Ա��Ƶ������Ӧ��reason ���Video_Conference_status��conferenceId ����ID��tmember��Ա��ype���ECVideoDataType��

private:
	log4cplus::Logger log;
public:
	int RGB24Snapshot(BYTE* pData, int width, int height, unsigned char ** outData, unsigned int *outSize);
};

