# Microsoft Developer Studio Project File - Name="dreamSock" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dreamSock - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dreamSock.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dreamSock.mak" CFG="dreamSock - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dreamSock - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "dreamSock - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "dreamSock"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dreamSock - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /c
# ADD BASE RSC /l 0x40b /d "NDEBUG"
# ADD RSC /l 0x40b /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"dreamSock.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy dreamSock.h ..\tutGame	copy dreamSock.h ..\tutGameServer	copy dreamSockLog.h ..\tutGame	copy dreamSockLog.h ..\tutGameServer	copy dreamSock.lib ..\tutGame	copy dreamSock.lib ..\tutGameServer
# End Special Build Tool

!ELSEIF  "$(CFG)" == "dreamSock - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x40b /d "_DEBUG"
# ADD RSC /l 0x40b /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"dreamSock.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy dreamSock.h ..\tutGame	copy dreamSock.h ..\tutGameServer	copy dreamSockLog.h ..\tutGame	copy dreamSockLog.h ..\tutGameServer	copy dreamSock.lib ..\tutGame	copy dreamSock.lib ..\tutGameServer
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "dreamSock - Win32 Release"
# Name "dreamSock - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\dreamLinuxSock.cpp
# End Source File
# Begin Source File

SOURCE=.\dreamSock.cpp
# End Source File
# Begin Source File

SOURCE=.\dreamSockGlobal.cpp
# End Source File
# Begin Source File

SOURCE=.\dreamSockLog.cpp
# End Source File
# Begin Source File

SOURCE=.\dreamWinSock.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\dreamSock.h
# End Source File
# Begin Source File

SOURCE=.\dreamSockLog.h
# End Source File
# End Group
# End Target
# End Project
