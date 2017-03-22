#include "stdafx.h"

int main(int argc, char **argv) {
  int   portnr   = 3742;
  char *hostName = NULL;

  argv++;
  if(*argv) hostName = *(argv++);
  if(*argv) portnr   = atoi(*argv);

  try {
    SOCKET tcp = tcpOpen(portnr, hostName);
    int tmp;
    tcpWrite(tcp,&tmp, sizeof(tmp));
  } catch(Exception e) {
    _tprintf(_T("%s\n"), e.what());
  }
  return 0;
}
