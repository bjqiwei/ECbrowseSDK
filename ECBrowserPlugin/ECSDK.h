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
	std::string Id;//����ID
	std::vector<std::string> members;//��Ա����
	std::string userdata;//�Զ�������
	char isSpeak;//0���ɽ� 1�ɽ�
	char isListen;//0������ 1����

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
	 /****************************��¼�ӿڵĻص�******************************/
	static void sonLogInfo(const char* loginfo); // ���ڽ��յײ��log��Ϣ,���Գ��ֵ�����.
	static void sonLogOut(int reason);//�ǳ��ص���reason��200�ɹ�����������
	static void sonConnectState(const ECConnectState state);//���ӻص���
	static void sonSetPersonInfo(unsigned int matchKey, int reason, unsigned long long version);//���ø������ϡ�reason��200�ɹ�����������version:�������ϰ汾��
	static void sonGetPersonInfo(unsigned int matchKey, int reason, ECPersonInfo *pPersonInfo);//��ȡ�������ϡ�reason��200�ɹ�����������
	static void sonGetUserState(unsigned int matchKey, int reason, int count, ECPersonState *pStateArr);//��ȡ�û�״̬��reason��200�ɹ�����������;count ���������pStateArr �û�״̬������ָ��
	static void sonPublishPresence(unsigned int matchKey, int reason);//�����û�״̬��reason��200�ɹ�����������
	static void sonReceiveFriendsPublishPresence(int count, ECPersonState *pStateArr);//�յ����������͵��û�״̬�仯,count ���������pStateArr �û�״̬������ָ��
	static void sonSoftVersion(const char* softVersion, int updateMode, const char* updateDesc);//Ӧ��������°汾��������ʾ����Ӧ���Լ������á�softVersion��������°汾�ţ�updateMode:����ģʽ  1���ֶ����� 2��ǿ�Ƹ��£�updateDesc���������˵�� 
	static void sonGetOnlineMultiDevice(unsigned int matchKey, int reason, int count, ECMultiDeviceState *pMultiDeviceStateArr);//��õ�¼�߶��豸��¼״̬�仯�ص�,reason��200�ɹ�����������count ���������pMultiDeviceStateArr ���豸��¼״̬������ָ��
	static void sonReceiveMultiDeviceState(int count, ECMultiDeviceState *pMultiDeviceStateArr);//�յ����������͵ĵ�¼�߶��豸��¼״̬�仯,count ���������pMultiDeviceStateArr ���豸��¼״̬������ָ��

	/****************************VOIP�ص��ӿ�******************************/
	static void sonMakeCallBack(unsigned int matchKey, int reason, ECCallBackInfo *pInfo);//�ز�����reason��200�ɹ�����������
	static void sonCallEvents(int reason, const ECVoIPCallMsg * call);//�¼�״̬
	static void sonCallIncoming(ECVoIPComingInfo *pComing);  //����Ƶ���нӵ�����
	static void sonDtmfReceived(const char *callId, char dtmf);//�յ�DTMF����ʱ�Ļص���callId ͨ��ID��	
	static void sonSwitchCallMediaTypeRequest(const char*callId, int video); // �յ��Է�����ý������ callId ͨ��ID��video��1  ����������Ƶ����Ҫ��Ӧ�� 0:����ɾ����Ƶ�����Բ���Ӧ���ײ��Զ�ȥ����Ƶ��
	static void sonSwitchCallMediaTypeResponse(const char*callId, int video);  // �Է�Ӧ��ý��״̬�����callId ͨ��ID�� video 1 ����Ƶ 0 ����Ƶ
	static void sonRemoteVideoRatio(const char *CallidOrConferenceId, int width, int height, int type, const char *member);//Զ����Ƶý��ֱ��ʱ仯ʱ�ϱ���type : 0 ��Ե�,1 ��Ƶ���飬2 ������飻type=0 CallidOrConferenceIdΪͨ��id��type>0 CallidOrConferenceIdΪ����id��member��Ч

	/****************************��Ϣ����ӿڵĻص�******************************/
	static void sonOfflineMessageCount(int count);//����������
	static int  sonGetOfflineMessage();//������ȡ������<0 ȫ����0 ����������0��ֻ��ȡ���µ�������
	static void sonOfflineMessageComplete();//��ȡ���߳ɹ�
	static void sonReceiveOfflineMessage(ECMessage *pMsg);//������Ϣ�ص�
	static void sonReceiveMessage(ECMessage *pMsg);//ʵʱ��Ϣ�ص�
	static void sonReceiveFile(ECMessage* pFileInfo);//�û��յ������߷��͹������ļ��ȴ��û������Ƿ������ļ��Ļص����ûص���Ϣ����Ϣ����(MsgType)Ĭ����6
	static void sonDownloadFileComplete(unsigned int matchKey, int reason, ECMessage *msg);//�����ļ��ص���������ɺ󱻵��ã�����ʧ��Ҳ�ᱻ������reason:0�ɹ�����������offset ���ݴ�Сƫ����
	static void sonRateOfProgressAttach(unsigned int matchKey, unsigned long long rateSize, unsigned long long fileSize, ECMessage *msg);//�ϴ������ļ����ȡ�ratesize �Ѿ���ɽ��ȴ�С���ֽڣ���filezise ���ļ���С���ֽڣ�
	static void sonSendTextMessage(unsigned int matchKey, int reason, ECMessage *msg);//������Ϣ�ص���reason��200�ɹ�����������
	static void sonSendMediaFile(unsigned int matchKey, int reason, ECMessage *msg);//�ϴ��ļ��ص����ϴ���ɺ󱻵��ã��ϴ�ʧ��Ҳ�ᱻ������reason:0�ɹ�����������offset ���ݴ�Сƫ����
	static void sonDeleteMessage(unsigned int matchKey, int reason, int type, const char* msgId);//ɾ���������Ϣ��reason:0�ɹ�����������type:͸����Ϣ���ͣ�msgId:
	static void sonOperateMessage(unsigned int tcpMsgIdOut, int reason);//ɾ����Ϣ
	static void sonReceiveOpreateNoticeMessage(const ECOperateNoticeMessage *pMsg);
	static void sonUploadVTMFileOrBuf(unsigned int matchKey, int reason, const char* fileUrl);//�ϴ�VTM�ļ��ص�, fileIdOut���ļ�id, fileUrl:�ļ������������ַ

	/****************************Ⱥ��ӿڶ�Ӧ�Ļص�******************************/
	static void sonCreateGroup(unsigned int matchKey, int reason, ECGroupDetailInfo *pInfo);//����Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������
	static void sonDismissGroup(unsigned int matchKey, int reason, const char* groupid);//��ɢȺ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��
	static void sonQuitGroup(unsigned int matchKey, int reason, const char* groupid);//�˳�Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��
	static void sonJoinGroup(unsigned int matchKey, int reason, const char* groupid);//����Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��
	static void sonReplyRequestJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int confirm);//�ظ��������Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��member ��Ա��confirm 1�ܾ� 2ͬ��
	static void sonInviteJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char** members, int membercount, int confirm);//�������Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��members ��Ա���飻membercount ������� ��confirm 1�ܾ� 2ͬ��
	static void sonReplyInviteJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int confirm);//�ظ��������Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��member ��Ա��confirm 1�ܾ� 2ͬ��
	static void sonQueryOwnGroup(unsigned int matchKey, int reason, int count, ECGroupSimpleInfo *group);//��ѯ�ҵ�Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������
	static void sonQueryGroupDetail(unsigned int matchKey, int reason, ECGroupDetailInfo *detail);//��ѯȺ�����顣matchKey ƥ��������ֵ��reason��200�ɹ�����������
	static void sonModifyGroup(unsigned int matchKey, int reason, ECModifyGroupInfo *pInfo);//�޸�Ⱥ����Ϣ��matchKey ƥ��������ֵ��reason��200�ɹ�����������
	static void sonSearchPublicGroup(unsigned int matchKey, int reason, int searchType, const char* keyword, int count, ECGroupSearchInfo *group);//��������Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������searchType �������� 1����GroupId����ȷ���� 2����GroupName��ģ�������� keyword �����ؼ��ʣ�count Ⱥ�������
	static void sonQueryGroupMember(unsigned int matchKey, int reason, const char *groupid, int count, ECGroupMember *member);//��ѯȺ���Ա��matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��
	static void sonDeleteGroupMember(unsigned int matchKey, int reason, const char* groupid, const char* member);//ɾ��Ⱥ���Ա��matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��member ��Ա
	static void sonQueryGroupMemberCard(unsigned int matchKey, int reason, ECGroupMemberCard *card);//��ѯȺ���Ա��Ƭ��matchKey ƥ��������ֵ��reason��200�ɹ�����������
	static void sonModifyGroupMemberCard(unsigned int matchKey, int reason, ECGroupMemberCard* pCard);//�޸�Ⱥ���Ա��Ƭ��matchKey ƥ��������ֵ��reason��200�ɹ�����������
	static void sonForbidMemberSpeakGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int isBan);//Ⱥ����ԡ�matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��member ��Ա��isBan 1������ 2��ֹ����
	static void sonSetGroupMessageRule(unsigned int matchKey, int reason, const char* groupid, bool notice);//����Ⱥ����Ϣ����matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��notice �Ƿ�֪ͨ
    static void sonSetGroupMemberRole(unsigned int matchKey, int reason);//����Ⱥ���ԱȨ��
	/****************************�������·���Ⱥ�����֪ͨ******************************/
	static void sonReceiveGroupNoticeMessage(const ECGroupNoticeMessage * pMsg);

	/****************************��Ƶ���Żص��ӿ�******************************/
	static void sonRecordingTimeOut(int ms, const char* sessionId);//¼����ʱ��ms Ϊ¼��ʱ������λ����,sessionId ͸���ỰID
	static void sonFinishedPlaying(const char* sessionId, int reason);//���Ž���,sessionId͸���ỰID,reason 200�ɹ�.����ʧ��
	static void sonRecordingAmplitude(double amplitude, const char* sessionId);//���������������¼��ʱ�����Χ0-1,sessionId ͸���ỰID

	/********************************************����ص��ӿ�***********************************************/
	static void sonMeetingIncoming(ECMeetingComingInfo *pCome);//����ӵ����롣
	static void sonReceiveInterphoneMeetingMessage(ECInterphoneMsg* pInterphoneMsg);// �յ�ʵʱ�Խ������֪ͨ��Ϣ��
	static void sonReceiveVoiceMeetingMessage(ECVoiceMeetingMsg* pVoiceMeetingMsg);// �յ��������������֪ͨ��Ϣ��
	static void sonReceiveVideoMeetingMessage(ECVideoMeetingMsg* pVideoMeetingMsg);// �յ���Ƶ���������֪ͨ��Ϣ��
	static void sonCreateMultimediaMeeting(unsigned int matchKey, int reason, ECMeetingInfo *pInfo);//������Ƶ����Ƶ����Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������
	static void sonJoinMeeting(int reason, const char* conferenceId);//�����������Ļص���//reason��200�ɹ�����������conferenceId ����ID��
	static void sonExitMeeting(const char* conferenceId);//�˳����顣conferenceId ����ID��
	static void sonQueryMeetingMembers(unsigned int matchKey, int reason, const char* conferenceId, int count, ECMeetingMemberInfo* pMembers);//��ѯ�������ĳ�Ա��matchKey ƥ��������ֵ��reason��200�ɹ�����������conferenceId ����ID��count ��Ա���������pMembers ��Ա����
	static void sonDismissMultiMediaMeeting(unsigned int matchKey, int reason, const char*conferenceId);//��ɢʵʱ�Խ�����Ƶ����Ƶ����Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������conferenceId ����ID��
	static void sonQueryMultiMediaMeetings(unsigned int matchKey, int reason, int count, ECMeetingInfo* pMeetingInfo);//��ѯ������Ϣ�Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������count �������������pMembers ��������
	static void sonDeleteMemberMultiMediaMeeting(unsigned int matchKey, int reason, ECMeetingDeleteMemberInfo *pInfo);//ɾ������Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������
	static void sonInviteJoinMultiMediaMeeting(unsigned int matchKey, int reason, ECMeetingInviteInfo *pInfo);//����������Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������
	static void sonCreateInterphoneMeeting(unsigned int matchKey, int reason, const char* interphoneId);//����ʵʱ�Խ�����Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������interphoneId ʵʱ�Խ�ID
	static void sonExitInterphoneMeeting(const char* interphoneId);//�˳�ʵʱ�Խ���interphoneId ʵʱ�Խ�ID
	static void sonSetMeetingSpeakListen(unsigned int matchKey, int reason, ECMeetingMemberSpeakListenInfo* pInfo);//���û����Ա����״̬�Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������
	static void sonControlInterphoneMic(unsigned int matchKey, int reason, const char* controller, const char* interphoneId, bool requestIsControl);//ʵʱ�Խ��������Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������controller �����Ա��interphoneId ʵʱ�Խ�ID��requestIsControl �Ƿ������
	static void sonPublishVideo(unsigned int matchKey, int reason, const char* conferenceId);//������Ƶ�Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������conferenceId ����ID��
	static void sonUnpublishVideo(unsigned int matchKey, int reason, const char* conferenceId);//ȡ����Ƶ�Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������conferenceId ����ID��
	static void sonRequestConferenceMemberVideo(int reason, const char *conferenceId, const char *member, int type);//��Ƶ����ʱ����������Ա��Ƶ���ݡ�reason ���Video_Conference_status��conferenceId ����ID��member��Ա��type���ECVideoDataType��
	static void sonCancelConferenceMemberVideo(int reason, const char *conferenceId, const char *member, int type);//��Ƶ����ʱ��ȡ�������Ա��Ƶ������Ӧ��reason ���Video_Conference_status��conferenceId ����ID��tmember��Ա��ype���ECVideoDataType��


private:
	/****************************��¼�ӿڵĻص�******************************/
	virtual void onLogInfo(const char* loginfo) =0 ; // ���ڽ��յײ��log��Ϣ,���Գ��ֵ�����.
	virtual void onLogOut(int reason) = 0;//�ǳ��ص���reason��200�ɹ�����������
	virtual void onConnectState(const ECConnectState state) = 0;//���ӻص���
	virtual void onSetPersonInfo(unsigned int matchKey, int reason, unsigned long long version) = 0; //���ø������ϡ�reason��200�ɹ�����������version:�������ϰ汾��
	virtual void onGetPersonInfo(unsigned int matchKey, int reason, ECPersonInfo *pPersonInfo) = 0;//��ȡ�������ϡ�reason��200�ɹ�����������
	virtual void onGetUserState(unsigned int matchKey, int reason, int count, ECPersonState *pStateArr) = 0;//��ȡ�û�״̬��reason��200�ɹ�����������;count ���������pStateArr �û�״̬������ָ��
	virtual void onPublishPresence(unsigned int matchKey, int reason) = 0;//�����û�״̬��reason��200�ɹ�����������
	virtual void onReceiveFriendsPublishPresence(int count, ECPersonState *pStateArr) = 0;//�յ����������͵��û�״̬�仯,count ���������pStateArr �û�״̬������ָ��
	virtual void onSoftVersion(const char* softVersion, int updateMode, const char* updateDesc) = 0;//Ӧ��������°汾��������ʾ����Ӧ���Լ������á�softVersion��������°汾�ţ�updateMode:����ģʽ  1���ֶ����� 2��ǿ�Ƹ��£�updateDesc���������˵�� 
	virtual void onGetOnlineMultiDevice(unsigned int matchKey, int reason, int count, ECMultiDeviceState *pMultiDeviceStateArr) = 0;//��õ�¼�߶��豸��¼״̬�仯�ص�,reason��200�ɹ�����������count ���������pMultiDeviceStateArr ���豸��¼״̬������ָ��
	virtual void onReceiveMultiDeviceState(int count, ECMultiDeviceState *pMultiDeviceStateArr) = 0;//�յ����������͵ĵ�¼�߶��豸��¼״̬�仯,count ���������pMultiDeviceStateArr ���豸��¼״̬������ָ��

	/****************************VOIP�ص��ӿ�******************************/
	virtual void onMakeCallBack(unsigned int matchKey, int reason, ECCallBackInfo *pInfo)=0;//�ز�����reason��200�ɹ�����������
	virtual void onCallEvents(int reason, const ECVoIPCallMsg * call)=0;//�¼�״̬
	virtual void onCallIncoming(ECVoIPComingInfo *pComing)=0;  //����Ƶ���нӵ�����
	virtual void onDtmfReceived(const char *callId, char dtmf)=0;//�յ�DTMF����ʱ�Ļص���callId ͨ��ID��	
	virtual void onSwitchCallMediaTypeRequest(const char*callId, int video)=0; // �յ��Է�����ý������ callId ͨ��ID��video��1  ����������Ƶ����Ҫ��Ӧ�� 0:����ɾ����Ƶ�����Բ���Ӧ���ײ��Զ�ȥ����Ƶ��
	virtual void onSwitchCallMediaTypeResponse(const char*callId, int video)=0;  // �Է�Ӧ��ý��״̬�����callId ͨ��ID�� video 1 ����Ƶ 0 ����Ƶ
	virtual void onRemoteVideoRatio(const char *CallidOrConferenceId, int width, int height, int type, const char *member)=0;//Զ����Ƶý��ֱ��ʱ仯ʱ�ϱ���type : 0 ��Ե�,1 ��Ƶ���飬2 ������飻type=0 CallidOrConferenceIdΪͨ��id��type>0 CallidOrConferenceIdΪ����id��member��Ч

	
	/****************************��Ϣ����ӿڵĻص�******************************/
	virtual void onOfflineMessageCount(int count)=0;//����������
	virtual int  onGetOfflineMessage()=0;//������ȡ������<0 ȫ����0 ����������0��ֻ��ȡ���µ�������
	virtual void onOfflineMessageComplete()=0;//��ȡ���߳ɹ�
	virtual void onReceiveOfflineMessage(ECMessage *pMsg)=0;//������Ϣ�ص�
	virtual void onReceiveMessage(ECMessage *pMsg)=0;//ʵʱ��Ϣ�ص�
	virtual void onReceiveFile(ECMessage* pFileInfo)=0;//�û��յ������߷��͹������ļ��ȴ��û������Ƿ������ļ��Ļص����ûص���Ϣ����Ϣ����(MsgType)Ĭ����6
	virtual void onDownloadFileComplete(unsigned int matchKey, int reason, ECMessage *msg)=0;//�����ļ��ص���������ɺ󱻵��ã�����ʧ��Ҳ�ᱻ������reason:0�ɹ�����������offset ���ݴ�Сƫ����
	virtual void onRateOfProgressAttach(unsigned int matchKey, unsigned long long rateSize, unsigned long long fileSize, ECMessage *msg)=0;//�ϴ������ļ����ȡ�ratesize �Ѿ���ɽ��ȴ�С���ֽڣ���filezise ���ļ���С���ֽڣ�
	virtual void onSendTextMessage(unsigned int matchKey, int reason, ECMessage *msg)=0;//������Ϣ�ص���reason��200�ɹ�����������
	virtual void onSendMediaFile(unsigned int matchKey, int reason, ECMessage *msg)=0;//�ϴ��ļ��ص����ϴ���ɺ󱻵��ã��ϴ�ʧ��Ҳ�ᱻ������reason:0�ɹ�����������offset ���ݴ�Сƫ����
	virtual void onDeleteMessage(unsigned int matchKey, int reason, int type, const char* msgId)=0;//ɾ���������Ϣ��reason:0�ɹ�����������type:͸����Ϣ���ͣ�msgId:
	virtual void onOperateMessage(unsigned int tcpMsgIdOut, int reason)=0;//ɾ����Ϣ
	virtual void onReceiveOpreateNoticeMessage(const ECOperateNoticeMessage *pMsg) = 0;
	virtual void onUploadVTMFileOrBuf(unsigned int matchKey, int reason, const char* fileUrl) = 0;//�ϴ�VTM�ļ��ص�, fileIdOut���ļ�id, fileUrl:�ļ������������ַ

	/****************************Ⱥ��ӿڶ�Ӧ�Ļص�******************************/
	virtual void onCreateGroup(unsigned int matchKey, int reason, ECGroupDetailInfo *pInfo)=0;//����Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������
	virtual void onDismissGroup(unsigned int matchKey, int reason, const char* groupid)=0;//��ɢȺ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��
	virtual void onQuitGroup(unsigned int matchKey, int reason, const char* groupid)=0;//�˳�Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��
	virtual void onJoinGroup(unsigned int matchKey, int reason, const char* groupid)=0;//����Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��
	virtual void onReplyRequestJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int confirm)=0;//�ظ��������Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��member ��Ա��confirm 1�ܾ� 2ͬ��
	virtual void onInviteJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char** members, int membercount, int confirm)=0;//�������Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��members ��Ա���飻membercount ������� ��confirm 1�ܾ� 2ͬ��
	virtual void onReplyInviteJoinGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int confirm)=0;//�ظ��������Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��member ��Ա��confirm 1�ܾ� 2ͬ��
	virtual void onQueryOwnGroup(unsigned int matchKey, int reason, int count, ECGroupSimpleInfo *group)=0;//��ѯ�ҵ�Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������
	virtual void onQueryGroupDetail(unsigned int matchKey, int reason, ECGroupDetailInfo *detail)=0;//��ѯȺ�����顣matchKey ƥ��������ֵ��reason��200�ɹ�����������
	virtual void onModifyGroup(unsigned int matchKey, int reason, ECModifyGroupInfo *pInfo)=0;//�޸�Ⱥ����Ϣ��matchKey ƥ��������ֵ��reason��200�ɹ�����������
	virtual void onSearchPublicGroup(unsigned int matchKey, int reason, int searchType, const char* keyword, int count, ECGroupSearchInfo *group, int pageNo)=0;//��������Ⱥ�顣matchKey ƥ��������ֵ��reason��200�ɹ�����������searchType �������� 1����GroupId����ȷ���� 2����GroupName��ģ�������� keyword �����ؼ��ʣ�count Ⱥ�������int pageNo ������ҳ����
	virtual void onQueryGroupMember(unsigned int matchKey, int reason, const char *groupid, int count, ECGroupMember *member)=0;//��ѯȺ���Ա��matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��
	virtual void onDeleteGroupMember(unsigned int matchKey, int reason, const char* groupid, const char* member)=0;//ɾ��Ⱥ���Ա��matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��member ��Ա
	virtual void onQueryGroupMemberCard(unsigned int matchKey, int reason, ECGroupMemberCard *card)=0;//��ѯȺ���Ա��Ƭ��matchKey ƥ��������ֵ��reason��200�ɹ�����������
	virtual void onModifyGroupMemberCard(unsigned int matchKey, int reason, ECGroupMemberCard* pCard)=0;//�޸�Ⱥ���Ա��Ƭ��matchKey ƥ��������ֵ��reason��200�ɹ�����������
	virtual void onForbidMemberSpeakGroup(unsigned int matchKey, int reason, const char* groupid, const char* member, int isBan)=0;//Ⱥ����ԡ�matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��member ��Ա��isBan 1������ 2��ֹ����
	virtual void onSetGroupMessageRule(unsigned int matchKey, int reason, const char* groupid, bool notice)=0;//����Ⱥ����Ϣ����matchKey ƥ��������ֵ��reason��200�ɹ�����������groupid Ⱥ��ID��notice �Ƿ�֪ͨ
    virtual void onSetGroupMemberRole(unsigned int matchKey, int reason)=0;//����Ⱥ���ԱȨ��
	/****************************�������·���Ⱥ�����֪ͨ******************************/
	virtual void onReceiveGroupNoticeMessage(const ECGroupNoticeMessage * pMsg)=0;

	/****************************��Ƶ���Żص��ӿ�******************************/
	virtual void onRecordingTimeOut(int ms, const char* sessionId)=0;//¼����ʱ��ms Ϊ¼��ʱ������λ����,sessionId ͸���ỰID
	virtual void onFinishedPlaying(const char* sessionId, int reason)=0;//���Ž���,sessionId͸���ỰID,reason 200�ɹ�.����ʧ��
	virtual void onRecordingAmplitude(double amplitude, const char* sessionId)=0;//���������������¼��ʱ�����Χ0-1,sessionId ͸���ỰID

	/********************************************����ص��ӿ�***********************************************/
	virtual void onMeetingIncoming(ECMeetingComingInfo *pCome) = 0;//����ӵ����롣
	virtual void onReceiveInterphoneMeetingMessage(ECInterphoneMsg* pInterphoneMsg) = 0;// �յ�ʵʱ�Խ������֪ͨ��Ϣ��
	virtual void onReceiveVoiceMeetingMessage(ECVoiceMeetingMsg* pVoiceMeetingMsg) = 0;// �յ��������������֪ͨ��Ϣ��
	virtual void onReceiveVideoMeetingMessage(ECVideoMeetingMsg* pVideoMeetingMsg) = 0;// �յ���Ƶ���������֪ͨ��Ϣ��
	virtual void onCreateMultimediaMeeting(unsigned int matchKey, int reason, ECMeetingInfo *pInfo) = 0;//������Ƶ����Ƶ����Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������
	virtual void onJoinMeeting(int reason, const char* conferenceId) = 0;//�����������Ļص���//reason��200�ɹ�����������conferenceId ����ID��
	virtual void onExitMeeting(const char* conferenceId) = 0;//�˳����顣conferenceId ����ID��
	virtual void onQueryMeetingMembers(unsigned int matchKey, int reason, const char* conferenceId, int count, ECMeetingMemberInfo* pMembers) = 0;//��ѯ�������ĳ�Ա��matchKey ƥ��������ֵ��reason��200�ɹ�����������conferenceId ����ID��count ��Ա���������pMembers ��Ա����
	virtual void onDismissMultiMediaMeeting(unsigned int matchKey, int reason, const char*conferenceId) = 0;//��ɢʵʱ�Խ�����Ƶ����Ƶ����Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������conferenceId ����ID��
	virtual void onQueryMultiMediaMeetings(unsigned int matchKey, int reason, int count, ECMeetingInfo* pMeetingInfo) = 0;//��ѯ������Ϣ�Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������count �������������pMembers ��������
	virtual void onDeleteMemberMultiMediaMeeting(unsigned int matchKey, int reason, ECMeetingDeleteMemberInfo *pInfo) = 0;//ɾ������Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������
	virtual void onInviteJoinMultiMediaMeeting(unsigned int matchKey, int reason, ECMeetingInviteInfo *pInfo) = 0;//����������Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������
	virtual void onCreateInterphoneMeeting(unsigned int matchKey, int reason, const char* interphoneId) = 0;//����ʵʱ�Խ�����Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������interphoneId ʵʱ�Խ�ID
	virtual void onExitInterphoneMeeting(const char* interphoneId) = 0;//�˳�ʵʱ�Խ���interphoneId ʵʱ�Խ�ID
	virtual void onSetMeetingSpeakListen(unsigned int matchKey, int reason, ECMeetingMemberSpeakListenInfo* pInfo) = 0;//���û����Ա����״̬�Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������
	virtual void onControlInterphoneMic(unsigned int matchKey, int reason, const char* controller, const char* interphoneId, bool requestIsControl) = 0;//ʵʱ�Խ��������Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������controller �����Ա��interphoneId ʵʱ�Խ�ID��requestIsControl �Ƿ������
	virtual void onPublishVideo(unsigned int matchKey, int reason, const char* conferenceId) = 0;//������Ƶ�Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������conferenceId ����ID��
	virtual void onUnpublishVideo(unsigned int matchKey, int reason, const char* conferenceId) = 0;//ȡ����Ƶ�Ļص���matchKey ƥ��������ֵ��reason��200�ɹ�����������conferenceId ����ID��
	virtual void onRequestConferenceMemberVideo(int reason, const char *conferenceId, const char *member, int type) = 0;//��Ƶ����ʱ����������Ա��Ƶ���ݡ�reason ���Video_Conference_status��conferenceId ����ID��member��Ա��type���ECVideoDataType��
	virtual void onCancelConferenceMemberVideo(int reason, const char *conferenceId, const char *member, int type) = 0;//��Ƶ����ʱ��ȡ�������Ա��Ƶ������Ӧ��reason ���Video_Conference_status��conferenceId ����ID��tmember��Ա��ype���ECVideoDataType��


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

