/*
 *  Copyright (c) 2013-2014 The CCP project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a Beijing Speedtong Information Technology Co.,Ltd license
 *  that can be found in the LICENSE file in the root of the web site.
 *
 *   http://www.yuntongxun.com
 *
 *  An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef _EC_APP_CLIENT_H_
#define _EC_APP_CLIENT_H_
#include "ECStructDef.h"

#ifdef __cplusplus
extern "C" 
{
#endif
//////////////////////////////////////////////////回调函数接口///////////////////////////////////////////////////////////
	//回调设置接口，必须主调前调用
	EC_CALL_API void SetGeneralCallBackFuction(ECGeneralCallBackInterface *InitInterface);
	
	
//////////////////////////////////////////////////主调函数接口///////////////////////////////////////////////////////////
     /*! @function
     ********************************************************************************
     函数名   : SetTraceFlag
     功能     : 日志开关。包括日志打印回调和写日志文件
	  参数     : [IN]  level				  : 日志级别，参考EClogLevel，越大越打印的日志越多越详细
				 [IN]  logFileName			  : 底层日志全路径，空则不写日志文件		 
     返回值   : 无。
     *******************************************************************************/
	EC_CALL_API void SetTraceFlag(int level,const char *logFileName);

	/*! @function
	********************************************************************************
	函数名   : ECSDK_Initialize
	功能     : 初始化CCP SDK
	返回值   : 是否初始化成功 0：成功； 非0失败
	*******************************************************************************/
	EC_CALL_API int ECSDK_Initialize();

	/*! @function
	********************************************************************************
	函数名   : ECSDK_UnInitialize
	功能     : 客户端注销
	返回值   : 是否成功 0：成功； 非0失败
	*******************************************************************************/
	EC_CALL_API int ECSDK_UnInitialize();
	
	/* @function
     ********************************************************************************
     函数名   : SetServerEnvType 
     功能     : 设置服务器环境地址的类型
	 参数     : [IN]  nEnvType         : 地址类型。0 生产；1 沙盒
				[IN]  ServerXmlFileName: 输入文件全路径，含配件文件名(可选)。为空时，内部默认当前路径
     返回值   : 0成功，非0失败 
     *******************************************************************************/
    EC_CALL_API int SetServerEnvType(int nEnvType,const char *ServerXmlFileName);  

	/* @function
     ********************************************************************************
     函数名   : SetInternalDNS 
     功能     : 设置内置DNS。当重连失败后将从内置DNS下载配置文件
	 参数     : [IN]  enable: 是否启用。底层默认开启，如果私有云无新dns，需要关闭
				[IN]  dns	: 域名,enable为true时有效。内部已经内置了公有云的DNS
				[IN]  port	: 端口,enable为true时有效。例如 8090
     返回值   : 0成功，非0失败 
     *******************************************************************************/
     EC_CALL_API int SetInternalDNS(bool enable,const char *dns,int port); 

	 /*! @function
     ********************************************************************************
     函数名   : Login
     功能     : 登录服务器
     参数     :[IN]  loginInfo    : 登录信息
     返回值   : 0成功，非0失败 
	 回调函数 ： void (*onConnectState)(const ECConnectState state )
     *******************************************************************************/
	EC_CALL_API int Login(const ECLoginInfo *loginInfo);
 
	/*! @function
     ********************************************************************************
     函数名   : Logout
     功能     : 登出服务器
	 参数     : [IN] 无
     返回值   : 0成功； 非0失败
	 回调函数 ：void (*onLogOut)(int reason);//登出回调
     *******************************************************************************/
    EC_CALL_API int Logout();

	/*! @function
     ********************************************************************************
     函数名   : SetPersonInfo
     功能     : 设置个人资料信息
     参数     : [OUT] matchKey    : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  pInfo		  : 用户信息
     返回值   : 0成功，非0失败 
	 回调函数 ：void (*onSetPersonInfo)
     *******************************************************************************/
    EC_CALL_API int SetPersonInfo(unsigned int *matchKey, ECPersonInfo* pInfo);

	/*! @function
     ********************************************************************************
     函数名   : GetPersonInfo
     功能     : 获取个人资料信息
     参数     : [OUT] matchKey : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  username : 用户账号
     返回值   : 0成功，非0失败 
	 回调函数 ：void (*onGetPersonInfo)
     *******************************************************************************/
    EC_CALL_API int GetPersonInfo(unsigned int *matchKey, const char* username);

	 /*! @function
     ********************************************************************************
     函数名   : GetUserState
     功能     : 获取个人状态
     参数     : [OUT] matchKey    : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN]  members : 多人用户账号。
				[IN]  membercount : 账号个数。
     返回值   : 0成功，非0失败
     回调函数 ：void (*onGetUserState)
     *******************************************************************************/
     EC_CALL_API int GetUserState(unsigned int *matchKey, const char** members, int membercount);

	 /*! @function
     ********************************************************************************
     函数名   : publishPresence
     功能     : 发布状态
     参数     : [IN] matchKey : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
				[IN] type     : 类型 1：在线 2：离线
				[IN] subtype  : 在线子类型 1:  请勿打扰  2：离开 3：忙碌 4：隐身
				[IN] userdata : 私有数据。保留字段，暂时不用
     返回值   : 0成功； 非0失败
     回调函数 ：void (*onPublishPresence)
     *******************************************************************************/
     EC_CALL_API int PublishPresence(unsigned int* matchKey, int type, int subtype, const char* userdata);

    /*! @function
     ********************************************************************************
     函数名   : PublishPresence_RX 容信使用
     功能     : 发布状态
     参数     : [IN]  matchKey    : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
                [IN]  type        : 类型 1：在线 2：离线
                [IN]  subtype     : 在线子类型 1:  请勿打扰  2：离开 3：忙碌 4：隐身
                [IN]  userdata    : 私有数据
     返回值   : 0成功； 非0失败
     回调函数 ：void (*onPublishPresence_RX)(unsigned int matchKey, int reason);//登出回调
     *******************************************************************************/
	 EC_CALL_API int PublishPresence_RX(unsigned int* matchKey, int type, int subtype, const char* userdata);

	 /* @function
     ********************************************************************************
     函数名   : queryErrorDescribe
     功能     : 查询错误码描述
	 参数     : [out]  errCodeDescribeOut: 回传错误码描述
				[IN]  errCodeIn          : 错误码
     返回值   : 0成功，非0失败 
     *******************************************************************************/
	 EC_CALL_API int QueryErrorDescribe(const char** errCodeDescribeOut,int errCodeIn);

	  /*! @function
     ********************************************************************************
     函数名   : GetOnlineMultiDevice
     功能     : 获取在线的其他设备
     参数     : [IN]  matchKey : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
     返回值   : 0成功，非0失败
     回调函数 ：void (*onGetOnlineMultiDevice)
     *******************************************************************************/
     EC_CALL_API int GetOnlineMultiDevice(unsigned int* matchKey);

	 /*! @function
     ********************************************************************************
     函数名   : getSerialNunber
     功能     : 生产流水号。供其他库用，例如协调库,不能用于本sdk生成matchKey
     返回值   : 流水号
     *******************************************************************************/
     EC_CALL_API unsigned int GetSerialNunber();

	 /*! @function
     ********************************************************************************
     函数名: SendCmdToRest
     功能  : 与Rest命令交互
     参数  : [IN]  matchKey    : SDK内部生成，开发者匹配请求和结果回调，不关心可以传入null
             [IN]  cmdid : 参考RestCMD.pb.h文件中枚举RestCMDEnumInner
             [IN]  cmdData : 根据不同的命令枚举RestCMDEnumInner，发送不同的命令，具体key值参考下面枚举后说明（json格式）    
				 枚举及内容说明
				 enum RestCMDEnumInner {
				 RestCmd_ConfStartRecord=1; //开始录制  1)meetingId:会议id； 2)member:账号； 3)type:1手机号 2VoIP账号; 4)rType:录制类型 0音频 1摄像头 2屏幕 3全部; 5)userdata;
				 RestCmd_ConfEndRecord=2; //停止录制 内容同上
				 }
			 [IN]  userData :自定义参数,暂时不用
     返回值   : 0成功，非0失败
     *******************************************************************************/
     EC_CALL_API int SendCmdToRest(unsigned int* matchKey, int cmdid, const char* cmdData, const char* userData);
	 
	 /*! @function
     ********************************************************************************
     函数名: SetServerAddress
     功能  : 临时设置服务器地址，供上层sdk临时测试用
     参数  : [IN]  protobuf_addr    : protobuf地址
             [IN]  protobuf_port    : protobuf端口
			 [IN]  filehttp_addr    : filehttp地址
			 [IN]  filehttp_port    : filehttp端口
     返回值   : 0成功，非0失败
     *******************************************************************************/
	 EC_CALL_API int SetServerAddress(const char *protobuf_addr, int protobuf_port, const char *filehttp_addr, int filehttp_port);
   
#ifdef __cplusplus
}
#endif

#endif 
