# Microsoft Developer Studio Project File - Name="Viewer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Viewer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Viewer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Viewer.mak" CFG="Viewer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Viewer - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Viewer - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Viewer - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../omnithread" /I ".." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_WINDOWS" /D "__NT__" /D "_WINSTATIC" /D "__WIN32__" /D "XMD_H" /D "_STLP_USE_STATIC_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Viewer - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../omnithread" /I ".." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_WINDOWS" /D "__NT__" /D "_WINSTATIC" /D "__WIN32__" /D "XMD_H" /YX /FD /GZ /c
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

# Name "Viewer - Win32 Release"
# Name "Viewer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ClientConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnectionClipboard.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnectionCopyRect.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnectionCoRRE.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnectionCursor.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnectionFullScreen.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnectionHextile.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnectionRaw.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnectionRRE.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnectionTight.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnectionZlib.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnectionZlibHex.cpp
# End Source File
# Begin Source File

SOURCE=.\d3des.c
# End Source File
# Begin Source File

SOURCE=.\Exception.cpp
# End Source File
# Begin Source File

SOURCE=.\FIFO.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyMap.cpp
# End Source File
# Begin Source File

SOURCE=.\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\PCritSec.cpp
# End Source File
# Begin Source File

SOURCE=.\PSyncPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\QueueChannel.cpp
# End Source File
# Begin Source File

SOURCE=.\stdhdrs.cpp
# End Source File
# Begin Source File

SOURCE=.\VNCOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\VNCviewerApp.cpp
# End Source File
# Begin Source File

SOURCE=.\VNCviewerApp32.cpp
# End Source File
# Begin Source File

SOURCE=.\VncViewerDllImpl.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ClientConnection.h
# End Source File
# Begin Source File

SOURCE=.\d3des.h
# End Source File
# Begin Source File

SOURCE=.\Exception.h
# End Source File
# Begin Source File

SOURCE=.\FIFO.h
# End Source File
# Begin Source File

SOURCE=.\KeyMap.h
# End Source File
# Begin Source File

SOURCE=.\keysymdef.h
# End Source File
# Begin Source File

SOURCE=.\Log.h
# End Source File
# Begin Source File

SOURCE=.\PCritSec.h
# End Source File
# Begin Source File

SOURCE=.\PSyncPoint.h
# End Source File
# Begin Source File

SOURCE=.\QueueChannel.h
# End Source File
# Begin Source File

SOURCE=.\rfb.h
# End Source File
# Begin Source File

SOURCE=.\stdhdrs.h
# End Source File
# Begin Source File

SOURCE=.\VNCOptions.h
# End Source File
# Begin Source File

SOURCE=.\VNCviewerApp.h
# End Source File
# Begin Source File

SOURCE=.\VNCviewerApp32.h
# End Source File
# Begin Source File

SOURCE=.\VncViewerDllImpl.h
# End Source File
# End Group
# End Target
# End Project
