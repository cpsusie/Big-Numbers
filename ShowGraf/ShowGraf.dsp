# Microsoft Developer Studio Project File - Name="ShowGraf" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ShowGraf - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ShowGraf.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ShowGraf.mak" CFG="ShowGraf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ShowGraf - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ShowGraf - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ShowGraf - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x406 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /out:"c:\bin\ShowGraf.exe"

!ELSEIF  "$(CFG)" == "ShowGraf - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX"stdafx.h" /FD /GZ /c
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

# Name "ShowGraf - Win32 Release"
# Name "ShowGraf - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CustomFitThread.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomFitThreadDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DataGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\DataGraphDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DataGraphParameters.cpp
# End Source File
# Begin Source File

SOURCE=.\DegreeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ExpressionGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\ExpressionGraphParameters.cpp
# End Source File
# Begin Source File

SOURCE=.\ExprGraphDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FunctionFitter.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphArray.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphItem.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphParameters.cpp
# End Source File
# Begin Source File

SOURCE=.\IntervalDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\IsoCurveFinder.cpp
# End Source File
# Begin Source File

SOURCE=.\IsoCurveGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\IsoCurveGraphDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\IsoCurveGraphParameters.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\PointGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\RollsizeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ShowGraf.cpp
# End Source File
# Begin Source File

SOURCE=.\ShowGraf.rc
# End Source File
# Begin Source File

SOURCE=.\ShowGrafDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\ShowGrafView.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Util.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CustomFitThread.h
# End Source File
# Begin Source File

SOURCE=.\CustomFitThreadDlg.h
# End Source File
# Begin Source File

SOURCE=.\DataGraph.h
# End Source File
# Begin Source File

SOURCE=.\DataGraphDlg.h
# End Source File
# Begin Source File

SOURCE=.\DataGraphParameters.h
# End Source File
# Begin Source File

SOURCE=.\DataRange.h
# End Source File
# Begin Source File

SOURCE=.\DegreeDlg.h
# End Source File
# Begin Source File

SOURCE=.\ExpressionGraph.h
# End Source File
# Begin Source File

SOURCE=.\ExpressionGraphParameters.h
# End Source File
# Begin Source File

SOURCE=.\ExprGraphDlg.h
# End Source File
# Begin Source File

SOURCE=.\FunctionFitter.h
# End Source File
# Begin Source File

SOURCE=.\Graph.h
# End Source File
# Begin Source File

SOURCE=.\GraphArray.h
# End Source File
# Begin Source File

SOURCE=.\GraphItem.h
# End Source File
# Begin Source File

SOURCE=.\GraphParameters.h
# End Source File
# Begin Source File

SOURCE=.\IntervalDlg.h
# End Source File
# Begin Source File

SOURCE=.\IsoCurveFinder.h
# End Source File
# Begin Source File

SOURCE=.\IsoCurveGraph.h
# End Source File
# Begin Source File

SOURCE=.\IsoCurveGraphDlg.h
# End Source File
# Begin Source File

SOURCE=.\IsoCurveGraphParameters.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RollsizeDlg.h
# End Source File
# Begin Source File

SOURCE=.\ShowGraf.h
# End Source File
# Begin Source File

SOURCE=.\ShowGrafDoc.h
# End Source File
# Begin Source File

SOURCE=.\ShowGrafView.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Util.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmpFitCustomFunction.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmpFitExpoFunction.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmpFitPolyFunction.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmpFitPotensFunction.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ShowGraf.ico
# End Source File
# Begin Source File

SOURCE=.\res\ShowGraf.rc2
# End Source File
# Begin Source File

SOURCE=.\res\ShowGrafDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# End Target
# End Project
