#include "stdafx.h"
#include <Console.h>
#include <fcntl.h>

static void usage() {
  _ftprintf(stderr,_T("Usage:prCut left,top,width,height [image]\n"
                      "If image is omitted, stdin is used as input.\n"
                      "The resulting image is written to stdout, which should be redirected to another image-filter or a file.\n")
           );
  exit(-1);
}

int main(int argc, char **argv) {
  UINT left,top,width,height;

  try {
    argv++;
    if(!*argv) {
      usage();
    }
    if(sscanf(*argv,"%lu,%lu,%lu,%lu", &left, &top, &width, &height) != 4) {
      usage();
    }
    argv++;

    FILE *input = stdin;

    if(*argv) {
      input = FOPEN(*argv,"rb");
      argv++;
    }

    if(isatty(input)) {
      throwException("Cannot read image from keyboard");
    }

    if(isatty(stdout)) {
      throwException("Cannot write image to console. Redirect stdout to prShow,another image filter or a file");
    }

    if(input == stdin) {
      setFileMode(input,_O_BINARY);
    }

    PixRectDevice device;
    device.attach(Console::getWindow());
    PixRect *src = PixRect::load(device, ByteInputFile(input));

    PixRect *result = new PixRect(device, PIXRECT_PLAINSURFACE, width, height);

    result->rop(0,0,width,height, SRCCOPY, src, left,top);

    result->writeAsBMP(ByteOutputFile(stdout));
  } catch(Exception e) {
    _ftprintf(stderr, _T("prCut:%s\n"), e.what());
    return -1;
  }
  return 0;
}
