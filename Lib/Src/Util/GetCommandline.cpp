#include "pch.h"
#include <ProcessTools.h>
#include <winternl.h>

#pragma comment(lib, "advapi32.lib")

typedef NTSTATUS (*pNtQueryInformationProcess)(IN HANDLE           ProcessHandle
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
  m_func   = (pNtQueryInformationProcess)GetProcAddress(m_module, "NtQueryInformationProcess");
}

NtDllLoader::~NtDllLoader() {
}

String getProcessCommandLine(HANDLE hProcess) {
  static NtDllLoader ntdll;
  if(hProcess == NULL) hProcess = GetCurrentProcess();
  PROCESS_BASIC_INFORMATION pinfo;
  ULONG                     returnLength;
  size_t                    bytesRead;
  NTSTATUS status = ntdll.NtQueryInformationProcess(hProcess
                                                   ,ProcessBasicInformation
                                                   ,&pinfo
                                                   ,sizeof(PROCESS_BASIC_INFORMATION)
                                                   ,&returnLength);
  PEB  peb;
  BOOL result = ReadProcessMemory(hProcess
                                 ,pinfo.PebBaseAddress
                                 ,&peb
                                 ,sizeof(PEB)
                                 ,&bytesRead);

  RTL_USER_PROCESS_PARAMETERS processParameters;
  result = ReadProcessMemory(hProcess
                            ,peb.ProcessParameters
                            ,&processParameters
                            ,sizeof(RTL_USER_PROCESS_PARAMETERS)
                            ,&bytesRead);
  USHORT  len =  processParameters.CommandLine.Length;
  WCHAR  *str = MALLOC(WCHAR,len+1);
  result = ReadProcessMemory(hProcess
                            ,processParameters.CommandLine.Buffer
                            ,str // command line goes here
                            ,len
                            ,&bytesRead);

  str[len] = 0;
  const String s = str;
  FREE(str);
  return s;
}

static DWORD getPreviousStatebufferSize(HANDLE token, BOOL disableAllPrivileges, TOKEN_PRIVILEGES newState) {
  DWORD dwSize = 0;
  TOKEN_PRIVILEGES previousState;
  if(AdjustTokenPrivileges(token, disableAllPrivileges, &newState, 1, &previousState, &dwSize) == 0) {
    return dwSize;
  }
  throwInvalidArgumentException(__TFUNCTION__,_T("token=%p"), token);
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
  if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token) == 0) {
    throwLastErrorOnSysCallException(_T("OpenProcessToken"));
  }

  PTOKEN_PRIVILEGES previousState = NULL;
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
