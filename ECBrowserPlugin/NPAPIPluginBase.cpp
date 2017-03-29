#include "stdafx.h"
#include "NPAPIPluginBase.h"



// Helper class that can be used to map calls to the NPObject hooks
// into virtual methods on instances of classes that derive from this
// class.
class ScriptablePluginObjectBase : public NPObject
{
public:
	explicit ScriptablePluginObjectBase(NPP npp)
		: mNpp(npp)
	{
	}

	virtual ~ScriptablePluginObjectBase()
	{
	}

	// Virtual NPObject hooks called through this base class. Override
	// as you see fit.
	virtual void Invalidate();
	virtual bool HasMethod(NPIdentifier name);
	virtual bool Invoke(NPIdentifier name, const NPVariant *args,
		uint32_t argCount, NPVariant *result);
	virtual bool InvokeDefault(const NPVariant *args, uint32_t argCount,
		NPVariant *result);
	virtual bool HasProperty(NPIdentifier name);
	virtual bool GetProperty(NPIdentifier name, NPVariant *result);
	virtual bool SetProperty(NPIdentifier name, const NPVariant *value);
	virtual bool RemoveProperty(NPIdentifier name);
	virtual bool Enumerate(NPIdentifier **identifier, uint32_t *count);
	virtual bool Construct(const NPVariant *args, uint32_t argCount,
		NPVariant *result);

public:
	static void _Deallocate(NPObject *npobj);
	static void _Invalidate(NPObject *npobj);
	static bool _HasMethod(NPObject *npobj, NPIdentifier name);
	static bool _Invoke(NPObject *npobj, NPIdentifier name,
		const NPVariant *args, uint32_t argCount,
		NPVariant *result);
	static bool _InvokeDefault(NPObject *npobj, const NPVariant *args,
		uint32_t argCount, NPVariant *result);
	static bool _HasProperty(NPObject * npobj, NPIdentifier name);
	static bool _GetProperty(NPObject *npobj, NPIdentifier name,
		NPVariant *result);
	static bool _SetProperty(NPObject *npobj, NPIdentifier name,
		const NPVariant *value);
	static bool _RemoveProperty(NPObject *npobj, NPIdentifier name);
	static bool _Enumerate(NPObject *npobj, NPIdentifier **identifier,
		uint32_t *count);
	static bool _Construct(NPObject *npobj, const NPVariant *args,
		uint32_t argCount, NPVariant *result);

protected:
	NPP mNpp;
};

#define DECLARE_NPOBJECT_CLASS_WITH_BASE(_class, ctor)                        \
	static NPClass s##_class##_NPClass = {                                        \
	NP_CLASS_STRUCT_VERSION_CTOR,                                               \
	ctor,                                                                       \
	ScriptablePluginObjectBase::_Deallocate,                                    \
	ScriptablePluginObjectBase::_Invalidate,                                    \
	ScriptablePluginObjectBase::_HasMethod,                                     \
	ScriptablePluginObjectBase::_Invoke,                                        \
	ScriptablePluginObjectBase::_InvokeDefault,                                 \
	ScriptablePluginObjectBase::_HasProperty,                                   \
	ScriptablePluginObjectBase::_GetProperty,                                   \
	ScriptablePluginObjectBase::_SetProperty,                                   \
	ScriptablePluginObjectBase::_RemoveProperty,                                \
	ScriptablePluginObjectBase::_Enumerate,                                     \
	ScriptablePluginObjectBase::_Construct                                      \
}

#define GET_NPOBJECT_CLASS(_class) &s##_class##_NPClass

void
	ScriptablePluginObjectBase::Invalidate()
{
}

bool
	ScriptablePluginObjectBase::HasMethod(NPIdentifier name)
{
	return false;
}

bool
	ScriptablePluginObjectBase::Invoke(NPIdentifier name, const NPVariant *args,
	uint32_t argCount, NPVariant *result)
{
	return false;
}

bool
	ScriptablePluginObjectBase::InvokeDefault(const NPVariant *args,
	uint32_t argCount, NPVariant *result)
{
	return false;
}

bool
	ScriptablePluginObjectBase::HasProperty(NPIdentifier name)
{
	return false;
}

bool
	ScriptablePluginObjectBase::GetProperty(NPIdentifier name, NPVariant *result)
{
	return false;
}

bool
	ScriptablePluginObjectBase::SetProperty(NPIdentifier name,
	const NPVariant *value)
{
	return false;
}

bool
	ScriptablePluginObjectBase::RemoveProperty(NPIdentifier name)
{
	return false;
}

bool
	ScriptablePluginObjectBase::Enumerate(NPIdentifier **identifier,
	uint32_t *count)
{
	return false;
}

bool
	ScriptablePluginObjectBase::Construct(const NPVariant *args, uint32_t argCount,
	NPVariant *result)
{
	return false;
}

// static
void
	ScriptablePluginObjectBase::_Deallocate(NPObject *npobj)
{
	// Call the virtual destructor.
	delete static_cast<ScriptablePluginObjectBase *>(npobj);
}

// static
void
	ScriptablePluginObjectBase::_Invalidate(NPObject *npobj)
{
	(static_cast<ScriptablePluginObjectBase *>(npobj))->Invalidate();
}

// static
bool
	ScriptablePluginObjectBase::_HasMethod(NPObject *npobj, NPIdentifier name)
{
	return (static_cast<ScriptablePluginObjectBase *>(npobj))->HasMethod(name);
}

// static
bool
	ScriptablePluginObjectBase::_Invoke(NPObject *npobj, NPIdentifier name,
	const NPVariant *args, uint32_t argCount,
	NPVariant *result)
{
	return (static_cast<ScriptablePluginObjectBase *>(npobj))->Invoke(name, args, argCount,
		result);
}

// static
bool
	ScriptablePluginObjectBase::_InvokeDefault(NPObject *npobj,
	const NPVariant *args,
	uint32_t argCount,
	NPVariant *result)
{
	return (static_cast<ScriptablePluginObjectBase *>(npobj))->InvokeDefault(args, argCount,
		result);
}

// static
bool
	ScriptablePluginObjectBase::_HasProperty(NPObject * npobj, NPIdentifier name)
{
	return (static_cast<ScriptablePluginObjectBase *>(npobj))->HasProperty(name);
}

// static
bool
	ScriptablePluginObjectBase::_GetProperty(NPObject *npobj, NPIdentifier name,
	NPVariant *result)
{
	return (static_cast<ScriptablePluginObjectBase *>(npobj))->GetProperty(name, result);
}

// static
bool
	ScriptablePluginObjectBase::_SetProperty(NPObject *npobj, NPIdentifier name,
	const NPVariant *value)
{
	return (static_cast<ScriptablePluginObjectBase *>(npobj))->SetProperty(name, value);
}

// static
bool
	ScriptablePluginObjectBase::_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
	return (static_cast<ScriptablePluginObjectBase *>(npobj))->RemoveProperty(name);
}

// static
bool
	ScriptablePluginObjectBase::_Enumerate(NPObject *npobj,
	NPIdentifier **identifier,
	uint32_t *count)
{
	return (static_cast<ScriptablePluginObjectBase *>(npobj))->Enumerate(identifier, count);
}

// static
bool
	ScriptablePluginObjectBase::_Construct(NPObject *npobj, const NPVariant *args,
	uint32_t argCount, NPVariant *result)
{
	return (static_cast<ScriptablePluginObjectBase *>(npobj))->Construct(args, argCount,
		result);
}


class ScriptablePluginObject : public ScriptablePluginObjectBase
{
public:
	explicit ScriptablePluginObject(NPP npp)
		: ScriptablePluginObjectBase(npp)
	{
	}

	virtual bool HasMethod(NPIdentifier name);
	virtual bool HasProperty(NPIdentifier name);
	virtual bool GetProperty(NPIdentifier name, NPVariant *result);
	virtual bool SetProperty(NPIdentifier name, const NPVariant *value);
	virtual bool RemoveProperty(NPIdentifier name);
	virtual bool Invoke(NPIdentifier name, const NPVariant *args,
		uint32_t argCount, NPVariant *result);
private:
};

static NPObject *
	AllocateScriptablePluginObject(NPP npp, NPClass *aClass)
{
	return new ScriptablePluginObject(npp);
}

DECLARE_NPOBJECT_CLASS_WITH_BASE(ScriptablePluginObject,
AllocateScriptablePluginObject);

bool
	ScriptablePluginObject::HasMethod(NPIdentifier name)
{
	return (static_cast<NPAPIPluginBase *>(mNpp->pdata))->HasMethod(name);
}

bool
	ScriptablePluginObject::HasProperty(NPIdentifier name)
{
	return (static_cast<NPAPIPluginBase *>(mNpp->pdata))->HasProperty(name);
}

bool
	ScriptablePluginObject::GetProperty(NPIdentifier name, NPVariant *result)
{
	return (static_cast<NPAPIPluginBase *>(mNpp->pdata))->GetProperty(name, result);
}

bool
	ScriptablePluginObject::SetProperty(NPIdentifier name, const NPVariant *value)
{
	return (static_cast<NPAPIPluginBase *>(mNpp->pdata))->SetProperty(name, value);
}


bool ScriptablePluginObject::RemoveProperty(NPIdentifier name)
{
	return (static_cast<NPAPIPluginBase *>(mNpp->pdata))->RemoveProperty(name);
}

bool
	ScriptablePluginObject::Invoke(NPIdentifier name, const NPVariant *args,
	uint32_t argCount, NPVariant *result)
{
	return (static_cast<NPAPIPluginBase *>(mNpp->pdata))->Invoke(name,args,argCount,result);
}


NPAPIPluginBase::NPAPIPluginBase(NPP pNPInstance):
#ifdef XP_WIN
m_hWnd(NULL),
#endif
	m_pNPInstance(pNPInstance),
	m_Window(NULL),
	m_bInitialized(FALSE),
	m_pScriptableObject(NULL),
	m_pBrowserWindow(NULL)
#ifdef XP_WIN
	,lpOldProc(NULL)
#endif
{
	NPN_GetValue(pNPInstance, NPNVWindowNPObject, &m_pBrowserWindow);
}


NPAPIPluginBase::~NPAPIPluginBase(void)
{
	NPN_ReleaseObject(m_pScriptableObject);
	NPN_ReleaseObject(m_pBrowserWindow);
}

#ifdef XP_WIN
static LRESULT CALLBACK PluginWinProc(HWND, UINT, WPARAM, LPARAM);
#endif

NPBool NPAPIPluginBase::init(NPWindow* pNPWindow)
{
	if(pNPWindow == NULL)
		return FALSE;

#ifdef XP_WIN
	m_hWnd = (HWND)pNPWindow->window;
	if(m_hWnd == NULL)
		return FALSE;

	// subclass window so we can intercept window messages and
	// do our drawing to it
	lpOldProc = SubclassWindow(m_hWnd, (WNDPROC)::PluginWinProc);

	// associate window with our CPlugin object so we can access 
	// it in the window procedure
	::SetWindowLong(m_hWnd, GWLP_USERDATA, (LONG)this);
#endif

	m_Window = pNPWindow;

	m_bInitialized = TRUE;
	return TRUE;
}
void NPAPIPluginBase::shut()
{
#ifdef XP_WIN
	// subclass it back
	SubclassWindow(m_hWnd, lpOldProc);
	m_hWnd = NULL;
#endif

	m_bInitialized = FALSE;
}
NPBool NPAPIPluginBase::isInitialized()
{
	return m_bInitialized;
}

int16_t NPAPIPluginBase::handleEvent(void* event)
{
#ifdef XP_MAC
	NPEvent* ev = (NPEvent*)event;
	if (m_Window) {
		Rect box = { m_Window->y, m_Window->x,
			m_Window->y + m_Window->height, m_Window->x + m_Window->width };
		if (ev->what == updateEvt) {
			::TETextBox(m_String, strlen(m_String), &box, teJustCenter);
		}
	}
#endif
	return 0;
}

NPObject *NPAPIPluginBase::GetScriptableObject()
{
	if (!m_pScriptableObject) {
		m_pScriptableObject =
			NPN_CreateObject(m_pNPInstance,
			GET_NPOBJECT_CLASS(ScriptablePluginObject));
	}

	if (m_pScriptableObject) {
		NPN_RetainObject(m_pScriptableObject);
	}
	return m_pScriptableObject;
}

bool NPAPIPluginBase::JSEvaluate(NPString *script, NPVariant *result)
{
	return NPN_Evaluate(m_pNPInstance, m_pBrowserWindow, script, result);
}

bool NPAPIPluginBase::JSNewObject(NPVariant * result)
{
	const char * strObject = "Object();";
	NPString npStr;
	npStr.UTF8Characters = strObject;
	npStr.UTF8Length = strlen(strObject);
	return NPN_Evaluate(m_pNPInstance, m_pBrowserWindow, &npStr, result);
}

bool NPAPIPluginBase::JSNewArray(NPVariant * result)
{
	const char * strArray = "Array();";
	NPString npStr;
	npStr.UTF8Characters = strArray;
	npStr.UTF8Length = strlen(strArray);
	return NPN_Evaluate(m_pNPInstance, m_pBrowserWindow, &npStr, result);
}

bool NPAPIPluginBase::HasMethod(const NPIdentifier name) const
{
	return false;
}

bool NPAPIPluginBase::HasProperty(const NPIdentifier name) const
{
	return false;
}

bool NPAPIPluginBase::GetProperty(const NPIdentifier name, NPVariant *result)const
{
	return false;
}

bool NPAPIPluginBase::SetProperty(const NPIdentifier name, const NPVariant *value)
{
	return false;
}

bool NPAPIPluginBase::RemoveProperty(const NPIdentifier name) const
{
	return false;
}

bool NPAPIPluginBase::Invoke(const NPIdentifier name, const NPVariant *args,
	uint32_t argCount, NPVariant *result)
{
	return false;
}

LRESULT CALLBACK NPAPIPluginBase::PluginWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg) {
	case WM_PAINT:
		{
			// draw a frame 
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			RECT rc;
			GetClientRect(hWnd, &rc);
			FrameRect(hdc, &rc, GetStockBrush(BLACK_BRUSH));
			//::RedrawWindow(hWnd,NULL, NULL , 261U);
			EndPaint(hWnd, &ps);
		}
		break;

		default:
			{
			}
			break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);

}
static LRESULT CALLBACK PluginWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	NPAPIPluginBase * plugin = (NPAPIPluginBase *)GetWindowLong(hWnd, GWLP_USERDATA);
	return plugin->PluginWinProc(hWnd, msg, wParam, lParam);
}

void NPVARIANT_TO_INT32_SAFE(const NPVariant &v, int32_t &result)
{

	if (NPVARIANT_IS_INT32(v))
	{
		result = NPVARIANT_TO_INT32(v);
	}
	else if (NPVARIANT_IS_DOUBLE(v))
	{
		result = NPVARIANT_TO_DOUBLE(v);
	}
	else if (NPVARIANT_IS_STRING(v))
	{
		std::string str;
		NPVARIANT_TO_STRING_SAFE(v,str);
		if (!str.empty())
			result = std::stoi(str);
	}
	return ;
}


void NPVARIANT_TO_BOOLEAN_SAFE(const NPVariant &v, bool &result)
{
	if (NPVARIANT_IS_BOOLEAN(v))
	{
		result = NPVARIANT_TO_BOOLEAN(v);
	}
	else if (NPVARIANT_IS_INT32(v))
	{
		result = NPVARIANT_TO_INT32(v);
	}
	else if (NPVARIANT_IS_DOUBLE(v))
	{
		result = NPVARIANT_TO_DOUBLE(v);
	}
	else if (NPVARIANT_IS_STRING(v))
	{
		std::string str;
		NPVARIANT_TO_STRING_SAFE(v, str);
		if (str == "true")
		{
			result = true;
		}
		else if (str == "false")
		{
			result = false;
		}
	}
}

void NPVARIANT_TO_DOUBLE_SAFE(const NPVariant &v, double &result)
{
	if (NPVARIANT_IS_INT32(v))
	{
		result = NPVARIANT_TO_INT32(v);
	}
	else if (NPVARIANT_IS_DOUBLE(v))
	{
		result = NPVARIANT_TO_DOUBLE(v);
	}
	else if (NPVARIANT_IS_STRING(v))
	{
		std::string str;
		NPVARIANT_TO_STRING_SAFE(v, str);
		if (!str.empty())
			result = std::stod(str);
	}
	return;
}