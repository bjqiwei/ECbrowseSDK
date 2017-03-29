// dllmain.h : Declaration of module class.

class CECBrowserPluginModule : public ATL::CAtlDllModuleT< CECBrowserPluginModule >
{
public :
	DECLARE_LIBID(LIBID_ECBrowserPluginLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ECBROWSERPLUGIN, "{A3281AE1-B53C-4FCE-AE99-953C4916F1DB}")
};

extern class CECBrowserPluginModule _AtlModule;
