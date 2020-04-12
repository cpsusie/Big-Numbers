#include "pch.h"
#include <MyUtil.h>
#include <ProcessTools.h>

#pragma warning(disable:4005)

#include <ntstatus.h>
#include <winternl.h>

#pragma comment(lib, "advapi32.lib")

typedef __kernel_entry NTSTATUS (*pNtQueryInformationProcess)(IN HANDLE           ProcessHandle
                                                             ,IN PROCESSINFOCLASS ProcessInformationClass
                                                             ,OUT PVOID           ProcessInformation
                                                             ,IN ULONG            ProcessInformationLength
                                                             ,OUT PULONG          ReturnLength
                                                             );

class NtDllLoader {
private:
  HMODULE                    m_module;
  pNtQueryInformationProcess m_func;
public:
  NtDllLoader();
 ~NtDllLoader();
  inline NTSTATUS NtQueryInformationProcess( IN HANDLE           ProcessHandle
                                           , IN PROCESSINFOCLASS ProcessInformationClass
                                           , OUT PVOID           ProcessInformation
                                           , IN ULONG            ProcessInformationLength
                                           , OUT PULONG          ReturnLength
                                           ) {
    return m_func(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength);
  }
};

NtDllLoader::NtDllLoader() {
  m_module = LoadLibrary(_T("ntdll.dll"));
  if(m_module == NULL) {
    throwLastErrorOnSysCallException(__TFUNCTION__);
  }
  m_func = (pNtQueryInformationProcess)GetProcAddress(m_module, "NtQueryInformationProcess");
  if(m_func == NULL) {
    throwLastErrorOnSysCallException(__TFUNCTION__);
  }
}

NtDllLoader::~NtDllLoader() {
}

static void checkBoolResult(const TCHAR *func, BOOL ok) {
  if(ok != TRUE) {
    throwLastErrorOnSysCallException(func);
  }
}

#define V(result) checkBoolResult(_T(#result), result)

String getProcessCommandLine(HANDLE hProcess) {
  static NtDllLoader ntdll;
  if(hProcess == NULL) hProcess = GetCurrentProcess();
  WCHAR *str = NULL;
  try {
    PROCESS_BASIC_INFORMATION pinfo;
    ULONG                     returnLength;
    NTSTATUS status = ntdll.NtQueryInformationProcess(hProcess
                                                     ,ProcessBasicInformation
                                                     ,&pinfo
                                                     ,sizeof(PROCESS_BASIC_INFORMATION)
                                                     ,&returnLength);
    if(status != STATUS_SUCCESS) {
      throwException(_T("NtQueryInformationProcess failed:return code:%#08x"), status);
    }
    PEB    peb;
    SIZE_T bytesRead;
    V(ReadProcessMemory(hProcess
                       ,pinfo.PebBaseAddress
                       ,&peb
                       ,sizeof(PEB)
                       ,&bytesRead));

    RTL_USER_PROCESS_PARAMETERS processParameters;
    V(ReadProcessMemory(hProcess
                       ,peb.ProcessParameters
                       ,&processParameters
                       ,sizeof(RTL_USER_PROCESS_PARAMETERS)
                       ,&bytesRead));
    const USHORT  len =  processParameters.CommandLine.Length;
    str = MALLOC(WCHAR,len+1);
    V(ReadProcessMemory(hProcess
                       ,processParameters.CommandLine.Buffer
                       ,str // command line goes here
                       ,len
                       ,&bytesRead));

    str[len] = 0;
    const String s = str;
    FREE(str);
    return s;
  } catch(...) {
    if(str != NULL) {
      FREE(str);
    }
    throw;
  }
}

static DWORD getPreviousStatebufferSize(HANDLE token, BOOL disableAllPrivileges, TOKEN_PRIVILEGES newState) {
  DWORD dwSize = 0;
  TOKEN_PRIVILEGES previousState;
  if(AdjustTokenPrivileges(token, disableAllPrivileges, &newState, 1, &previousState, &dwSize) == 0) {
    return dwSize;
  }
  throwInvalidArgumentException(__TFUNCTION__, _T("token=%p"), token);
  return 0;
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
  V(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token));

  PTOKEN_PRIVILEGES previousState = NULL;
  try {
    // look up the privilege LUID and enable it
    TOKEN_PRIVILEGES tp;            // token provileges
    // initialize privilege structure
    ZeroMemory (&tp, sizeof (tp));
    tp.PrivilegeCount = 1;
    V(LookupPrivilegeValue(NULL, privilege, &tp.Privileges[0].Luid));

    tp.Privileges[0].Attributes = enable ? SE_PRIVILEGE_ENABLED : 0;

    // adjust token privileges
    DWORD dwSize = 0, bufferLength = getPreviousStatebufferSize(token, false, tp);
    previousState = (PTOKEN_PRIVILEGES)new BYTE[bufferLength]; TRACE_NEW(previousState);
    if(AdjustTokenPrivileges(token, FALSE, &tp, bufferLength, previousState, &dwSize) == 0) {
      throwLastErrorOnSysCallException(_T("AdjustTokenPrivileges"));
    } else if(GetLastError() != ERROR_SUCCESS) {
      throwLastErrorOnSysCallException(_T("AdjustTokenPrivileges"));
    }
  } catch(...) {
    SAFEDELETEARRAY(previousState);
    CloseHandle(token);
    throw;
  }
  SAFEDELETEARRAY(previousState);
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


    if( argc != 2 )
    {
        puts ( "Usage: cmdline pid" );
        return 0;
    }

    pid = wcstoul ( argv[1], &stop, 10 );

    // before attemting to open the process,
    // enable the SE_DEBUG_NAME privilege
    // in our own token, so we can open any
    // process in the system.
    if( !EnableTokenPrivilege ( SE_DEBUG_NAME ) )
    {
        printf ( "Failed o get required privileges, error %lu\n", GetLastError () );
        return 0;
    }

    // open process
    hProcess = OpenProcess ( PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_QUERY_INFORMATION,
                            FALSE, pid );

    if( hProcess == NULL )
    {
        printf ( "Failed to open process. pid: %lu, error %lu\n", pid, GetLastError () );
        return 0;
    }
    if( !GetProcessCmdLine ( hProcess, cmdline ) )
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
