// shutdown.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <windows.h>

#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "User32.lib")

BOOL MySystemShutdown(bool reboot) {
   HANDLE hToken;
   TOKEN_PRIVILEGES tkp;

   // Get a token for this process.

   if (!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
      return FALSE;

   // Get the LUID for the shutdown privilege.

   LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

   tkp.PrivilegeCount = 1;  // one privilege to set
   tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

   // Get the shutdown privilege for this process.

   AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

   if (GetLastError() != ERROR_SUCCESS)
      return FALSE;

   // Shut down the system and force all applications to close.
   UINT flags = EWX_FORCE;
   if(reboot)
     flags |= EWX_REBOOT;
   else
     flags |= EWX_SHUTDOWN;

   if(!ExitWindowsEx(flags, 0))
      return FALSE;

   return TRUE;
}

static void usage() {
  fprintf(stderr,"Illegal syntax. Usage:shutdown -[r]\n");
  exit(-1);
}

int main(int argc, char **argv) {
  bool reboot = false;
  char *cp;
  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'r': reboot = true; continue;
      default : usage();
      }
      break;
    }
  }
  MySystemShutdown(reboot);
  return 0;
}
