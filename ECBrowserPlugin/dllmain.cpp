// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "ECBrowserPlugin_i.h"
#include "dllmain.h"
#include "xdlldata.h"
#include <log4cplus/logger.h>
#include <log4cplus/appender.h>
#include <log4cplus/fileappender.h>
#include <ShlObj.h>
#include <Dbghelp.h>
#include <sstream>
#include "codingHelper.h"
#include "ECSDK.h"

#pragma comment( lib, "DbgHelp" )

CECBrowserPluginModule _AtlModule;
std::wstring utf16Path;
std::string utf8AppDataDir;

#define CALL_FIRST 1  
#define CALL_LAST 0

BOOL CALLBACK MiniDumpCallback(PVOID pParam, const PMINIDUMP_CALLBACK_INPUT   pInput,
	PMINIDUMP_CALLBACK_OUTPUT pOutput)

{
	// Callback implementation

	if (pInput == NULL)
		return FALSE;

	if (pOutput == NULL)
		return FALSE;

	/*if (pInput->CallbackType == ModuleCallback)
	{

		std::wstring fullPaht = pInput->Module.FullPath;
		if (fullPaht.find(L"npECBrowserPlugin.dll") != std::wstring::npos || fullPaht.find(L"ECsdk.dll") != std::wstring::npos) {

		}
		else{
			pOutput->ModuleWriteFlags &= (~ModuleWriteDataSeg);
		}

		return TRUE;
	}*/

	return TRUE;

}

LONG WINAPI
VectoredHandler(
struct _EXCEPTION_POINTERS *ExceptionInfo
	)
{

	if (ExceptionInfo->ExceptionRecord->ExceptionCode == 0x406D1388
		|| ExceptionInfo->ExceptionRecord->ExceptionCode == 0xE06D7363
		|| ExceptionInfo->ExceptionRecord->ExceptionCode == 0x000006BA
		|| ExceptionInfo->ExceptionRecord->ExceptionCode == 0x8001010D
		|| ExceptionInfo->ExceptionRecord->ExceptionCode == 0x80010108
		|| ExceptionInfo->ExceptionRecord->ExceptionCode == 0x40010006
		|| ExceptionInfo->ExceptionRecord->ExceptionCode == 0x80000003
		|| ExceptionInfo->ExceptionRecord->ExceptionCode == 0x80000004
		|| ExceptionInfo->ExceptionRecord->ExceptionCode == 0xC0000095
		)
	{
	}
	else{
		DWORD pId = GetCurrentProcessId();
        //::MessageBox(nullptr, "", "", MB_OK);
		std::wstringstream oss;
		oss << Utf82Unicode(utf8AppDataDir) << L"\\" << pId << L"." << Utf82Unicode(ECSDK::GetVersion()) << L".dmp";
		std::wstring dmpFile = oss.str();
		HANDLE lhDumpFile = CreateFileW(dmpFile.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
		loExceptionInfo.ExceptionPointers = ExceptionInfo;
		loExceptionInfo.ThreadId = GetCurrentThreadId();
		loExceptionInfo.ClientPointers = TRUE;
		if (lhDumpFile != INVALID_HANDLE_VALUE)
		{
			MINIDUMP_CALLBACK_INFORMATION mci;

			mci.CallbackRoutine = MiniDumpCallback;
			mci.CallbackParam = NULL;     // this example does not use the context

			MiniDumpWriteDump(GetCurrentProcess(), pId, lhDumpFile, MINIDUMP_TYPE(
				MiniDumpWithDataSegs |
				MiniDumpWithUnloadedModules |
				MiniDumpWithProcessThreadData),
				&loExceptionInfo,
				NULL,
				NULL);
			CloseHandle(lhDumpFile);
			//MessageBox(NULL, "Stop", "" ,MB_OK);
		}
	}
	/*std::string info = "你所使用的坐席程序发生了一个意外错误，请将此窗口截图和文件\r\n";
	info += dmpFile;
	info += "提交给开发人员，并重启程序。";

	std::string caption = "坐席插件崩溃:";
	caption += CloopenAgentBase::GetVersion();*/
	//MessageBox(NULL, info.c_str() , caption.c_str() , MB_OK);
	return EXCEPTION_CONTINUE_SEARCH;
}

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(hInstance, dwReason, lpReserved))
		return FALSE;
#endif
	hInstance;
	static PVOID h1 = NULL;
    static DWORD threadId = 0;
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		std::string utf8Path;
		WCHAR wszPath[MAX_PATH];
		GetModuleFileNameW(hInstance, wszPath, MAX_PATH);
		utf16Path = wszPath;
		utf8Path = Unicode2Utf8(wszPath);
		// log4cplus initialize;
		std::string utf8ModuleName = utf8Path;
		utf8ModuleName = utf8ModuleName.substr(utf8ModuleName.rfind("\\"), utf8ModuleName.rfind(".") - utf8ModuleName.rfind("\\"));
		WCHAR appDir[MAX_PATH];
		SHGetSpecialFolderPathW(NULL, appDir, CSIDL_LOCAL_APPDATA, 0);
		utf8AppDataDir = Unicode2Utf8(appDir);
		utf8AppDataDir.append(utf8ModuleName);
		std::wstring wstrAppDataDir = Utf82Unicode(utf8AppDataDir);
		SHCreateDirectoryExW(NULL, wstrAppDataDir.c_str(), nullptr);
		std::string logFile;

		logFile = utf8AppDataDir + utf8ModuleName;
		log4cplus::initialize();
        threadId = GetCurrentThreadId();
		{
			log4cplus::SharedAppenderPtr _append(new log4cplus::TimeBasedRollingFileAppender(
				logFile, log4cplus::DailyRollingFileSchedule::DAILY,
				"%y-%m-%d.log",
				30,
				std::ios_base::binary | std::ios_base::app));

			_append->setName(utf8ModuleName);
			std::string pattern = "%D{%y/%m/%d %H:%M:%S.%Q} [%t] %-5p %c{3} %x -  %m;line:%L %n";
			std::auto_ptr<log4cplus::Layout> _layout(new log4cplus::PatternLayout(pattern));

			_append->setLayout(_layout);
			log4cplus::Logger root = log4cplus::Logger::getRoot();

			root.addAppender(_append);
			root.setLogLevel(log4cplus::ALL_LOG_LEVEL);
		}

		{
			logFile = utf8AppDataDir + utf8ModuleName + ".mediaDebug";
			log4cplus::Logger mediaDebug = log4cplus::Logger::getInstance("mediaDebug");
			log4cplus::SharedAppenderPtr _append(new log4cplus::TimeBasedRollingFileAppender(
				logFile, log4cplus::DailyRollingFileSchedule::DAILY,
				"%y-%m-%d.log",
				30,
				std::ios_base::binary | std::ios_base::app));
			_append->setName("mediaDebug");
			std::string pattern = "%m";
			std::auto_ptr<log4cplus::Layout> _layout(new log4cplus::PatternLayout(pattern));
			_append->setLayout(_layout);
			mediaDebug.addAppender(_append);
			mediaDebug.setAdditivity(false);
			mediaDebug.setLogLevel(log4cplus::ALL_LOG_LEVEL);
		}

		if (h1 == NULL)
		{
			//h1 = AddVectoredExceptionHandler(CALL_FIRST, VectoredHandler);
		}

	}
    else if (dwReason == DLL_THREAD_DETACH)
    {
        if (GetCurrentThreadId() == threadId)
        {
            log4cplus::unInitialize();
        }
        
    }
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		if (h1){
			RemoveVectoredExceptionHandler(h1);
			h1 = NULL;
		}
	}

	return _AtlModule.DllMain(dwReason, lpReserved);
}
