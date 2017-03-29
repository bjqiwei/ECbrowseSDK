#pragma once
#include "NPAPIPluginBase.h"
#include <log4cplus/logger.h>
#include <map>

class CChildWindow;
class NPAPIVideoObject:
		public NPAPIPluginBase
{
public:
	explicit NPAPIVideoObject(NPP pNPInstance);
	virtual ~NPAPIVideoObject(void);

private:
	log4cplus::Logger log;
	std::map<HWND, CChildWindow*>m_ChildWindow;
	//propety id 
	NPIdentifier pHWND_id;


	//function id
	NPIdentifier fCreateChild_id;
	NPIdentifier fRemoveChild_id;
	NPIdentifier fMoveWindow_id;

public:
	virtual NPBool init(NPWindow* pNPWindow);	
	virtual void shut();
	virtual const char * getName()const
	{
		return "Cloopen Video Object Plugin.";
	}
	virtual const char * getDescription()const
	{
		return "Cloopen Video Object Plugin for FireFox or Chrome.";
	}

private:
	virtual bool HasProperty(const NPIdentifier name) const;
	virtual bool GetProperty(const NPIdentifier name, NPVariant *result) const;

	virtual bool HasMethod(const NPIdentifier name) const;
	virtual bool Invoke(const NPIdentifier name, const NPVariant *args,
		uint32_t argCount, NPVariant *result);
};

