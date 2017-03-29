// ATLECSDK.cpp : Implementation of CATLECSDK
#include "stdafx.h"
#include "ATLECSDK.h"
#include <log4cplus/loggingmacros.h>
#include "codingHelper.h"
#include <comutil.h>
#include <vector>
#include "json.h"
#include <CCPVncViewer.h>
#include "ZBase64.h"

#ifdef DEBUG
#pragma comment(lib, "comsuppwd.lib")
#else
#pragma comment(lib, "comsuppw.lib")
#endif


static HRESULT JSCreateObject(IDispatch * pScript, CComBSTR objName, DISPPARAMS params, IDispatch ** newObject)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance("JSCreateObject");
	VARIANT  rVar;
	VariantInit(&rVar);
	DISPID idMethod = 0;
	CComQIPtr<IDispatchEx> pScriptEx;

	HRESULT hr = pScript->QueryInterface(IID_IDispatchEx, (void**)&pScriptEx);
	if (!SUCCEEDED(hr)){
		LOG4CPLUS_ERROR(log, __FUNCTION__ " cannot find interface IDispatchEx, :" << _bstr_t(objName) << ",ErrorCode:" << hr);
		return hr;
	}

	hr = pScriptEx->GetDispID(objName, 0, &idMethod);

	if (!SUCCEEDED(hr)){
		LOG4CPLUS_ERROR(log, __FUNCTION__ " cannot find " << _bstr_t(objName) << ",ErrorCode:" << hr);
		return hr;
	}

	hr = pScriptEx->InvokeEx(idMethod, LOCALE_SYSTEM_DEFAULT, DISPATCH_CONSTRUCT,
		&params, &rVar, NULL, NULL);

	if (SUCCEEDED(hr) && rVar.vt == VT_DISPATCH){
		*newObject = rVar.pdispVal;
	}
	else
		LOG4CPLUS_ERROR(log, __FUNCTION__ "," << _bstr_t(objName) << ", ErrorCode:" << hr);
	
	return hr;
}

static HRESULT JSNewObject(IDispatch * pScript, IDispatch ** newObject)
{
	DISPPARAMS dpNoArgs = { NULL, NULL, 0, 0 };
	return JSCreateObject(pScript, "Object", dpNoArgs, newObject);
}

static HRESULT JSNewArray(IDispatch * pScript, IDispatch ** newArrary)
{
	DISPPARAMS dpNoArgs = { NULL, NULL, 0, 0 };
	return JSCreateObject(pScript, "Array", dpNoArgs, newArrary);
}

static HRESULT JSNewNumber(IDispatch * pScript, IDispatch ** newNumber, CComVariant value)
{
	DISPPARAMS params = { &value, NULL, 1, 0 };
	return JSCreateObject(pScript, "Number", params, newNumber);
}

static HRESULT JSGetProperty(IDispatch* pDisp, CComBSTR pName, VARIANT * value)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance("JSGetProperty");
	DISPID dispId;
	HRESULT hr = S_OK;
	DISPPARAMS noArgs = { NULL, NULL, 0, 0 };

	hr = pDisp->GetIDsOfNames(IID_NULL, &pName, 1, LOCALE_USER_DEFAULT, &dispId);

	if (FAILED(hr)){
		LOG4CPLUS_ERROR(log, __FUNCTION__ " no " << _bstr_t(pName) << " property," << hr);
		return hr;
	}

	hr = pDisp->Invoke(dispId, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &noArgs, value, NULL, NULL);

	return hr;
}

static HRESULT JSSetProperty(IDispatch* pDisp, CComBSTR pName, CComVariant value)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance("JSSetProperty");
	DISPID dispId;
	HRESULT hr = S_OK;
	DISPID putid = DISPID_PROPERTYPUT;
	DISPPARAMS params = { &value, &putid, 1, 1 };
	CComQIPtr<IDispatchEx> pDispEx;

	//IE8 不支持ULONGLONG 类型
	if (value.vt == VT_UI8){
		value.vt = VT_UI4;
		value.ulVal = value.ullVal;
	}

	hr = pDisp->QueryInterface(IID_IDispatchEx, (void**)&pDispEx);
	if (FAILED(hr)){
		LOG4CPLUS_ERROR(log, __FUNCTION__ << "," << _bstr_t(pName) << " QueryInterface IDispatchEx error, " << hr);
		return hr;
	}

	hr = pDispEx->GetDispID(pName, fdexNameEnsure, &dispId);

	if (FAILED(hr)){
		LOG4CPLUS_ERROR(log, __FUNCTION__ ", " << _bstr_t(pName) << "," << hr);
		return hr;
	}

	hr = pDispEx->InvokeEx(dispId, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYPUTREF, &params, NULL, NULL, NULL);
	if (FAILED(hr))
	{
		LOG4CPLUS_ERROR(log, __FUNCTION__ << ", " << _bstr_t(pName) << "," << hr);
	}

	return hr;
}

static HRESULT JSSetValue(IDispatch* pDisp, CComVariant value)
{
	return JSSetProperty(pDisp, "value", value);
}

static HRESULT JSAddArrayElement(IDispatch* pDisp, CComVariant value)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance("JSAddArrayElement");
	HRESULT hr = S_OK;
	DISPID      dispid = 0;
	CComBSTR funcName(L"push");
	DISPPARAMS params = { &value, NULL, 1, 0 };

	hr = pDisp->GetIDsOfNames(IID_NULL, &funcName, 1, LOCALE_USER_DEFAULT, &dispid);

	if (FAILED(hr)){
		LOG4CPLUS_ERROR(log, __FUNCTION__ "," << hr);
		return hr;
	}

	hr = pDisp->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &params, NULL, NULL, NULL);
	if (FAILED(hr)){
		LOG4CPLUS_ERROR(log, __FUNCTION__ << "," << hr);
	}

	return hr;

}

static HRESULT JSGetArrayElementOfIndex(IDispatch* pDisp, int index, VARIANT * pValue)
{
	static log4cplus::Logger log = log4cplus::Logger::getInstance("JSGetArrayElementOfIndex");

	CComVariant varName(index, VT_I4);   // 数组下标
	DISPPARAMS noArgs = { NULL, NULL, 0, 0 };
	DISPID dispId;
	HRESULT hr = S_OK;

	varName.ChangeType(VT_BSTR);         // 将数组下标转为数字型，以进行GetIDsOfNames
	hr = pDisp->GetIDsOfNames(IID_NULL, &varName.bstrVal, 1, LOCALE_USER_DEFAULT, &dispId);
	if (FAILED(hr)){
		LOG4CPLUS_ERROR(log, ", Get Array Index:" << _bstr_t(varName) << ",ErrorCode:" << hr);
		return hr;
	}

	hr = pDisp->Invoke(dispId, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &noArgs, pValue, NULL, NULL);
	if (FAILED(hr)){
		LOG4CPLUS_ERROR(log,  ",Get Array Index:" << _bstr_t(varName) << ",ErrorCode:" << hr);
	}

	return hr;

}

// CATLECSDK

LRESULT CATLECSDK::onLoginfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	std::string * param = reinterpret_cast<std::string *>(lParam);
	LOG4CPLUS_TRACE(log, "Fire_onLogInfo");
	this->Fire_onLogInfo(_bstr_t(Utf82Unicode(*param).c_str()));
	delete param;
	return S_OK;
}


STDMETHODIMP CATLECSDK::Initialize(LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::Initialize();
	m_VncServer = new CCPVncServer(this);
	m_VncViewer = new CCPVncViewer(this);
	return S_OK;
}


STDMETHODIMP CATLECSDK::UnInitialize(LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::UnInitialize();
	return S_OK;
}


STDMETHODIMP CATLECSDK::Login(IDispatch* loginInfo, LONG* result)
{
	// TODO: Add your implementation code here
	VARIANT varValue;
	_bstr_t username;
	_bstr_t passWord;
	_bstr_t appKey;
	_bstr_t appToken;
	_bstr_t timestamp;
	_bstr_t MD5Token;

	ECLoginInfo info;
	info.authType = LoginAuthType_NormalAuth;

	if (SUCCEEDED(JSGetProperty(loginInfo, "authType", &varValue)))
	{
		info.authType = (LoginAuthType)varValue.intVal;
	}
	if (SUCCEEDED(JSGetProperty(loginInfo, "username", &varValue))){
		username = varValue;
		info.username =  username;
	}
	if (SUCCEEDED(JSGetProperty(loginInfo, "userPassword", &varValue))){
		passWord = varValue;
		info.userPassword = passWord;
	}
	if (SUCCEEDED(JSGetProperty(loginInfo, "appKey", &varValue))){
		appKey = varValue;
		info.appKey = appKey;
	}
	if (SUCCEEDED(JSGetProperty(loginInfo, "appToken", &varValue))){
		appToken = varValue;
		info.appToken = appToken;
	}
	if (SUCCEEDED(JSGetProperty(loginInfo, "timestamp", &varValue))){
		timestamp = varValue;
		info.timestamp = timestamp;
	}
	if (SUCCEEDED(JSGetProperty(loginInfo, "MD5Token", &varValue))){
		MD5Token = varValue;
		info.MD5Token = MD5Token;
	}
	

	*result = ECSDKBase::Login(&info);
	return S_OK;
}

LRESULT CATLECSDK::onLogOut(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	LOG4CPLUS_TRACE(log, "Fire_onLogOut");
	this->Fire_onLogOut(lParam);
	return S_OK;
}

LRESULT CATLECSDK::onConnectState(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	ECConnectState * param = reinterpret_cast<ECConnectState *>(lParam);
	LOG4CPLUS_TRACE(log, "Fire_onConnectState");
	IDispatch * obj = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}

	JSNewObject(m_Script, &obj);
	JSSetProperty(obj, "code", param->code);
	JSSetProperty(obj, "reason", param->reason);

	this->Fire_onConnectState(obj);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onSetPersonInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConSetPersonInfo * param = reinterpret_cast<EConSetPersonInfo *>(lParam);
	LOG4CPLUS_TRACE(log, "Fire_onSetPersonInfo");
	this->Fire_onSetPersonInfo(param->matchKey, param->reason, param->version);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onGetPersonInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConGetPersonInfo * param = reinterpret_cast<EConGetPersonInfo *>(lParam);
	LOG4CPLUS_TRACE(log, "Fire_onGetPersonInfo");
	IDispatch * personInfo = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &personInfo);
	JSSetProperty(personInfo, "version", param->PersonInfo.version);
	JSSetProperty(personInfo, "sex", param->PersonInfo.sex);
	JSSetProperty(personInfo, "nickName", Utf82Unicode(param->PersonInfo.nickName).c_str());
	JSSetProperty(personInfo, "birth", Utf82Unicode(param->PersonInfo.birth).c_str());
	JSSetProperty(personInfo, "sign", Utf82Unicode(param->PersonInfo.sign).c_str());
	this->Fire_onGetPersonInfo(param->matchKey, param->reason, personInfo);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onGetUserState(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConGetUserState * param = reinterpret_cast<EConGetUserState *>(lParam);

	IDispatch * stateArray = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewArray(m_Script, &stateArray);
	for (auto it : param->States)
	{
		IDispatch * psersonState = nullptr;
		JSNewObject(m_Script, &psersonState);
		JSSetProperty(psersonState, "accound", Utf82Unicode(it.accound).c_str());
		JSSetProperty(psersonState, "nickName", Utf82Unicode(it.nickName).c_str());
		JSSetProperty(psersonState, "device", it.device);
		JSSetProperty(psersonState, "netType", it.netType);
		JSSetProperty(psersonState, "state", it.state);
		JSSetProperty(psersonState, "subState", it.subState);
		JSSetProperty(psersonState, "timestamp", Utf82Unicode(it.timestamp).c_str());
		JSSetProperty(psersonState, "userData", Utf82Unicode(it.userData).c_str());

		JSAddArrayElement(stateArray, psersonState);
	}

	LOG4CPLUS_TRACE(log, "Fire_onGetUserState");
	
	this->Fire_onGetUserState(param->matchKey, param->reason, stateArray);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onPublishPresence(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	LOG4CPLUS_TRACE(log, "Fire_onPublishPresence");
	this->Fire_onPublishPresence(wParam, lParam);
	return S_OK;
}

LRESULT CATLECSDK::onReceiveFriendsPublishPresence(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	std::vector<ECPersonState> * param = reinterpret_cast<std::vector<ECPersonState> *>(lParam);
	IDispatch * personArray = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewArray(m_Script, &personArray);
	for (auto it : *param)
	{
		IDispatch * personState = nullptr;
		JSNewObject(m_Script, &personState);
		JSSetProperty(personState, "accound", Utf82Unicode(it.accound).c_str());
		JSSetProperty(personState, "device", it.device);
		JSSetProperty(personState, "netType", it.netType);
		JSSetProperty(personState, "nickName", Utf82Unicode(it.nickName).c_str());
		JSSetProperty(personState, "state", it.state);
		JSSetProperty(personState, "subState", it.subState);
		JSSetProperty(personState, "timestamp", Utf82Unicode(it.timestamp).c_str());
		JSSetProperty(personState, "userData", Utf82Unicode(it.userData).c_str());

		JSAddArrayElement(personArray, personState);
	}
	
	LOG4CPLUS_TRACE(log, "Fire_onReceiveFriendsPublishPresence");
	this->Fire_onReceiveFriendsPublishPresence(personArray);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onSoftVersion(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConSoftVersion * param = reinterpret_cast<EConSoftVersion *>(lParam);
	LOG4CPLUS_TRACE(log, "Fire_onSoftVersion");
	this->Fire_onSoftVersion(CComBSTR(Utf82Unicode(param->softVersion).c_str()), param->updateMode, CComBSTR(Utf82Unicode(param->updateDesc).c_str()));
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onGetOnlineMultiDevice(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConGetOnlineMultiDevice * param = reinterpret_cast<EConGetOnlineMultiDevice *>(lParam);
	IDispatch * mdStateArray = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewArray(m_Script, &mdStateArray);
	for (auto it : param->MultiDeviceStates)
	{
		IDispatch * mdState = nullptr;
		JSNewObject(m_Script, &mdState);
		JSSetProperty(mdState, "device", it.device);
		JSSetProperty(mdState, "state", it.state);

		JSAddArrayElement(mdStateArray, mdState);
	}
	LOG4CPLUS_TRACE(log, "Fire_onGetOnlineMultiDevice");
	this->Fire_onGetOnlineMultiDevice(param->matchKey, param->reason, mdStateArray);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onReceiveMultiDeviceState(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	std::vector<ECMultiDeviceState>  * param = reinterpret_cast<std::vector<ECMultiDeviceState>  *>(lParam);
	IDispatch * mdStateArray = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewArray(m_Script, &mdStateArray);
	for (auto it:*param)
	{
		IDispatch * mdState = nullptr;
		JSNewObject(m_Script, &mdState);
		JSSetProperty(mdState, "device", it.device);
		JSSetProperty(mdState, "state", it.state);

		JSAddArrayElement(mdStateArray, mdState);
	}
	LOG4CPLUS_TRACE(log, "Fire_onReceiveMultiDeviceState");
	this->Fire_onReceiveMultiDeviceState(mdStateArray);
	delete param;
	return S_OK;
}

STDMETHODIMP CATLECSDK::SetTraceFlag(LONG level, BSTR logFileName)
{
	// TODO: Add your implementation code here
	std::string utf8LogFileName = Unicode2Utf8(logFileName?logFileName:L"");
	ECSDKBase::SetTraceFlag(level, utf8LogFileName.c_str());
	return S_OK;
}


STDMETHODIMP CATLECSDK::SetServerEnvType(LONG nEnvType, BSTR xmlFileName, LONG* result)
{
	// TODO: Add your implementation code here
	std::string utf8xmlFileName = Unicode2Utf8(xmlFileName?xmlFileName:L"");
	*result = ECSDKBase::SetServerEnvType(nEnvType, utf8xmlFileName.c_str());
	return S_OK;
}


STDMETHODIMP CATLECSDK::SetInternalDNS(VARIANT_BOOL enable, BSTR dns, LONG port, LONG* result)
{
	// TODO: Add your implementation code here
	std::string utf8DNS = Unicode2Utf8(dns?dns:L"");
	*result = ECSDKBase::SetInternalDNS(enable == VARIANT_TRUE ? true : false, utf8DNS.c_str(), port);
	return S_OK;
}


STDMETHODIMP CATLECSDK::Logout(LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::Logout();
	return S_OK;
}


STDMETHODIMP CATLECSDK::SetPersonInfo(IDispatch* matchKey, IDispatch* psersonInfo, LONG* result)
{
	// TODO: Add your implementation code here
	ECPersonInfo info;
	unsigned int rmatchKey = 0;
	VARIANT varValue;
	_bstr_t birth;
	_bstr_t nickName;
	_bstr_t sign;


	if (SUCCEEDED(JSGetProperty(psersonInfo, "birth", &varValue))){
		birth = varValue;
		strncpy_s(info.birth, birth, sizeof(info.birth)-1);
	}

	if (SUCCEEDED(JSGetProperty(psersonInfo, "nickName", &varValue))){
		nickName = varValue;
		strncpy_s(info.nickName, nickName, sizeof(info.nickName)-1);
	}

	if (SUCCEEDED(JSGetProperty(psersonInfo, "sex", &varValue))){
		info.sex = varValue.intVal;
	}

	if (SUCCEEDED(JSGetProperty(psersonInfo, "sign", &varValue))){
		sign = varValue;
		strncpy_s(info.sign, sign,sizeof(info.sign)-1);
	}

	if (SUCCEEDED(JSGetProperty(psersonInfo, "version", &varValue))){
		info.version = varValue.intVal;
	}
	
	*result = ECSDKBase::SetPersonInfo(&rmatchKey, &info);
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::GetPersonInfo(IDispatch* matchKey, BSTR username, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	std::string utf8Username = Unicode2Utf8(username?username:L"");
	*result = ECSDKBase::GetPersonInfo(&rmatchKey, utf8Username.c_str());
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::GetUserState(IDispatch* matchKey, IDispatch* members, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	VARIANT length;
	VariantInit(&length);
	if (SUCCEEDED(JSGetProperty(members, "length", &length)) && length.vt == VT_I4){
		std::vector<const char *> arr(length.intVal);
		std::vector<_bstr_t> bstrArr(length.intVal);
		for (int i = 0; i < length.intVal; i++)
		{
			VARIANT element;
			VariantInit(&element);
			if (SUCCEEDED(JSGetArrayElementOfIndex(members, i, &element)))
			{
				bstrArr[i] = _bstr_t(element);
				arr[i] = bstrArr[i];
			}
		}

		*result = ECSDKBase::GetUserState(&rmatchKey, arr.data(), length.intVal);
		JSSetValue(matchKey, rmatchKey);
	}
	else{
		LOG4CPLUS_ERROR(log, __FUNCTION__ " Parameter not array.");
		*result = ECSDKBase::GetUserState(&rmatchKey, nullptr, 0);
	}

	return S_OK;
}


STDMETHODIMP CATLECSDK::QueryErrorDescribe(IDispatch * errCodeDes, LONG errCode, LONG* result)
{
	// TODO: Add your implementation code here
	const char * errBuf = nullptr;
	*result = ECSDKBase::QueryErrorDescribe(&errBuf, errCode);
	JSSetValue(errCodeDes, _bstr_t(errBuf).Detach());
	return S_OK;
}

LRESULT CATLECSDK::onMakeCallBack(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConMakeCallBack * param = reinterpret_cast<EConMakeCallBack *>(lParam);
	IDispatch * cbInfo = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &cbInfo);
	JSSetProperty(cbInfo, "called", Utf82Unicode(param->CallBackInfo.called).c_str());
	JSSetProperty(cbInfo, "calledDisplay", Utf82Unicode(param->CallBackInfo.calledDisplay).c_str());
	JSSetProperty(cbInfo, "caller", Utf82Unicode(param->CallBackInfo.caller).c_str());
	JSSetProperty(cbInfo, "callerDisplay", Utf82Unicode(param->CallBackInfo.callerDisplay).c_str());

	LOG4CPLUS_TRACE(log, "Fire_onMakeCallBack");
	Fire_onMakeCallBack(param->matchKey, param->reason, cbInfo);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onCallEvents(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	ECVoIPCallMsg * param = reinterpret_cast<ECVoIPCallMsg *>(lParam);
	IDispatch * call = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &call);
	JSSetProperty(call, "callee", Utf82Unicode(param->callee).c_str());
	JSSetProperty(call, "caller", Utf82Unicode(param->caller).c_str());
	JSSetProperty(call, "callid", Utf82Unicode(param->callid).c_str());
	JSSetProperty(call, "callType", param->callType);
	JSSetProperty(call, "eCode", param->eCode);

	LOG4CPLUS_TRACE(log, "Fire_onCallEvents");
	Fire_onCallEvents(wParam, call);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onCallIncoming(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	ECVoIPComingInfo * param = reinterpret_cast<ECVoIPComingInfo *>(lParam);
	IDispatch * coming = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &coming);
	JSSetProperty(coming, "caller", Utf82Unicode(param->caller).c_str());
	JSSetProperty(coming, "callid", Utf82Unicode(param->callid).c_str());
	JSSetProperty(coming, "callType", param->callType);
	JSSetProperty(coming, "display", Utf82Unicode(param->display).c_str());
	JSSetProperty(coming, "nickname", Utf82Unicode(param->nickname).c_str());

	LOG4CPLUS_TRACE(log, "Fire_onCallIncoming");
	Fire_onCallIncoming(coming);
	delete param;
	return S_OK;
}


LRESULT CATLECSDK::onDtmfReceived(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	std::string * callid = reinterpret_cast<std::string *>(wParam);
	std::string dtmf;
	dtmf.assign(1, (char)lParam);
	LOG4CPLUS_TRACE(log, "Fire_onDtmfReceived");
	Fire_onDtmfReceived(_bstr_t(Utf82Unicode(*callid).c_str()), _bstr_t(Utf82Unicode(dtmf).c_str()));
	delete callid;
	return S_OK;
}

LRESULT CATLECSDK::onSwitchCallMediaTypeRequest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	std::string * callid = reinterpret_cast<std::string *>(wParam);
	LOG4CPLUS_TRACE(log, "Fire_onSwitchCallMediaTypeRequest");
	Fire_onSwitchCallMediaTypeRequest(_bstr_t(Utf82Unicode(*callid).c_str()), lParam);
	delete callid;
	return S_OK;
}

LRESULT CATLECSDK::onSwitchCallMediaTypeResponse(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	std::string * callid = reinterpret_cast<std::string *>(wParam);
	LOG4CPLUS_TRACE(log, "Fire_onSwitchCallMediaTypeResponse");
	Fire_onSwitchCallMediaTypeResponse(_bstr_t(Utf82Unicode(*callid).c_str()), lParam);
	delete callid;
	return S_OK;
}

LRESULT CATLECSDK::onRemoteVideoRatio(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConRemoteVideoRatio * param = reinterpret_cast<EConRemoteVideoRatio *>(lParam);
	LOG4CPLUS_TRACE(log, "Fire_onRemoteVideoRatio");
	Fire_onRemoteVideoRatio(_bstr_t(Utf82Unicode(param->CallidOrConferenceId).c_str()), param->width, param->height, param->type,
		_bstr_t(Utf82Unicode(param->member).c_str()));
	delete param;
	return S_OK;
}

STDMETHODIMP CATLECSDK::MakeCallBack(IDispatch* matchKey, BSTR caller, BSTR called, BSTR callerDisplay, BSTR calledDisplay, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::MakeCallBack(&rmatchKey, Unicode2Utf8(caller?caller:L"").c_str(), Unicode2Utf8(called?called:L"").c_str(),
	Unicode2Utf8(callerDisplay?callerDisplay:L"").c_str(), Unicode2Utf8(calledDisplay?calledDisplay:L"").c_str());
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::MakeCall(IDispatch* OutCallid, LONG callType, BSTR called, LONG* result)
{
	// TODO: Add your implementation code here
	const char * rOutCallid = nullptr;
	*result = ECSDKBase::MakeCall(&rOutCallid, callType, Unicode2Utf8(called?called:L"").c_str());
	JSSetValue(OutCallid, Utf82Unicode(rOutCallid?rOutCallid:"").c_str());
	return S_OK;
}


STDMETHODIMP CATLECSDK::AcceptCall(BSTR callId, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::AcceptCall(Unicode2Utf8(callId?callId:L"").c_str());
	return S_OK;
}


STDMETHODIMP CATLECSDK::ReleaseCall(BSTR callId, LONG reason, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::ReleaseCall(Unicode2Utf8(callId?callId:L"").c_str(), reason);
	return S_OK;
}


STDMETHODIMP CATLECSDK::SendDTMF(BSTR callId, BSTR dtmf, LONG* result)
{
	// TODO: Add your implementation code here
	std::string utf8DTMF = Unicode2Utf8(dtmf?dtmf:L"");
	*result = ECSDKBase::SendDTMF(Unicode2Utf8(callId?callId:L"").c_str(), utf8DTMF.at(0));
	return S_OK;
}


STDMETHODIMP CATLECSDK::GetCurrentCall(BSTR* result)
{
	// TODO: Add your implementation code here
	const char * callid = ECSDKBase::GetCurrentCall();
	if (callid == nullptr){
		callid = "";
	}

	*result = SysAllocString(Utf82Unicode(callid).c_str());
	return S_OK;
}


STDMETHODIMP CATLECSDK::SetMute(VARIANT_BOOL mute, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::SetMute(mute == VARIANT_TRUE ? true : false);
	return S_OK;
}


STDMETHODIMP CATLECSDK::GetMuteStatus(VARIANT_BOOL* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::GetMuteStatus();
	return S_OK;
}

STDMETHODIMP CATLECSDK::SetSoftMute(BSTR callid, VARIANT_BOOL on, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::SetSoftMute(Unicode2Utf8(callid?callid:L"").c_str(), on == VARIANT_TRUE ? true : false);
	return S_OK;
}


STDMETHODIMP CATLECSDK::GetSoftMuteStatus(BSTR callid, IDispatch* bMute, LONG* result)
{
	// TODO: Add your implementation code here
	bool rbMute = false;
	*result = ECSDKBase::GetSoftMuteStatus(Unicode2Utf8(callid?callid:L"").c_str(), &rbMute);
	JSSetValue(bMute, rbMute);
	return S_OK;
}

STDMETHODIMP CATLECSDK::SetAudioConfigEnabled(LONG type, VARIANT_BOOL enabled, LONG mode, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::SetAudioConfigEnabled(type, enabled == VARIANT_TRUE ? true : false, mode);
	return S_OK;
}


STDMETHODIMP CATLECSDK::GetAudioConfigEnabled(LONG type, IDispatch* enabled, IDispatch* mode, LONG* result)
{
	// TODO: Add your implementation code here
	bool renabled;
	int rmode;
	*result = ECSDKBase::GetAudioConfigEnabled(type, &renabled, &rmode);
	JSSetValue(enabled, renabled);
	JSSetValue(mode, rmode);
	return S_OK;
}


STDMETHODIMP CATLECSDK::SetCodecEnabled(LONG type, VARIANT_BOOL enabled, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::SetCodecEnabled(type, enabled == VARIANT_TRUE ? true : false);
	return S_OK;
}


STDMETHODIMP CATLECSDK::GetCodecEnabled(LONG type, VARIANT_BOOL* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::GetCodecEnabled(type);
	return S_OK;
}


STDMETHODIMP CATLECSDK::SetCodecNack(VARIANT_BOOL bAudioNack, VARIANT_BOOL bVideoNack, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::SetCodecNack(bAudioNack == VARIANT_TRUE ? true : false, bVideoNack == VARIANT_TRUE ? true : false);
	return S_OK;
}


STDMETHODIMP CATLECSDK::GetCodecNack(IDispatch* bAudioNack, IDispatch* bVideoNack, LONG* result)
{
	// TODO: Add your implementation code here
	bool rbAudioNack;
	bool rbVideoNack;
	*result = ECSDKBase::GetCodecNack(&rbAudioNack, &rbVideoNack);
	JSSetValue(bAudioNack, rbAudioNack);
	JSSetValue(bVideoNack, rbVideoNack);
	return S_OK;
}


STDMETHODIMP CATLECSDK::GetCallStatistics(BSTR callId, VARIANT_BOOL bVideo, IDispatch* statistics, LONG* result)
{
	// TODO: Add your implementation code here
	ECMediaStatisticsInfo info;
	*result = ECSDKBase::GetCallStatistics(Unicode2Utf8(callId?callId:L"").c_str(), bVideo == VARIANT_TRUE ? true : false, &info);
	JSSetProperty(statistics, "bytesReceived", info.bytesReceived);
	JSSetProperty(statistics, "bytesSent", info.bytesSent);
	JSSetProperty(statistics, "cumulativeLost", info.cumulativeLost);
	JSSetProperty(statistics, "extendedMax", info.extendedMax);
	JSSetProperty(statistics, "fractionLost", info.fractionLost);
	JSSetProperty(statistics, "jitterSamples", info.jitterSamples);
	JSSetProperty(statistics, "packetsReceived", info.packetsReceived);
	JSSetProperty(statistics, "packetsSent", info.packetsSent);
	JSSetProperty(statistics, "rttMs", info.rttMs);
	return S_OK;
}


STDMETHODIMP CATLECSDK::SetVideoView(LONG view, LONG localView, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::SetVideoView((void*)view, (void*)localView);
	return S_OK;
}


STDMETHODIMP CATLECSDK::SetVideoBitRates(LONG bitrates)
{
	// TODO: Add your implementation code here
	ECSDKBase::SetVideoBitRates(bitrates);
	return S_OK;
}


STDMETHODIMP CATLECSDK::SetRing(BSTR fileName, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::SetRing(Unicode2Utf8((fileName?fileName:L"")).c_str());
	return S_OK;
}


STDMETHODIMP CATLECSDK::SetRingback(BSTR fileName, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::SetRingback(Unicode2Utf8((fileName ? fileName : L"")).c_str());
	return S_OK;
}


STDMETHODIMP CATLECSDK::RequestSwitchCallMediaType(BSTR callId, LONG video, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::RequestSwitchCallMediaType(Unicode2Utf8(callId ? callId : L"").c_str(), video);
	return S_OK;
}


STDMETHODIMP CATLECSDK::ResponseSwitchCallMediaType(BSTR callId, LONG video, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::ResponseSwitchCallMediaType(Unicode2Utf8(callId ? callId : L"").c_str(), video);
	return S_OK;
}


STDMETHODIMP CATLECSDK::GetCameraInfo(IDispatch* cameraInfo, LONG* result)
{
	// TODO: Add your implementation code here
	ECCameraInfo * info = nullptr;
	*result = ECSDKBase::GetCameraInfo(&info);

	//JSNewArray(pScript, & cameraInfo);

	for (int nIndex = 0; nIndex < *result; nIndex++)
	{
		IDispatch * cameraObj = nullptr;
		if (m_Script == NULL){
			get_Script(&m_Script);
		}
		JSNewObject(m_Script, &cameraObj);
		JSSetProperty(cameraObj, "index", info[nIndex].index);
		JSSetProperty(cameraObj, "id", Utf82Unicode(info[nIndex].id).c_str());
		JSSetProperty(cameraObj, "name", Utf82Unicode(info[nIndex].name).c_str());
		JSSetProperty(cameraObj, "capabilityCount", info[nIndex].capabilityCount);

		IDispatch * capabilityArray = nullptr;
		JSNewArray(m_Script, &capabilityArray);

		ECCameraCapability * pCapability = info[nIndex].capability;
		for (int CapabilityIndex = 0; CapabilityIndex < info[nIndex].capabilityCount; CapabilityIndex++)
		{
			IDispatch * capabilityObj = nullptr;
			JSNewObject(m_Script, &capabilityObj);

			JSSetProperty(capabilityObj, "width", pCapability[CapabilityIndex].width);
			JSSetProperty(capabilityObj, "height", pCapability[CapabilityIndex].height);
			JSSetProperty(capabilityObj, "maxfps", pCapability[CapabilityIndex].maxfps);
			JSAddArrayElement(capabilityArray, capabilityObj);
		}

		JSSetProperty(cameraObj, "capability", capabilityArray);
		JSAddArrayElement(cameraInfo, cameraObj);
	}
	return S_OK;
}


STDMETHODIMP CATLECSDK::SelectCamera(LONG cameraIndex, LONG capabilityIndex, LONG fps, LONG rotate, VARIANT_BOOL force, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::SelectCamera(cameraIndex, capabilityIndex, fps, rotate, force == VARIANT_TRUE ? true : false);
	return S_OK;
}


STDMETHODIMP CATLECSDK::GetNetworkStatistic(BSTR callId, IDispatch* duration, IDispatch* sendTotalWifi, IDispatch* recvTotalWifi, LONG* result)
{
	// TODO: Add your implementation code here
	long long rduration;
	long long rsendTotalWifi;
	long long rrecvTotalWifi;
	*result = ECSDKBase::GetNetworkStatistic(Unicode2Utf8(callId?callId:L"").c_str(), &rduration, &rsendTotalWifi, &rrecvTotalWifi);
	JSSetValue(duration, rduration);
	JSSetValue(sendTotalWifi, rsendTotalWifi);
	JSSetValue(recvTotalWifi, rrecvTotalWifi);
	return S_OK;
}


STDMETHODIMP CATLECSDK::GetSpeakerVolume(IDispatch* volume, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rvolume;
	*result = ECSDKBase::GetSpeakerVolume(&rvolume);
	JSSetValue(volume, rvolume);
	return S_OK;
}


STDMETHODIMP CATLECSDK::SetSpeakerVolume(LONG volume, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::SetSpeakerVolume(volume);
	return S_OK;
}


STDMETHODIMP CATLECSDK::SetDtxEnabled(VARIANT_BOOL enabled)
{
	// TODO: Add your implementation code here
	ECSDKBase::SetDtxEnabled(enabled == VARIANT_TRUE ? true : false);
	return S_OK;
}


STDMETHODIMP CATLECSDK::SetSelfPhoneNumber(BSTR phoneNumber)
{
	// TODO: Add your implementation code here
	ECSDKBase::SetSelfPhoneNumber(Unicode2Utf8(phoneNumber?phoneNumber:L"").c_str());
	return S_OK;
}


STDMETHODIMP CATLECSDK::SetSelfName(BSTR nickName)
{
	// TODO: Add your implementation code here
	ECSDKBase::SetSelfName(Unicode2Utf8(nickName?nickName:L"").c_str());
	return S_OK;
}


STDMETHODIMP CATLECSDK::EnableLoudsSpeaker(VARIANT_BOOL enable, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::EnableLoudsSpeaker(enable == VARIANT_TRUE ? true : false);
	return S_OK;
}


STDMETHODIMP CATLECSDK::GetLoudsSpeakerStatus(VARIANT_BOOL* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::GetLoudsSpeakerStatus() == true ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}


STDMETHODIMP CATLECSDK::ResetVideoView(BSTR callid, LONG remoteView, LONG localView, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::ResetVideoView(Unicode2Utf8(callid?callid:L"").c_str(), 
		(void*)remoteView, (void*)localView);
	return S_OK;
}


LRESULT CATLECSDK::onOfflineMessageCount(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	LOG4CPLUS_DEBUG(log, "Fire_onOfflineMessageCount");
	this->Fire_onOfflineMessageCount(lParam);
	return S_OK;
}

LRESULT CATLECSDK::onGetOfflineMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	LONG varResult = 0;
	this->Fire_onGetOfflineMessage(&varResult);
	LOG4CPLUS_DEBUG(log, "Fire_onGetOfflineMessage:" << varResult);
	m_onGetOfflineMessage_Result = varResult;
	std::unique_lock<std::mutex> lck(m_onGetOfflineMessage_mtx);
	m_onGetOfflineMessage_cv.notify_one();
	return S_OK;
}

LRESULT CATLECSDK::onOfflineMessageComplete(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	LOG4CPLUS_DEBUG(log, "Fire_onOfflineMessageComplete");
	this->Fire_onOfflineMessageComplete();
	return S_OK;
}
LRESULT CATLECSDK::onReceiveOfflineMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	ECMessage * param = reinterpret_cast<ECMessage*>(lParam);
	IDispatch * Iparam = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &Iparam);
	JSSetProperty(Iparam, "dateCreated", Utf82Unicode(param->dateCreated).c_str());
	JSSetProperty(Iparam, "fileName", Utf82Unicode(param->fileName).c_str());
	JSSetProperty(Iparam, "fileSize", param->fileSize);
	JSSetProperty(Iparam, "fileUrl", Utf82Unicode(param->fileUrl).c_str());
	JSSetProperty(Iparam, "msgContent", Utf82Unicode(param->msgContent).c_str());
	JSSetProperty(Iparam, "msgId", Utf82Unicode(param->msgId).c_str());
	JSSetProperty(Iparam, "msgType", param->msgType);
	JSSetProperty(Iparam, "offset", param->offset);
	JSSetProperty(Iparam, "receiver", Utf82Unicode(param->receiver).c_str());
	JSSetProperty(Iparam, "sender", Utf82Unicode(param->sender).c_str());
	JSSetProperty(Iparam, "senderNickName", Utf82Unicode(param->senderNickName).c_str());
	JSSetProperty(Iparam, "sessionId", Utf82Unicode(param->sessionId).c_str());
	JSSetProperty(Iparam, "sessionType", param->sessionType);
	JSSetProperty(Iparam, "userData", Utf82Unicode(param->userData).c_str());
	JSSetProperty(Iparam, "isAt", param->isAt);
	JSSetProperty(Iparam, "type", param->type);

	LOG4CPLUS_DEBUG(log, "Fire_onReceiveOfflineMessage");
	this->Fire_onReceiveOfflineMessage(Iparam);
	delete param;
	return S_OK;
}
LRESULT CATLECSDK::onReceiveMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	ECMessage * param = reinterpret_cast<ECMessage*>(lParam);
	IDispatch * Iparam = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &Iparam);
	JSSetProperty(Iparam, "dateCreated", Utf82Unicode(param->dateCreated).c_str());
	JSSetProperty(Iparam, "fileName", Utf82Unicode(param->fileName).c_str());
	JSSetProperty(Iparam, "fileSize",param->fileSize);
	JSSetProperty(Iparam, "fileUrl", Utf82Unicode(param->fileUrl).c_str());
	JSSetProperty(Iparam, "msgContent", Utf82Unicode(param->msgContent).c_str());
	JSSetProperty(Iparam, "msgId", Utf82Unicode(param->msgId).c_str());
	JSSetProperty(Iparam, "msgType", param->msgType);
	JSSetProperty(Iparam, "offset", param->offset);
	JSSetProperty(Iparam, "receiver", Utf82Unicode(param->receiver).c_str());
	JSSetProperty(Iparam, "sender", Utf82Unicode(param->sender).c_str());
	JSSetProperty(Iparam, "senderNickName", Utf82Unicode(param->senderNickName).c_str());
	JSSetProperty(Iparam, "sessionId", Utf82Unicode(param->sessionId).c_str());
	JSSetProperty(Iparam, "sessionType", param->sessionType);
	JSSetProperty(Iparam, "userData", Utf82Unicode(param->userData).c_str());
	JSSetProperty(Iparam, "isAt", param->isAt);
	JSSetProperty(Iparam, "type", param->type);


	if (param->msgType == Msg_Type_Text)
	{
		std::string msgContent = param->msgContent;
		if (msgContent.compare(0, 7, "VncCmd:") == 0)
		{
			handleVncCmd(msgContent.substr(7));
		}
		else
		{
			LOG4CPLUS_DEBUG(log, "Fire_onReceiveMessage");
			this->Fire_onReceiveMessage(Iparam);
		}
	}
	else
	{
		LOG4CPLUS_DEBUG(log, "Fire_onReceiveMessage");
		this->Fire_onReceiveMessage(Iparam);
	}
	delete param;
	return S_OK;
}
LRESULT CATLECSDK::onReceiveFile(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	ECMessage * param = reinterpret_cast<ECMessage*>(lParam);
	IDispatch * Iparam = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &Iparam);
	JSSetProperty(Iparam, "dateCreated", Utf82Unicode(param->dateCreated).c_str());
	JSSetProperty(Iparam, "fileName", Utf82Unicode(param->fileName).c_str());
	JSSetProperty(Iparam, "fileSize", param->fileSize);
	JSSetProperty(Iparam, "fileUrl", Utf82Unicode(param->fileUrl).c_str());
	JSSetProperty(Iparam, "msgContent", Utf82Unicode(param->msgContent).c_str());
	JSSetProperty(Iparam, "msgId", Utf82Unicode(param->msgId).c_str());
	JSSetProperty(Iparam, "msgType", param->msgType);
	JSSetProperty(Iparam, "offset", param->offset);
	JSSetProperty(Iparam, "receiver", Utf82Unicode(param->receiver).c_str());
	JSSetProperty(Iparam, "sender", Utf82Unicode(param->sender).c_str());
	JSSetProperty(Iparam, "senderNickName", Utf82Unicode(param->senderNickName).c_str());
	JSSetProperty(Iparam, "sessionId", Utf82Unicode(param->sessionId).c_str());
	JSSetProperty(Iparam, "sessionType", param->sessionType);
	JSSetProperty(Iparam, "userData", Utf82Unicode(param->userData).c_str());
	JSSetProperty(Iparam, "isAt", param->isAt);
	JSSetProperty(Iparam, "type", param->type);

	LOG4CPLUS_DEBUG(log, "Fire_onReceiveFile");
	this->Fire_onReceiveFile(Iparam);
	delete param;
	return S_OK;
}
LRESULT CATLECSDK::onDownloadFileComplete(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConDownloadFileComplete * param = reinterpret_cast<EConDownloadFileComplete*>(lParam);
	IDispatch * msg = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &msg);
	JSSetProperty(msg, "dateCreated", Utf82Unicode(param->msg.dateCreated).c_str());
	JSSetProperty(msg, "fileName", Utf82Unicode(param->msg.fileName).c_str());
	JSSetProperty(msg, "fileSize", param->msg.fileSize);
	JSSetProperty(msg, "fileUrl", Utf82Unicode(param->msg.fileUrl).c_str());
	JSSetProperty(msg, "msgContent", Utf82Unicode(param->msg.msgContent).c_str());
	JSSetProperty(msg, "msgId", Utf82Unicode(param->msg.msgId).c_str());
	JSSetProperty(msg, "msgType", param->msg.msgType);
	JSSetProperty(msg, "offset", param->msg.offset);
	JSSetProperty(msg, "receiver", Utf82Unicode(param->msg.receiver).c_str());
	JSSetProperty(msg, "sender", Utf82Unicode(param->msg.sender).c_str());
	JSSetProperty(msg, "senderNickName", Utf82Unicode(param->msg.senderNickName).c_str());
	JSSetProperty(msg, "sessionId", Utf82Unicode(param->msg.sessionId).c_str());
	JSSetProperty(msg, "sessionType", param->msg.sessionType);
	JSSetProperty(msg, "userData", Utf82Unicode(param->msg.userData).c_str());

	LOG4CPLUS_DEBUG(log, "Fire_onDownloadFileComplete");
	this->Fire_onDownloadFileComplete(param->matchKey, param->reason, msg);
	delete param;
	return S_OK;
}
LRESULT CATLECSDK::onRateOfProgressAttach(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConRateOfProgressAttach * param = reinterpret_cast<EConRateOfProgressAttach*>(lParam);
	IDispatch * msg = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &msg);
	JSSetProperty(msg, "dateCreated", Utf82Unicode(param->msg.dateCreated).c_str());
	JSSetProperty(msg, "fileName", Utf82Unicode(param->msg.fileName).c_str());
	JSSetProperty(msg, "fileSize", param->msg.fileSize);
	JSSetProperty(msg, "fileUrl", Utf82Unicode(param->msg.fileUrl).c_str());
	JSSetProperty(msg, "msgContent", Utf82Unicode(param->msg.msgContent).c_str());
	JSSetProperty(msg, "msgId", Utf82Unicode(param->msg.msgId).c_str());
	JSSetProperty(msg, "msgType", param->msg.msgType);
	JSSetProperty(msg, "offset", param->msg.offset);
	JSSetProperty(msg, "receiver", Utf82Unicode(param->msg.receiver).c_str());
	JSSetProperty(msg, "sender", Utf82Unicode(param->msg.sender).c_str());
	JSSetProperty(msg, "senderNickName", Utf82Unicode(param->msg.senderNickName).c_str());
	JSSetProperty(msg, "sessionId", Utf82Unicode(param->msg.sessionId).c_str());
	JSSetProperty(msg, "sessionType", param->msg.sessionType);
	JSSetProperty(msg, "userData", Utf82Unicode(param->msg.userData).c_str());

	LOG4CPLUS_DEBUG(log, "Fire_onRateOfProgressAttach");
	this->Fire_onRateOfProgressAttach(param->matchKey, param->rateSize, param->fileSize, msg);
	delete param;
	return S_OK;
}
LRESULT CATLECSDK::onSendTextMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConSendTextMessage * param = reinterpret_cast<EConSendTextMessage*>(lParam);
	IDispatch * msg = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &msg);
	JSSetProperty(msg, "dateCreated", Utf82Unicode(param->msg.dateCreated).c_str());
	JSSetProperty(msg, "fileName", Utf82Unicode(param->msg.fileName).c_str());
	JSSetProperty(msg, "fileSize", param->msg.fileSize);
	JSSetProperty(msg, "fileUrl", Utf82Unicode(param->msg.fileUrl).c_str());
	JSSetProperty(msg, "msgContent", Utf82Unicode(param->msg.msgContent).c_str());
	JSSetProperty(msg, "msgId", Utf82Unicode(param->msg.msgId).c_str());
	JSSetProperty(msg, "msgType", param->msg.msgType);
	JSSetProperty(msg, "offset", param->msg.offset);
	JSSetProperty(msg, "receiver", Utf82Unicode(param->msg.receiver).c_str());
	JSSetProperty(msg, "sender", Utf82Unicode(param->msg.sender).c_str());
	JSSetProperty(msg, "senderNickName", Utf82Unicode(param->msg.senderNickName).c_str());
	JSSetProperty(msg, "sessionId", Utf82Unicode(param->msg.sessionId).c_str());
	JSSetProperty(msg, "sessionType", param->msg.sessionType);
	JSSetProperty(msg, "userData", Utf82Unicode(param->msg.userData).c_str());

	LOG4CPLUS_DEBUG(log, "Fire_onSendTextMessage");
	this->Fire_onSendTextMessage(param->matchKey, param->reason, msg);
	delete param;
	return S_OK;

}
LRESULT CATLECSDK::onSendMediaFile(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConSendMediaFile * param = reinterpret_cast<EConSendMediaFile*>(lParam);
	IDispatch * msg = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &msg);
	JSSetProperty(msg, "dateCreated", Utf82Unicode(param->msg.dateCreated).c_str());
	JSSetProperty(msg, "fileName", Utf82Unicode(param->msg.fileName).c_str());
	JSSetProperty(msg, "fileSize", param->msg.fileSize);
	JSSetProperty(msg, "fileUrl", Utf82Unicode(param->msg.fileUrl).c_str());
	JSSetProperty(msg, "msgContent", Utf82Unicode(param->msg.msgContent).c_str());
	JSSetProperty(msg, "msgId", Utf82Unicode(param->msg.msgId).c_str());
	JSSetProperty(msg, "msgType", param->msg.msgType);
	JSSetProperty(msg, "offset", param->msg.offset);
	JSSetProperty(msg, "receiver", Utf82Unicode(param->msg.receiver).c_str());
	JSSetProperty(msg, "sender", Utf82Unicode(param->msg.sender).c_str());
	JSSetProperty(msg, "senderNickName", Utf82Unicode(param->msg.senderNickName).c_str());
	JSSetProperty(msg, "sessionId", Utf82Unicode(param->msg.sessionId).c_str());
	JSSetProperty(msg, "sessionType", param->msg.sessionType);
	JSSetProperty(msg, "userData", Utf82Unicode(param->msg.userData).c_str());

	LOG4CPLUS_DEBUG(log, "Fire_onSendMediaFile");
	this->Fire_onSendMediaFile(param->matchKey, param->reason, msg);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onDeleteMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConDeleteMessage * param = reinterpret_cast<EConDeleteMessage*>(lParam);

	LOG4CPLUS_DEBUG(log, "Fire_onDeleteMessage");
	this->Fire_onDeleteMessage(param->matchKey, param->reason, param->type,
		SysAllocString(Utf82Unicode(param->msgId).c_str()));
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onOperateMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConOperateMessage * param = reinterpret_cast<EConOperateMessage*>(lParam);

	LOG4CPLUS_DEBUG(log, "Fire_onOperateMessage");
	this->Fire_onOperateMessage(param->tcpMsgIdOut, param->reason);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onReceiveOpreateNoticeMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConReceiveOpreateNoticeMessage * param = reinterpret_cast<EConReceiveOpreateNoticeMessage*>(lParam);

	IDispatch * JSOBJECT = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &JSOBJECT);
	JSSetProperty(JSOBJECT, "notice", param->notice);
	
	IDispatch * msg = nullptr;
	JSNewObject(m_Script, &msg);
	switch (param->notice)
	{
	case NTDeleteMessage: 
		JSSetProperty(msg, "msgContent", Utf82Unicode(param->DeleteMsg.msgContent).c_str());
		JSSetProperty(msg, "msgId", Utf82Unicode(param->DeleteMsg.msgId).c_str());
		JSSetProperty(msg, "receiver", Utf82Unicode(param->DeleteMsg.receiver).c_str());
		JSSetProperty(msg, "sender", Utf82Unicode(param->DeleteMsg.sender).c_str());
		JSSetProperty(msg, "senderNickName", Utf82Unicode(param->DeleteMsg.senderNickName).c_str());
		JSSetProperty(msg, "sessionId", Utf82Unicode(param->DeleteMsg.sessionId).c_str());
		JSSetProperty(msg, "userData", Utf82Unicode(param->DeleteMsg.userData).c_str());
		break;
	case NTReadMessage: 
		JSSetProperty(msg, "msgContent", Utf82Unicode(param->ReadMsg.msgContent).c_str());
		JSSetProperty(msg, "msgId", Utf82Unicode(param->ReadMsg.msgId).c_str());
		JSSetProperty(msg, "receiver", Utf82Unicode(param->ReadMsg.receiver).c_str());
		JSSetProperty(msg, "sender", Utf82Unicode(param->ReadMsg.sender).c_str());
		JSSetProperty(msg, "senderNickName", Utf82Unicode(param->ReadMsg.senderNickName).c_str());
		JSSetProperty(msg, "sessionId", Utf82Unicode(param->ReadMsg.sessionId).c_str());
		JSSetProperty(msg, "userData", Utf82Unicode(param->ReadMsg.userData).c_str());
		break;
	case NTWithdrawMessage: 
		JSSetProperty(msg, "msgContent", Utf82Unicode(param->WithDrawMessage.msgContent).c_str());
		JSSetProperty(msg, "msgId", Utf82Unicode(param->WithDrawMessage.msgId).c_str());
		JSSetProperty(msg, "receiver", Utf82Unicode(param->WithDrawMessage.receiver).c_str());
		JSSetProperty(msg, "sender", Utf82Unicode(param->WithDrawMessage.sender).c_str());
		JSSetProperty(msg, "senderNickName", Utf82Unicode(param->WithDrawMessage.senderNickName).c_str());
		JSSetProperty(msg, "sessionId", Utf82Unicode(param->WithDrawMessage.sessionId).c_str());
		JSSetProperty(msg, "userData", Utf82Unicode(param->WithDrawMessage.userData).c_str());
		break;
	default:
		break;
	}

	JSSetProperty(JSOBJECT, "msg", msg);
	LOG4CPLUS_DEBUG(log, "Fire_onReceiveOpreateNoticeMessage");
	this->Fire_onReceiveOpreateNoticeMessage(JSOBJECT);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onUploadVTMFileOrBuf(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConUploadVTMFileOrBuf * param = reinterpret_cast<EConUploadVTMFileOrBuf*>(lParam);

	LOG4CPLUS_DEBUG(log, "Fire_onUploadVTMFileOrBuf");
	this->Fire_onUploadVTMFileOrBuf(param->matchKey, param->reason,
		SysAllocString(Utf82Unicode(param->fileUrl).c_str()));
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onCreateGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConCreateGroup * param = reinterpret_cast<EConCreateGroup*>(lParam);
	IDispatch * msg = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &msg);
	JSSetProperty(msg, "city", Utf82Unicode(param->info.city).c_str());
	JSSetProperty(msg, "dateCreated", Utf82Unicode(param->info.dateCreated).c_str());
	JSSetProperty(msg, "declared", Utf82Unicode(param->info.declared).c_str());
	JSSetProperty(msg, "discuss", param->info.discuss);
	JSSetProperty(msg, "groupDomain", Utf82Unicode(param->info.groupDomain).c_str());
	JSSetProperty(msg, "groupId", Utf82Unicode(param->info.groupId).c_str());
	JSSetProperty(msg, "isNotice", param->info.isNotice);
	JSSetProperty(msg, "memberCount", param->info.memberCount);
	JSSetProperty(msg, "name", Utf82Unicode(param->info.name).c_str());
	JSSetProperty(msg, "owner", Utf82Unicode(param->info.owner).c_str());
	JSSetProperty(msg, "permission", param->info.permission);
	JSSetProperty(msg, "province", Utf82Unicode(param->info.province).c_str());
	JSSetProperty(msg, "scope", param->info.scope);
	JSSetProperty(msg, "type", param->info.type);

	LOG4CPLUS_DEBUG(log, "Fire_onCreateGroup");
	this->Fire_onCreateGroup(param->matchKey, param->reason, msg);
	delete param;
	return S_OK;
}
LRESULT CATLECSDK::onDismissGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConDismissGroup * param = reinterpret_cast<EConDismissGroup*>(lParam);

	LOG4CPLUS_DEBUG(log, "Fire_onDismissGroup");
	this->Fire_onDismissGroup(param->matchKey, param->reason, ::SysAllocString(Utf82Unicode(param->groupid).c_str()));
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onQuitGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConQuitGroup * param = reinterpret_cast<EConQuitGroup*>(lParam);

	LOG4CPLUS_DEBUG(log, "Fire_onQuitGroup");
	this->Fire_onQuitGroup(param->matchKey, param->reason, ::SysAllocString(Utf82Unicode(param->groupid).c_str()));
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onJoinGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConJoinGroup * param = reinterpret_cast<EConJoinGroup*>(lParam);

	LOG4CPLUS_DEBUG(log, "Fire_onJoinGroup");
	this->Fire_onJoinGroup(param->matchKey, param->reason, ::SysAllocString(Utf82Unicode(param->groupid).c_str()));
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onReplyRequestJoinGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConReplyRequestJoinGroup * param = reinterpret_cast<EConReplyRequestJoinGroup*>(lParam);

	LOG4CPLUS_DEBUG(log, "Fire_onReplyRequestJoinGroup");
	this->Fire_onReplyRequestJoinGroup(param->matchKey, param->reason, ::SysAllocString(Utf82Unicode(param->groupid).c_str()),
		::SysAllocString(Utf82Unicode(param->member).c_str()) ,param->confirm);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onInviteJoinGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConInviteJoinGroup * param = reinterpret_cast<EConInviteJoinGroup*>(lParam);
	IDispatch * members = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewArray(m_Script, &members);
	for (auto it:param->members)
	{
		JSAddArrayElement(members, it.c_str());
	}

	LOG4CPLUS_DEBUG(log, "Fire_onInviteJoinGroup");
	this->Fire_onInviteJoinGroup(param->matchKey, param->reason, ::SysAllocString(Utf82Unicode(param->groupid).c_str()), members, param->confirm);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onReplyInviteJoinGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConReplyInviteJoinGroup * param = reinterpret_cast<EConReplyInviteJoinGroup*>(lParam);

	LOG4CPLUS_DEBUG(log, "Fire_onReplyInviteJoinGroup");
	this->Fire_onReplyInviteJoinGroup(param->matchKey, param->reason, ::SysAllocString(Utf82Unicode(param->groupid).c_str()),
		::SysAllocString(Utf82Unicode(param->member).c_str()), param->confirm);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onQueryOwnGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConQueryOwnGroup * param = reinterpret_cast<EConQueryOwnGroup*>(lParam);
	IDispatch * groups = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewArray(m_Script, &groups);

	for (auto it : param->group)
	{
		IDispatch * group = nullptr;
		JSNewObject(m_Script, &group);
		JSSetProperty(group, "discuss", it.discuss);
		JSSetProperty(group, "groupId", Utf82Unicode(it.groupId).c_str());
		JSSetProperty(group, "isNotice", it.isNotice);
		JSSetProperty(group, "memberCount", it.memberCount);
		JSSetProperty(group, "name", Utf82Unicode(it.name).c_str());
		JSSetProperty(group, "owner", Utf82Unicode(it.owner).c_str());
		JSSetProperty(group, "permission", it.permission);
		JSSetProperty(group, "scope", it.scope);

		JSAddArrayElement(groups, group);
	}

	LOG4CPLUS_DEBUG(log, "Fire_onQueryOwnGroup");
	this->Fire_onQueryOwnGroup(param->matchKey, param->reason, groups);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onQueryGroupDetail(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConQueryGroupDetail * param = reinterpret_cast<EConQueryGroupDetail*>(lParam);
	IDispatch * detail = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &detail);
	JSSetProperty(detail, "city", Utf82Unicode(param->detail.city).c_str());
	JSSetProperty(detail, "dateCreated", Utf82Unicode(param->detail.dateCreated).c_str());
	JSSetProperty(detail, "declared", Utf82Unicode(param->detail.declared).c_str());
	JSSetProperty(detail, "discuss", param->detail.discuss);
	JSSetProperty(detail, "groupDomain", Utf82Unicode(param->detail.groupDomain).c_str());
	JSSetProperty(detail, "groupId", Utf82Unicode(param->detail.groupId).c_str());
	JSSetProperty(detail, "isNotice", param->detail.isNotice);
	JSSetProperty(detail, "memberCount", param->detail.memberCount);
	JSSetProperty(detail, "name", Utf82Unicode(param->detail.name).c_str());
	JSSetProperty(detail, "owner", Utf82Unicode(param->detail.owner).c_str());
	JSSetProperty(detail, "permission", param->detail.permission);
	JSSetProperty(detail, "province", Utf82Unicode(param->detail.province).c_str());
	JSSetProperty(detail, "scope", param->detail.scope);
	JSSetProperty(detail, "type", param->detail.type);

	LOG4CPLUS_DEBUG(log, "Fire_onQueryGroupDetail");
	this->Fire_onQueryGroupDetail(param->matchKey, param->reason, detail);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onModifyGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConModifyGroup * param = reinterpret_cast<EConModifyGroup*>(lParam);
	IDispatch * detail = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &detail);
	JSSetProperty(detail, "city", Utf82Unicode(param->info.city).c_str());
	JSSetProperty(detail, "declared", Utf82Unicode(param->info.declared).c_str());
	JSSetProperty(detail, "discuss", param->info.discuss);
	JSSetProperty(detail, "groupDomain", Utf82Unicode(param->info.groupDomain).c_str());
	JSSetProperty(detail, "groupId", Utf82Unicode(param->info.groupId).c_str());
	JSSetProperty(detail, "name", Utf82Unicode(param->info.name).c_str());
	JSSetProperty(detail, "permission", param->info.permission);
	JSSetProperty(detail, "province", Utf82Unicode(param->info.province).c_str());
	JSSetProperty(detail, "scope", param->info.scope);
	JSSetProperty(detail, "type", param->info.type);

	LOG4CPLUS_DEBUG(log, "Fire_onModifyGroup");
	this->Fire_onModifyGroup(param->matchKey, param->reason, detail);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onSearchPublicGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConSearchPublicGroup * param = reinterpret_cast<EConSearchPublicGroup*>(lParam);
	IDispatch * groups = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewArray(m_Script, &groups);

	for (auto it : param->group)
	{
		IDispatch * group = nullptr;
		JSNewObject(m_Script, &group);
		JSSetProperty(group, "declared", Utf82Unicode(it.declared).c_str());
		JSSetProperty(group, "discuss", it.discuss);
		JSSetProperty(group, "groupId", Utf82Unicode(it.groupId).c_str());
		JSSetProperty(group, "memberCount", it.memberCount);
		JSSetProperty(group, "name", Utf82Unicode(it.name).c_str());
		JSSetProperty(group, "owner", Utf82Unicode(it.owner).c_str());
		JSSetProperty(group, "permission", it.permission);
		JSSetProperty(group, "scope", it.scope);

		JSAddArrayElement(groups, group);
	}

	LOG4CPLUS_DEBUG(log, "Fire_onSearchPublicGroup");
	this->Fire_onSearchPublicGroup(param->matchKey, param->reason, param->searchType,
		SysAllocString(Utf82Unicode(param->keyword).c_str()), groups, param->pageNo);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onQueryGroupMember(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConQueryGroupMember * param = reinterpret_cast<EConQueryGroupMember*>(lParam);
	IDispatch * groups = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewArray(m_Script, &groups);

	for (auto it : param->members)
	{
		IDispatch * group = nullptr;
		JSNewObject(m_Script, &group);
		JSSetProperty(group, "member", Utf82Unicode(it.member).c_str());
		JSSetProperty(group, "nickName", Utf82Unicode(it.nickName).c_str());
		JSSetProperty(group, "role", it.role);
		JSSetProperty(group, "sex", it.sex);
		JSSetProperty(group, "speakState", it.speakState);

		JSAddArrayElement(groups, group);
	}

	LOG4CPLUS_DEBUG(log, "Fire_onQueryGroupMember");
	this->Fire_onQueryGroupMember(param->matchKey, param->reason,
		SysAllocString(Utf82Unicode(param->groupid).c_str()), groups);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onDeleteGroupMember(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConDeleteGroupMember * param = reinterpret_cast<EConDeleteGroupMember*>(lParam);

	LOG4CPLUS_DEBUG(log, "Fire_onDeleteGroupMember");
	this->Fire_onDeleteGroupMember(param->matchKey, param->reason,
		SysAllocString(Utf82Unicode(param->groupid).c_str()),
		SysAllocString(Utf82Unicode(param->member).c_str()));
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onQueryGroupMemberCard(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConQueryGroupMemberCard * param = reinterpret_cast<EConQueryGroupMemberCard*>(lParam);
	IDispatch * card = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &card);
	JSSetProperty(card, "city", Utf82Unicode(param->card.display).c_str());
	JSSetProperty(card, "declared", Utf82Unicode(param->card.groupId).c_str());
	JSSetProperty(card, "discuss", Utf82Unicode(param->card.mail).c_str());
	JSSetProperty(card, "groupDomain", Utf82Unicode(param->card.member).c_str());
	JSSetProperty(card, "groupId", Utf82Unicode(param->card.phone).c_str());
	JSSetProperty(card, "name", Utf82Unicode(param->card.remark).c_str());
	JSSetProperty(card, "permission", param->card.role);
	JSSetProperty(card, "province", param->card.sex);
	JSSetProperty(card, "scope", param->card.speakState);

	LOG4CPLUS_DEBUG(log, "Fire_onQueryGroupMemberCard");
	this->Fire_onQueryGroupMemberCard(param->matchKey, param->reason, card);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onModifyGroupMemberCard(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConModifyGroupMemberCard * param = reinterpret_cast<EConModifyGroupMemberCard*>(lParam);
	IDispatch * card = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &card);
	JSSetProperty(card, "city", Utf82Unicode(param->card.display).c_str());
	JSSetProperty(card, "declared", Utf82Unicode(param->card.groupId).c_str());
	JSSetProperty(card, "discuss", Utf82Unicode(param->card.mail).c_str());
	JSSetProperty(card, "groupDomain", Utf82Unicode(param->card.member).c_str());
	JSSetProperty(card, "groupId", Utf82Unicode(param->card.phone).c_str());
	JSSetProperty(card, "name", Utf82Unicode(param->card.remark).c_str());
	JSSetProperty(card, "permission", param->card.role);
	JSSetProperty(card, "province", param->card.sex);
	JSSetProperty(card, "scope", param->card.speakState);

	LOG4CPLUS_DEBUG(log, "Fire_onModifyGroupMemberCard");
	this->Fire_onModifyGroupMemberCard(param->matchKey, param->reason, card);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onForbidMemberSpeakGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConForbidMemberSpeakGroup * param = reinterpret_cast<EConForbidMemberSpeakGroup*>(lParam);

	LOG4CPLUS_DEBUG(log, "Fire_onForbidMemberSpeakGroup");
	this->Fire_onForbidMemberSpeakGroup(param->matchKey, param->reason, 
		SysAllocString(Utf82Unicode(param->groupid).c_str()),
		SysAllocString(Utf82Unicode(param->member).c_str()),
		param->isBan);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onSetGroupMessageRule(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConSetGroupMessageRule * param = reinterpret_cast<EConSetGroupMessageRule*>(lParam);

	LOG4CPLUS_DEBUG(log, "Fire_onSetGroupMessageRule");
	this->Fire_onSetGroupMessageRule(param->matchKey, param->reason,
		SysAllocString(Utf82Unicode(param->groupid).c_str()),
		param->notice);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onReceiveGroupNoticeMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{

	EConReceiveGroupNoticeMessage * param = reinterpret_cast<EConReceiveGroupNoticeMessage*>(lParam);

	IDispatch * notice = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &notice);
	JSSetProperty(notice, "notice", param->notice);

	IDispatch * msg = nullptr;
	JSNewObject(m_Script, &msg);

	switch (param->notice)
	{
	case NTRequestJoinGroup:
		JSSetProperty(msg, "confirm", param->requestMsg.confirm);
		JSSetProperty(msg, "dateCreated", Utf82Unicode(param->requestMsg.dateCreated).c_str());
		JSSetProperty(msg, "declared", Utf82Unicode(param->requestMsg.declared).c_str());
		JSSetProperty(msg, "groupId", Utf82Unicode(param->requestMsg.groupId).c_str());
		JSSetProperty(msg, "groupName", Utf82Unicode(param->requestMsg.groupName).c_str());
		JSSetProperty(msg, "nickName", Utf82Unicode(param->requestMsg.nickName).c_str());
		JSSetProperty(msg, "proposer", Utf82Unicode(param->requestMsg.proposer).c_str());
		JSSetProperty(msg, "sender", Utf82Unicode(param->requestMsg.sender).c_str());
		break;
	case NTInviteJoinGroup:
		JSSetProperty(msg, "admin", Utf82Unicode(param->inviteMsg.admin).c_str());
		JSSetProperty(msg, "confirm", param->inviteMsg.confirm);
		JSSetProperty(msg, "dateCreated", Utf82Unicode(param->inviteMsg.dateCreated).c_str());
		JSSetProperty(msg, "declared", Utf82Unicode(param->inviteMsg.declared).c_str());
		JSSetProperty(msg, "groupId", Utf82Unicode(param->inviteMsg.groupId).c_str());
		JSSetProperty(msg, "groupName", Utf82Unicode(param->inviteMsg.groupName).c_str());
		JSSetProperty(msg, "nickName", Utf82Unicode(param->inviteMsg.nickName).c_str());
		JSSetProperty(msg, "sender", Utf82Unicode(param->inviteMsg.sender).c_str());
		break;
	case NTMemberJoinedGroup:
		JSSetProperty(msg, "dateCreated", Utf82Unicode(param->memberMsg.dateCreated).c_str());
		JSSetProperty(msg, "declared", Utf82Unicode(param->memberMsg.declared).c_str());
		JSSetProperty(msg, "groupId", Utf82Unicode(param->memberMsg.groupId).c_str());
		JSSetProperty(msg, "groupName", Utf82Unicode(param->memberMsg.groupName).c_str());
		JSSetProperty(msg, "member", Utf82Unicode(param->memberMsg.member).c_str());
		JSSetProperty(msg, "nickName", Utf82Unicode(param->memberMsg.nickName).c_str());
		JSSetProperty(msg, "sender", Utf82Unicode(param->memberMsg.sender).c_str());
		break;
	case NTDismissGroup:
		JSSetProperty(msg, "dateCreated", Utf82Unicode(param->dismissMsg.dateCreated).c_str());
		JSSetProperty(msg, "groupId", Utf82Unicode(param->dismissMsg.groupId).c_str());
		JSSetProperty(msg, "groupName", Utf82Unicode(param->dismissMsg.groupName).c_str());
		JSSetProperty(msg, "sender", Utf82Unicode(param->dismissMsg.sender).c_str());
		break;
	case NTQuitGroup:
		JSSetProperty(msg, "dateCreated", Utf82Unicode(param->quitMsg.dateCreated).c_str());
		JSSetProperty(msg, "groupId", Utf82Unicode(param->quitMsg.groupId).c_str());
		JSSetProperty(msg, "groupName", Utf82Unicode(param->quitMsg.groupName).c_str());
		JSSetProperty(msg, "member", Utf82Unicode(param->quitMsg.member).c_str());
		JSSetProperty(msg, "nickName", Utf82Unicode(param->quitMsg.nickName).c_str());
		JSSetProperty(msg, "sender", Utf82Unicode(param->quitMsg.sender).c_str());
		break;
	case NTRemoveGroupMember:
		JSSetProperty(msg, "dateCreated", Utf82Unicode(param->removeMsg.dateCreated).c_str());
		JSSetProperty(msg, "groupId", Utf82Unicode(param->removeMsg.groupId).c_str());
		JSSetProperty(msg, "groupName", Utf82Unicode(param->removeMsg.groupName).c_str());
		JSSetProperty(msg, "member", Utf82Unicode(param->removeMsg.member).c_str());
		JSSetProperty(msg, "nickName", Utf82Unicode(param->removeMsg.nickName).c_str());
		JSSetProperty(msg, "sender", Utf82Unicode(param->removeMsg.sender).c_str());
		break;
	case NTReplyRequestJoinGroup:
		JSSetProperty(msg, "admin", Utf82Unicode(param->replyrequestMsg.admin).c_str());
		JSSetProperty(msg, "confirm", param->replyrequestMsg.confirm);
		JSSetProperty(msg, "dateCreated", Utf82Unicode(param->replyrequestMsg.dateCreated).c_str());
		JSSetProperty(msg, "groupId", Utf82Unicode(param->replyrequestMsg.groupId).c_str());
		JSSetProperty(msg, "groupName", Utf82Unicode(param->replyrequestMsg.groupName).c_str());
		JSSetProperty(msg, "member", Utf82Unicode(param->replyrequestMsg.member).c_str());
		JSSetProperty(msg, "nickName", Utf82Unicode(param->replyrequestMsg.nickName).c_str());
		JSSetProperty(msg, "sender", Utf82Unicode(param->replyrequestMsg.sender).c_str());
		break;
	case NTReplyInviteJoinGroup:
		JSSetProperty(msg, "admin", Utf82Unicode(param->replyinviteMsg.admin).c_str());
		JSSetProperty(msg, "confirm", param->replyinviteMsg.confirm);
		JSSetProperty(msg, "dateCreated", Utf82Unicode(param->replyinviteMsg.dateCreated).c_str());
		JSSetProperty(msg, "groupId", Utf82Unicode(param->replyinviteMsg.groupId).c_str());
		JSSetProperty(msg, "groupName", Utf82Unicode(param->replyinviteMsg.groupName).c_str());
		JSSetProperty(msg, "member", Utf82Unicode(param->replyinviteMsg.member).c_str());
		JSSetProperty(msg, "nickName", Utf82Unicode(param->replyinviteMsg.nickName).c_str());
		JSSetProperty(msg, "sender", Utf82Unicode(param->replyinviteMsg.sender).c_str());
		break;
	case NTModifyGroup:
		JSSetProperty(msg, "dateCreated", Utf82Unicode(param->modifyMsg.dateCreated).c_str());
		JSSetProperty(msg, "groupId", Utf82Unicode(param->modifyMsg.groupId).c_str());
		JSSetProperty(msg, "groupName", Utf82Unicode(param->modifyMsg.groupName).c_str());
		JSSetProperty(msg, "member", Utf82Unicode(param->modifyMsg.member).c_str());
		JSSetProperty(msg, "modifyDic", Utf82Unicode(param->modifyMsg.modifyDic).c_str());
		JSSetProperty(msg, "sender", Utf82Unicode(param->modifyMsg.sender).c_str());
		break;
	default:
		break;
	}
	JSSetProperty(notice, "NoticeMsg", msg);

	LOG4CPLUS_DEBUG(log, "Fire_onReceiveGroupNoticeMessage");
	this->Fire_onReceiveGroupNoticeMessage(notice);

	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onRecordingTimeOut(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConRecordingTimeOut * param = reinterpret_cast<EConRecordingTimeOut*>(lParam);

	LOG4CPLUS_DEBUG(log, "Fire_onRecordingTimeOut");
	this->Fire_onRecordingTimeOut(param->ms,
		SysAllocString(Utf82Unicode(param->sessionId).c_str()));
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onFinishedPlaying(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConFinishedPlaying * param = reinterpret_cast<EConFinishedPlaying*>(lParam);

	LOG4CPLUS_DEBUG(log, "Fire_onFinishedPlaying");
	this->Fire_onFinishedPlaying(SysAllocString(Utf82Unicode(param->sessionId).c_str()),
		param->reason);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onRecordingAmplitude(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConRecordingAmplitude * param = reinterpret_cast<EConRecordingAmplitude*>(lParam);

	LOG4CPLUS_DEBUG(log, "Fire_onRecordingAmplitude");
	this->Fire_onRecordingAmplitude(param->amplitude,
		SysAllocString(Utf82Unicode(param->sessionId).c_str()));
	delete param;
	return S_OK;
}


LRESULT CATLECSDK::onMeetingIncoming(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	ECMeetingComingInfo * param = reinterpret_cast<ECMeetingComingInfo*>(lParam);
	IDispatch * come = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &come);
	JSSetProperty(come, "caller", Utf82Unicode(param->caller).c_str());
	JSSetProperty(come, "callid", Utf82Unicode(param->callid).c_str());
	JSSetProperty(come, "display", Utf82Unicode(param->display).c_str());
	JSSetProperty(come, "Id", Utf82Unicode(param->Id).c_str());
	JSSetProperty(come, "meetingType", param->meetingType);
	JSSetProperty(come, "nickname", Utf82Unicode(param->nickname).c_str());
    JSSetProperty(come, "sdkUserData", Utf82Unicode(param->sdkUserData).c_str());

	LOG4CPLUS_DEBUG(log, "Fire_onMeetingIncoming");
	this->Fire_onMeetingIncoming(come);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onReceiveInterphoneMeetingMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	ECInterphoneMsg * param = reinterpret_cast<ECInterphoneMsg*>(lParam);
	IDispatch * msg = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &msg);
	JSSetProperty(msg, "datecreated", Utf82Unicode(param->datecreated).c_str());
	JSSetProperty(msg, "from", Utf82Unicode(param->from).c_str());
	JSSetProperty(msg, "interphoneid", Utf82Unicode(param->interphoneid).c_str());
	JSSetProperty(msg, "member", Utf82Unicode(param->member).c_str());
	JSSetProperty(msg, "receiver", Utf82Unicode(param->receiver).c_str());
	JSSetProperty(msg, "state", Utf82Unicode(param->state).c_str());
	JSSetProperty(msg, "type", param->type);
	JSSetProperty(msg, "userdata", Utf82Unicode(param->userdata).c_str());
	JSSetProperty(msg, "var", param->var);

	LOG4CPLUS_DEBUG(log, "Fire_onReceiveInterphoneMeetingMessage");
	this->Fire_onReceiveInterphoneMeetingMessage(msg);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onReceiveVoiceMeetingMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	ECVoiceMeetingMsg * param = reinterpret_cast<ECVoiceMeetingMsg*>(lParam);
	IDispatch * msg = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &msg);
	JSSetProperty(msg, "chatroomid", Utf82Unicode(param->chatroomid).c_str());
	JSSetProperty(msg, "forbid", Utf82Unicode(param->forbid).c_str());
	JSSetProperty(msg, "isVoIP", param->isVoIP);
	JSSetProperty(msg, "member", Utf82Unicode(param->member).c_str());
	JSSetProperty(msg, "receiver", Utf82Unicode(param->receiver).c_str());
	JSSetProperty(msg, "sender", Utf82Unicode(param->sender).c_str());
	JSSetProperty(msg, "state", Utf82Unicode(param->state).c_str());
	JSSetProperty(msg, "type", param->type);
	JSSetProperty(msg, "userdata", Utf82Unicode(param->userdata).c_str());
	JSSetProperty(msg, "var", param->var);

	LOG4CPLUS_DEBUG(log, "Fire_onReceiveVoiceMeetingMessage");
	this->Fire_onReceiveVoiceMeetingMessage(msg);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onReceiveVideoMeetingMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	ECVideoMeetingMsg * param = reinterpret_cast<ECVideoMeetingMsg*>(lParam);
	IDispatch * msg = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &msg);
	JSSetProperty(msg, "dataState", param->dataState);
	JSSetProperty(msg, "forbid", Utf82Unicode(param->forbid).c_str());
	JSSetProperty(msg, "isVoIP", param->isVoIP);
	JSSetProperty(msg, "member", Utf82Unicode(param->member).c_str());
	JSSetProperty(msg, "receiver", Utf82Unicode(param->receiver).c_str());
	JSSetProperty(msg, "roomid", Utf82Unicode(param->roomid).c_str());
	JSSetProperty(msg, "sender", Utf82Unicode(param->sender).c_str());
	JSSetProperty(msg, "state", Utf82Unicode(param->state).c_str());
	JSSetProperty(msg, "type", param->type);
	JSSetProperty(msg, "userdata", Utf82Unicode(param->userdata).c_str());
	JSSetProperty(msg, "var", param->var);
	JSSetProperty(msg, "videoState", param->videoState);

	LOG4CPLUS_DEBUG(log, "Fire_onReceiveVideoMeetingMessage");
	this->Fire_onReceiveVideoMeetingMessage(msg);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onCreateMultimediaMeeting(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConCreateMultimediaMeeting * param = reinterpret_cast<EConCreateMultimediaMeeting*>(lParam);
	IDispatch * info = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &info);
	JSSetProperty(info, "confirm", param->info.confirm);
	JSSetProperty(info, "creator", Utf82Unicode(param->info.creator).c_str());
	JSSetProperty(info, "Id", Utf82Unicode(param->info.Id).c_str());
	JSSetProperty(info, "joinedCount", param->info.joinedCount);
	JSSetProperty(info, "keywords", Utf82Unicode(param->info.keywords).c_str());
	JSSetProperty(info, "meetingType", param->info.meetingType);
	JSSetProperty(info, "name", Utf82Unicode(param->info.name).c_str());
	JSSetProperty(info, "square", param->info.square);

	LOG4CPLUS_DEBUG(log, "Fire_onCreateMultimediaMeeting");
	this->Fire_onCreateMultimediaMeeting(param->matchKey, param->reason, info);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onJoinMeeting(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConJoinMeeting * param = reinterpret_cast<EConJoinMeeting*>(lParam);

	LOG4CPLUS_DEBUG(log, "Fire_onJoinMeeting");
	BSTR conferenceId = SysAllocString(Utf82Unicode(param->conferenceId).c_str());
	this->Fire_onJoinMeeting(param->reason,conferenceId);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onExitMeeting(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	std::string * param = reinterpret_cast<std::string*>(lParam);

	LOG4CPLUS_DEBUG(log, "Fire_onExitMeeting");
	BSTR conferenceId = SysAllocString(Utf82Unicode(*param).c_str());
	this->Fire_onExitMeeting(conferenceId);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onQueryMeetingMembers(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConQueryMeetingMembers * param = reinterpret_cast<EConQueryMeetingMembers*>(lParam);
	IDispatch * members = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewArray(m_Script, &members);
	for (auto it : param->Members)
	{
		IDispatch * info = nullptr;
		JSNewObject(m_Script, &info);
		JSSetProperty(info, "dataState", it.dataState);
		JSSetProperty(info, "forbid", Utf82Unicode(it.forbid).c_str());
		JSSetProperty(info, "isMic", it.isMic);
		JSSetProperty(info, "isOnline", it.isOnline);
		JSSetProperty(info, "isVoIP", it.isVoIP);
		JSSetProperty(info, "meetingType", it.meetingType);
		JSSetProperty(info, "member", Utf82Unicode(it.member).c_str());
		JSSetProperty(info, "state", Utf82Unicode(it.state).c_str());
		JSSetProperty(info, "type", it.type);
		JSSetProperty(info, "userdata", Utf82Unicode(it.userdata).c_str());
		JSSetProperty(info, "videoState", it.videoState);

		JSAddArrayElement(members, info);
	}

	BSTR conferenceId = SysAllocString(Utf82Unicode(param->conferenceId).c_str());
	LOG4CPLUS_DEBUG(log, "Fire_onQueryMeetingMembers");
	this->Fire_onQueryMeetingMembers(param->matchKey,param->reason,conferenceId,members);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onDismissMultiMediaMeeting(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConDismissMultiMediaMeeting * param = reinterpret_cast<EConDismissMultiMediaMeeting*>(lParam);

	BSTR conferenceId = SysAllocString(Utf82Unicode(param->conferenceId).c_str());
	LOG4CPLUS_DEBUG(log, "Fire_onDismissMultiMediaMeeting");
	this->Fire_onDismissMultiMediaMeeting(param->matchKey, param->reason, conferenceId);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onQueryMultiMediaMeetings(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConQueryMultiMediaMeetings * param = reinterpret_cast<EConQueryMultiMediaMeetings*>(lParam);
	IDispatch * infos = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewArray(m_Script, &infos);
	for (auto it : param->infos)
	{
		IDispatch * info = nullptr;
		JSNewObject(m_Script, &info);
		JSSetProperty(info, "confirm", it.confirm);
		JSSetProperty(info, "creator", Utf82Unicode(it.creator).c_str());
		JSSetProperty(info, "Id", Utf82Unicode(it.Id).c_str());
		JSSetProperty(info, "joinedCount", it.joinedCount);
		JSSetProperty(info, "keywords", Utf82Unicode(it.keywords).c_str());
		JSSetProperty(info, "meetingType", it.meetingType);
		JSSetProperty(info, "name", Utf82Unicode(it.name).c_str());
		JSSetProperty(info, "square", it.square);

		JSAddArrayElement(infos, info);
	}

	LOG4CPLUS_DEBUG(log, "Fire_onQueryMultiMediaMeetings");
	this->Fire_onQueryMultiMediaMeetings(param->matchKey, param->reason, infos);
	delete param;
	return S_OK;
}


LRESULT CATLECSDK::onDeleteMemberMultiMediaMeeting(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConDeleteMemberMultiMediaMeeting * param = reinterpret_cast<EConDeleteMemberMultiMediaMeeting*>(lParam);
	IDispatch * info = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &info);
	JSSetProperty(info, "Id", Utf82Unicode(param->info.Id).c_str());
	JSSetProperty(info, "isVoIP", param->info.isVoIP);
	JSSetProperty(info, "meetingType", param->info.meetingType);
	JSSetProperty(info, "member", Utf82Unicode(param->info.member).c_str());

	LOG4CPLUS_DEBUG(log, "Fire_onDeleteMemberMultiMediaMeeting");
	this->Fire_onDeleteMemberMultiMediaMeeting(param->matchKey, param->reason, info);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onInviteJoinMultiMediaMeeting(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConInviteJoinMultiMediaMeeting * param = reinterpret_cast<EConInviteJoinMultiMediaMeeting*>(lParam);
	IDispatch * info = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &info);
	JSSetProperty(info, "Id", Utf82Unicode(param->info.Id).c_str());
	JSSetProperty(info, "isListen", param->info.isListen);
	JSSetProperty(info, "isSpeak", param->info.isSpeak);
	JSSetProperty(info, "userdata", Utf82Unicode(param->info.userdata).c_str());

	IDispatch * members = nullptr;
	JSNewArray(m_Script, &members);
	for (auto it : param->info.members)
	{
		JSAddArrayElement(members, Utf82Unicode(it).c_str());
	}

	JSSetProperty(info, "members", members);

	LOG4CPLUS_DEBUG(log, "Fire_onInviteJoinMultiMediaMeeting");
	this->Fire_onInviteJoinMultiMediaMeeting(param->matchKey, param->reason, info);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onCreateInterphoneMeeting(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConCreateInterphoneMeeting * param = reinterpret_cast<EConCreateInterphoneMeeting*>(lParam);

	BSTR interphoneId = SysAllocString(Utf82Unicode(param->interphoneId).c_str());
	LOG4CPLUS_DEBUG(log, "Fire_onCreateInterphoneMeeting");
	this->Fire_onCreateInterphoneMeeting(param->matchKey, param->reason, interphoneId);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onExitInterphoneMeeting(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	std::string * param = reinterpret_cast<std::string*>(lParam);

	BSTR interphoneId = SysAllocString(Utf82Unicode(*param).c_str());
	LOG4CPLUS_DEBUG(log, "Fire_onExitInterphoneMeeting");
	this->Fire_onExitInterphoneMeeting(interphoneId);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onSetMeetingSpeakListen(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConSetMeetingSpeakListen * param = reinterpret_cast<EConSetMeetingSpeakListen*>(lParam);
	IDispatch * info = nullptr;
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	JSNewObject(m_Script, &info);
	JSSetProperty(info, "Id", Utf82Unicode(param->Info.Id).c_str());
	JSSetProperty(info, "isVoIP", param->Info.isVoIP);
	JSSetProperty(info, "meetingType", param->Info.meetingType);
	JSSetProperty(info, "member", Utf82Unicode(param->Info.member).c_str());
	JSSetProperty(info, "speaklisen", param->Info.speaklisen);

	LOG4CPLUS_DEBUG(log, "Fire_onSetMeetingSpeakListen");
	this->Fire_onSetMeetingSpeakListen(param->matchKey, param->reason, info);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onControlInterphoneMic(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConControlInterphoneMic * param = reinterpret_cast<EConControlInterphoneMic*>(lParam);

	BSTR interphoneId = SysAllocString(Utf82Unicode(param->interphoneId).c_str());
	LOG4CPLUS_DEBUG(log, "Fire_onControlInterphoneMic");
	this->Fire_onControlInterphoneMic(param->matchKey, param->reason,
		SysAllocString(Utf82Unicode(param->controller).c_str()),
		interphoneId,param->requestIsControl);
	delete param;
	return S_OK;
}


LRESULT CATLECSDK::onPublishVideo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConPublishVideo * param = reinterpret_cast<EConPublishVideo*>(lParam);

	BSTR conferenceId = SysAllocString(Utf82Unicode(param->conferenceId).c_str());
	LOG4CPLUS_DEBUG(log, "Fire_onPublishVideo");
	this->Fire_onPublishVideo(param->matchKey, param->reason, conferenceId);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onUnpublishVideo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConUnpublishVideo * param = reinterpret_cast<EConUnpublishVideo*>(lParam);

	BSTR conferenceId = SysAllocString(Utf82Unicode(param->conferenceId).c_str());
	LOG4CPLUS_DEBUG(log, "Fire_onUnpublishVideo");
	this->Fire_onUnpublishVideo(param->matchKey, param->reason, conferenceId);
	delete param;
	return S_OK;
}


LRESULT CATLECSDK::onRequestConferenceMemberVideo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConRequestConferenceMemberVideo * param = reinterpret_cast<EConRequestConferenceMemberVideo*>(lParam);

	BSTR conferenceId = SysAllocString(Utf82Unicode(param->conferenceId).c_str());
	LOG4CPLUS_DEBUG(log, "Fire_onRequestConferenceMemberVideo");
	this->Fire_onRequestConferenceMemberVideo(param->reason, conferenceId,
		SysAllocString(Utf82Unicode(param->member).c_str())
		, param->type);
	delete param;
	return S_OK;
}

LRESULT CATLECSDK::onCancelConferenceMemberVideo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	EConCancelConferenceMemberVideo * param = reinterpret_cast<EConCancelConferenceMemberVideo*>(lParam);

	BSTR conferenceId = SysAllocString(Utf82Unicode(param->conferenceId).c_str());
	LOG4CPLUS_DEBUG(log, "Fire_onCancelConferenceMemberVideo");
	this->Fire_onCancelConferenceMemberVideo(param->reason, conferenceId,
		SysAllocString(Utf82Unicode(param->member).c_str())
		, param->type);
	delete param;
	return S_OK;
}

STDMETHODIMP CATLECSDK::SendTextMessage(IDispatch* matchKey, BSTR receiver, BSTR message, LONG type, BSTR userdata, IDispatch* msgId, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	char rmsgId[64];
	*result = ECSDKBase::SendTextMessage(&rmatchKey, Unicode2Utf8(receiver?receiver:L"").c_str(),Unicode2Utf8(message?message:L"").c_str(), 
		(MsgType)type, Unicode2Utf8(userdata?userdata:L"").c_str(), rmsgId);
	JSSetValue(matchKey, rmatchKey);
	JSSetValue(msgId, rmsgId);
	return S_OK;
}


STDMETHODIMP CATLECSDK::SendMediaMessage(IDispatch* matchKey, BSTR receiver, BSTR fileName, LONG type, BSTR userdata, IDispatch* msgId, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	char rmsgId[64];
	*result = ECSDKBase::SendMediaMessage(&rmatchKey, Unicode2Utf8(receiver?receiver:L"").c_str(), Unicode2Utf8(fileName?fileName:L"").c_str(),
		(MsgType)type, Unicode2Utf8(userdata?userdata:L"").c_str(), rmsgId);
	JSSetValue(matchKey, rmatchKey);
	JSSetValue(msgId, rmsgId);
	return S_OK;
}


STDMETHODIMP CATLECSDK::DownloadFileMessage(IDispatch* matchKey, BSTR sender, BSTR recv, BSTR url, BSTR fileName, BSTR msgId, LONG type, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::DownloadFileMessage(&rmatchKey, Unicode2Utf8(sender?sender:L"").c_str(), Unicode2Utf8(recv?recv:L"").c_str(), 
		Unicode2Utf8(url?url:L"").c_str(), Unicode2Utf8(fileName?fileName:L"").c_str(),
		Unicode2Utf8(msgId?msgId:L"").c_str(), (MsgType)type);
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::CancelUploadOrDownloadNOW(ULONG matchKey)
{
	// TODO: Add your implementation code here
	ECSDKBase::CancelUploadOrDownloadNOW(matchKey);
	return S_OK;
}

STDMETHODIMP CATLECSDK::DeleteMessage(IDispatch * matchKey, LONG type, BSTR msgId, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::DeleteMessage(&rmatchKey,type,Unicode2Utf8(msgId?msgId:L"").c_str());
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}

STDMETHODIMP CATLECSDK::OperateMessage(IDispatch * matchKey, BSTR version, BSTR msgId, LONG type, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::OperateMessage(&rmatchKey, Unicode2Utf8(version?version:L"").c_str(), Unicode2Utf8(msgId ? msgId : L"").c_str(), type);
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}

STDMETHODIMP CATLECSDK::CreateGroup(IDispatch* matchKey, BSTR groupName, LONG type, BSTR province, BSTR city, LONG scope, BSTR declared, LONG permission, LONG isDismiss, BSTR groupDomain, VARIANT_BOOL isDiscuss, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::CreateGroup(&rmatchKey, Unicode2Utf8(groupName?groupName:L"").c_str(), type, Unicode2Utf8(province?province:L"").c_str(),
		Unicode2Utf8(city?city:L"").c_str(), scope, Unicode2Utf8(declared?declared:L"").c_str(), permission, isDismiss,
		Unicode2Utf8(groupDomain?groupDomain:L"").c_str(), isDiscuss == VARIANT_TRUE ? true : false);
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::DismissGroup(IDispatch* matchKey, BSTR groupid, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::DismissGroup(&rmatchKey, Unicode2Utf8(groupid?groupid:L"").c_str());
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::QuitGroup(IDispatch* matchKey, BSTR groupid, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::QuitGroup(&rmatchKey, Unicode2Utf8(groupid?groupid:L"").c_str());
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::JoinGroup(IDispatch* matchKey, BSTR groupid, BSTR declared, LONG * result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::JoinGroup(&rmatchKey, Unicode2Utf8(groupid?groupid:L"").c_str(), Unicode2Utf8(declared?declared:L"").c_str());
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::ReplyRequestJoinGroup(IDispatch* matchKey, BSTR groupId, BSTR member, LONG confirm, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::ReplyRequestJoinGroup(&rmatchKey, Unicode2Utf8(groupId?groupId:L"").c_str(), Unicode2Utf8(member?member:L"").c_str(),confirm);
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::InviteJoinGroup(IDispatch* matchKey, BSTR groupId, BSTR declard, IDispatch* members, LONG confirm, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;

	VARIANT length;
	VariantInit(&length);
	std::vector<const char *> arr;
	std::vector<_bstr_t> bstrArr;
	if (SUCCEEDED(JSGetProperty(members, "length", &length)) && length.vt == VT_I4){
		arr.resize(length.intVal);
		bstrArr.resize(length.intVal);

		for (int i = 0; i < length.intVal; i++)
		{
			VARIANT element;
			VariantInit(&element);
			if (SUCCEEDED(JSGetArrayElementOfIndex(members, i, &element)))
			{
				bstrArr[i] = _bstr_t(element);
				arr[i] = bstrArr[i];
			}
		}
	}
	*result = ECSDKBase::InviteJoinGroup(&rmatchKey, Unicode2Utf8(groupId?groupId:L"").c_str(), Unicode2Utf8(declard?declard:L"").c_str(),
		arr.data(), arr.size(), confirm);
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::ReplyInviteJoinGroup(IDispatch* matchKey, BSTR groupId, BSTR invitor, LONG confirm, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::ReplyInviteJoinGroup(&rmatchKey, Unicode2Utf8(groupId?groupId:L"").c_str(), Unicode2Utf8(invitor?invitor:L"").c_str(), confirm);
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::QueryOwnGroup(IDispatch* matchKey, BSTR borderGroupid, LONG pageSize, LONG target, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::QueryOwnGroup(&rmatchKey, Unicode2Utf8(borderGroupid?borderGroupid:L"").c_str(), pageSize, target);
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::QueryGroupDetail(IDispatch* matchKey, BSTR groupId, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::QueryGroupDetail(&rmatchKey, Unicode2Utf8(groupId?groupId:L"").c_str());
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::ModifyGroup(IDispatch* matchKey, IDispatch* groupInfo, LONG* result)
{
	// TODO: Add your implementation code here
	ECModifyGroupInfo groupInfo2;
	unsigned int rmatchKey;
	VARIANT varValue;

	if (SUCCEEDED(JSGetProperty(groupInfo, "city", &varValue))){
		_bstr_t var = varValue;
		strncpy_s(groupInfo2.city, var, sizeof(groupInfo2.city) - 1);
	}

	if (SUCCEEDED(JSGetProperty(groupInfo, "declared", &varValue))){
		_bstr_t var = varValue;
		strncpy_s(groupInfo2.declared, var, sizeof(groupInfo2.declared) - 1);
	}

	if (SUCCEEDED(JSGetProperty(groupInfo, "discuss", &varValue))){
		groupInfo2.discuss = varValue.intVal;
	}

	if (SUCCEEDED(JSGetProperty(groupInfo, "groupDomain", &varValue))){
		_bstr_t var = varValue;
		strncpy_s(groupInfo2.groupDomain, var, sizeof(groupInfo2.groupDomain) - 1);
	}

	if (SUCCEEDED(JSGetProperty(groupInfo, "groupId", &varValue))){
		_bstr_t var = varValue;
		strncpy_s(groupInfo2.groupId, var, sizeof(groupInfo2.groupId) - 1);
	}

	if (SUCCEEDED(JSGetProperty(groupInfo, "name", &varValue))){
		_bstr_t var = varValue;
		strncpy_s(groupInfo2.name, var, sizeof(groupInfo2.name) - 1);
	}

	if (SUCCEEDED(JSGetProperty(groupInfo, "permission", &varValue))){
		groupInfo2.permission = varValue.intVal;
	}

	if (SUCCEEDED(JSGetProperty(groupInfo, "province", &varValue))){
		_bstr_t var = varValue;
		strncpy_s(groupInfo2.province, var, sizeof(groupInfo2.province) - 1);
	}

	if (SUCCEEDED(JSGetProperty(groupInfo, "scope", &varValue))){
		groupInfo2.scope = varValue.intVal;
	}

	if (SUCCEEDED(JSGetProperty(groupInfo, "type", &varValue))){
		groupInfo2.type = varValue.intVal;
	}

	*result = ECSDKBase::ModifyGroup(&rmatchKey, &groupInfo2);
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::SearchPublicGroups(IDispatch* matchKey, LONG searchType, BSTR keyword, LONG pageNo, LONG pageSize, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::SearchPublicGroups(&rmatchKey, searchType, Unicode2Utf8(keyword ? keyword : L"").c_str(), pageNo, pageSize);
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::QueryGroupMember(IDispatch* matchKey, BSTR groupId, BSTR borderMember, LONG pageSize, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::QueryGroupMember(&rmatchKey, Unicode2Utf8(groupId?groupId:L"").c_str(), Unicode2Utf8(borderMember?borderMember:L"").c_str(), pageSize);
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::DeleteGroupMember(IDispatch* matchKey, BSTR groupId, BSTR member, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::DeleteGroupMember(&rmatchKey, Unicode2Utf8(groupId?groupId:L"").c_str(), Unicode2Utf8(member?member:L"").c_str());
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::QueryGroupMemberCard(IDispatch* matchKey, BSTR groupid, BSTR member, LONG * result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::QueryGroupMemberCard(&rmatchKey, Unicode2Utf8(groupid?groupid:L"").c_str(), Unicode2Utf8(member?member:L"").c_str());
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::ModifyMemberCard(IDispatch* matchKey, IDispatch* card, LONG* result)
{
	// TODO: Add your implementation code here
	ECGroupMemberCard card2;
	unsigned int rmatchKey;
	VARIANT varValue;

	if (SUCCEEDED(JSGetProperty(card, "display", &varValue))){
		_bstr_t var = varValue;
		strncpy_s(card2.display, var, sizeof(card2.display) - 1);
	}

	if (SUCCEEDED(JSGetProperty(card, "groupId", &varValue))){
		_bstr_t var = varValue;
		strncpy_s(card2.groupId, var, sizeof(card2.groupId) - 1);
	}

	if (SUCCEEDED(JSGetProperty(card, "mail", &varValue))){
		_bstr_t var = varValue;
		strncpy_s(card2.mail, var, sizeof(card2.mail) - 1);
	}

	if (SUCCEEDED(JSGetProperty(card, "member", &varValue))){
		_bstr_t var = varValue;
		strncpy_s(card2.member, var, sizeof(card2.member) - 1);
	}

	if (SUCCEEDED(JSGetProperty(card, "phone", &varValue))){
		_bstr_t var = varValue;
		strncpy_s(card2.phone, var, sizeof(card2.phone) - 1);
	}

	if (SUCCEEDED(JSGetProperty(card, "remark", &varValue))){
		_bstr_t var = varValue;
		strncpy_s(card2.remark, var, sizeof(card2.remark) - 1);
	}

	if (SUCCEEDED(JSGetProperty(card, "role", &varValue))){
		card2.role = varValue.intVal;
	}

	if (SUCCEEDED(JSGetProperty(card, "sex", &varValue))){
		card2.sex = varValue.intVal;
	}

	if (SUCCEEDED(JSGetProperty(card, "speakState", &varValue))){
		card2.speakState = varValue.intVal;
	}

	*result = ECSDKBase::ModifyMemberCard(&rmatchKey, &card2);
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::ForbidMemberSpeak(IDispatch* matchKey, BSTR groupid, BSTR member, LONG isBan, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::ForbidMemberSpeak(&rmatchKey, Unicode2Utf8(groupid?groupid:L"").c_str(), Unicode2Utf8(member?member:L"").c_str(),isBan);
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::SetGroupMessageRule(IDispatch* matchKey, BSTR groupid, VARIANT_BOOL notice, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::SetGroupMessageRule(&rmatchKey, Unicode2Utf8(groupid?groupid:L"").c_str(), notice == VARIANT_TRUE ? true:false);
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::StartVoiceRecording(BSTR fileName, BSTR sessionId, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::StartVoiceRecording(Unicode2Utf8(fileName?fileName:L"").c_str(), Unicode2Utf8(sessionId).c_str());
	return S_OK;
}


STDMETHODIMP CATLECSDK::StopVoiceRecording()
{
	// TODO: Add your implementation code here
	ECSDKBase::StopVoiceRecording();
	return S_OK;
}


STDMETHODIMP CATLECSDK::PlayVoiceMsg(BSTR fileName, BSTR sessionId, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::PlayVoiceMsg(Unicode2Utf8(fileName?fileName:L"").c_str(), Unicode2Utf8(sessionId?sessionId:L"").c_str());
	return S_OK;
}


STDMETHODIMP CATLECSDK::StopVoiceMsg()
{
	// TODO: Add your implementation code here
	ECSDKBase::StopVoiceMsg();
	return S_OK;
}


STDMETHODIMP CATLECSDK::GetVoiceDuration(BSTR fileName, IDispatch* duration, LONG* result)
{
	// TODO: Add your implementation code here
	int rduration;
	*result = ECSDKBase::GetVoiceDuration(Unicode2Utf8(fileName?fileName:L"").c_str(), &rduration);
	JSSetValue(duration, rduration);
	return S_OK;
}


STDMETHODIMP CATLECSDK::RequestConferenceMemberVideo(BSTR conferenceId, BSTR conferencePassword, BSTR member, LONG videoWindow, LONG type, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::RequestConferenceMemberVideo(
		Unicode2Utf8(conferenceId?conferenceId:L"").c_str(), 
		Unicode2Utf8(conferencePassword?conferencePassword:L"").c_str(), 
		Unicode2Utf8(member?member:L"").c_str(), (void *)videoWindow, type);
	return S_OK;
}


STDMETHODIMP CATLECSDK::CancelMemberVideo(BSTR conferenceId, BSTR conferencePassword, BSTR member, LONG type, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::CancelMemberVideo(
		Unicode2Utf8(conferenceId?conferenceId:L"").c_str(), 
		Unicode2Utf8(conferencePassword?conferencePassword:L"").c_str(), 
		Unicode2Utf8(member?member:L"").c_str(), type);
	return S_OK;
}


STDMETHODIMP CATLECSDK::ResetVideoConfWindow(BSTR conferenceId, BSTR member, LONG newWindow, LONG type, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::ResetVideoConfWindow(Unicode2Utf8(conferenceId?conferenceId:L"").c_str(), Unicode2Utf8(member?member:L"").c_str(), 
		(void *)newWindow, type);
	return S_OK;
}


STDMETHODIMP CATLECSDK::CreateMultimediaMeeting(IDispatch* matchKey, LONG meetingType, BSTR meetingName, BSTR password, BSTR keywords, LONG voiceMode, LONG square, VARIANT_BOOL bAutoJoin, VARIANT_BOOL autoClose, VARIANT_BOOL autoDelete, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::CreateMultimediaMeeting(&rmatchKey, meetingType, Unicode2Utf8(meetingName?meetingName:L"").c_str(),
		Unicode2Utf8(password?password:L"").c_str(), Unicode2Utf8(keywords?keywords:L"").c_str(),voiceMode, square, 
		bAutoJoin == VARIANT_TRUE?true:false, 
		autoClose == VARIANT_TRUE?true:false, 
		autoDelete == VARIANT_TRUE?true:false);
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::JoinMeeting(LONG meetingType, BSTR conferenceId, BSTR password, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::JoinMeeting(meetingType, Unicode2Utf8(conferenceId?conferenceId:L"").c_str(), Unicode2Utf8(password?password:L"").c_str());
	return S_OK;
}


STDMETHODIMP CATLECSDK::ExitMeeting(BSTR conferenceId, LONG* result)
{
	// TODO: Add your implementation code here
	*result = ECSDKBase::ExitMeeting(Unicode2Utf8(conferenceId?conferenceId:L"").c_str());
	return S_OK;
}


STDMETHODIMP CATLECSDK::QueryMeetingMembers(IDispatch* matchKey, LONG meetingType, BSTR conferenceId, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::QueryMeetingMembers(&rmatchKey, meetingType, Unicode2Utf8(conferenceId?conferenceId:L"").c_str());
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::DismissMultiMediaMeeting(IDispatch* matchKey, LONG meetingType, BSTR conferenceId, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::DismissMultiMediaMeeting(&rmatchKey, meetingType, Unicode2Utf8(conferenceId?conferenceId:L"").c_str());
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::QueryMultiMediaMeetings(IDispatch* matchKey, LONG meetingType, BSTR keywords, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::QueryMultiMediaMeetings(&rmatchKey, meetingType, Unicode2Utf8(keywords?keywords:L"").c_str());
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::InviteJoinMultiMediaMeeting(IDispatch* matchKey, BSTR conferenceId, IDispatch* members, VARIANT_BOOL blanding, VARIANT_BOOL isSpeak, VARIANT_BOOL isListen, BSTR disNumber, BSTR userData, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	VARIANT length;
	VariantInit(&length);
	std::vector<const char *> arr;
	std::vector<_bstr_t> bstrArr;
	if (SUCCEEDED(JSGetProperty(members, "length", &length)) && length.vt == VT_I4){
		arr.resize(length.intVal);
		bstrArr.resize(length.intVal);

		for (int i = 0; i < length.intVal; i++)
		{
			VARIANT element;
			VariantInit(&element);
			if (SUCCEEDED(JSGetArrayElementOfIndex(members, i, &element)))
			{
				bstrArr[i] = _bstr_t(element);
				arr[i] = bstrArr[i];
			}
		}
	}
	*result = ECSDKBase::InviteJoinMultiMediaMeeting(&rmatchKey, Unicode2Utf8(conferenceId?conferenceId:L"").c_str(), arr.data(), arr.size(),
		blanding == VARIANT_TRUE ? true : false,
		isSpeak == VARIANT_TRUE ? true : false,
		isListen == VARIANT_TRUE ? true : false, 
		Unicode2Utf8(disNumber?disNumber:L"").c_str(), Unicode2Utf8(userData?userData:L"").c_str());
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::DeleteMemberMultiMediaMeeting(IDispatch* matchKey, LONG meetingType, BSTR conferenceId, BSTR member, VARIANT_BOOL isVoIP, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::DeleteMemberMultiMediaMeeting(&rmatchKey, meetingType, Unicode2Utf8(conferenceId?conferenceId:L"").c_str(),
		Unicode2Utf8(member?member:L"").c_str(), isVoIP == VARIANT_TRUE ? true : false);
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::PublishVideo(IDispatch* matchKey, BSTR conferenceId, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::PublishVideo(&rmatchKey, Unicode2Utf8(conferenceId?conferenceId:L"").c_str());
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::UnpublishVideo(IDispatch* matchKey, BSTR conferenceId, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::UnpublishVideo(&rmatchKey, Unicode2Utf8(conferenceId?conferenceId:L"").c_str());
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::CreateInterphoneMeeting(IDispatch* matchKey, IDispatch* members, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	VARIANT length;
	VariantInit(&length);
	std::vector<const char *> arr;
	std::vector<_bstr_t> bstrArr;
	if (SUCCEEDED(JSGetProperty(members, "length", &length)) && length.vt == VT_I4){
		arr.resize(length.intVal);
		bstrArr.resize(length.intVal);

		for (int i = 0; i < length.intVal; i++)
		{
			VARIANT element;
			VariantInit(&element);
			if (SUCCEEDED(JSGetArrayElementOfIndex(members, i, &element)))
			{
				bstrArr[i] = _bstr_t(element);
				arr[i] = bstrArr[i];
			}
		}
	}
	*result = ECSDKBase::CreateInterphoneMeeting(&rmatchKey,arr.data(),arr.size());
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::ControlInterphoneMic(IDispatch* matchKey, VARIANT_BOOL isControl, BSTR interphoneId, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::ControlInterphoneMic(&rmatchKey, isControl == VARIANT_TRUE ? true : false, Unicode2Utf8(interphoneId?interphoneId:L"").c_str());
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}

STDMETHODIMP CATLECSDK::get_version(BSTR* pVal)
{
	// TODO: Add your implementation code here
	std::wstring wstr = Utf82Unicode(ECSDKBase::GetVersion());
	*pVal = SysAllocString(wstr.c_str());
	return S_OK;
}


LRESULT CATLECSDK::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default
	LOG4CPLUS_DEBUG(log, __FUNCTION__ ":" << m_hWnd);

	delete m_VncViewer;
	m_VncViewer = nullptr;
	delete m_VncServer;
	m_VncServer = nullptr;
	
	ECSDKBase::UnInitialize();
	return 0;
}


LRESULT CATLECSDK::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// TODO: Add your message handler code here and/or call default
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " m_hWnd:" << m_hWnd);
	if (m_Script == NULL){
		get_Script(&m_Script);
	}
	if (m_UserAgent.empty())
	{
		get_userAgent(m_UserAgent);
	}

	LOG4CPLUS_DEBUG(log, "UserAgent:" << m_UserAgent);
	
	return TRUE;
}


STDMETHODIMP CATLECSDK::SetDesktopWnd(OLE_HANDLE hWnd, LONG * result)
{
	*result = ECSDKBase::SetDesktopWnd((HANDLE)hWnd);
	return S_OK;
}

STDMETHODIMP CATLECSDK::AcceptShareDesktop(SHORT * result)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	if (m_VncServerState != EVncServerState_RecvReqing)
	{
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " CCCPTerminalCtrl AcceptShareDesktop wrong state=" << m_VncServerState);
		*result = -1;
		return S_OK;
	}
	*result = -1;
	if (m_VncRoomID != 0) {
		if(m_VncServer == NULL) 
			m_VncServer = new CCPVncServer(this);

		if (m_VncServer->initServer(m_VncRoomID, m_VncServerIP.c_str(), m_VncServerPort) != 0)
		{
			LOG4CPLUS_TRACE(log, __FUNCTION__ << "CCCPTerminalCtrl AcceptShareDesktop initServer failed.");
			sendVncCmdToRemote(m_VncRoomID, EVncCmd_Resp_ShareDesktop, 400, "init VncServer failed.");
			*result = -1;
		}
		m_VncServer->joinShareRoom(m_VncRoomID);
		SetTimer(WM_TIMER_JoinShareRoom, 5000, 0);
		*result = 0;
	}
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return S_OK;

}

STDMETHODIMP CATLECSDK::AcceptRemoteControl(SHORT * result)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	*result = -1;
	if (m_VncRoomID != 0 && m_VncServer) {
		*result = m_VncServer->acceptRemoteControl(m_VncRoomID);
	}
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return S_OK;
}

STDMETHODIMP CATLECSDK::mediaTerminalRejectShareDesktop(SHORT * result)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	m_VncServerState = EVncServerState_Normal;
	LOG4CPLUS_DEBUG(log, __FUNCTION__",ServerState:EVncServerState_Normal");
	*result = sendVncCmdToRemote(m_VncRoomID, EVncCmd_Resp_ShareDesktop, 603, "Terminal reject share Desktop.");
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return S_OK;
}
STDMETHODIMP CATLECSDK::mediaTerminalRejectRemoteControl(SHORT * result)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	*result = -1;
	if (m_VncRoomID != 0 && m_VncServer) {
		*result = m_VncServer->rejectRemoteControl(m_VncRoomID);
	}
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return S_OK;
}
STDMETHODIMP CATLECSDK::mediaTerminalRequestRemoteControl(SHORT * result)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	*result = -1;
	if (m_VncRoomID != 0 && m_VncServer) {
		*result = m_VncServer->reqRemoteControl(m_VncRoomID);
	}
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return S_OK;
}

STDMETHODIMP CATLECSDK::RequestShareDesktop(BSTR remoteNO, BSTR serverIP, LONG serverPort, OLE_HANDLE vncWND, LONG* result)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");

	if (m_VncViewerState != EVncViewerState_Normal)
	{
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " wrong share state." << m_VncViewerState);
		*result = -1;
		return S_OK;
	}

	m_VncRemoteId = Unicode2Utf8(remoteNO ? remoteNO : L"");
	m_VncServerIP = Unicode2Utf8(serverIP ? serverIP : L"");
	m_VncServerPort = serverPort;
	m_VncViewerWnd = (HANDLE)vncWND;

	LOG4CPLUS_DEBUG(log, __FUNCTION__",RemoteId:" << m_VncRemoteId << ",ServerIP:" << m_VncServerIP << ",ServerPort:" << m_VncServerPort
		<< ",ViewerWnd:" << m_VncViewerWnd);

	if (m_VncRemoteId.length() == 0 || m_VncServerIP.length() == 0 || m_VncServerPort == 0 || !m_VncViewerWnd)
	{
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " failed. invalid parameter.");
		*result = -1;
		return S_OK;
	}
	if (m_VncViewer) {
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " m_VncViewer did't delete last time.");
		delete m_VncViewer;
		m_VncViewer = NULL;
	}
	m_VncViewer = new CCPVncViewer(this);

	*result = 0;
	HWND wnd = (HWND)m_VncViewerWnd;
	if (m_VncViewer->initViewer(m_VncServerIP.c_str(), m_VncServerPort, wnd) != 0)
	{
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " initViewer failed.");
		delete m_VncViewer;
		m_VncViewer = NULL;
		*result = -1;
		return S_OK;
	}

	m_VncViewerState = EVncViewerState_Reqing;
	LOG4CPLUS_DEBUG(log, __FUNCTION__",ViewerState:EVncViewerState_Reqing");
	m_VncViewer->createShareRoom();
	SetTimer(WM_TIMER_CreateShareRoom, 5000, 0);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end." << *result);
	return S_OK;
}

STDMETHODIMP CATLECSDK::StopShareDesktop(LONG* result)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	/*if (m_VncShareState != EVncViewerState_Sharing && m_VncShareState != EVncViewerState_Reqing)
	{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " wrong share state." << m_VncShareState);
	*result = -1;
	return S_OK;
	}*/

	*result = 0;
	if (m_VncRoomID) {

		if (m_VncServerState != EVncServerState_Normal)
		{
			LOG4CPLUS_DEBUG(log, "VNC server quit room:" << m_VncRoomID);
			if (m_VncServer->quitShareRoom(m_VncRoomID) == 0) {
				SetTimer(WM_TIMER_QuitShareRoom, 5000, 0);
				m_VncServerState = EVncServerState_Quiting;
				LOG4CPLUS_DEBUG(log, __FUNCTION__",ServerState:EVncServerState_Quiting");
			}
			else {
				m_VncServerState = EVncServerState_Normal;
				LOG4CPLUS_DEBUG(log, __FUNCTION__",ServerState:EVncServerState_Normal");
				Json::Value root;
				root["message"] = "Success";
				root["retcode"] = 0;
				this->Fire_onStopShareDesktopResult(_bstr_t(root.toStyledString().c_str()));
			}
			*result = sendVncCmdToRemote(m_VncRoomID, EVncCmd_Req_ShareDesktop, 0, "stop share desktop");
		}

		if (m_VncViewerState != EVncViewerState_Normal)
		{
			LOG4CPLUS_DEBUG(log, "VNC viwer quit room:" << m_VncRoomID);
			if (m_VncViewer->quitShareRoom(m_VncRoomID) == 0) {
				m_VncViewerState = EVncViewerState_Quiting;
				LOG4CPLUS_DEBUG(log, __FUNCTION__",ViwerState:EVncViewerState_Quiting");
				SetTimer(WM_TIMER_QuitShareRoom, 5000, 0);
			}
			else {
				m_VncViewerState = EVncViewerState_Normal;
				LOG4CPLUS_DEBUG(log, __FUNCTION__",ViwerState:EVncViewerState_Normal");
				Json::Value root;
				root["message"] = "Success";
				root["retcode"] = 0;
				this->Fire_onStopShareDesktopResult(_bstr_t(root.toStyledString().c_str()));
			}
			*result = sendVncCmdToRemote(m_VncRoomID, EVncCmd_Req_ShareDesktop, 0, "stop share desktop");
		}
		
	}
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return S_OK;
}

STDMETHODIMP CATLECSDK::RequestRemoteControl(LONG* result)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	*result = -1;
	if (m_VncRoomID && m_VncViewer)
		*result = m_VncViewer->reqRemoteControl(m_VncRoomID);


	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return S_OK;
}

STDMETHODIMP CATLECSDK::StopRemoteControl(LONG* result)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	*result = 0;
	if (m_VncRoomID && m_VncViewer)
		m_VncViewer->stopRemoteControl(m_VncRoomID);

	if (m_VncRoomID && m_VncServer)
		m_VncServer->stopRemoteControl(m_VncRoomID);

	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return S_OK;
}

STDMETHODIMP CATLECSDK::mediaTellerRefreshDesktopWnd(LONG* result)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	*result = -1;
	if (m_VncRoomID && m_VncViewer)
		*result = m_VncViewer->refreshDesktopWnd(m_VncRoomID);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return S_OK;
}

STDMETHODIMP CATLECSDK::mediaTellerRefreshDesktopWndByCoordinate(INT x, INT y, LONG* result)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return S_OK;
}

STDMETHODIMP CATLECSDK::mediaTellerSetShareDesktopDisplaySize(INT w, INT h, LONG* result)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	HWND viewerWnd = (HWND)m_VncViewer->GetViewerWnd();
	if (viewerWnd) {
		*result = 0;
		::MoveWindow(viewerWnd, 0, 0, w, h, TRUE);
	}
	else {
		*result = -1;
	}
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return S_OK;
}

STDMETHODIMP CATLECSDK::mediaTellerRejectRemoteControl(LONG* result)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	*result = -1;
	if (m_VncRoomID && m_VncViewer)
		*result = m_VncViewer->rejectRemoteControl(m_VncRoomID);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return S_OK;
}

LRESULT CATLECSDK::OnTimer(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	switch (wParam)
	{
	case WM_TIMER_JoinShareRoom:
	{
		sendVncCmdToRemote(m_VncRoomID, EVncCmd_Resp_ShareDesktop, 400, "Join share room timeout.");
		m_VncServer->uninitServer();
		m_VncServerState = EVncServerState_Normal;
		LOG4CPLUS_DEBUG(log, __FUNCTION__",ServerState:EVncServerState_Normal");
		m_VncRoomID = 0;
		Json::Value root;
		root["message"] = "Success.";
		root["retcode"] = 481;
		this->Fire_onStopShareDesktop(_bstr_t(root.toStyledString().c_str()));
		KillTimer(WM_TIMER_JoinShareRoom);
	}
	break;
	case WM_TIMER_QuitShareRoom:
	{
		if (m_VncServerState == EVncServerState_Quiting)
		{
			m_VncServer->uninitServer();
			m_VncServerState = EVncServerState_Normal;
			LOG4CPLUS_DEBUG(log, __FUNCTION__",ServerState:EVncServerState_Normal");
		}
		else if (m_VncViewerState == EVncViewerState_Quiting)
		{
			m_VncViewer->uninitViewer();
			m_VncViewerState = EVncViewerState_Normal;
			LOG4CPLUS_DEBUG(log, __FUNCTION__",ViewerState:EVncViewerState_Normal");
		}

		m_VncRoomID = 0;
		Json::Value root;
		root["message"] = "Success.";
		root["retcode"] = 480;
		this->Fire_onStopShareDesktop(_bstr_t(root.toStyledString().c_str()));
		KillTimer(WM_TIMER_QuitShareRoom);
	}
	break;
	case WM_TIMER_CreateShareRoom:
	{
		//m_VncViewer->uninitViewer();
		delete m_VncViewer;
		m_VncViewer = NULL;
		m_VncViewerState = EVncViewerState_Normal;
		LOG4CPLUS_DEBUG(log, __FUNCTION__",ViwerState:EVncViewerState_Normal");
		m_VncRoomID = 0;
		KillTimer(WM_TIMER_CreateShareRoom);
		Json::Value root;
		root["message"] = "Success";
		root["retcode"] = 480;
		this->Fire_onRequestShareDesktopResult(_bstr_t(root.toStyledString().c_str()));
		LOG4CPLUS_TRACE(log, __FUNCTION__ << " create room timeout.");
	}
	break;
	//case WM_TIMER_QuitShareRoom:
	//{
	//	m_VncShareState = EVncViewerState_Normal;
	//	//m_VncViewer->uninitViewer();
	//	delete m_VncViewer;
	//	m_VncViewer = NULL;
	//	m_VncRoomID = 0;
	//	KillTimer(WM_TIMER_QuitShareRoom);
	//	Json::Value root;
	//	root["message"] = "";
	//	root["retcode"] = 1;
	//	this->Fire_onStopShareDesktopResult(_bstr_t(root.toStyledString().c_str()));
	//	LOG4CPLUS_TRACE(log, __FUNCTION__ << " quit room timeout.");
	//}
	//break;
	default:
		break;
	}
	return 0;
}

LRESULT CATLECSDK::onVncViewerCallBack(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	VncCallbackMsg *msg = (VncCallbackMsg*)lParam;
	int roomID = msg->roomID;
	int status = msg->status;

	switch (msg->msgType)
	{
	case EVncCallbackType_CreateRoom:
	{
		if (status != 0 || roomID == 0)
		{
			m_VncViewer->uninitViewer();
			m_VncViewerState = EVncViewerState_Normal;
			LOG4CPLUS_DEBUG(log, __FUNCTION__",ViewerState:EVncViewerState_Normal");
			m_VncRoomID = 0;
			Json::Value root;
			root["message"] = "Success";
			root["retcode"] = 400 + status;
			this->Fire_onRequestShareDesktopResult(_bstr_t(root.toStyledString().c_str()));
		}
		else
		{
			m_VncRoomID = roomID;
			KillTimer(WM_TIMER_CreateShareRoom);
			sendVncCmdToRemote(roomID, EVncCmd_Req_ShareDesktop, 1, "require share desktop");
		}
	}
	break;
	case EVncCallbackType_RoomClosed:
	{
		m_VncServer->uninitServer();
		m_VncServerState = EVncServerState_Normal;
		LOG4CPLUS_DEBUG(log, __FUNCTION__",ServerState:EVncServerState_Normal");
		m_VncRoomID = 0;
		Json::Value root;
		root["message"] = "Success.";
		root["retcode"] = 0;
		this->Fire_onStopShareDesktop(_bstr_t(root.toStyledString().c_str()));
	}
	break;
	case EVncCallbackType_JoinRoom:
	{
		KillTimer(WM_TIMER_JoinShareRoom);
		if (m_hShareWnd)
			m_VncServer->setWindowShared((HWND)m_hShareWnd);

		m_VncServerState = EVncServerState_Sharing;
		LOG4CPLUS_DEBUG(log, __FUNCTION__",ServerState:EVncServerState_Sharing");
		sendVncCmdToRemote(m_VncRoomID, EVncCmd_AcceptShareDesktop, 200, "accept share Desktop.");
		Json::Value root;
		root["message"] = "Success.";
		root["retcode"] = 0;
		this->Fire_onStartDesktopShare(_bstr_t(root.toStyledString().c_str()));
	}
	break;
	case EVncCallbackType_QuitRoom:
	{
		KillTimer(WM_TIMER_QuitShareRoom);
		if (m_VncServerState == EVncServerState_Quiting){
			m_VncServer->uninitServer();
			m_VncServerState = EVncServerState_Normal;
			LOG4CPLUS_DEBUG(log, "ServerState:EVncServerState_Normal");
		}
		else if (m_VncViewerState == EVncViewerState_Quiting)
		{
			m_VncViewer->uninitViewer();
			m_VncViewerState = EVncViewerState_Normal;
			LOG4CPLUS_DEBUG(log, "ViewerState:EVncViewerState_Normal");
		}

		m_VncRoomID = 0;
		Json::Value root;
		root["message"] = "Success.";
		root["retcode"] = 0;
		this->Fire_onStopShareDesktop(_bstr_t(root.toStyledString().c_str()));
	}
	break;
	case EVncCallbackType_RecvReqRemoteControl:
	{
		Json::Value root;
		root["message"] = "Success.";
		root["retcode"] = status;
		this->Fire_onRequestRemoteControl(_bstr_t(root.toStyledString().c_str()));
	}
	break;
	case EVncCallbackType_CancelReqRemoteControl:
	{
		Json::Value root;
		root["message"] = "Success.";
		root["retcode"] = 0;
		//root["userID"] =  "8001";
		this->Fire_CancelRequestDesktopControlEvent(_bstr_t(root.toStyledString().c_str()));
	}
	break;
	case EVncCallbackType_StopRemoteControl:
	{
		Json::Value root;
		root["message"] = "Success.";
		root["retcode"] = 0;
		this->Fire_onStopRemoteControl(_bstr_t(root.toStyledString().c_str()));
	}
	break;
	case EVncCallbackType_AcceptRemoteControl:
	{
		Json::Value root;
		root["message"] = "Success.";
		root["retcode"] = 200;
		this->Fire_onRequestRemoteControlResult(_bstr_t(root.toStyledString().c_str()));
	}
	break;
	case EVncCallbackType_RejectRemoteControl:
	{
		Json::Value root;
		root["message"] = "Success.";
		root["retcode"] = 603;
		this->Fire_onRequestRemoteControlResult(_bstr_t(root.toStyledString().c_str()));
	}
	break;
	}
	return S_OK;
}

void CATLECSDK::OnVncServerCreateRoom(int roomID, int status)
{
	VncCallbackMsg *msg = new VncCallbackMsg;
	msg->roomID = roomID;
	msg->status = status;
	msg->msgType = EVncCallbackType_CreateRoom;
	LOG4CPLUS_DEBUG(log, __FUNCTION__",roomId:" << roomID);
	::PostMessage(m_hWnd, WM_onVncViewerCallBack, NULL, (LPARAM)msg);
}

void CATLECSDK::OnVncServerRoomClosed(int roomID, int status)
{
	VncCallbackMsg *msg = new VncCallbackMsg;
	msg->roomID = roomID;
	msg->status = status;
	msg->msgType = EVncCallbackType_RoomClosed;
	LOG4CPLUS_DEBUG(log, __FUNCTION__",roomId:" << roomID);
	::PostMessage(m_hWnd, WM_onVncViewerCallBack, NULL, (LPARAM)msg);
}

void CATLECSDK::OnVncServerJoinRoom(int roomID, int status)
{
	VncCallbackMsg *msg = new VncCallbackMsg;
	msg->roomID = roomID;
	msg->status = status;
	msg->msgType = EVncCallbackType_JoinRoom;
	LOG4CPLUS_DEBUG(log, __FUNCTION__",roomId:" << roomID);
	::PostMessage(m_hWnd, WM_onVncViewerCallBack, NULL, (LPARAM)msg);
}

void CATLECSDK::OnVncServerQuitRoom(int roomID, int status)
{
	VncCallbackMsg *msg = new VncCallbackMsg;
	msg->roomID = roomID;
	msg->status = status;
	msg->msgType = EVncCallbackType_QuitRoom;
	LOG4CPLUS_DEBUG(log, __FUNCTION__",roomId:" << roomID);
	::PostMessage(m_hWnd, WM_onVncViewerCallBack, NULL, (LPARAM)msg);
}
void CATLECSDK::ServerQuitRoom(int roomID)
{
	if (roomID) {

		if (m_VncServerState != EVncServerState_Normal)
		{
			LOG4CPLUS_DEBUG(log, __FUNCTION__",VNC server quit room:" << roomID);

			if (m_VncServer->quitShareRoom(m_VncRoomID) == 0) {
				SetTimer(WM_TIMER_QuitShareRoom, 5000, 0);
				m_VncServerState = EVncServerState_Quiting;
				LOG4CPLUS_DEBUG(log, __FUNCTION__",ServerState:EVncServerState_Quiting");
			}
			else {
				m_VncServerState = EVncServerState_Normal;
				LOG4CPLUS_DEBUG(log, __FUNCTION__",ServerState:EVncServerState_Normal");
				Json::Value root;
				root["message"] = "Success";
				root["retcode"] = 0;
				this->Fire_onStopShareDesktopResult(_bstr_t(root.toStyledString().c_str()));
			}
		}

	

	}
}
void CATLECSDK::OnVncRecvReqRemoteControl(int roomID)
{
	VncCallbackMsg *msg = new VncCallbackMsg;
	msg->roomID = roomID;
	msg->msgType = EVncCallbackType_RecvReqRemoteControl;
	msg->status = 0;
	LOG4CPLUS_DEBUG(log, __FUNCTION__",roomId:" << roomID);
	::PostMessage(m_hWnd, WM_onVncViewerCallBack, NULL, (LPARAM)msg);
}
void CATLECSDK::OnVncCancelReqRemoteControl(int roomID)
{
	VncCallbackMsg *msg = new VncCallbackMsg;
	msg->roomID = roomID;
	msg->status = 0;
	msg->msgType = EVncCallbackType_CancelReqRemoteControl;
	LOG4CPLUS_DEBUG(log, __FUNCTION__",roomId:" << roomID);
	::PostMessage(m_hWnd, WM_onVncViewerCallBack, NULL, (LPARAM)msg);
}
void CATLECSDK::OnVncStopRemoteControl(int roomID)
{
	VncCallbackMsg *msg = new VncCallbackMsg;
	msg->roomID = roomID;
	msg->status = 0;
	msg->msgType = EVncCallbackType_StopRemoteControl;
	LOG4CPLUS_DEBUG(log, __FUNCTION__",roomId:" << roomID);
	::PostMessage(m_hWnd, WM_onVncViewerCallBack, NULL, (LPARAM)msg);
}
void CATLECSDK::OnVncAcceptRemoteControl(int roomID)
{
	VncCallbackMsg *msg = new VncCallbackMsg;
	msg->roomID = roomID;
	msg->status = 0;
	msg->msgType = EVncCallbackType_AcceptRemoteControl;
	LOG4CPLUS_DEBUG(log, __FUNCTION__",roomId:" << roomID);
	::PostMessage(m_hWnd, WM_onVncViewerCallBack, NULL, (LPARAM)msg);
}
void CATLECSDK::OnVncRejectRemoteControl(int roomID)
{
	VncCallbackMsg *msg = new VncCallbackMsg;
	msg->roomID = roomID;
	msg->status = 0;
	msg->msgType = EVncCallbackType_RejectRemoteControl;
	LOG4CPLUS_DEBUG(log, __FUNCTION__",roomId:" << roomID);
	::PostMessage(m_hWnd, WM_onVncViewerCallBack, NULL, (LPARAM)msg);
}

void CATLECSDK::OnVncViewerCreateRoom(int roomID, int status)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	VncCallbackMsg *msg = new VncCallbackMsg;
	msg->roomID = roomID;
	msg->status = status;
	msg->msgType = EVncCallbackType_CreateRoom;
	LOG4CPLUS_DEBUG(log, __FUNCTION__",roomId:" << roomID);
	::PostMessage(m_hWnd, WM_onVncViewerCallBack, NULL, (LPARAM)msg);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void CATLECSDK::OnVncViewerRoomClosed(int roomID, int status)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	VncCallbackMsg *msg = new VncCallbackMsg;
	msg->roomID = roomID;
	msg->status = status;
	msg->msgType = EVncCallbackType_RoomClosed;
	LOG4CPLUS_DEBUG(log, __FUNCTION__",roomId:" << roomID);
	::PostMessage(m_hWnd, WM_onVncViewerCallBack, NULL, (LPARAM)msg);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void CATLECSDK::OnVncViewerJoinRoom(int roomID, int status)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	VncCallbackMsg *msg = new VncCallbackMsg;
	msg->roomID = roomID;
	msg->status = status;
	msg->msgType = EVncCallbackType_JoinRoom;
	LOG4CPLUS_DEBUG(log, __FUNCTION__",roomId:" << roomID);
	::PostMessage(m_hWnd, WM_onVncViewerCallBack, NULL, (LPARAM)msg);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void CATLECSDK::OnVncViewerQuitRoom(int roomID, int status)
{
	VncCallbackMsg *msg = new VncCallbackMsg;
	msg->roomID = roomID;
	msg->status = status;
	msg->msgType = EVncCallbackType_QuitRoom;
	LOG4CPLUS_DEBUG(log, __FUNCTION__",roomId:" << roomID);
	::PostMessage(m_hWnd, WM_onVncViewerCallBack, NULL, (LPARAM)msg);
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
}

void CATLECSDK::ViewerQuitRoom(int roomID)
{
	if (roomID) {
		if (m_VncViewerState != EVncViewerState_Normal)
		{
			LOG4CPLUS_DEBUG(log, __FUNCTION__",VNC viewer quit room:" << roomID);
			if (m_VncViewer->quitShareRoom(m_VncRoomID) == 0) {
				m_VncViewerState = EVncViewerState_Quiting;
				LOG4CPLUS_DEBUG(log, __FUNCTION__",ViwerState:EVncViewerState_Quiting");
				SetTimer(WM_TIMER_QuitShareRoom, 5000, 0);
			}
			else {
				m_VncViewerState = EVncViewerState_Normal;
				LOG4CPLUS_DEBUG(log, __FUNCTION__",ViwerState:EVncViewerState_Normal");
				Json::Value root;
				root["message"] = "Success";
				root["retcode"] = 0;
				this->Fire_onStopShareDesktopResult(_bstr_t(root.toStyledString().c_str()));
			}
		}
	}
}

int CATLECSDK::handleVncCmd(const string & cmdStr)
{
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " start.");
	Json::Value indata(Json::objectValue);
	Json::Reader reader;
	if (!reader.parse(cmdStr, indata)) {
		LOG4CPLUS_TRACE(log, __FUNCTION__ << "CATLCCPClientSDK::handleVncCmd parse VncCmd failed. ");
		return -1;
	}

	int roomID = indata["roomID"].asInt();
	int type = indata["type"].asInt();
	int status = indata["status"].asInt();
	string desc = indata["desc"].asString();
	m_VncRemoteId = indata["account"].asString();
	m_VncServerIP = indata["svrIP"].asString();
	m_VncServerPort = indata["svrPort"].asInt();

	LOG4CPLUS_DEBUG(log, __FUNCTION__",recv cmdstr:" << cmdStr << " status: " << status);

	if (type == EVncCmd_Req_ShareDesktop) {
		Json::Value root;
		root["message"] = "Success.";
		root["retcode"] = status;
		if (status == 0)
		{
			if (m_VncServerState == EVncServerState_Sharing){
				if (m_VncServer->quitShareRoom(m_VncRoomID) == 0)
				{
					SetTimer(WM_TIMER_QuitShareRoom, 1000, 0);
					m_VncServerState = EVncServerState_Quiting;
				}
			//	m_VncServer->uninitServer();
			//	m_VncServerState = EVncServerState_Normal;
				LOG4CPLUS_DEBUG(log, __FUNCTION__",ServerState:EVncServerState_Normal");
			}
			else if (m_VncViewerState == EVncViewerState_Sharing)
			{
				if (m_VncViewer->quitShareRoom(m_VncRoomID) == 0) {
					m_VncViewerState = EVncViewerState_Quiting;
					SetTimer(WM_TIMER_QuitShareRoom, 1000, 0);
				}
				//m_VncViewer->uninitViewer();
				//m_VncViewerState = EVncViewerState_Normal;
				LOG4CPLUS_DEBUG(log, __FUNCTION__",ViwerState:EVncViewerState_Normal");
			}
			m_VncRoomID = 0;
			if (m_VncViewerState == EVncServerState_RecvReqing)
			{
				this->Fire_CancelRequestShareDesktopEvent(_bstr_t(root.toStyledString().c_str()));
				LOG4CPLUS_TRACE(log, __FUNCTION__ << " cancel request share event.");
			}
			else
			{
				this->Fire_onStopShareDesktop(_bstr_t(root.toStyledString().c_str()));
				LOG4CPLUS_TRACE(log, __FUNCTION__ << " stop share event.");
			}
		}
		else
		{
			m_VncRoomID = roomID;
			m_VncServerState = EVncServerState_RecvReqing;
			LOG4CPLUS_DEBUG(log, __FUNCTION__",ServerState:EVncServerState_RecvReqing");
			this->Fire_onRequestShareDesktop(_bstr_t(root.toStyledString().c_str()));
			LOG4CPLUS_TRACE(log, __FUNCTION__ << " recevie request share.");
		}
	}
	else if (type == EVncCmd_AcceptShareDesktop)
	{
		if (status == 200) {    //terminal accept share,  callback
			m_VncViewerState = EVncViewerState_Sharing;
			LOG4CPLUS_DEBUG(log, __FUNCTION__",ViwerState:EVncViewerState_Sharing");
		}
		Json::Value root;
		root["message"] = "Success";
		root["retcode"] = status;
		this->Fire_onRequestShareDesktopResult(_bstr_t(root.toStyledString().c_str()));
	
	}
	else if (type == EVncCmd_Resp_ShareDesktop) {
		if (status == 603) {	//terminal reject share,  callback
			m_VncViewerState = EVncViewerState_Normal;
			LOG4CPLUS_DEBUG(log, __FUNCTION__",ViwerState:EVncViewerState_Normal");
			//m_VncViewer->uninitViewer();
			delete m_VncViewer;
			m_VncViewer = NULL;
			m_VncRoomID = 0;
			LOG4CPLUS_TRACE(log, __FUNCTION__ << " reject share.");
		}
		Json::Value root;
		root["message"] = "Success";
		root["retcode"] = status;
		this->Fire_onRequestShareDesktopResult(_bstr_t(root.toStyledString().c_str()));
	}
	
	LOG4CPLUS_TRACE(log, __FUNCTION__ << " end.");
	return 0;
}

STDMETHODIMP CATLECSDK::GetLocalVideoSnapshotEx(BSTR callid, IDispatch* pictureData, IDispatch* size, IDispatch* width, IDispatch* height, LONG* result)
{
	// TODO: Add your implementation code here
	std::string strCallid = Unicode2Utf8(callid ? callid : L"");
	unsigned char *buf;
	unsigned int uSize;
	unsigned int uWidth;
	unsigned int uHeight;
	*result = ECSDKBase::GetLocalVideoSnapshotEx(strCallid.c_str(), &buf, &uSize, &uWidth, &uHeight);

	//unsigned char * jpgBuf = nullptr;
	//ECSDKBase::RGB24Snapshot(buf, uWidth, uHeight, &jpgBuf, &uSize);
	std::string data = ZBase64::Encode(buf, uSize);
	//free(jpgBuf);
	LOG4CPLUS_DEBUG(log, __FUNCTION__",data:" << data);
	JSSetValue(pictureData, Utf82Unicode(data).c_str());
	JSSetValue(size, uSize);
	JSSetValue(width, uWidth);
	JSSetValue(height, uHeight);

	return S_OK;
}


STDMETHODIMP CATLECSDK::GetRemoteVideoSnapshotEx(BSTR callid, IDispatch* pictureData, IDispatch* size, IDispatch* width, IDispatch* height, LONG* result)
{
	// TODO: Add your implementation code here
	std::string strCallid = Unicode2Utf8(callid?callid:L"");
	unsigned char *buf;
	unsigned int uSize;
	unsigned int uWidth;
	unsigned int uHeight;
	*result = ECSDKBase::GetRemoteVideoSnapshotEx(strCallid.c_str(), &buf, &uSize, &uWidth, &uHeight);

	//unsigned char * jpgBuf = nullptr;
	//ECSDKBase::RGB24Snapshot(buf, uWidth, uHeight, &jpgBuf, &uSize);
	std::string data = ZBase64::Encode(buf, uSize);
	//free(jpgBuf);
	LOG4CPLUS_DEBUG(log, __FUNCTION__",data:" << data);
	JSSetValue(pictureData, Utf82Unicode(data).c_str());
	JSSetValue(size, uSize);
	JSSetValue(width, uWidth);
	JSSetValue(height, uHeight);
	return S_OK;
}


STDMETHODIMP CATLECSDK::StartRecordScreen(BSTR callid, BSTR fileName, LONG bitrate, LONG fps, LONG type, LONG* result)
{
	// TODO: Add your implementation code here
	std::string strCallid = Unicode2Utf8(callid ? callid : L"");
	std::string strFileName = Unicode2Utf8(fileName ? fileName : L"");

	*result = ECSDKBase::StartRecordScreen(strCallid.c_str(), strFileName.c_str(), bitrate, fps, type);
	return S_OK;
}


STDMETHODIMP CATLECSDK::StartRecordScreenEx(BSTR callid, BSTR fileName, LONG bitrate, LONG fps, LONG type, LONG left, LONG top, LONG width, LONG height, LONG* result)
{
	// TODO: Add your implementation code here
	std::string strCallid = Unicode2Utf8(callid ? callid : L"");
	std::string strFileName = Unicode2Utf8(fileName ? fileName : L"");

	*result = ECSDKBase::StartRecordScreenEx(strCallid.c_str(), strFileName.c_str(), bitrate, fps, type, left, top, width, height);
	return S_OK;
}


STDMETHODIMP CATLECSDK::StopRecordScreen(BSTR callid, LONG* result)
{
	// TODO: Add your implementation code here
	std::string strCallid = Unicode2Utf8(callid ? callid : L"");
	*result = ECSDKBase::StopRecordScreen(strCallid.c_str());
	return S_OK;
}


STDMETHODIMP CATLECSDK::SetServerAddress(BSTR connector, LONG connectorPort, BSTR fileServer, LONG fileSeverPort, LONG* result)
{
    // TODO: Add your implementation code here
    std::string utf8Connector = Unicode2Utf8(connector ? connector : L"");
    std::string utf8fileServer = Unicode2Utf8(fileServer ? fileServer : L"");
    *result = ECSDKBase::SetServerAddress(utf8Connector.c_str(), connectorPort, utf8fileServer.c_str(), fileSeverPort);
    return S_OK;
}


STDMETHODIMP CATLECSDK::UploadVTMFile(IDispatch* matchKey, BSTR companyId, BSTR companyPwd, BSTR fileName, BSTR callbackUrl, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	*result = ECSDKBase::UploadVTMFile(&rmatchKey, Unicode2Utf8(companyId ? companyId : L"").c_str(),
		Unicode2Utf8(companyPwd?companyPwd:L"").c_str(),
		Unicode2Utf8(fileName?fileName:L"").c_str(),
		Unicode2Utf8(callbackUrl?callbackUrl:L"").c_str());
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::UploadVTMBuf(IDispatch* matchKey, BSTR companyId, BSTR companyPwd, BSTR Name, BSTR buf, BSTR callbackUrl, LONG* result)
{
	// TODO: Add your implementation code here
	unsigned int rmatchKey;
	std::string base64Buf = Unicode2Utf8(buf ? buf : L"");
	int len;
	std::string sbuf = ZBase64::Decode(base64Buf.c_str(), base64Buf.size(), len);

	*result = ECSDKBase::UploadVTMBuf(&rmatchKey, Unicode2Utf8(companyId ? companyId : L"").c_str(),
		Unicode2Utf8(companyPwd ? companyPwd : L"").c_str(),
		Unicode2Utf8(Name ? Name : L"").c_str(),
		sbuf.c_str(),sbuf.length(),
		Unicode2Utf8(callbackUrl ? callbackUrl : L"").c_str());
	JSSetValue(matchKey, rmatchKey);
	return S_OK;
}


STDMETHODIMP CATLECSDK::get_appDataDir(BSTR* pVal)
{
	// TODO: Add your implementation code here
	std::wstring wstr = Utf82Unicode(ECSDKBase::getAppDataDir());
	*pVal = SysAllocString(wstr.c_str());
	return S_OK;
}
