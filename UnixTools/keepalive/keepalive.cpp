#include "stdafx.h"
#include <MyUtil.h>
#include <conio.h>
#include <Thread.h>

static void usage() {
  _ftprintf(stderr,_T("Usage:keepalive [-s]\n"
                      "        -s:Prevent screen from being turned off.\n"
                      "           Default is, that the system keeps running and the screen will be turned off by the power managment system.\n")
           );
  exit(-1);
}
int main(int argc, char **argv) {
  int flags = ES_CONTINUOUS | ES_SYSTEM_REQUIRED;

  argv++;

  if(*argv) {
    if(strcmp(*argv,"-s") == 0) {
      flags |= ES_DISPLAY_REQUIRED;
    } else {
      usage();
    }
  }

  Thread::keepAlive(flags);

  _tprintf(_T("Keeps the system alive. Press any key to stop..."));
  _getch();
  exit(0);

  return 0;
}
