// ATLECSDK.h : Declaration of the CATLECSDK
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include <comutil.h>
#include "ECBrowserPlugin_i.h"
#include "_IATLECSDKEvents_CP.h"
#include "ECSDKBase.h"
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include "CCPVncServer.h"
#include <CCPVncViewer.h>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

#define WM_TIMER_CreateShareRoom  WM_USER + 1000
#define WM_TIMER_ReqTerminalShare   WM_USER + 1001
#define WM_TIMER_QuitShareRoom   WM_USER + 1002
#define WM_TIMER_MoveViewerWindow   WM_USER + 1003

#define WM_TIMER_JoinShareRoom           WM_USER + 1004

typedef enum _EVncCmd {
	EVncCmd_Req_ShareDesktop = 0,
	EVncCmd_AcceptShareDesktop,
	EVncCmd_Resp_ShareDesktop = 1000
}EVncCmd;

// CATLECSDK
class ATL_NO_VTABLE CATLECSDK :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IATLECSDK, &IID_IATLECSDK, &LIBID_ECBrowserPluginLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IOleControlImpl<CATLECSDK>,
	public IOleObjectImpl<CATLECSDK>,
	public IOleInPlaceActiveObjectImpl<CATLECSDK>,
	public IViewObjectExImpl<CATLECSDK>,
	public IOleInPlaceObjectWindowlessImpl<CATLECSDK>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CATLECSDK>,
	public CProxy_IATLECSDKEvents<CATLECSDK>,
	public IObjectWithSiteImpl<CATLECSDK>,
	public IQuickActivateImpl<CATLECSDK>,
#ifndef _WIN32_WCE
	public IDataObjectImpl<CATLECSDK>,
#endif
	public IProvideClassInfo2Impl<&CLSID_ATLECSDK, &__uuidof(_IATLECSDKEvents), &LIBID_ECBrowserPluginLib>,
	public IPropertyNotifySinkCP<CATLECSDK>,
	public IObjectSafetyImpl<CATLECSDK, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
	public CComCoClass<CATLECSDK, &CLSID_ATLECSDK>,
	public CComCompositeControl<CATLECSDK>,
	public ECSDKBase,
	public CCPVncViewerObserver,
	public CCPVncServerObserver
{
public:


	CATLECSDK() :m_VncViewer(NULL),
		m_VncRoomID(0),
		m_VncViewerWnd(NULL), 
		m_VncViewerState(EVncViewerState_Normal),
		m_VncServer(NULL),
		m_VncServerState(EVncServerState_Normal)
	{
		m_bWindowOnly = TRUE;
		CalcExtent(m_sizeExtent);
		this->log = log4cplus::Logger::getInstance("ATLECSDK");
	}

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_ATLECSDK)


BEGIN_COM_MAP(CATLECSDK)
	COM_INTERFACE_ENTRY(IATLECSDK)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IQuickActivate)
#ifndef _WIN32_WCE
	COM_INTERFACE_ENTRY(IDataObject)
#endif
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
END_COM_MAP()

BEGIN_PROP_MAP(CATLECSDK)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CATLECSDK)
	CONNECTION_POINT_ENTRY(IID_IPropertyNotifySink)
	CONNECTION_POINT_ENTRY(__uuidof(_IATLECSDKEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CATLECSDK)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	CHAIN_MSG_MAP(CComCompositeControl<CATLECSDK>)
	MESSAGE_HANDLER(WM_onLoginfo, onLoginfo)
	MESSAGE_HANDLER(WM_onLogOut, onLogOut)
	MESSAGE_HANDLER(WM_onConnectState, onConnectState)
	MESSAGE_HANDLER(WM_onSetPersonInfo,onSetPersonInfo)
	MESSAGE_HANDLER(WM_onGetPersonInfo,onGetPersonInfo)
	MESSAGE_HANDLER(WM_onGetUserState, onGetUserState)
	MESSAGE_HANDLER(WM_onPublishPresence,onPublishPresence)
	MESSAGE_HANDLER(WM_onReceiveFriendsPublishPresence,onReceiveFriendsPublishPresence)
	MESSAGE_HANDLER(WM_onSoftVersion,onSoftVersion)
	MESSAGE_HANDLER(WM_onGetOnlineMultiDevice,onGetOnlineMultiDevice)
	MESSAGE_HANDLER(WM_onReceiveMultiDeviceState,onReceiveMultiDeviceState)
	MESSAGE_HANDLER(WM_onMakeCallBack, onMakeCallBack)
	MESSAGE_HANDLER(WM_onCallEvents, onCallEvents)
	MESSAGE_HANDLER(WM_onCallIncoming, onCallIncoming)
	MESSAGE_HANDLER(WM_onDtmfReceived, onDtmfReceived)
	MESSAGE_HANDLER(WM_onSwitchCallMediaTypeRequest, onSwitchCallMediaTypeRequest)
	MESSAGE_HANDLER(WM_onSwitchCallMediaTypeResponse, onSwitchCallMediaTypeResponse)
	MESSAGE_HANDLER(WM_onRemoteVideoRatio, onRemoteVideoRatio)
	MESSAGE_HANDLER(WM_onOfflineMessageCount, onOfflineMessageCount)
	MESSAGE_HANDLER(WM_onGetOfflineMessage, onGetOfflineMessage)
	MESSAGE_HANDLER(WM_onOfflineMessageComplete, onOfflineMessageComplete)
	MESSAGE_HANDLER(WM_onReceiveOfflineMessage, onReceiveOfflineMessage)
	MESSAGE_HANDLER(WM_onReceiveMessage, onReceiveMessage)
	MESSAGE_HANDLER(WM_onReceiveFile, onReceiveFile)
	MESSAGE_HANDLER(WM_onDownloadFileComplete, onDownloadFileComplete)
	MESSAGE_HANDLER(WM_onRateOfProgressAttach, onRateOfProgressAttach)
	MESSAGE_HANDLER(WM_onSendTextMessage, onSendTextMessage)
	MESSAGE_HANDLER(WM_onSendMediaFile, onSendMediaFile)
	MESSAGE_HANDLER(WM_onDeleteMessage, onDeleteMessage)
	MESSAGE_HANDLER(WM_onOperateMessage, onOperateMessage)
	MESSAGE_HANDLER(WM_onReceiveOpreateNoticeMessage,onReceiveOpreateNoticeMessage)
	MESSAGE_HANDLER(WM_onUploadVTMFileOrBuf, onUploadVTMFileOrBuf)
	MESSAGE_HANDLER(WM_onCreateGroup, onCreateGroup)
	MESSAGE_HANDLER(WM_onDismissGroup, onDismissGroup)
	MESSAGE_HANDLER(WM_onQuitGroup, onQuitGroup)
	MESSAGE_HANDLER(WM_onJoinGroup, onJoinGroup)
	MESSAGE_HANDLER(WM_onReplyRequestJoinGroup, onReplyRequestJoinGroup)
	MESSAGE_HANDLER(WM_onInviteJoinGroup, onInviteJoinGroup)
	MESSAGE_HANDLER(WM_onReplyInviteJoinGroup, onReplyInviteJoinGroup)
	MESSAGE_HANDLER(WM_onQueryOwnGroup, onQueryOwnGroup)
	MESSAGE_HANDLER(WM_onQueryGroupDetail, onQueryGroupDetail)
	MESSAGE_HANDLER(WM_onModifyGroup, onModifyGroup)
	MESSAGE_HANDLER(WM_onSearchPublicGroup, onSearchPublicGroup)
	MESSAGE_HANDLER(WM_onQueryGroupMember, onQueryGroupMember)
	MESSAGE_HANDLER(WM_onDeleteGroupMember, onDeleteGroupMember)
	MESSAGE_HANDLER(WM_onQueryGroupMemberCard, onQueryGroupMemberCard)
	MESSAGE_HANDLER(WM_onModifyGroupMemberCard, onModifyGroupMemberCard)
	MESSAGE_HANDLER(WM_onForbidMemberSpeakGroup, onForbidMemberSpeakGroup)
	MESSAGE_HANDLER(WM_onSetGroupMessageRule, onSetGroupMessageRule)
	MESSAGE_HANDLER(WM_onReceiveGroupNoticeMessage, onReceiveGroupNoticeMessage)
	MESSAGE_HANDLER(WM_onRecordingTimeOut, onRecordingTimeOut)
	MESSAGE_HANDLER(WM_onFinishedPlaying, onFinishedPlaying)
	MESSAGE_HANDLER(WM_onRecordingAmplitude, onRecordingAmplitude)
	MESSAGE_HANDLER(WM_onMeetingIncoming, onMeetingIncoming)
	MESSAGE_HANDLER(WM_onReceiveInterphoneMeetingMessage, onReceiveInterphoneMeetingMessage)
	MESSAGE_HANDLER(WM_onReceiveVoiceMeetingMessage, onReceiveVoiceMeetingMessage)
	MESSAGE_HANDLER(WM_onReceiveVideoMeetingMessage, onReceiveVideoMeetingMessage)
	MESSAGE_HANDLER(WM_onJoinMeeting, onJoinMeeting)
	MESSAGE_HANDLER(WM_onExitMeeting, onExitMeeting)
	MESSAGE_HANDLER(WM_onQueryMeetingMembers, onQueryMeetingMembers)
	MESSAGE_HANDLER(WM_onDismissMultiMediaMeeting, onDismissMultiMediaMeeting)
	MESSAGE_HANDLER(WM_onQueryMultiMediaMeetings, onQueryMultiMediaMeetings)
	MESSAGE_HANDLER(WM_onDeleteMemberMultiMediaMeeting, onDeleteMemberMultiMediaMeeting)
	MESSAGE_HANDLER(WM_onInviteJoinMultiMediaMeeting, onInviteJoinMultiMediaMeeting)
	MESSAGE_HANDLER(WM_onCreateInterphoneMeeting, onCreateInterphoneMeeting)
	MESSAGE_HANDLER(WM_onExitInterphoneMeeting, onExitInterphoneMeeting)
	MESSAGE_HANDLER(WM_onSetMeetingSpeakListen, onSetMeetingSpeakListen)
	MESSAGE_HANDLER(WM_onControlInterphoneMic, onControlInterphoneMic)
	MESSAGE_HANDLER(WM_onPublishVideo, onPublishVideo)
	MESSAGE_HANDLER(WM_onUnpublishVideo, onUnpublishVideo)
	MESSAGE_HANDLER(WM_onRequestConferenceMemberVideo, onRequestConferenceMemberVideo)
	MESSAGE_HANDLER(WM_onCancelConferenceMemberVideo, onCancelConferenceMemberVideo)
	MESSAGE_HANDLER(WM_TIMER, OnTimer)
	MESSAGE_HANDLER(WM_onVncViewerCallBack, onVncViewerCallBack)
	DEFAULT_REFLECTION_HANDLER()
	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);


CComQIPtr<IHTMLDocument2> m_Document;
CComQIPtr<IDispatch> m_Script;
std::string m_UserAgent;

HRESULT get_Document(void ** pDoc){
	HRESULT hr = E_POINTER;
	if (this->m_spClientSite != NULL){

		CComQIPtr<IServiceProvider> pISP;
		if (FAILED(hr = m_spClientSite->QueryInterface(IID_IServiceProvider, (void **)&pISP))){
			LOG4CPLUS_ERROR(log, "get IServiceProvider");
		}
		else {
			CComQIPtr<IWebBrowser2> pIWebBrowser2;
			if (FAILED(hr = pISP->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void **)&pIWebBrowser2))){
				LOG4CPLUS_ERROR(log, "get WebBrowser2");
			}
			else
			{
				CComQIPtr<IDispatch> pqiDisp;
				if (FAILED(hr = pIWebBrowser2->get_Document(&pqiDisp))){
					LOG4CPLUS_ERROR(log, "get Document");
				}
				else{
					if (FAILED(hr = pqiDisp->QueryInterface(IID_IHTMLDocument2, pDoc))){
						LOG4CPLUS_ERROR(log, "get HTMLDocument");
					}
				}
			}
		}
	}
	return hr;
}

HRESULT get_userAgent(std::string & sUserAgent){
	
	HRESULT hr = E_POINTER;
	CComQIPtr<IHTMLWindow2>pWindow;
	if (m_Document == NULL)
	{
		get_Document((void **)&m_Document);
	}
	if (m_Document)
	{
		if (FAILED(hr = m_Document->get_parentWindow(&pWindow))){
			LOG4CPLUS_ERROR(log, "get parent Window");
		}
		else{
			CComQIPtr<IOmNavigator>pNavigator;
			if (FAILED(hr = pWindow->get_navigator(&pNavigator))){
				LOG4CPLUS_ERROR(log, "get Navigator");
			}
			else{
				BSTR  userAgent = NULL;
				if (SUCCEEDED(hr = pNavigator->get_userAgent(&userAgent)))
				{
					sUserAgent = (const char *)_bstr_t(userAgent);
					SysFreeString(userAgent);
				}
			}
		}
	}
	
	return hr;
}

HRESULT get_Script(IDispatch ** script){
	HRESULT hr = E_POINTER;
	if (m_Document == NULL)
	{
		get_Document((void **)&m_Document);
	}

	if (m_Document){
		hr = m_Document->get_Script(script);
	}

	return hr;
}

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* const arr[] =
		{
			&IID_IATLECSDK,
		};

		for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
		{
			if (InlineIsEqualGUID(*arr[i], riid))
				return S_OK;
		}
		return S_FALSE;
	}

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IATLECSDK

	enum { IDD = IDD_ATLECSDK };

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}
	LRESULT onLoginfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onLogOut(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled); 
	LRESULT onConnectState(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onSetPersonInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onGetPersonInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onGetUserState(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onPublishPresence(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onReceiveFriendsPublishPresence(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onSoftVersion(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onGetOnlineMultiDevice(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onReceiveMultiDeviceState(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	LRESULT onMakeCallBack(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onCallEvents(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onCallIncoming(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onDtmfReceived(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onSwitchCallMediaTypeRequest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onSwitchCallMediaTypeResponse(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onRemoteVideoRatio(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled); 

	LRESULT onOfflineMessageCount(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onGetOfflineMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onOfflineMessageComplete(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onReceiveOfflineMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onReceiveMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onReceiveFile(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onDownloadFileComplete(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onRateOfProgressAttach(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onSendTextMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onSendMediaFile(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onDeleteMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onOperateMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onReceiveOpreateNoticeMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onUploadVTMFileOrBuf(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onCreateGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onDismissGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onQuitGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onJoinGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onReplyRequestJoinGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onInviteJoinGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onReplyInviteJoinGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onQueryOwnGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onQueryGroupDetail(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onModifyGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onSearchPublicGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onQueryGroupMember(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onDeleteGroupMember(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onQueryGroupMemberCard(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onModifyGroupMemberCard(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onForbidMemberSpeakGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onSetGroupMessageRule(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onReceiveGroupNoticeMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onRecordingTimeOut(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onFinishedPlaying(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onRecordingAmplitude(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onMeetingIncoming(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onReceiveInterphoneMeetingMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled); 
	LRESULT onReceiveVoiceMeetingMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled); 
	LRESULT onReceiveVideoMeetingMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled); 
	LRESULT onCreateMultimediaMeeting(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onJoinMeeting(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onExitMeeting(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onQueryMeetingMembers(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onDismissMultiMediaMeeting(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onQueryMultiMediaMeetings(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onDeleteMemberMultiMediaMeeting(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onInviteJoinMultiMediaMeeting(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onCreateInterphoneMeeting(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onExitInterphoneMeeting(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled); 
	LRESULT onSetMeetingSpeakListen(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onControlInterphoneMic(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onPublishVideo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onUnpublishVideo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onRequestConferenceMemberVideo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT onCancelConferenceMemberVideo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

private:
	log4cplus::Logger log;

	CCPVncServer *m_VncServer;
	INT  m_VncRoomID;
	EVncServerState m_VncServerState;
	EVncViewerState m_VncViewerState;


	CCPVncViewer *m_VncViewer;
	HANDLE m_VncViewerWnd;
public:
	virtual HWND  gethWnd() const
	{
		return this->m_hWnd;
	}
	STDMETHOD(Initialize)(LONG* result);
	STDMETHOD(UnInitialize)(LONG* result);
	STDMETHOD(Login)(IDispatch* loginInfo, LONG* result);
	STDMETHOD(SetTraceFlag)(LONG level, BSTR logFileName);
	STDMETHOD(SetServerEnvType)(LONG nEnvType, BSTR xmlFileName, LONG* result);
	STDMETHOD(SetInternalDNS)(VARIANT_BOOL enable, BSTR dns, LONG port, LONG* result);
	STDMETHOD(Logout)(LONG* result);
	STDMETHOD(SetPersonInfo)(IDispatch* matchKey, IDispatch* psersonInfo, LONG* result);
	STDMETHOD(GetPersonInfo)(IDispatch* matchKey, BSTR username, LONG* result);
	STDMETHOD(GetUserState)(IDispatch* matchKey, IDispatch* members, LONG* result);
	STDMETHOD(QueryErrorDescribe)(IDispatch* errCodeDes, LONG errCode, LONG* result);
	STDMETHOD(MakeCallBack)(IDispatch* matchKey, BSTR caller, BSTR called, BSTR callerDisplay, BSTR calledDisplay, LONG* result);
	STDMETHOD(MakeCall)(IDispatch* OutCallid, LONG callType, BSTR called, LONG* result);
	STDMETHOD(AcceptCall)(BSTR callId, LONG* result);
	STDMETHOD(ReleaseCall)(BSTR callId, LONG reason, LONG* result);
	STDMETHOD(SendDTMF)(BSTR callId, BSTR dtmf, LONG* result);
	STDMETHOD(GetCurrentCall)(BSTR* result);
	STDMETHOD(SetMute)(VARIANT_BOOL mute, LONG* result);
	STDMETHOD(GetMuteStatus)(VARIANT_BOOL* result);
	STDMETHOD(SetSoftMute)(BSTR callid, VARIANT_BOOL on, LONG* result);
	STDMETHOD(GetSoftMuteStatus)(BSTR callid, IDispatch* bMute, LONG* result);
	STDMETHOD(SetAudioConfigEnabled)(LONG type, VARIANT_BOOL enabled, LONG mode, LONG* result);
	STDMETHOD(GetAudioConfigEnabled)(LONG type, IDispatch* enabled, IDispatch* mode, LONG* result);
	STDMETHOD(SetCodecEnabled)(LONG type, VARIANT_BOOL enabled, LONG* result);
	STDMETHOD(GetCodecEnabled)(LONG type, VARIANT_BOOL* result);
	STDMETHOD(SetCodecNack)(VARIANT_BOOL bAudioNack, VARIANT_BOOL bVideoNack, LONG* result);
	STDMETHOD(GetCodecNack)(IDispatch* bAudioNack, IDispatch* bVideoNack, LONG* result);
	STDMETHOD(GetCallStatistics)(BSTR callId, VARIANT_BOOL bVideo, IDispatch* statistics, LONG* result);
	STDMETHOD(SetVideoView)(LONG view, LONG localView, LONG* result);
	STDMETHOD(SetVideoBitRates)(LONG bitrates);
	STDMETHOD(SetRing)(BSTR fileName, LONG* result);
	STDMETHOD(SetRingback)(BSTR fileName, LONG* result);
	STDMETHOD(RequestSwitchCallMediaType)(BSTR callId, LONG video, LONG* result);
	STDMETHOD(ResponseSwitchCallMediaType)(BSTR callId, LONG video, LONG* result);
	STDMETHOD(GetCameraInfo)(IDispatch* cameraInfo, LONG* result);
	STDMETHOD(SelectCamera)(LONG cameraIndex, LONG capabilityIndex, LONG fps, LONG rotate, VARIANT_BOOL force, LONG* result);
	STDMETHOD(GetNetworkStatistic)(BSTR callId, IDispatch* duration, IDispatch* sendTotalWifi, IDispatch* recvTotalWifi, LONG* result);
	STDMETHOD(GetSpeakerVolume)(IDispatch* volume, LONG* result);
	STDMETHOD(SetSpeakerVolume)(LONG volume, LONG* result);
	STDMETHOD(SetDtxEnabled)(VARIANT_BOOL enabled);
	STDMETHOD(SetSelfPhoneNumber)(BSTR phoneNumber);
	STDMETHOD(SetSelfName)(BSTR nickName);
	STDMETHOD(EnableLoudsSpeaker)(VARIANT_BOOL enable, LONG* result);
	STDMETHOD(GetLoudsSpeakerStatus)(VARIANT_BOOL* result);
	STDMETHOD(ResetVideoView)(BSTR callid, LONG remoteView, LONG localView, LONG* result);
	STDMETHOD(SendTextMessage)(IDispatch* matchKey, BSTR receiver, BSTR message, LONG type, BSTR userdata, IDispatch* msgId, LONG* result);
	STDMETHOD(SendMediaMessage)(IDispatch* matchKey, BSTR receiver, BSTR fileName, LONG type, BSTR userdata, IDispatch* msgId, LONG* result);
	STDMETHOD(DownloadFileMessage)(IDispatch* matchKey, BSTR sender, BSTR recv, BSTR url, BSTR fileName, BSTR msgId, LONG type, LONG* result);
	STDMETHOD(CancelUploadOrDownloadNOW)(ULONG matchKey);
	STDMETHOD(DeleteMessage)(IDispatch * matchKey, LONG type, BSTR msgId, LONG* result);
	STDMETHOD(OperateMessage)(IDispatch * matchKey, BSTR version, BSTR msgId, LONG type, LONG* result);
	STDMETHOD(CreateGroup)(IDispatch* matchKey, BSTR groupName, LONG type, BSTR province, BSTR city, LONG scope, BSTR declared, LONG permission, LONG isDismiss, BSTR groupDomain, VARIANT_BOOL isDiscuss, LONG* result);
	STDMETHOD(DismissGroup)(IDispatch* matchKey, BSTR groupid, LONG* result);
	STDMETHOD(QuitGroup)(IDispatch* matchKey, BSTR groupid, LONG* result);
	STDMETHOD(JoinGroup)(IDispatch* matchKey, BSTR groupid, BSTR declared, LONG * result);
	STDMETHOD(ReplyRequestJoinGroup)(IDispatch* matchKey, BSTR groupId, BSTR member, LONG confirm, LONG* result);
	STDMETHOD(InviteJoinGroup)(IDispatch* matchKey, BSTR groupId, BSTR declard, IDispatch* members, LONG confirm, LONG* result);
	STDMETHOD(ReplyInviteJoinGroup)(IDispatch* matchKey, BSTR groupId, BSTR invitor, LONG confirm, LONG* result);
	STDMETHOD(QueryOwnGroup)(IDispatch* matchKey, BSTR borderGroupid, LONG pageSize, LONG target, LONG* result);
	STDMETHOD(QueryGroupDetail)(IDispatch* matchKey, BSTR groupId, LONG* result);
	STDMETHOD(ModifyGroup)(IDispatch* matchKey, IDispatch* groupInfo, LONG* result);
	STDMETHOD(SearchPublicGroups)(IDispatch* matchKey, LONG searchType, BSTR keyword, LONG pageNo, LONG pageSize, LONG* result);
	STDMETHOD(QueryGroupMember)(IDispatch* matchKey, BSTR groupId, BSTR borderMember, LONG pageSize, LONG* result);
	STDMETHOD(DeleteGroupMember)(IDispatch* matchKey, BSTR groupId, BSTR member, LONG* result);
	STDMETHOD(QueryGroupMemberCard)(IDispatch* matchKey, BSTR groupid, BSTR member, LONG * result);
	STDMETHOD(ModifyMemberCard)(IDispatch* matchKey, IDispatch* card, LONG* result);
	STDMETHOD(ForbidMemberSpeak)(IDispatch* matchKey, BSTR groupid, BSTR member, LONG isBan, LONG* result);
	STDMETHOD(SetGroupMessageRule)(IDispatch* matchKey, BSTR groupid, VARIANT_BOOL notice, LONG* result);
	STDMETHOD(StartVoiceRecording)(BSTR fileName, BSTR sessionId, LONG* result);
	STDMETHOD(StopVoiceRecording)();
	STDMETHOD(PlayVoiceMsg)(BSTR fileName, BSTR sessionId, LONG* result);
	STDMETHOD(StopVoiceMsg)();
	STDMETHOD(GetVoiceDuration)(BSTR fileName, IDispatch* duration, LONG* result);
	STDMETHOD(RequestConferenceMemberVideo)(BSTR conferenceId, BSTR conferencePassword, BSTR member, LONG videoWindow, LONG type, LONG* result);
	STDMETHOD(CancelMemberVideo)(BSTR conferenceId, BSTR conferencePassword, BSTR member, LONG type, LONG* result);
	STDMETHOD(ResetVideoConfWindow)(BSTR conferenceId, BSTR member, LONG newWindow, LONG type, LONG* result);
	STDMETHOD(CreateMultimediaMeeting)(IDispatch* matchKey, LONG meetingType, BSTR meetingName, BSTR password, BSTR keywords, LONG voiceMode, LONG square, VARIANT_BOOL bAutoJoin, VARIANT_BOOL autoClose, VARIANT_BOOL autoDelete, LONG* result);
	STDMETHOD(JoinMeeting)(LONG meetingType, BSTR conferenceId, BSTR password, LONG* result);
	STDMETHOD(ExitMeeting)(BSTR conferenceId, LONG* result);
	STDMETHOD(QueryMeetingMembers)(IDispatch* matchKey, LONG meetingType, BSTR conferenceId, LONG* result);
	STDMETHOD(DismissMultiMediaMeeting)(IDispatch* matchKey, LONG meetingType, BSTR conferenceId, LONG* result);
	STDMETHOD(QueryMultiMediaMeetings)(IDispatch* matchKey, LONG meetingType, BSTR keywords, LONG* result);
	STDMETHOD(InviteJoinMultiMediaMeeting)(IDispatch* matchKey, BSTR conferenceId, IDispatch* members, VARIANT_BOOL blanding, VARIANT_BOOL isSpeak, VARIANT_BOOL isListen, BSTR disNumber, BSTR userData, LONG* result);
	STDMETHOD(DeleteMemberMultiMediaMeeting)(IDispatch* matchKey, LONG meetingType, BSTR conferenceId, BSTR member, VARIANT_BOOL isVoIP, LONG* result);
	STDMETHOD(PublishVideo)(IDispatch* matchKey, BSTR conferenceId, LONG* result);
	STDMETHOD(UnpublishVideo)(IDispatch* matchKey, BSTR conferenceId, LONG* result);
	STDMETHOD(CreateInterphoneMeeting)(IDispatch* matchKey, IDispatch* members, LONG* result);
	STDMETHOD(ControlInterphoneMic)(IDispatch* matchKey, VARIANT_BOOL isControl, BSTR interphoneId, LONG* result);
	STDMETHOD(get_version)(BSTR* pVal);


	STDMETHOD(SetDesktopWnd)(OLE_HANDLE  hWnd, LONG * result);
	STDMETHOD(AcceptShareDesktop)(SHORT * result);
	STDMETHOD(AcceptRemoteControl)(SHORT * result);
	STDMETHOD(mediaTerminalRejectShareDesktop)(SHORT * result);
	STDMETHOD(mediaTerminalRejectRemoteControl)(SHORT * result);
	STDMETHOD(mediaTerminalRequestRemoteControl)(SHORT * result);

	STDMETHOD(RequestShareDesktop)(BSTR remoteVoIP, BSTR serverIP, LONG serverPort, OLE_HANDLE vncWND, LONG* result);
	STDMETHOD(StopShareDesktop)(LONG* result);
	STDMETHOD(RequestRemoteControl)(LONG* result);
	STDMETHOD(StopRemoteControl)(LONG* result);
	STDMETHOD(mediaTellerRefreshDesktopWnd)(LONG* result);
	STDMETHOD(mediaTellerRefreshDesktopWndByCoordinate)(INT x, INT y, LONG* result);
	STDMETHOD(mediaTellerSetShareDesktopDisplaySize)(INT w, INT h, LONG* result);
	STDMETHOD(mediaTellerRejectRemoteControl)(LONG* result);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);


	virtual void OnVncServerCreateRoom(int roomID, int status);
	virtual void OnVncServerRoomClosed(int roomID, int status);
	virtual void OnVncServerJoinRoom(int roomID, int status);
	virtual void OnVncServerQuitRoom(int roomID, int status);

	virtual void ServerQuitRoom(int roomID);
	//virtual void OnVncServerReqRemoteControl(int roomID, int status);
	//virtual void OnVncServerRespRemoteControl(int roomID, int status);

	virtual void OnVncRecvReqRemoteControl(int roomID);
	virtual void OnVncCancelReqRemoteControl(int roomID);
	virtual void OnVncStopRemoteControl(int roomID);
	virtual void OnVncAcceptRemoteControl(int roomID);
	virtual void OnVncRejectRemoteControl(int roomID);

	virtual void OnVncViewerCreateRoom(int roomID, int status);
	virtual void OnVncViewerRoomClosed(int roomID, int status);
	virtual void OnVncViewerJoinRoom(int roomID, int status);
	virtual void OnVncViewerQuitRoom(int roomID, int status);

	virtual void ViewerQuitRoom(int roomID);

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onVncViewerCallBack(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

private:
	int handleVncCmd(const string & cmdStr);

public:
	STDMETHOD(GetLocalVideoSnapshotEx)(BSTR callid, IDispatch* pictureData, IDispatch* size, IDispatch* width, IDispatch* height, LONG* result);
	STDMETHOD(GetRemoteVideoSnapshotEx)(BSTR callid, IDispatch* pictureData, IDispatch* size, IDispatch* width, IDispatch* height, LONG* result);
	STDMETHOD(StartRecordScreen)(BSTR callid, BSTR fileName, LONG bitrate, LONG fps, LONG type, LONG* result);
	STDMETHOD(StartRecordScreenEx)(BSTR callid, BSTR fileName, LONG bitrate, LONG fps, LONG type, LONG left, LONG top, LONG width, LONG height, LONG* result);
	STDMETHOD(StopRecordScreen)(BSTR callid, LONG* result);
    STDMETHOD(SetServerAddress)(BSTR connector, LONG connectorPort, BSTR fileServer, LONG fileSeverPort, LONG* result);
	STDMETHOD(UploadVTMFile)(IDispatch* matchKey, BSTR companyId, BSTR companyPwd, BSTR fileName, BSTR callbackUrl, LONG* result);
	STDMETHOD(UploadVTMBuf)(IDispatch* matchKey, BSTR companyId, BSTR companyPwd, BSTR Name, BSTR buf, BSTR callbackUrl, LONG* result);
	STDMETHOD(get_appDataDir)(BSTR* pVal);
};

OBJECT_ENTRY_AUTO(__uuidof(ATLECSDK), CATLECSDK)
