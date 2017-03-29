#ifndef _EC_IM_STRUCT_DEFINE_H
#define _EC_IM_STRUCT_DEFINE_H

#include "ECStructDef.h"

typedef enum
{
	Msg_Type_Unknown	= 0,//未知类型
	Msg_Type_Text		= 1,//文本消息
	Msg_Type_Voice		= 2,//语音消息
	Msg_Type_Video		= 3,//视频消息
	Msg_Type_Picture	= 4,//图片消息
	Msg_Type_Position	= 5,//位置消息
	Msg_Type_File		= 6,//文件,底层默认压缩
	Msg_Type_File_UnCompress		= 7,//文件，底层不压缩
	Msg_Type_GroupNty	   = 9,//群组通知
	Msg_Type_GroupAt       = 11,//群组中@成员
	Msg_Type_UserState     = 12,//用户输入状态
	Msg_Type_DeleteMessage = 21,//删除消息
	Msg_Type_Read          = 24,//设置消息已读状态
	Msg_Type_WithDraw      = 25,//撤回消息

	Msg_Type_End
}MsgType;

//群组通知消息类型
typedef enum{
	NTUnknownNotice = -1,
	NTRequestJoinGroup,//申请加入群组
	NTInviteJoinGroup,//邀请加入群组
	NTMemberJoinedGroup,//成员加入群组
	NTDismissGroup,//解散群组
	NTQuitGroup,//退出群组
	NTRemoveGroupMember,//删除成员
	NTReplyRequestJoinGroup,//回复申请
	NTReplyInviteJoinGroup,//回复邀请
	NTModifyGroup,//修改群组信息
	NTChangeAdmin,//讨论组管理员退出后，服务器选择第一个被邀请加入的人作为管理员
	NTChangeMemberRole,//设置、取消管理员，转让群组

}ECMessageNoticeCode;

typedef enum{
	NTOperateUnknownNotice = -1,
	NTDeleteMessage,
	NTReadMessage,
	NTWithdrawMessage,
}ECOperateMessageNoticeCode;

#pragma pack (1)
//消息
typedef struct _ECMessage
{
	MsgType msgType;   //消息类型消息类型 Msg_Type_Text:文本消息 Msg_Type_Voice：语音消息 Msg_Type_Video：视频消息  Msg_Type_Picture：图片消息  Msg_Type_Position：位置消息  Msg_Type_File：文件 
	char msgId[BUF_LEN_64];//消息ID
	char dateCreated[BUF_LEN_20];//服务器接收消息时间,毫秒数
	char sender[BUF_LEN_64]; //发送方
	char receiver[BUF_LEN_64]; //点对点时为接收者VoIP、群组消息时为群组ID
	char msgContent[BUF_LEN_4096];//内容
	char userData[BUF_LEN_4096];//扩展
	char senderNickName[BUF_LEN_128];//发送者昵称
	char fileName[BUF_LEN_512];//文件名	
	char sessionId[BUF_LEN_64];//会话id
	char fileUrl[BUF_LEN_512];//文件下载地址
	int  sessionType;//会话类型:0 点对点，1 群组
	unsigned long long fileSize;//文件大小
	unsigned long long  offset;//偏移
	int isAt;//是否被发送者@了 
	int type;//消息操作类型  0：阅后即焚  1：撤回  3：置为已读

	_ECMessage()
	{
		memset(this,0,sizeof(_ECMessage));
	}

	_ECMessage(const _ECMessage& rOther)
	{
		*this = rOther;
	}
}ECMessage;

//搜索群组 
typedef struct _ECGroupSearchInfo
{
	char groupId[BUF_LEN_33];  //群组ID
	char name[BUF_LEN_128];    //群组名字
	char owner[BUF_LEN_128];   //群组创建者
	char declared[BUF_LEN_512];//群组公告
	int  scope;        //群组规模 1：临时组(上限100人) 2普通组(上限300人)3：普通组(上限500人) 4：付费普通组 (上限1000人) 5：付费VIP组（上限2000人）
	int  permission;   //申请加入模式 1：默认可直接加入  2：需要身份验证    3:私有群组（不能主动加入，仅能管理员邀请）
	int  memberCount;  //群组的成员人数
	char discuss;		//是否是讨论组
	_ECGroupSearchInfo()
	{
		memset(this,0,sizeof(_ECGroupSearchInfo));
	}

	_ECGroupSearchInfo(const _ECGroupSearchInfo& rOther)
	{
		*this = rOther;
	}

}ECGroupSearchInfo;

//获得自己的群组 
typedef struct _ECGroupSimpleInfo
{
	char groupId[BUF_LEN_33];  //群组ID
	char name[BUF_LEN_128];    //群组名字
	char owner[BUF_LEN_128];   //群组创建者
	int  scope;        //群组规模 1：临时组(上限100人) 2普通组(上限300人)3：普通组(上限500人) 4：付费普通组 (上限1000人) 5：付费VIP组（上限2000人）
	int  permission;   //申请加入模式 1：默认可直接加入  2：需要身份验证    3:私有群组（不能主动加入，仅能管理员邀请）
	int  memberCount;  //群组的成员人数
	int  isNotice;	   //用来表示是否需要提醒消息。1：提醒；2：不提醒
	char discuss;//是否是讨论组
	_ECGroupSimpleInfo()
	{
		memset(this,0,sizeof(_ECGroupSimpleInfo));
	}

	_ECGroupSimpleInfo(const _ECGroupSimpleInfo& rOther)
	{
		*this = rOther;
	}

}ECGroupSimpleInfo;


//群组属性(详细信息) 
typedef struct _ECGroupDetailInfo
{
	char groupId[BUF_LEN_33];     //群组ID
	char name[BUF_LEN_128];       //群组名字
	char owner[BUF_LEN_128];      //群组创建者
	int  type;            //群组类型 （同学、朋友、同事等）1: 同学  2: 朋友 3: 同事
	char province[BUF_LEN_64];    //群组省份
	char city[BUF_LEN_64];        //群组地区
	int  scope;					//群组规模 1：临时组(上限100人) 2普通组(上限300人)3：普通组(上限500人) 4：普通组2 (上限1000人) 5：VIP组（上限2000人）
	char declared[BUF_LEN_512];   //群组公告
	char dateCreated[BUF_LEN_20]; //该群组的创建时间
	int  memberCount;     //群组的成员人数
	int  isNotice;	      //用来表示是否需要提醒消息。1：提醒；2：不提醒
	int  permission;      //申请加入模式 1：默认可直接加入  2：需要身份验证    3:私有群组（不能主动加入，仅能管理员邀请）	
	char groupDomain[BUF_LEN_512];//群组扩展信息（用于第三方应用传递自己的数据，原样返回）
	char discuss;//是否是讨论组
	_ECGroupDetailInfo()
	{
		memset(this,0,sizeof(_ECGroupDetailInfo));
	}

	_ECGroupDetailInfo(const _ECGroupDetailInfo& rOther)
	{
		*this = rOther;
	}

}ECGroupDetailInfo;

//修改群组信息 
typedef struct _ECModifyGroupInfo
{
	char groupId[BUF_LEN_33];     //群组ID
	char name[BUF_LEN_128];       //群组名字
	int  type;            //群组类型 （同学、朋友、同事等）1: 同学  2: 朋友 3: 同事
	char province[BUF_LEN_64];    //群组省份
	char city[BUF_LEN_64];        //群组地区
	int  scope;           //群组规模 1：临时组(上限100人) 2普通组(上限300人)3：普通组(上限500人) 4：付费普通组 (上限1000人) 5：付费VIP组（上限2000人）
	char declared[BUF_LEN_512];   //群组公告
	int  permission;      //申请加入模式 1：默认可直接加入  2：需要身份验证    3:私有群组（不能主动加入，仅能管理员邀请）	
	char groupDomain[BUF_LEN_512];//群组扩展信息（用于第三方应用传递自己的数据，原样返回）
	char discuss;//是否是讨论组
	_ECModifyGroupInfo()
	{
		memset(this,0,sizeof(_ECModifyGroupInfo));
		scope = 1;
		permission = 1;
	}

	_ECModifyGroupInfo(const _ECModifyGroupInfo& rOther)
	{
		*this = rOther;
	}

	//_ECModifyGroupInfo& operator=(const _ECModifyGroupInfo& rOther);

}ECModifyGroupInfo;

//群成员属性 
typedef struct _GroupMember
{
	char member[BUF_LEN_64];  //成员
    char nickName[BUF_LEN_128];//昵称 
	int  speakState;  // 是否禁言  1:不禁言  2:禁言
	int  role;		  //角色  1:创建者  2:管理员  3：成员 
	int  sex ;        //性别 1:男 2：女
	_GroupMember()
	{
		memset(this,0,sizeof(_GroupMember));
	}

	_GroupMember(const _GroupMember& rOther)
	{
		*this = rOther;
	}

	//_ECGroupMember& operator=(const _ECGroupMember& rOther);

}ECGroupMember;

//群成员名片 。 
typedef struct _ECGroupMemberCard
{
	char member[BUF_LEN_64];  //成员
	char groupId[BUF_LEN_33]; //群组ID
    char display[BUF_LEN_128];//用户名字 
	char phone[BUF_LEN_33];    //用户电话
	char mail[BUF_LEN_128];   //用户邮箱
	char remark[BUF_LEN_512]; //用户备注
	int  speakState;		  // 是否禁言  1:不禁言  2:禁言
	int  role;				  //角色  1:创建者  2:管理员  3：成员 
	int  sex ;				  //性别 1:男 2：女
	_ECGroupMemberCard()
	{
		memset(this,0,sizeof(_ECGroupMemberCard));
	}

	_ECGroupMemberCard(const _ECGroupMemberCard& rOther)
	{
		*this = rOther;
	}

	//_ECGroupMemberCard& operator=(const _ECGroupMemberCard& rOther);

}ECGroupMemberCard;

typedef struct _ECMessageReceipt
{
	char useracc[BUF_LEN_128];
	char time[BUF_LEN_128];
	_ECMessageReceipt()
	{
		memset(this,0,sizeof(_ECMessageReceipt));
	}

	_ECMessageReceipt(const _ECMessageReceipt& rOther)
	{
		*this = rOther;
	}

	_ECMessageReceipt& operator=(const _ECMessageReceipt& rOther);

}ECMessageReceipt;

//管理员收到群员加入群组的申请
typedef struct _ECRequestJoinGroupNtyMsg
{
	char sender[BUF_LEN_64];//发送通知消息的账号
	char groupId[BUF_LEN_33];//群组ID
	char groupName[BUF_LEN_128];//群组名
	char dateCreated[BUF_LEN_20];//时间戳
	char declared[BUF_LEN_512];//申请理由
	char nickName[BUF_LEN_128];//昵称
	char proposer[BUF_LEN_64];//申请者id
	int confirm;//是否需要管理员确认 1不需要确认 2需要确认
	_ECRequestJoinGroupNtyMsg()
	{
		memset(this,0,sizeof(_ECRequestJoinGroupNtyMsg));
	}

	_ECRequestJoinGroupNtyMsg(const _ECRequestJoinGroupNtyMsg& rOther)
	{
		*this = rOther;
	}

	//_ECRequestJoinGroupNtyMsg& operator=(const _ECRequestJoinGroupNtyMsg& rOther);

}ECRequestJoinGroupNtyMsg;

//群员收到管理员加入群组的邀请
typedef struct _ECInviteJoinGroupNtyMsg
{
	int confirm;//是否需要对方确认 1不需要确认 2需要确认
	char sender[BUF_LEN_64];//发送通知消息的账号
	char groupId[BUF_LEN_33];//群组ID
	char groupName[BUF_LEN_128];//群组名
	char dateCreated[BUF_LEN_20];//发送消息时间
	char declared[BUF_LEN_512];//邀请理由
	char admin[BUF_LEN_64];//管理员
	char nickName[BUF_LEN_128];//昵称
	_ECInviteJoinGroupNtyMsg()
	{
		memset(this,0,sizeof(_ECInviteJoinGroupNtyMsg));
	}

	_ECInviteJoinGroupNtyMsg(const _ECInviteJoinGroupNtyMsg& rOther)
	{
		*this = rOther;
	}

	//_ECInviteJoinGroupNtyMsg& operator=(const _ECInviteJoinGroupNtyMsg& rOther);

}ECInviteJoinGroupNtyMsg;

//群员收到有新群员加入了群的通知
typedef struct _ECMemberJoinedGroupNtyMsg
{
	char sender[BUF_LEN_64];//发送通知消息的账号
	char groupId[BUF_LEN_33];//群组ID
	char groupName[BUF_LEN_128];//群组名
	char dateCreated[BUF_LEN_20];//发送消息时间
	char member[BUF_LEN_64];//加入者id
	char declared[BUF_LEN_512];//加入理由
	char nickName[BUF_LEN_128];//昵称
	_ECMemberJoinedGroupNtyMsg()
	{
		memset(this,0,sizeof(_ECMemberJoinedGroupNtyMsg));
	}

	_ECMemberJoinedGroupNtyMsg(const _ECMemberJoinedGroupNtyMsg& rOther)
	{
		*this = rOther;
	}

	//_ECMemberJoinedGroupNtyMsg& operator=(const _ECMemberJoinedGroupNtyMsg& rOther);

}ECMemberJoinedGroupNtyMsg;

//解散群组
typedef struct _ECDismissGroupNtyMsg
{
	char sender[BUF_LEN_64];//发送通知消息的账号
	char groupId[BUF_LEN_33];//群组ID
	char groupName[BUF_LEN_128];//群组名
	char dateCreated[BUF_LEN_20];//发送消息时间
	_ECDismissGroupNtyMsg()
	{
		memset(this,0,sizeof(_ECDismissGroupNtyMsg));
	}

	_ECDismissGroupNtyMsg(const _ECDismissGroupNtyMsg& rOther)
	{
		*this = rOther;
	}

	//_ECDismissGroupNtyMsg& operator=(const _ECDismissGroupNtyMsg& rOther);

}ECDismissGroupNtyMsg;

//成员退出群组
typedef struct _ECQuitGroupNtyMsg
{
	char sender[BUF_LEN_64];//发送通知消息的账号
	char groupId[BUF_LEN_33];//群组ID
	char groupName[BUF_LEN_128];//群组名
	char dateCreated[BUF_LEN_20];//发送消息时间
	char member[BUF_LEN_64];//加入者id
	char nickName[BUF_LEN_128];//昵称
	_ECQuitGroupNtyMsg()
	{
		memset(this,0,sizeof(_ECQuitGroupNtyMsg));
	}

	_ECQuitGroupNtyMsg(const _ECQuitGroupNtyMsg& rOther)
	{
		*this = rOther;
	}

	//_ECQuitGroupNtyMsg& operator=(const _ECQuitGroupNtyMsg& rOther);

}ECQuitGroupNtyMsg;

//踢出成员
typedef struct _ECRemoveGroupMemberNtyMsg
{
	char sender[BUF_LEN_64];//发送通知消息的账号
	char groupId[BUF_LEN_33];//群组ID
	char groupName[BUF_LEN_128];//群组名
	char dateCreated[BUF_LEN_20];//发送消息时间
	char member[BUF_LEN_64];//移除的成员id
	char nickName[BUF_LEN_128];//昵称
	_ECRemoveGroupMemberNtyMsg()
	{
		memset(this,0,sizeof(_ECRemoveGroupMemberNtyMsg));
	}

	_ECRemoveGroupMemberNtyMsg(const _ECRemoveGroupMemberNtyMsg& rOther)
	{
		*this = rOther;
	}

	//_ECRemoveGroupMemberNtyMsg& operator=(const _ECRemoveGroupMemberNtyMsg& rOther);

}ECRemoveGroupMemberNtyMsg;

//管理员回复成员申请加入
typedef struct _ECReplyRequestJoinGroupNtyMsg
{
	char sender[BUF_LEN_64];//发送通知消息的账号
	char groupId[BUF_LEN_33];//群组ID
	char groupName[BUF_LEN_128];//群组名
	char dateCreated[BUF_LEN_20];//发送消息时间
	char member[BUF_LEN_64];//加入的成员id
	char admin[BUF_LEN_64];//管理员
	char nickName[BUF_LEN_128];//昵称
	int confirm;//是否同意：1拒绝 2同意
	_ECReplyRequestJoinGroupNtyMsg()
	{
		memset(this,0,sizeof(_ECReplyRequestJoinGroupNtyMsg));
	}

	_ECReplyRequestJoinGroupNtyMsg(const _ECReplyRequestJoinGroupNtyMsg& rOther)
	{
		*this = rOther;
	}

	//_ECReplyRequestJoinGroupNtyMsg& operator=(const _ECReplyRequestJoinGroupNtyMsg& rOther);

}ECReplyRequestJoinGroupNtyMsg;

//成员回复管理员邀请加入群
typedef struct _ECReplyInviteJoinGroupNtyMsg
{
	char sender[BUF_LEN_64];//发送通知消息的账号
	char groupId[BUF_LEN_33];//群组ID
	char groupName[BUF_LEN_128];//群组名
	char dateCreated[BUF_LEN_20];//发送消息时间
	char member[BUF_LEN_64];//收到邀请的成员id
	char admin[BUF_LEN_64];//管理员
	char nickName[BUF_LEN_128];//昵称
	int confirm;//是否同意：1拒绝 2同意
	_ECReplyInviteJoinGroupNtyMsg()
	{
		memset(this,0,sizeof(_ECReplyInviteJoinGroupNtyMsg));
	}

	_ECReplyInviteJoinGroupNtyMsg(const _ECReplyInviteJoinGroupNtyMsg& rOther)
	{
		*this = rOther;
	}

	//_ECReplyInviteJoinGroupNtyMsg& operator=(const _ECReplyInviteJoinGroupNtyMsg& rOther);

}ECReplyInviteJoinGroupNtyMsg;

//修改群组信息
typedef struct _ECModifyGroupNtyMsg
{
	char sender[BUF_LEN_64];//发送通知消息的账号
	char groupId[BUF_LEN_33];//群组ID
	char groupName[BUF_LEN_128];//群组名
	char dateCreated[BUF_LEN_20];//发送消息时间
	char member[BUF_LEN_64];//修改者id
	char modifyDic[BUF_LEN_1024];//群组修改的信息
	_ECModifyGroupNtyMsg()
	{
		memset(this,0,sizeof(_ECModifyGroupNtyMsg));
	}

	_ECModifyGroupNtyMsg(const _ECModifyGroupNtyMsg& rOther)
	{
		*this = rOther;
	}

	//_ECModifyGroupNtyMsg& operator=(const _ECModifyGroupNtyMsg& rOther);

}ECModifyGroupNtyMsg;

typedef struct _ECDeleteMessageNtyMsg
{
	char msgId[BUF_LEN_64];//消息ID
	char sender[BUF_LEN_64]; //发送方
	char receiver[BUF_LEN_64]; //点对点时为接收者VoIP、群组消息时为群组ID
	char msgContent[BUF_LEN_4096];//内容
	char userData[BUF_LEN_4096];//扩展
	char senderNickName[BUF_LEN_128];//发送者昵称
	char sessionId[BUF_LEN_64];//会话id
	_ECDeleteMessageNtyMsg()
	{
		memset(this,0,sizeof(_ECDeleteMessageNtyMsg));
	}

	_ECDeleteMessageNtyMsg(const _ECDeleteMessageNtyMsg& rOther)
	{
		*this = rOther;
	}

	//_ECDeleteMessageNtyMsg& operator=(const _ECDeleteMessageNtyMsg& rOther);
}ECDeleteMessageNtyMsg;

typedef struct _ECReadMessageNtyMsg
{
	char msgId[BUF_LEN_64];//消息ID
	char sender[BUF_LEN_64]; //发送方
	char receiver[BUF_LEN_64]; //点对点时为接收者VoIP、群组消息时为群组ID
	char msgContent[BUF_LEN_4096];//内容
	char userData[BUF_LEN_4096];//扩展
	char senderNickName[BUF_LEN_128];//发送者昵称
	char sessionId[BUF_LEN_64];//会话id
	_ECReadMessageNtyMsg()
	{
		memset(this,0,sizeof(_ECReadMessageNtyMsg));
	}

	_ECReadMessageNtyMsg(const _ECReadMessageNtyMsg& rOther)
	{
		*this = rOther;
	}

	//_ECReadMessageNtyMsg& operator=(const _ECReadMessageNtyMsg& rOther);
}ECReadMessageNtyMsg;

typedef struct _ECWithDrawMessageNtyMsg
{
	char msgId[BUF_LEN_64];//消息ID
	char sender[BUF_LEN_64]; //发送方
	char receiver[BUF_LEN_64]; //点对点时为接收者VoIP、群组消息时为群组ID
	char msgContent[BUF_LEN_4096];//内容
	char userData[BUF_LEN_4096];//扩展
	char senderNickName[BUF_LEN_128];//发送者昵称
	char sessionId[BUF_LEN_64];//会话id
	_ECWithDrawMessageNtyMsg()
	{
		memset(this,0,sizeof(_ECWithDrawMessageNtyMsg));
	}

	_ECWithDrawMessageNtyMsg(const _ECWithDrawMessageNtyMsg& rOther)
	{
		*this = rOther;
	}

	//_ECWithDrawMessageNtyMsg& operator=(const _ECWithDrawMessageNtyMsg& rOther);
}ECWithDrawMessageNtyMsg;

typedef struct _GroupNoticeMessage{
	ECMessageNoticeCode notice;
	void *pNoticeMsg;
	_GroupNoticeMessage():notice(NTUnknownNotice),pNoticeMsg(NULL)
	{

	}
}ECGroupNoticeMessage;

typedef struct _OperateNoticeMessage
{
	ECOperateMessageNoticeCode notice;
	void *pNoticeMsg;
	_OperateNoticeMessage():notice(NTOperateUnknownNotice),pNoticeMsg(NULL)
	{

	}
}ECOperateNoticeMessage;

typedef struct _ECChangeMemberRoleMsg
{
	char sender[BUF_LEN_64];//发送通知消息的账号
	char groupId[BUF_LEN_33];//群组ID
	char groupName[BUF_LEN_128];//群组名
	char dateCreated[BUF_LEN_20];//发送消息时间
	char member[BUF_LEN_64];//修改者id
	char nickName[BUF_LEN_128];//昵称
	int role;//1.群主 2.管理员 3.成员

	_ECChangeMemberRoleMsg()
	{
		memset(this,0,sizeof(_ECChangeMemberRoleMsg));
	}

	_ECChangeMemberRoleMsg(const _ECChangeMemberRoleMsg& rOther)
	{
		*this = rOther;
	}

	_ECChangeMemberRoleMsg& operator=(const _ECChangeMemberRoleMsg& rOther);

}ECChangeMemberRoleMsg;



/****************************消息处理接口的回调******************************/
typedef void (*onOfflineMessageCount)(int count );//离线条数；
typedef int  (*onGetOfflineMessage)();//离线拉取条数。<0 全拉；0 不拉；大于0，只拉取最新的条数，
typedef void (*onOfflineMessageComplete)();//拉取离线成功
typedef void (*onReceiveOfflineMessage)(ECMessage *pMsg);//离线消息回调
typedef void (*onReceiveMessage)(ECMessage *pMsg);//实时消息回调
typedef void (*onReceiveFile)(ECMessage* pFileInfo);//用户收到发送者发送过来的文件等待用户决定是否下载文件的回调。该回调消息的消息类型(MsgType)默认是6
typedef void (*onDownloadFileComplete)(unsigned int matchKey, int reason, ECMessage *msg);//下载文件回调，下载完成后被调用（下载失败也会被调）。reason:0成功，其他报错；offset 内容大小偏移量
typedef void (*onRateOfProgressAttach)(unsigned int matchKey, unsigned long long rateSize,unsigned long long fileSize,ECMessage *msg);//上传下载文件进度。ratesize 已经完成进度大小（字节）；filezise 总文件大小（字节）
typedef void (*onSendTextMessage)(unsigned int matchKey, int reason,ECMessage *msg);//发送消息回调。reason：200成功，其他报错；
typedef void (*onSendMediaFile)(unsigned int matchKey, int reason,ECMessage *msg);//上传文件回调，上传完成后被调用（上传失败也会被调）。reason:0成功，其他报错；offset 内容大小偏移量
typedef void (*onDeleteMessage)(unsigned int matchKey, int reason,int type,const char* msgId);//删除服务端消息。reason:0成功，其他报错；type:透传消息类型；msgId:
typedef void (*onReceiveNoVersionMessage)(ECMessage *pMsg);//实时消息状态回调
typedef void (*onOperateMessage)(unsigned int tcpMsgIdOut, int reason);//删除消息
typedef void (*onReceiveOpreateNoticeMessage)(const ECOperateNoticeMessage *pMsg);//消息已读状态通知
typedef void (*onSetTopContact)(unsigned int matchKey, int reason);//设置会话置顶
typedef void (*onGetTopContact)(unsigned int clientNo, int reason, int count, char** members);//获取置顶联系人列表

typedef void (*onUploadVTMFileOrBuf)(unsigned int matchKey, int reason, const char* fileUrl);//上传VTM文件回调, fileIdOut：文件id, fileUrl:文件服务器保存地址


/****************************群组接口对应的回调******************************/
typedef void (*onCreateGroup)(unsigned int matchKey, int reason, ECGroupDetailInfo *pInfo);//创建群组。matchKey 匹配主调键值；reason：200成功，其他报错；
typedef void (*onDismissGroup)(unsigned int matchKey, int reason,const char* groupid);//解散群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
typedef void (*onQuitGroup)(unsigned int matchKey, int reason,const char* groupid);//退出群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
typedef void (*onJoinGroup)(unsigned int matchKey, int reason,const char* groupid);//加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
typedef void (*onReplyRequestJoinGroup)(unsigned int matchKey, int reason,const char* groupid,const char* member,int confirm);//回复请求加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员；confirm 1拒绝 2同意
typedef void (*onInviteJoinGroup)(unsigned int matchKey, int reason,const char* groupid,const char** members,int membercount,int confirm);//邀请加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；members 成员数组；membercount 数组个数 ；confirm 1拒绝 2同意
typedef void (*onReplyInviteJoinGroup)(unsigned int matchKey, int reason,const char* groupid,const char* member,int confirm);//回复邀请加入群组。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员；confirm 1拒绝 2同意
typedef void (*onQueryOwnGroup)(unsigned int matchKey, int reason, int count,ECGroupSimpleInfo *group);//查询我的群组。matchKey 匹配主调键值；reason：200成功，其他报错；
typedef void (*onQueryGroupDetail)(unsigned int matchKey, int reason, ECGroupDetailInfo *detail);//查询群组详情。matchKey 匹配主调键值；reason：200成功，其他报错；
typedef void (*onModifyGroup)(unsigned int matchKey, int reason,ECModifyGroupInfo *pInfo);//修改群组信息。matchKey 匹配主调键值；reason：200成功，其他报错；
typedef void (*onSearchPublicGroup)(unsigned int matchKey, int reason, int searchType,const char* keyword,int count,ECGroupSearchInfo *group);//搜索公共群组。matchKey 匹配主调键值；reason：200成功，其他报错；searchType 搜索类型 1根据GroupId，精确搜索 2根据GroupName，模糊搜索； keyword 搜索关键词；count 群组个数
typedef void (*onQueryGroupMember)(unsigned int matchKey, int reason, const char *groupid,int count,ECGroupMember *member);//查询群组成员。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；
typedef void (*onDeleteGroupMember)(unsigned int matchKey, int reason,const char* groupid,const char* member);//删除群组成员。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员
typedef void (*onQueryGroupMemberCard)(unsigned int matchKey, int reason, ECGroupMemberCard *card);//查询群组成员名片。matchKey 匹配主调键值；reason：200成功，其他报错；
typedef void (*onModifyGroupMemberCard)(unsigned int matchKey, int reason,ECGroupMemberCard* pCard);//修改群组成员名片。matchKey 匹配主调键值；reason：200成功，其他报错；
typedef void (*onForbidMemberSpeakGroup)(unsigned int matchKey, int reason,const char* groupid,const char* member,int isBan);//群组禁言。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；member 成员；isBan 1允许发言 2禁止发言
typedef void (*onSetGroupMessageRule)(unsigned int matchKey, int reason,const char* groupid, bool notice);//设置群组消息规则。matchKey 匹配主调键值；reason：200成功，其他报错；groupid 群组ID；notice 是否通知

typedef void (*onSetGroupMemberRole)(unsigned int matchKey, int reason);//设置群组成员权限
typedef void (*onSearchPublicGroupPaging)(unsigned int matchKey, int reason, int searchType,const char* keyword,int count,ECGroupSearchInfo *group,int pageNo);//搜索公共群组。matchKey 匹配主调键值；reason：200成功，其他报错；searchType 搜索类型 1根据GroupId，精确搜索 2根据GroupName，模糊搜索； keyword 搜索关键词；count 群组个数；int pageNo 搜索分页索引
typedef void (*onQueryMessageReceipt)(unsigned int clientNo, int reason, int unReadCount, ECMessageReceipt* unReadInfo, int readCount, ECMessageReceipt* readInfo);//查询消息的已读和未读人数

/****************************服务器下发的群组相关通知******************************/
typedef void (*onReceiveGroupNoticeMessage)(const ECGroupNoticeMessage * pMsg);

/****************************音频播放回调接口******************************/
typedef void (*onRecordingTimeOut)(int ms, const char* sessionId);//录音超时。ms 为录音时长，单位毫秒,sessionId 透传会话ID
typedef void (*onFinishedPlaying)(const char* sessionId,int reason);//播放结束,sessionId透传会话ID,reason 200成功.其他失败
typedef void (*onRecordingAmplitude)(double amplitude,const char* sessionId);//返回音量的振幅。录音时振幅范围0-1,sessionId 透传会话ID

//回调结构体
typedef struct _MessageCallBackInterface{
	onSendTextMessage pfonSendTextMessage;
	onReceiveMessage pfonReceiveMessage;
	onReceiveOfflineMessage pfonReceiveOfflineMessage;
	onSendMediaFile pfonSendMediaFile;
	onDownloadFileComplete pfonDownloadFileComplete;
	onRateOfProgressAttach pfonRateOfProgressAttach;
	onReceiveFile pfonReceiveFile;
	onOfflineMessageCount pfonOfflineMessageCount;
	onGetOfflineMessage pfonGetOfflineMessage;
	onOfflineMessageComplete pfonOfflineMessageComplete;
	onRecordingTimeOut pfonRecordingTimeOut;
	onFinishedPlaying pfonFinishedPlaying;
	onRecordingAmplitude pfonRecordingAmplitude;
	onDeleteMessage pfonDeleteMessage;
	onReceiveNoVersionMessage pfonReceiveNoVersionMessage;
	onOperateMessage pfonOperateMessage;
    onReceiveOpreateNoticeMessage pfonOnReceiveOperateNoticeMessage;
	onSetTopContact pfonSetTopContact;
	onGetTopContact pfonGetTopContact;

	onUploadVTMFileOrBuf pfonUploadVTMFileOrBuf;


	/****************************群组接口对应的回调******************************/
	onCreateGroup pfonCreateGroup;//reason：200成功，其他报错；
	onDismissGroup pfonDismissGroup;
	onQuitGroup pfonQuitGroup;
	onJoinGroup pfonJoinGroup;
	onReplyRequestJoinGroup pfonReplyRequestJoinGroup;
	onInviteJoinGroup pfonInviteJoinGroup;
	onReplyInviteJoinGroup pfonReplyInviteJoinGroup;
	onQueryOwnGroup pfonQueryOwnGroup;
	onQueryGroupDetail pfonQueryGroupDetail;
	onModifyGroup pfonModifyGroup;
	onSearchPublicGroup pfonSearchPublicGroup;
	onQueryGroupMember pfonQueryGroupMember;
	onDeleteGroupMember pfonDeleteGroupMember;
	onQueryGroupMemberCard pfonQueryGroupMemberCard;
	onModifyGroupMemberCard pfonModifyGroupMemberCard;
	onForbidMemberSpeakGroup pfonForbidMemberSpeakGroup;
	onSetGroupMessageRule pfonSetGroupMessageRule;
	onReceiveGroupNoticeMessage pfonReceiveGroupNoticeMessage;
	onSetGroupMemberRole pfonSetGroupMemberRole;
	onSearchPublicGroupPaging pfonSearchPublicGroupPaging;
	onQueryMessageReceipt pfonQueryMessageReceipt;
	_MessageCallBackInterface():pfonSendTextMessage(NULL),pfonReceiveMessage(NULL),pfonReceiveOfflineMessage(NULL),
		pfonSendMediaFile(NULL),pfonDownloadFileComplete(NULL),pfonRateOfProgressAttach(NULL),pfonReceiveFile(NULL),
		pfonOfflineMessageCount(NULL),pfonGetOfflineMessage(NULL),pfonOfflineMessageComplete(NULL),pfonRecordingTimeOut(NULL),
		pfonFinishedPlaying(NULL),pfonRecordingAmplitude(NULL),pfonDeleteMessage(NULL),pfonReceiveNoVersionMessage(NULL),
		pfonOperateMessage(NULL),pfonOnReceiveOperateNoticeMessage(NULL),pfonSetTopContact(NULL),pfonGetTopContact(NULL),

		pfonUploadVTMFileOrBuf(NULL),

		pfonCreateGroup(NULL),pfonDismissGroup(NULL),pfonQuitGroup(NULL),pfonJoinGroup(NULL),pfonReplyRequestJoinGroup(NULL),
		pfonInviteJoinGroup(NULL),pfonReplyInviteJoinGroup(NULL),pfonQueryOwnGroup(NULL),pfonQueryGroupDetail(NULL),pfonModifyGroup(NULL),
		pfonSearchPublicGroup(NULL),pfonQueryGroupMember(NULL),pfonDeleteGroupMember(NULL),pfonQueryGroupMemberCard(NULL),
		pfonModifyGroupMemberCard(NULL),pfonForbidMemberSpeakGroup(NULL),pfonSetGroupMessageRule(NULL),
		pfonReceiveGroupNoticeMessage(NULL),pfonSetGroupMemberRole(NULL),pfonSearchPublicGroupPaging(NULL),pfonQueryMessageReceipt(NULL)
	{

	}

}ECMessageCallBackInterface;

#pragma pack ()




#endif