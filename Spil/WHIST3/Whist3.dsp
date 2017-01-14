# Microsoft Developer Studio Project File - Name="whist3" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=whist3 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Whist3.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Whist3.mak" CFG="whist3 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "whist3 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "whist3 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "whist3 - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\util" /I "..\registry" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x406 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "whist3 - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\util" /I "..\registry" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x406 /d "_DEBUG"
# ADD RSC /l 0x406 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "whist3 - Win32 Release"
# Name "whist3 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BacksideDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CardBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\CardButton.cpp
# End Source File
# Begin Source File

SOURCE=.\CardHand.cpp
# End Source File
# Begin Source File

SOURCE=.\CardIndexSet.cpp
# End Source File
# Begin Source File

SOURCE=.\CardStatistic.cpp
# End Source File
# Begin Source File

SOURCE=.\Communication.cpp
# End Source File
# Begin Source File

SOURCE=.\DecideGameDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\DecideGameTrainingDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\DecideGameTrainingSet.cpp
# End Source File
# Begin Source File

SOURCE=.\GameDescription.cpp
# End Source File
# Begin Source File

SOURCE=.\GameHistory.cpp
# End Source File
# Begin Source File

SOURCE=.\GameTypes.cpp
# End Source File
# Begin Source File

SOURCE=.\IdentifyDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\log.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\RulesDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SummaryDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Whist3.cpp
# End Source File
# Begin Source File

SOURCE=.\Whist3.rc
# End Source File
# Begin Source File

SOURCE=.\Whist3Dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Whist3Eplayer.cpp
# End Source File
# Begin Source File

SOURCE=.\Whist3Player.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BacksideDialog.h
# End Source File
# Begin Source File

SOURCE=.\CardBitmap.h
# End Source File
# Begin Source File

SOURCE=.\CardButton.h
# End Source File
# Begin Source File

SOURCE=.\Communication.h
# End Source File
# Begin Source File

SOURCE=.\DecideGameDialog.h
# End Source File
# Begin Source File

SOURCE=.\DecideGameTrainingDialog.h
# End Source File
# Begin Source File

SOURCE=.\Gametypes.h
# End Source File
# Begin Source File

SOURCE=.\IdentifyDialog.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RulesDialog.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SummaryDialog.h
# End Source File
# Begin Source File

SOURCE=.\whist3.h
# End Source File
# Begin Source File

SOURCE=.\Whist3Dialog.h
# End Source File
# Begin Source File

SOURCE=.\Whist3EPlayer.h
# End Source File
# Begin Source File

SOURCE=.\Whist3Player.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\CARDSBMP.BMP
# End Source File
# Begin Source File

SOURCE=.\res\whist3.ico
# End Source File
# Begin Source File

SOURCE=.\res\whist3.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
