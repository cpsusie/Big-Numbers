#include "stdafx.h"
#include <MFCUtil/WinTools.h>

static void usage() {
  _ftprintf(stderr,_T("Usage:screenSize [-f]\n"));
  exit(-1);
}

int main(int argc, char **argv) {
  char *cp;
  bool fullInfo = false;
  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'f': fullInfo = true; continue;
      default : usage();
      }
      break;
    }
  }

  HDC hdcScreen = getScreenDC();
  int scrWidth  = GetDeviceCaps(hdcScreen,HORZRES);
  int scrHeight = GetDeviceCaps(hdcScreen,VERTRES);
  int planes    = GetDeviceCaps(hdcScreen,PLANES);
  int bitsPixel = GetDeviceCaps(hdcScreen,BITSPIXEL);
  DeleteDC(hdcScreen);

  _tprintf(_T("%dx%d\n"),scrWidth,scrHeight);
  if(fullInfo) {
    _tprintf(_T("Planes:%d. BitsPerPixel:%d\n"),planes,bitsPixel);
  }
  return 0;
}
