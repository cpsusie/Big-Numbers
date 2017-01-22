#include "stdafx.h"
#include <Console.h>
#include <fcntl.h>

static void usage() {
  _ftprintf(stderr,_T("Usage:ImageInfo [file]\n"));
  exit(-1);
}

int main(int argc, char **argv) {
  try {

    argv++;
    FILE *input = stdin;

    if(*argv) {
      input = FOPEN(*argv,"rb");
      argv++;
    }

    if(isatty(input)) {
      throwException("Cannot read image from keyboard");
    }

    if(input == stdin) {
      setFileMode(input,_O_BINARY);
    }

    PixRectDevice device;
    device.attach(Console::getWindow());
    PixRect *image = PixRect::load(device, ByteInputFile(input));

    _tprintf(_T("ImageSize :%d,%d\n"),image->getWidth(),image->getHeight());
//    _tprintf(_T("Bits/Pixel:%d\n")   ,image->getgetPixelFormat()getPixelFormat().dwRGBBitCount);
  } catch(Exception e) {
    _ftprintf(stderr, _T("ImageInfo:%s\n"), e.what());
    return -1;
  }

	return 0;
}
