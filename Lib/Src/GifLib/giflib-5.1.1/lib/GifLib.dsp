# Microsoft Developer Studio Project File - Name="GifLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=GifLib - Win32 Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GifLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GifLib.mak" CFG="GifLib - Win32 Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GifLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "GifLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "GifLib - Win32 Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "GifLib - Win32 Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GifLib - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX"stdafx.h" /FD /c
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x406 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"c:\mytools\lib\MTRelease\GifLib.lib"

!ELSEIF  "$(CFG)" == "GifLib - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x406 /d "_DEBUG"
# ADD RSC /l 0x406 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"c:\mytools\lib\MTDebug\GifLib.lib"

!ELSEIF  "$(CFG)" == "GifLib - Win32 Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GifLib___Win32_Debug_Unicode"
# PROP BASE Intermediate_Dir "GifLib___Win32_Debug_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Unicode"
# PROP Intermediate_Dir "Debug_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_LIB" /D "WIN32" /D "_DEBUG" /FR /YX"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x406 /d "_DEBUG"
# ADD RSC /l 0x406 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"c:\mytools\lib\MTDebug\GifLib.lib"
# ADD LIB32 /nologo /out:"c:\mytools\lib\MTUDebug\GifLib.lib"

!ELSEIF  "$(CFG)" == "GifLib - Win32 Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "GifLib___Win32_Release_Unicode"
# PROP BASE Intermediate_Dir "GifLib___Win32_Release_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_Unicode"
# PROP Intermediate_Dir "Release_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_LIB" /YX"stdafx.h" /FD /c
# ADD BASE RSC /l 0x406 /d "NDEBUG"
# ADD RSC /l 0x406 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"c:\mytools\lib\MTRelease\GifLib.lib"
# ADD LIB32 /nologo /out:"c:\mytools\lib\MTURelease\GifLib.lib"

!ENDIF 

# Begin Target

# Name "GifLib - Win32 Release"
# Name "GifLib - Win32 Debug"
# Name "GifLib - Win32 Debug Unicode"
# Name "GifLib - Win32 Release Unicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\dgif_lib.c
# End Source File
# Begin Source File

SOURCE=.\egif_lib.c
# End Source File
# Begin Source File

SOURCE=.\getopt.cpp
# End Source File
# Begin Source File

SOURCE=.\gif_err.c
# End Source File
# Begin Source File

SOURCE=.\gif_font.c
# End Source File
# Begin Source File

SOURCE=.\gif_hash.c
# End Source File
# Begin Source File

SOURCE=.\gifalloc.c
# End Source File
# Begin Source File

SOURCE=.\quantize.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\getopt.h
# End Source File
# Begin Source File

SOURCE=.\gif_hash.h
# End Source File
# Begin Source File

SOURCE=.\gif_lib.h
# End Source File
# Begin Source File

SOURCE=.\gif_lib_private.h
# End Source File
# Begin Source File

SOURCE=.\stdbool.h
# End Source File
# Begin Source File

SOURCE=.\stdint.h
# End Source File
# Begin Source File

SOURCE=.\unistd.h
# End Source File
# End Group
# End Target
# End Project
