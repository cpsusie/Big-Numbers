#include "stdafx.h"
#include <Console.h>
#include <fcntl.h>

static void usage() {
  fprintf(stderr,"Usage:prOp [-p1=x1,y1 [-p2=x2,y2] rasterop [image1 [image2]]\n"
                 "  -p1=x1,y1: Use offset (x1,y1) of in image1 to when performing the rasteroperation.\n"
                 "  -p2=x2,y2: Use offset (x2,y2) of in image2 to when performing the rasteroperation.\n"
                 "  rasterop is one of { COPY, AND, OR, XOR, NOT }.\n"
                 "COPY, AND, OR and XOR requires 2 input-images. NOT requires only 1.\n"
                 "If image1 and/or image2 is omitted, stdin is used as input.\n"
                 "The resulting image is written to stdout, which should be redirected to another image-filter or a file.\n"
         );
  exit(-1);
}

typedef enum {
  ROP_OR
 ,ROP_AND
 ,ROP_XOR
 ,ROP_NOT
 ,ROP_COPY
} RasterOp;

typedef struct {
  const char *m_name;
  RasterOp    m_op;
} RasterCommand;

#define RASTERCOMMAND(op) #op,ROP_##op

const RasterCommand RasterOpNames[] = {
  RASTERCOMMAND(OR)
 ,RASTERCOMMAND(AND)
 ,RASTERCOMMAND(XOR)
 ,RASTERCOMMAND(NOT)
 ,RASTERCOMMAND(COPY)
};

RasterOp findRasterOp(const char *name) {
  for(int i = 0; i < ARRAYSIZE(RasterOpNames); i++) {
    const RasterCommand &op = RasterOpNames[i];
    if(_stricmp(op.m_name, name) == 0) {
      return op.m_op;
    }
  }
  fprintf(stderr, "Illegal rasteroperation:%s\n", name);
  usage();
  return ROP_OR;
}


int main(int argc, char **argv) {
  try {
    unsigned int x1=0,y1=0,x2=0,y2=0;

    char *cp;
    for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
      for(cp++; *cp; cp++) {
        switch(*cp) {
        case 'p':
          switch(cp[1]) {
          case '1':
            if(sscanf(cp+2,"=%u,%u",&x1,&y1) != 2) {
              usage();
            }
            break;
          case '2':
            if(sscanf(cp+2,"=%u,%u",&x2,&y2) != 2) {
              usage();
            }
            break;
          default:usage();
          }
          break;
        default:
          usage();
        }
        break;
      }
    }

    if(!*argv) {
      usage();
    }
    RasterOp op = findRasterOp(*(argv++));

    FILE *f1 = stdin;
    FILE *f2 = NULL;

    if(*argv) {
      f1 = FOPEN(*argv,"rb");
      argv++;

      if(op == ROP_NOT) {
        if(*argv) {
          usage();
        }
      } else if(*argv) { // op != ROP_NOT. requires 2 inputs
        f2 = FOPEN(*argv,"rb");
        argv++;
      } else {
        f2 = f1;
        f1 = stdin;
      }
    }

    if(op != ROP_NOT && f2 == NULL) {
      usage();
    }

    if(isatty(f1)) {
      throwException("Cannot read image from keyboard");
    }

    if(isatty(stdout)) {
      throwException("Cannot write image to console. Redirect stdout to prShow,another image filter or a file");
    }

    PixRectDevice device;
    device.attach(Console::getWindow());

    PixRect *p1 = PixRect::load(device, ByteInputFile(f1));
    PixRect *p2 = (f2 != NULL) ? PixRect::load(device, ByteInputFile(f2)) : p1->clone(true, PIXRECT_PLAINSURFACE);

    const int w = p1->getWidth();
    const int h = p1->getHeight();

    switch(op) {
    case ROP_OR : p1->rop(x1,y1,w,h,SRCPAINT  ,p2,x2,y2); break;
    case ROP_AND: p1->rop(x1,y1,w,h,SRCAND    ,p2,x2,y2); break;
    case ROP_XOR: p1->rop(x1,y1,w,h,SRCINVERT ,p2,x2,y2); break;
    case ROP_NOT: p1->rop(x1,y1,w,h,NOTSRCCOPY,p2,x2,y2); break;
    }

    p1->writeAsBMP(ByteOutputFile(stdout));

  } catch(Exception e) {
    _ftprintf(stderr, _T("prOp:%s\n"), e.what());
    return -1;
  }

  return 0;
}
