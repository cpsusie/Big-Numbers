# Microsoft Developer Studio Project File - Name="SortDemo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SortDemo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SortDemo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SortDemo.mak" CFG="SortDemo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SortDemo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SortDemo - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SortDemo - Win32 Release"

# PROP BASE Use_MFC 6
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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x406 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x406 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /out:"c:\bin\SortDemo.exe"

!ELSEIF  "$(CFG)" == "SortDemo - Win32 Debug"

# PROP BASE Use_MFC 6
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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x406 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x406 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "SortDemo - Win32 Release"
# Name "SortDemo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AboutBoxDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AnalyzeSortMethod.cpp
# End Source File
# Begin Source File

SOURCE=.\BubbleSort.cpp
# End Source File
# Begin Source File

SOURCE=.\CombSort.cpp
# End Source File
# Begin Source File

SOURCE=.\ErrorMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\HeapSort.cpp
# End Source File
# Begin Source File

SOURCE=.\InitData.cpp
# End Source File
# Begin Source File

SOURCE=.\InsertionSort.cpp
# End Source File
# Begin Source File

SOURCE=.\LinePrinter.cpp
# End Source File
# Begin Source File

SOURCE=.\LinePrinterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LinePrinterThread.cpp
# End Source File
# Begin Source File

SOURCE=.\MergeSort.cpp
# End Source File
# Begin Source File

SOURCE=.\MTQuickSort.cpp
# End Source File
# Begin Source File

SOURCE=.\ParameterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\QuickSort1.cpp
# End Source File
# Begin Source File

SOURCE=.\QuickSort3.cpp
# End Source File
# Begin Source File

SOURCE=.\QuickSort3NoRecursion.cpp
# End Source File
# Begin Source File

SOURCE=.\QuickSort4.cpp
# End Source File
# Begin Source File

SOURCE=.\QuickSort5Optimal.cpp
# End Source File
# Begin Source File

SOURCE=.\QuickSortEx.cpp
# End Source File
# Begin Source File

SOURCE=.\QuickSortMS.cpp
# End Source File
# Begin Source File

SOURCE=.\QuickSortPivot5.cpp
# End Source File
# Begin Source File

SOURCE=.\QuickSortRandom.cpp
# End Source File
# Begin Source File

SOURCE=.\ShakerSort.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellSortFast.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellSortSlow.cpp
# End Source File
# Begin Source File

SOURCE=.\ShortSortMS.cpp
# End Source File
# Begin Source File

SOURCE=.\SortDemo.cpp
# End Source File
# Begin Source File

SOURCE=.\SortDemo.rc
# End Source File
# Begin Source File

SOURCE=.\SortDemoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\sortMethod.cpp
# End Source File
# Begin Source File

SOURCE=.\SortPanelWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\SortThread.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\Swap.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AboutBoxdlg.h
# End Source File
# Begin Source File

SOURCE=.\LinePrinter.h
# End Source File
# Begin Source File

SOURCE=.\LinePrinterDlg.h
# End Source File
# Begin Source File

SOURCE=.\LinePrinterThread.h
# End Source File
# Begin Source File

SOURCE=.\ParameterDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SortDemo.h
# End Source File
# Begin Source File

SOURCE=.\SortDemoDlg.h
# End Source File
# Begin Source File

SOURCE=.\SortMethod.h
# End Source File
# Begin Source File

SOURCE=.\SortPanelWnd.h
# End Source File
# Begin Source File

SOURCE=.\SortThread.h
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

SOURCE=.\res\SortDemo.ico
# End Source File
# Begin Source File

SOURCE=.\res\SortDemo.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
