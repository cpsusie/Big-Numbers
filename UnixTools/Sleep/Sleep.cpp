#include "stdafx.h"
#include <MyUtil.h>
#include <ProcessTools.h>

static bool isProcessRunning(int processId) {
  const CompactArray<DWORD> processArray = getProcessIds();
  for(size_t i = 0; i < processArray.size(); i++) {
    if(processArray[i] == processId) {
      return true;
    }
  }
  return false;
}

static void usage() {
  _ftprintf(stderr,_T("Usage:Sleep -mminutes|-sseconds|-pprocesid\n"));
  exit(-1);
}

int main(int argc, char **argv) {
  char *cp;

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 's':
        { int seconds;
          if((sscanf(cp+1,"%d", &seconds) != 1) || (seconds < 0)) {
            usage();
          }
          Sleep(1000 * seconds);
        }
        return 0;

      case 'm':
        { int minutes;
          if((sscanf(cp+1,"%d", &minutes) != 1) || (minutes < 0)) {
            usage();
          }
          Sleep(60000 * minutes);
        }
        return 0;

      case 'p':
        { int processId;
          if((sscanf(cp+1,"%d", &processId) != 1) || (processId <= 0)) {
            usage();
          }
          for(;;) {
            Sleep(20000);
            if(!isProcessRunning(processId)) {
              return 0;
            }
          }
        }
        break;
      default:
        usage();
      }
      break;
    }
  }
  usage();
  return 0;
}
