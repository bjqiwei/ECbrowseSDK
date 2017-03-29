#ifndef _EC_IM_INTERFACE_H
#define _EC_IM_INTERFACE_H

#include "ECImStructDef.h"
#ifdef __cplusplus
extern "C" 
{
#endif

//////////////////////////////////////////////////回调函数接口///////////////////////////////////////////////////////////
	//回调设置接口，必须在主调前调用
	EC_CALL_API void SetMessageCallBackFunction(ECMessageCallBackInterface *MessageInterface);

//////////////////////////////////////////////////主调函数接口///////////////////////////////////////////////////////////
/*! @function
     ********************************************************************************
     函数名   : SendTextMessage
     功能     : 发送消息
	 参数     : [OUT] matchKey        : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  receiver        : 接收者用户名
				[IN]  message         : 文本消息内容
				[IN]  type            : type消息类型 目前支持Msg_Type_Text:文本消息  以后会扩展类型
				[IN]  userdata        : 需要扩展的消息字段放在这里
				[OUT] msgId			  : 发送消息的ID, 需要先申请64字节长度的Buffer，接口会将ID写入到Buffer中。
     返回值   : 0成功； 非0失败
	 回调函数 ：void (*onSendTextMessage)
     *******************************************************************************/
	EC_CALL_API int SendTextMessage(unsigned int *matchKey,const char *receiver, const char *message, MsgType type, const char *userdata, char * msgId);//此接口建议废弃，使用下面的接口SendTextMessageEX

/*! @function
     ********************************************************************************
     函数名   : SendTextMessageEX
     功能     : 发送消息
	 参数     : [OUT] matchKey        : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  receiver        : 接收者用户名
				[IN]  message         : 文本消息内容
				[IN]  type            : type消息类型 目前支持Msg_Type_Text:文本消息  以后会扩展类型
				[IN]  userdata        : 需要扩展的消息字段放在这里
				[OUT] msgId			  : 发送消息的ID, 需要先申请64字节长度的Buffer，接口会将ID写入到Buffer中。
				[IN]  jsonString      : 用户传入，包括群组中被@的成员列表和被@的成员个数
     返回值   : 0成功； 非0失败
	 回调函数 ：void (*onSendTextMessage)
     *******************************************************************************/
	EC_CALL_API int SendTextMessageEX(unsigned int *matchKey,const char *receiver, const char *message, MsgType type, const char *userdata, char * msgId, const char *jsonString);
  
	 /*! @function
     ********************************************************************************
     函数名   : SendMediaMessage
     功能     : 上传文件
	 参数     : [OUT] matchKey    : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  receiver    : 接收者
				[IN]  fileName    : 附件全路径
				[IN]  type        : type消息类型  Msg_Type_Voice：语音消息 Msg_Type_Video：视频消息  Msg_Type_Picture：图片消息 Msg_Type_File：文件 
				[IN]  userdate    : 需要扩展的消息字段放在这里
				[OUT] msgId		  : 发送消息的ID, 需要先申请64字节长度的Buffer，接口会将ID写入到Buffer中。
     返回值   : 0成功； 非0失败
	 回调函数 ：void (*onSendMediaFile)
     *******************************************************************************/
	EC_CALL_API int SendMediaMessage(unsigned int *matchKey,const char* receiver,const char* fileName, MsgType type,const char* userdata,  char * msgId);

	 /*! @function
     ********************************************************************************
     函数名   : DownloadFileMessage
     功能     : 下载文件
	 参数     : [OUT] matchKey    : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  sender	  : 发送者
				[IN]  recv	      : 接收者
				[IN]  url		  : 下载url。可选字段，没有此字段，底层会拼接url
				[IN]  fileName    : 附件全路径
				[IN]  msgId       : 消息id
				[IN]  type        : type消息类型 Msg_Type_Voice：语音消息 Msg_Type_Video：视频消息  Msg_Type_Picture：图片消息  Msg_Type_File：文件 
     返回值   : 0成功； 非0失败
     回调函数 ：void (*onDownloadFileComplete)
     *******************************************************************************/
	EC_CALL_API int DownloadFileMessage(unsigned int *matchKey, const char* sender,const char* recv,const char* url, const char* fileName, const char* msgId, MsgType type);
	
	 /*! @function
     ********************************************************************************
     函数名   : CancelUploadOrDownloadNOW
     功能     : 取消正在上传和下载的附件
	 参数     : [IN] matchKey    : SDK内部生成，下载或发送消息OUT 的matchKey
     返回值   : 无
     *******************************************************************************/
	EC_CALL_API void CancelUploadOrDownloadNOW(unsigned int matchKey);

	 /*! @function
     ********************************************************************************
     函数名: DeleteMessage
     功能  : 删除消息
     参数  : [IN]  matchKey : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
	         [IN]  type     : 消息类型。0 删除读过的消息(现只支持删除接收到的消息)
             [IN]  msgId    : 删除消息的ID
     返回值   : 0成功，非0失败
	 回调函数 ：void (*onDeleteMessage)
     *******************************************************************************/
     EC_CALL_API int DeleteMessage(unsigned int *matchKey,int type,const char* msgId);
    
    /*! @function
     ********************************************************************************
     函数名: OperateMessage
     功能  : 消息操作
     参数  : [IN]  matchKey : 流水号
             [IN]  version : 消息的版本号
			 [IN]  type    : 0: 阅后即焚 1:消息撤回 3：消息已读
     返回值   : 0成功，非0失败
     *******************************************************************************/
     EC_CALL_API int OperateMessage(unsigned int* matchKey, const char* version, const char* messageId, int type);
	 
	 /*! @function
     ********************************************************************************
     函数名   : SetTopContact
     功能     : 设置会话置顶
     参数     : [IN]  matchKey : 回传流水号
               [IN]  contact : 用户账号
               [IN]  isTop : 是否置顶
     返回值   : 0成功，非0失败
     回调函数 ：void (*onSetTopContact)(unsigned int clientNo, int reason)
     *******************************************************************************/
    EC_CALL_API int SetTopContact(unsigned int* matchKey, const char* contact, bool isTop);
   
    /*! @function
     ********************************************************************************
     函数名   : GetTopContact
     功能     : 获取置顶联系人列表
     参数     : [IN]  tcpMsgIdOut : 回传流水号
     返回值   : 0成功，非0失败
     回调函数 ：void (*onGetTopContact)(unsigned int clientNo, int reason, const char *jsonString)
     *******************************************************************************/
    EC_CALL_API int GetTopContact(unsigned int* matchKey);

	/****************************群组******************************/
  
	/*! @function
     ********************************************************************************
     函数名   : CreateGroup
     功能     : 创建群组
     参数     : [OUT] matchKey    : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  groupName : 群组名
                [IN]  type : （同学、朋友、同事等）1: 同学  2: 朋友 3: 同事
                [IN]  province : 省份
                [IN]  city : 城市
                [IN]  scope : 群类型 1:(上限100人) 2:上限300人 3:(上限500人) 4:(上限1000人) 5:(上限2000人)
                [IN]  declared : 群公告
                [IN]  permission : 1直接加入 2需要身份验证 3私有群组
                [IN]  isDismiss  : 创建者退出，群组是否解散 1:不解散 2：解散
                [IN]  groupDomain : 群备注，用于用户扩展字段
				[IN]  isDiscuss : 是否讨论组模式
     返回值   : 0成功； 非0失败
     回调函数 ：void (*onCreateGroup)
     *******************************************************************************/
    EC_CALL_API int CreateGroup(unsigned int *matchKey, const char*groupName, int type, const char* province, const char* city, int scope, const char* declared, int permission, int isDismiss, const char* groupDomain, bool isDiscuss);

    /*! @function
     ********************************************************************************
     函数名   : DismissGroup
     功能     : 解散群组
     参数     : [OUT] matchKey  : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  groupid	: 群组id
     返回值   : 0成功； 非0失败
     回调函数 ：void (*onDismissGroup)
     *******************************************************************************/
    EC_CALL_API int DismissGroup(unsigned int *matchKey, const char* groupid);
    
    /*! @function
     ********************************************************************************
     函数名   : QuitGroup
     功能     : 退出群组
     参数     : [OUT] matchKey: SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  groupid : 群组id
     返回值   : 0成功； 非0失败
     回调函数 ：void (*onQuitGroup)
     *******************************************************************************/
    EC_CALL_API int QuitGroup(unsigned int *matchKey, const char* groupid);
    
    /*! @function
     ********************************************************************************
     函数名   : JoinGroup
     功能     : 加入群组
     参数     : [OUT] matchKey : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  groupid  : 申请加入的群组Id
                [IN]  declared : 申请加入的理由
     返回值   : 0成功； 非0失败
     回调函数 ：void (*onJoinGroup)
     *******************************************************************************/
    EC_CALL_API int JoinGroup(unsigned int *matchKey, const char* groupid, const char* declared);
    
    /*! @function
     ********************************************************************************
     函数名   : ReplyRequestJoinGroup
     功能     : 回复请求加入群组请求
     参数     : [OUT] matchKey: SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  groupid : 申请加入的群组id
                [IN]  member  : 申请加入的成员id
                [IN]  confirm : 是否同意 1拒绝 2同意
     返回值   : 0成功； 非0失败
     回调函数 ：void (*onReplyRequestJoinGroup)
     *******************************************************************************/
    EC_CALL_API int ReplyRequestJoinGroup(unsigned int *matchKey, const char* groupid, const char* member, int confirm);
    
    /*! @function
     ********************************************************************************
     函数名   : InviteJoinGroup
     功能     : 邀请加入群组
     参数     : [OUT] matchKey   : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  groupid    : 邀请加入的群组id
                [IN]  declard    : 邀请理由
                [IN]  members    : 邀请加入的人
                [IN]  membercount: 邀请的人数
                [IN]  confirm    : 1 直接加入(不需要验证)； 2 需要对方验证
     返回值   : 0成功； 非0失败
     回调函数 ：void (*onInviteJoinGroup)
     *******************************************************************************/
    EC_CALL_API int InviteJoinGroup(unsigned int *matchKey, const char* groupId, const char* declard, const char** members, int membercount, int confirm);
    
    /*! @function
     ********************************************************************************
     函数名   : ReplyInviteJoinGroup
     功能     : 回复邀请加入群组请求
     参数     : [OUT] matchKey: SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  groupid : 邀请加入的群组id
                [IN]  invitor : 邀请加入的成员id
                [IN]  confirm : 是否同意 1拒绝 2同意
     返回值   : 0成功； 非0失败
     回调函数 ：(*onReplyInviteJoinGroup)
     *******************************************************************************/
    EC_CALL_API int ReplyInviteJoinGroup(unsigned int *matchKey, const char* groupid, const char* invitor, int confirm);
    
    /*! @function
     ********************************************************************************
     函数名   : QueryOwnGroup
     功能     : 获取自己加入的群组（分页处理）
     参数     : [OUT] matchKey      : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  borderGroupid : 已知页最新群组id
                [IN]  pageSize      : 每页数量（最多50）
				[IN]  target        : 1获取讨论组；2获取群组；125获取全部
     返回值   : 0成功； 非0失败
     回调函数 ：(*onQueryOwnGroup)
     *******************************************************************************/
    EC_CALL_API int QueryOwnGroup(unsigned int *matchKey, const char* borderGroupid,int pageSize, int target);
    
    /*! @function
     ********************************************************************************
     函数名   : QueryGroupDetail
     功能     : 获取群组详情
     参数     : [OUT] matchKey: SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  groupid : 获取信息的群组id
     返回值   : 0成功； 非0失败
     回调函数 ：void (*onQueryGroupDetail)
     *******************************************************************************/
    EC_CALL_API int QueryGroupDetail(unsigned int *matchKey, const char* groupId);
    
    /*! @function
     ********************************************************************************
     函数名   : ModifyGroup
     功能     : 管理员修改群组
     参数     : [OUT] matchKey         : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  pModifyGroupInfo : 修改群组信息结构体
     返回值   : 0成功； 非0失败
     回调函数 ：void (*onModifyGroup)
     *******************************************************************************/
    EC_CALL_API int ModifyGroup(unsigned int *matchKey, ECModifyGroupInfo* pModifyGroupInfo);
    
    /*! @function
     ********************************************************************************
     函数名   : SearchPubliGroups
     功能     : 搜索公共群组
     参数     : [OUT] matchKey   : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  searchType : 1根据GroupId，精确搜索 2根据GroupName，模糊搜索
                [IN]  keyword    : 搜索关键词
     返回值   : 0成功； 非0失败
     回调函数 ：void (*onSearchPublicGroup)
     *******************************************************************************/
    EC_CALL_API int SearchPublicGroups(unsigned int *matchKey, int searchType, const char* keyword);
    
	    /*! @function
     ********************************************************************************
     函数名   : SearchPubliGroups
     功能     : 搜索公共群组
     参数     : [OUT] matchKey   : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  searchType : 1根据GroupId，精确搜索 2根据GroupName，模糊搜索
                [IN]  keyword    : 搜索关键词
                [IN]  pageNo     : 搜索分页索引。群组名称搜索时,第几页数据 默认为1
                [IN]  pageSize   : 搜索分页大小。群组名称搜索时,每页数据条数，默认为10
     返回值   : 0成功； 非0失败
     回调函数 ：void (*onSearchPublicGroupPaging)
     *******************************************************************************/
    EC_CALL_API int SearchPublicGroupsPaging(unsigned int *matchKey, int searchType, const char* keyword, int pageNo, int pageSize);
    
    /*! @function
     ********************************************************************************
     函数名   : QueryGroupMember
     功能     : 获取群组成员（分页处理）
     参数     : [OUT] matchKey     : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  groupid      : 群组id
                [IN]  borderMember : 已知页最新成员id
                [IN]  pageSize     : 每页数量（最多50）
     返回值   : 0成功； 非0失败
     回调函数 ：void (*onQueryGroupMember)
     *******************************************************************************/
    EC_CALL_API int QueryGroupMember(unsigned int *matchKey, const char* groupId, const char* borderMember,int pageSize);
    
    /*! @function
     ********************************************************************************
     函数名   : DeleteGroupMember
     功能     : 删除群组成员
     参数     : [OUT] matchKey: SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  groupid : 删除成员的群组id
                [IN]  member  : 删除的成员
     返回值   : 0成功； 非0失败
     回调函数 ：void (*onDeleteGroupMember)
     *******************************************************************************/
    EC_CALL_API int DeleteGroupMember(unsigned int *matchKey, const char* groupid, const char* member);
    
    /*! @function
     ********************************************************************************
     函数名   : QueryGroupMemberCard
     功能     : 查询群组成员详情
     参数     : [OUT] matchKey: SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  groupid : 成员所属群组id
                [IN]  member  : 查询的成员id
     返回值   : 0成功； 非0失败
     回调函数 ：void (*onQueryGroupMemberCard)
     *******************************************************************************/
    EC_CALL_API int QueryGroupMemberCard(unsigned int *matchKey, const char* groupid, const char* member);
    
    /*! @function
     ********************************************************************************
     函数名   : ModifyMemberCard
     功能     : 管理员修改群组成员信息
     参数     : [OUT] matchKey: SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  member  : 修改的成员id
                [IN]  display : 显示的名字
                [IN]  phone   : 电话
                [IN]  mail    : 邮箱
                [IN]  remark  : 备注
                [IN]  belong  : 成员所属群组id
     返回值   : 0成功； 非0失败
     回调函数 ：void (*onModifyGroupMemberCard)
     *******************************************************************************/
    EC_CALL_API int ModifyMemberCard(unsigned int *matchKey, ECGroupMemberCard *card);
    
    /*! @function
     ********************************************************************************
     函数名   : ForbidMemberSpeak
     功能     : 群组成员禁言
     参数     : [OUT] matchKey: SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  groupid : 成员所属群组id
                [IN]  member  : 成员id
                [IN]  isBan   : 1允许发言 2禁止发言
     返回值   : 0成功； 非0失败
     回调函数 ：void (*onForbidMemberSpeakGroup)
     *******************************************************************************/
    EC_CALL_API int ForbidMemberSpeak(unsigned int *matchKey, const char* groupid, const char* member, int isBan);
    
    /*! @function
     ********************************************************************************
     函数名   : SetGroupMessageRule
     功能     : 设置群组消息规则。
     参数     : [OUT] matchKey: SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  groupid : 群组id
                [IN]  notice  : 消息提示设置 true 提示 false不提示
     返回值   : 0成功； 非0失败
     回调函数 ：void (*onSetGroupMessageRule)
     *******************************************************************************/
    EC_CALL_API int SetGroupMessageRule(unsigned int *matchKey, const char* groupid, bool notice);


	/******************************************************************************/
	/******************************************************************************/
		/*! @function
	********************************************************************************
	函数名   : StartVoiceRecording
	功能     : 开始录制语音消息
	参数     : [IN]  fileName	   : 录音文件保存的全路径，含后缀名.amr
	           [IN]  sessionId     : 透传会话ID，在回调中原封不动返回。可以为NULL
	返回值   : 是否成功 0：成功； 非0失败
	回调函数 ：void (*onRecordingAmplitude)
	*******************************************************************************/
    EC_CALL_API int StartVoiceRecording(const char* fileName,const char* sessionId);

	/*! @function
	********************************************************************************
	函数名   : StopVoiceRecording
	功能     : 停止录制语音消息
	参数     : 无
	返回值   : 无
	回调函数 ：void (*onRecordingTimeOut)
	*******************************************************************************/
    EC_CALL_API void StopVoiceRecording();
	
	/*! @function
	********************************************************************************
	函数名   : PlayVoiceMsg
	功能     : 播放语音消息
	参数     : [IN]  fileName  : 录音文件保存的全路径，含后缀名.amr
	           [IN]  sessionId : 透传会话ID，在回调中原封不动返回。可以为NULL
	返回值   : 是否成功 0：成功； 非0失败
	*******************************************************************************/
	EC_CALL_API int PlayVoiceMsg(const char* fileName,const char* sessionId);

	/*! @function
	********************************************************************************
	函数名   : StopVoiceMsg
	功能     : 停止播放语音消息
	参数     : 无
	返回值   : 无
	回调函数 ：void (*onFinishedPlaying)
	*******************************************************************************/
	EC_CALL_API void StopVoiceMsg();

	/*! @function
	********************************************************************************
	函数名   : GetVoiceDuration
	功能     : 获取语音消息时长
	参数     : [IN]  fileName	  : 录音文件保存的全路径，含后缀名.amr
	           [IN]  duration	  : 语音消息播放时长,单位毫秒
	返回值   : 是否成功 0：成功； 非0失败
	*******************************************************************************/
	EC_CALL_API int GetVoiceDuration(const char* fileName,int *duration);
	
	/*! @function
	********************************************************************************
	函数名   : PrintScreen
	功能     : 屏幕截图
	参数     : [IN]  width      ：截屏矩形的宽度
			   [IN]  height     ：截屏矩形的高度
			   [IN]  x          : 截屏时起始x坐标
			   [IN]  y          : 截屏时起始y坐标
	返回值   : 无
	*******************************************************************************/
	EC_CALL_API void PrintScreen(int x, int y, int width, int height);
		
	/*! @function
	********************************************************************************
	函数名   : PrintScreen
	功能     : 屏幕截图另存为
	参数     : [IN]  fileName   : 截屏图片的默认名称(以时间命名)
			   [IN]  filePath   ：允许用户指定路径，否则保存在安装目录下的 /jt/ 目录下
			   [IN]  fileType   ：文件类型，默认是png格式  1：png  2：bmp  3：jpg
	返回值   : 无
	*******************************************************************************/
	EC_CALL_API void SaveAsPic(const char* fileName, const char* filePath, int fileType);

	/*! @function
	********************************************************************************
	函数名   : DecryptTool
	功能     : 解密。输入字节流，输出字符串
	参数     : [out] outData     : 解密后内容。字符，需要外部分配内存，至少需要比intSize多1
			   [IN]  outSize	 : 外部分配内存数值
			   [IN]  inData      : 需要解密内容。字节
			   [IN]  inSize     : 需要解密内容长度
	返回值   : 0成功，非0失败
	*******************************************************************************/
	EC_CALL_API int DecryptTool(const char* outData,int outSize,const char* inData, int inSize);
	
	/*! @function
	********************************************************************************
	函数名   : DecryptBase64Tool
	功能     : base64解码后解密。输入字符串，输出字符串
	参数     : [out] outData     : 解密后内容。字符，需要外部分配内存，至少需要比intSize多1
			   [IN]  outSize	 : 外部分配内存数值
			   [IN]  inData      : 需要解密内容。字符
	返回值   : 0成功，非0失败
	*******************************************************************************/
	EC_CALL_API int DecryptBase64Tool(const char*  outData,int outSize,const char* inData);

	/*! @function
	********************************************************************************
	函数名   : UncompressBase64Tool
	功能     : base64解码后解压缩。输入字符串，输出字符串
	参数     : [out] outData     : 解压缩后内容。字符串，需要外部分配内存，至少需要比压缩前源字符串长度多1,用于底层做'\0'字符串结尾
			   [IN]  outSize	 : 外部分配内存数值。至少需要比压缩前源字符串长度多1
			   [IN]  inData      : 需要解压缩内容。字符
	返回值   : 0成功，非0失败
	*******************************************************************************/
	EC_CALL_API int UncompressBase64Tool(const char*  outData,int outSize,const char* inData);

	 /*! @function
     ********************************************************************************
     函数名   : SetGroupMemberRole
     功能     : 设置群组成员权限
     参数     : [IN]  matchKey    : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
                [IN]  groupid     : 群组id
                [IN]  member      : 群成员号
                [IN]  role        : 2管理员 3普通成员
     返回值   : 0成功； 非0失败
     回调函数 ：void (*onSetGroupMemberRole)(unsigned int matchKey, int reason);
     *******************************************************************************/
    EC_CALL_API int SetGroupMemberRole(unsigned int* matchKey, const char* groupId, const char* member, int role);
	   
    /*! @function
     ********************************************************************************
     函数名   : queryMessageReceipt
     功能     : 查询消息的已读和未读人数（只支持群组，且发送的消息）
     参数     : [IN]  tcpMsgIdOut : 回传流水号
				[IN]  messageId   : 发送消息id
				[IN]  version     : 有可能是同步的消息，需要传入版本号
     返回值   : 0成功，非0失败
     回调函数 ：void (*onQueryMessageReceipt)(unsigned int clientNo, int reason, const char *jsonString)
     *******************************************************************************/
    EC_CALL_API int QueryMessageReceipt(unsigned int* matchKey, const char* messageId, const char* version);

	/*! @function
	*********************************************************************************
	函数名    : UploadVTMFile
	功能      : 上传到VTM
	参数      : [IN] matchKey         : 回传流水号
		        [IN] companyId        : 企业id
	            [IN] companyPwd       : 企业密码
	            [IN] fileName         : 附件全路径
	            [IN] callbackUrl      : 回调url。选用
	返回值   : 0成功，非0失败
	回调函数 ：void (*onUploadVTMFileOrBuf)( unsigned int matchKey, int reason, const char* fileUrl)
	***********************************************************************/
	EC_CALL_API int UploadVTMFile(unsigned int* matchKey,const char* companyId,const char* companyPwd,const char* fileName,const char* callbackUrl);

	/*! @function
	************************************************************************
	函数名   : UploadVTMBuf
	功能     : 上传到VTM
	参数     : [IN]  fileIdOut      : 回传流水号
			   [IN]  companyId      : 企业id
			   [IN]  companyPwd     : 企业密码
			   [IN]  Name           : 附件名称
			   [IN]  callbackUrl    : 回调url。选用
	返回值   : 0成功，非0失败
	回调函数 ：void (*onUploadVTMFileOrBuf)( unsigned int fileIdOut,int reason, const char* fileUrl)
	***********************************************************************/
	EC_CALL_API int UploadVTMBuf(unsigned int* matchKey,const char* companyId,const char* companyPwd,const char* Name,unsigned char* buf,unsigned int size, const char* callbackUrl);

#ifdef __cplusplus
}
#endif

#endif