////////////////////////////////////////////////////////////////////
//
// File:        GetCommandLine.cpp
// Project:     cmdline
//
// Desc:        this is a sample program that shows how to
//              get the command line for almost any process
//              on the system for WinNT 4 and up.
//
// Revisions:   Created 12/02/99
//
// Copyright(C) 1999, Tomas Restrepo. All rights reserved
//
///////////////////////////////////////////////////////////////////

#include "pch.h"

//#define UNICODE
//#define _UNICODE

#include <stdlib.h>
#include <tchar.h>
#include <ProcessTools.h>

#pragma comment(lib, "advapi32.lib")

// found by experimentation this is where the some
// process data block is found in an NT machine.
// On an Intel system, 0x00020000 is the 32
// memory page. At offset 0x0498 is the process
// current directory (or startup directory, not sure yet)
// followed by the system's PATH. After that is the  
// process full command command line, followed by 
// the exe name and the window
// station it's running on
#define BLOCK_ADDRESS   (LPVOID)0x00020498
// Additional comments:
// From experimentation I've found
// two notable exceptions where this doesn't seem to apply:
// smss.exe : the page is reserved, but not commited
//          which will get as an invalid memory address
//          error
// crss.exe : although we can read the memory, it's filled
//            with 00 comepletely. No trace of command line
//            information


// align pointer
#define ALIGN_DWORD(x) ( (x & 0xFFFFFFFC) ? (x & 0xFFFFFFFC) + sizeof(DWORD) : x )

////////////////////////////////////////////////////////////////////
//
// Function: GetProcessCmdLine()
//
// Added: 19/02/99
//
// Description: Gets the command line for the given process
//              NOTE: hProcess should be opened with
//              PROCESS_VM_READ , PROCESS_VM_OPERATION
//              and PROCESS_QUERY_INFORMATION rights
//
///////////////////////////////////////////////////////////////////
String getProcessCommandLine(HANDLE hProcess) {
  if(hProcess == NULL) hProcess = GetCurrentProcess();
  LPBYTE lpBuffer = NULL;

  try {
    // Get the system page size by using GetSystemInfo()
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    // allocate one on the heap to retrieve a full page
    // of memory
    lpBuffer = new BYTE[sysInfo.dwPageSize];
    if(lpBuffer == NULL) {
      throwException("Out of memory");
    }

    // first of all, use VirtualQuery to get the start of the memory
    // block
    MEMORY_BASIC_INFORMATION mbi;

    if(VirtualQueryEx(hProcess, BLOCK_ADDRESS, &mbi, sizeof(mbi)) == 0) {
      throwLastErrorOnSysCallException(_T("VirtualQueryEx"));
    }
    
    // read memory begining at the start of the page
    // after that, we know that the env strings block
    // will be 0x498 bytes after the start of the page
    DWORD dwBytesRead;
    if(!ReadProcessMemory(hProcess, mbi.BaseAddress, (LPVOID)lpBuffer, sysInfo.dwPageSize, &dwBytesRead)) {
      throwLastErrorOnSysCallException(_T("ReadProcessMemory"));
    }

    // now we've got the buffer on our side of the fence.
    // first, lpPos points to a string containing the current directory
    /// plus the path.
    LPBYTE lpPos = lpBuffer + ((DWORD)BLOCK_ADDRESS - (DWORD)mbi.BaseAddress);
    lpPos = lpPos + (wcslen ( (LPWSTR)lpPos ) + 1) * sizeof(WCHAR);
    // now goes full path an filename, aligned on a DWORD boundary
    // skip it
    lpPos = (LPBYTE)ALIGN_DWORD((DWORD)lpPos);
    lpPos = lpPos + (wcslen ( (LPWSTR)lpPos ) + 1) * sizeof(WCHAR);
    // hack: Sometimes, there will be another '\0' at this position
    // if that's so, skip it
    if (*lpPos == '\0') {
      lpPos += sizeof(WCHAR);
    }
    WCHAR wCmdLine[MAX_PATH+1];
    // now we have the actual command line
    // copy it to the buffer
    wcsncpy(wCmdLine, (LPWSTR)lpPos, MAX_PATH);
    // make sure the path is null-terminted
    wCmdLine[MAX_PATH-1] = L'\0';

    char result[MAX_PATH+1], *dst = result;
    for(int i = 0; wCmdLine[i]; i++) {
      *(dst++) = (unsigned char)wCmdLine[i];
    }
    *dst = '\0';
    delete[] lpBuffer;
    return result;
  } catch(...) {
    if(lpBuffer != NULL) {
      delete[] lpBuffer;
    }
    throw;
  }
}

////////////////////////////////////////////////////////////////////
//
// Function: enableTokenPrivilege()
//
// Added: 20/02/99
//
// Description: Enable or disable a specific token privilege
//
///////////////////////////////////////////////////////////////////
void enableTokenPrivilege(LPCTSTR privilege, bool enable) {
  HANDLE token;        // process token 

  // open the process token
  if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &token) == 0) {
    throwLastErrorOnSysCallException(_T("OpenProcessToken"));
  }
  
  try {
    // look up the privilege LUID and enable it
    TOKEN_PRIVILEGES tp;            // token provileges
    // initialize privilege structure
    ZeroMemory (&tp, sizeof (tp));
    tp.PrivilegeCount = 1;
    if(LookupPrivilegeValue(NULL, privilege, &tp.Privileges[0].Luid) == 0) { 
      throwException(_T("LookupPrivilegeValue(%s) failed. %s"), privilege, getLastErrorText().cstr());
    }
  
    tp.Privileges[0].Attributes = enable ? SE_PRIVILEGE_ENABLED : 0;

    // adjust token privileges
    DWORD dwSize;             
    if(AdjustTokenPrivileges(token, FALSE, &tp, 0, NULL, &dwSize) == 0) {
      throwException(_T("AdjustTokenPrivileges(%s) failed. %s"), privilege, getLastErrorText().cstr());
    } else if(GetLastError() != ERROR_SUCCESS) {
      throwException(_T("AdjustTokenPrivileges(%s) failed. %s"), privilege, getLastErrorText().cstr());
    }
  } catch(...) {
    CloseHandle(token);
    throw;
  }
  CloseHandle(token);
}

////////////////////////////////////////////////////////////////////
//
// Function: main()
//
// Added: 19/02/99
//
// Description: entry point
//
///////////////////////////////////////////////////////////////////
/*
int xxx ( int argc, wchar_t *argv[] )
{
    WCHAR   cmdline[MAX_PATH];
    WCHAR   *stop;
    DWORD   pid; // pid to get command line for
    HANDLE  hProcess = NULL;


    if ( argc != 2 )
    {
        puts ( "Usage: cmdline pid" );
        return 0;
    }

    pid = wcstoul ( argv[1], &stop, 10 );

    // before attemting to open the process,
    // enable the SE_DEBUG_NAME privilege
    // in our own token, so we can open any
    // process in the system.
    if ( !EnableTokenPrivilege ( SE_DEBUG_NAME ) )
    {
        printf ( "Failed o get required privileges, error %lu\n", GetLastError () );
        return 0;
    }

    // open process 
    hProcess = OpenProcess ( PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_QUERY_INFORMATION,
                            FALSE, pid );

    if ( hProcess == NULL )
    {
        printf ( "Failed to open process. pid: %lu, error %lu\n", pid, GetLastError () );
        return 0;
    }
    if ( !GetProcessCmdLine ( hProcess, cmdline ) )
    {
        printf ( "Failed to get process command line, error: %lu\n", GetLastError () );
        return 0;
    }
    else
        printf ( "Command line: %S\n", cmdline );

    // close process
    CloseHandle ( hProcess );
    
    return 0;
}

*/
