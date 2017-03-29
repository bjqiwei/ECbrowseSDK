# Microsoft Developer Studio Project File - Name="Server" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Server - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Server.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Server.mak" CFG="Server - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Server - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Server - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Server - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../omnithread" /I "../zlib" /I ".." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__WIN32__" /D "__NT__" /D "__x86__" /D "_WINSTATIC" /D "NCORBA" /D _WIN32_WINNT=0x0400 /D WINVER=0x0500 /D "_LIB" /D "_STLP_USE_STATIC_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Server - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../omnithread" /I "../zlib" /I ".." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WIN32__" /D "__NT__" /D "__x86__" /D "_WINSTATIC" /D "NCORBA" /D WINVER=0x0500 /D _WIN32_WINNT=0x0400 /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "Server - Win32 Release"
# Name "Server - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\d3des.c
# End Source File
# Begin Source File

SOURCE=.\DynamicFn.cpp
# End Source File
# Begin Source File

SOURCE=.\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\MinMax.cpp
# End Source File
# Begin Source File

SOURCE=.\PCritSec.cpp
# End Source File
# Begin Source File

SOURCE=.\PSyncPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\PWaitAndSignal.cpp
# End Source File
# Begin Source File

SOURCE=.\QueueChannel.cpp
# End Source File
# Begin Source File

SOURCE=.\RectList.cpp
# End Source File
# Begin Source File

SOURCE=.\stdhdrs.cpp
# End Source File
# Begin Source File

SOURCE=.\tableinitcmtemplate.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\tableinittctemplate.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\tabletranstemplate.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\translate.cpp
# End Source File
# Begin Source File

SOURCE=.\TsSessions.cpp
# End Source File
# Begin Source File

SOURCE=.\VideoDriver.cpp
# End Source File
# Begin Source File

SOURCE=.\vncBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\vncClient.cpp
# End Source File
# Begin Source File

SOURCE=.\vncDesktop.cpp
# End Source File
# Begin Source File

SOURCE=.\vncEncodeCoRRE.cpp
# End Source File
# Begin Source File

SOURCE=.\vncEncodeHexT.cpp
# End Source File
# Begin Source File

SOURCE=.\vncEncoder.cpp
# End Source File
# Begin Source File

SOURCE=.\vncEncodeRRE.cpp
# End Source File
# Begin Source File

SOURCE=.\vncEncodeTight.cpp
# End Source File
# Begin Source File

SOURCE=.\vncEncodeZlib.cpp
# End Source File
# Begin Source File

SOURCE=.\vncEncodeZlibHex.cpp
# End Source File
# Begin Source File

SOURCE=.\VNCHooks.cpp
# End Source File
# Begin Source File

SOURCE=.\vncKeymap.cpp
# End Source File
# Begin Source File

SOURCE=.\vncRegion.cpp
# End Source File
# Begin Source File

SOURCE=.\vncServer.cpp
# End Source File
# Begin Source File

SOURCE=.\vncService.cpp
# End Source File
# Begin Source File

SOURCE=.\VSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\WinVNCDllImpl.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\d3des.h
# End Source File
# Begin Source File

SOURCE=.\DynamicFn.h
# End Source File
# Begin Source File

SOURCE=.\keysymdef.h
# End Source File
# Begin Source File

SOURCE=.\Log.h
# End Source File
# Begin Source File

SOURCE=.\MemOutStream.h
# End Source File
# Begin Source File

SOURCE=.\MinMax.h
# End Source File
# Begin Source File

SOURCE=.\OutStream.h
# End Source File
# Begin Source File

SOURCE=.\PCritSec.h
# End Source File
# Begin Source File

SOURCE=.\PSyncPoint.h
# End Source File
# Begin Source File

SOURCE=.\PWaitAndSignal.h
# End Source File
# Begin Source File

SOURCE=.\QueueChannel.h
# End Source File
# Begin Source File

SOURCE=.\RectList.h
# End Source File
# Begin Source File

SOURCE=.\rfb.h
# End Source File
# Begin Source File

SOURCE=.\stdhdrs.h
# End Source File
# Begin Source File

SOURCE=.\translate.h
# End Source File
# Begin Source File

SOURCE=.\types.h
# End Source File
# Begin Source File

SOURCE=.\VideoDriver.h
# End Source File
# Begin Source File

SOURCE=.\vncBuffer.h
# End Source File
# Begin Source File

SOURCE=.\vncClient.h
# End Source File
# Begin Source File

SOURCE=.\vncDesktop.h
# End Source File
# Begin Source File

SOURCE=.\vncEncodeCoRRE.h
# End Source File
# Begin Source File

SOURCE=.\vncEncodeHexT.h
# End Source File
# Begin Source File

SOURCE=.\vncEncoder.h
# End Source File
# Begin Source File

SOURCE=.\vncEncodeRRE.h
# End Source File
# Begin Source File

SOURCE=.\vncEncodeTight.h
# End Source File
# Begin Source File

SOURCE=.\vncEncodeZlib.h
# End Source File
# Begin Source File

SOURCE=.\vncEncodeZlibHex.h
# End Source File
# Begin Source File

SOURCE=.\VNCHooks.h
# End Source File
# Begin Source File

SOURCE=.\vncKeymap.h
# End Source File
# Begin Source File

SOURCE=.\vncRegion.h
# End Source File
# Begin Source File

SOURCE=.\vncServer.h
# End Source File
# Begin Source File

SOURCE=.\vncService.h
# End Source File
# Begin Source File

SOURCE=.\VSocket.h
# End Source File
# Begin Source File

SOURCE=.\WinVNCDllImpl.h
# End Source File
# End Group
# End Target
# End Project
