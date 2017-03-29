#ifndef npapi_plugin_base_h_
#define npapi_plugin_base_h_

#include "npapi.h"
#include "npfunctions.h"

#ifdef XP_WIN
#include <windows.h>
#include <windowsx.h>
#endif


#ifdef XP_MAC
#include <TextEdit.h>
#endif

#ifdef XP_UNIX
#include <string.h>
#endif

#include <string>

#define  NPVARIANT_TO_STRING_SAFE(_v,_str)                            \
(_str).assign(NPVARIANT_TO_STRING(_v).UTF8Characters,                 \
NPVARIANT_TO_STRING(_v).UTF8Length)						              \

void NPVARIANT_TO_INT32_SAFE(const NPVariant &v, int32_t &result);
void NPVARIANT_TO_BOOLEAN_SAFE(const NPVariant &v, bool &result);
void NPVARIANT_TO_DOUBLE_SAFE(const NPVariant &v, double &result);

class CNPVariant:
	public NPVariant{
public:
	explicit CNPVariant()throw()
	{
		VOID_TO_NPVARIANT(*this);
	}
	~CNPVariant()throw()
	{
	}

	CNPVariant(const std::string & str)throw()
	{
		char* npOutString = (char *)NPN_MemAlloc(str.length() + 1);
		if (str.length()==0){
			npOutString[0] = '\0';
		}
		else{
			strcpy_s(npOutString, str.length()+1, str.c_str());
		}
		STRINGZ_TO_NPVARIANT(npOutString, *this);
	}

	CNPVariant(const char * src)throw()
	{
		unsigned int len = strlen(src);

		char* npOutString = (char *)NPN_MemAlloc(len + 1);
		if (len == 0){
			npOutString[0] = '\0';
		}
		else{
			strcpy_s(npOutString, len+1, src);
		}
		STRINGN_TO_NPVARIANT(npOutString, len, *this);
	}

	CNPVariant(unsigned int intValue) throw()
	{
		INT32_TO_NPVARIANT(intValue,*this);
	}

	CNPVariant(int intValue) throw()
	{
		INT32_TO_NPVARIANT(intValue, *this);
	}

	CNPVariant(bool bValue) throw()
	{
		BOOLEAN_TO_NPVARIANT(bValue, *this);
	}

	CNPVariant(unsigned long long ulValue) throw()
	{
		INT32_TO_NPVARIANT(ulValue, *this);
	}

	CNPVariant(double doubleValue) throw()
	{
		DOUBLE_TO_NPVARIANT(doubleValue, *this);
	}
};

class NPAPIPluginBase
{
public:
	explicit NPAPIPluginBase(NPP pNPInstance);
	virtual ~NPAPIPluginBase(void);

protected:
#ifdef XP_WIN
	HWND m_hWnd;
#endif

	NPP m_pNPInstance;

	NPWindow * m_Window;

	NPBool m_bInitialized;

	NPObject *m_pScriptableObject;
	NPObject *m_pBrowserWindow;
#ifdef XP_WIN
	WNDPROC lpOldProc;
#endif

public:

	virtual NPBool init(NPWindow* pNPWindow);
	virtual void shut();
	virtual NPBool isInitialized();

	virtual int16_t handleEvent(void* event);

	NPObject *GetScriptableObject();
	virtual const char * getName()const = 0;
	virtual const char * getDescription()const = 0;
	bool JSEvaluate(NPString *script, NPVariant *result);
	bool JSNewObject(NPVariant * result);
	bool JSNewArray(NPVariant * result);

protected:
	virtual bool HasMethod(const NPIdentifier name) const;
	virtual bool HasProperty(const NPIdentifier name) const;
	virtual bool GetProperty(const NPIdentifier name, NPVariant *result) const;
	virtual bool SetProperty(const NPIdentifier name, const NPVariant *value);
	virtual bool RemoveProperty(const NPIdentifier name) const;
	virtual bool Invoke(const NPIdentifier name, const NPVariant *args,
		uint32_t argCount, NPVariant *result);

	friend class ScriptablePluginObject;

public:
	virtual LRESULT CALLBACK PluginWinProc(HWND, UINT, WPARAM, LPARAM);
};
#endif
