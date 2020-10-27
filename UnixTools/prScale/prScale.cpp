#include "stdafx.h"
#include <Console.h>
#include <fcntl.h>

static void usage() {
  fprintf(stderr,"Usage:prScale [-fFactor| [-fxFactorX] [-fyFactorY] | -sWidth,Height] [image]\n"
                 "  -fFactor: Image is scaled with the given factor in both x- and y direction.\n"
                 "  -fxFactorX: Width of image is multiplied with FactorX.\n"
                 "  -fyFactorY: Height of image is multiplied with FactorY.\n"
                 "  -sWidth,Height : Image is scaled to size width,height pixels.\n"
                 "If image is omitted, stdin is used as input.\n"
                 "The resulting image is written to stdout, which should be redirected to another image-filter or a file.\n"
         );
  exit(-1);
}

typedef enum {
  SCALE_MULTIPLY
 ,SCALE_ABSOLUTSIZE
 ,NOT_SET
} ScaleType;


int main(int argc, char **argv) {
  try {
    char *cp;
    ScaleParameters param(false,1,1);
    double factor;
    int width,height;
    ScaleType scaleType = NOT_SET;

    for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
      for(cp++; *cp; cp++) {
        switch(*cp) {
        case 'f':
          if(scaleType == SCALE_ABSOLUTSIZE) {
            fprintf(stderr,"Options -f and -s are not allowed at the same time.\n");
            usage();
          }
          scaleType = SCALE_MULTIPLY;
          switch(cp[1]) {
          case 'x':
            if(sscanf(cp+1,"%le",&factor) != 1) {
              usage();
            }
            if(factor <= 0) {
              fprintf(stderr,"prScale:FactorX=%lf. Must be > 0\n",factor);
              usage();
            }
            param.m_scale.x() = factor;
            break;
          case 'y':
            if(sscanf(cp+1,"%le",&factor) != 1) {
              usage();
            }
            if(factor <= 0) {
              fprintf(stderr,"prScale:FactorY=%lf. Must be > 0\n",factor);
              usage();
            }
            param.m_scale.y() = factor;
            break;
          default:
            if(sscanf(cp+1,"%le",&factor) != 1) {
              usage();
            }
            param.m_scale.x() = param.m_scale.y() = factor;
            break;
          }
          break;
        case 's':
          if(scaleType == SCALE_MULTIPLY) {
            fprintf(stderr,"Options -f and -s are not allowed at the same time.\n");
            usage();
          }
          scaleType = SCALE_ABSOLUTSIZE;
          if(sscanf(cp+1,"%lu,%lu",&width,&height) != 2) {
            usage();
          }
          if(width == 0) {
            fprintf(stderr,"Width=0. Must be > 0\n");
            usage();
          }
          if(height == 0) {
            fprintf(stderr,"Height=0. Must be > 0\n");
            usage();
          }
          param.m_toSize = true;
          param.m_scale.x() = width;
          param.m_scale.y() = height;
          break;
        default:
          usage();
        }
        break;
      }
    }

    FILE *input = stdin;

    if(*argv) {
      input = FOPEN(*argv,"rb");
      argv++;
    }

    if(isatty(input)) {
      throwException("Cannot read image from keyboard");
    }

    if(isatty(stdout)) {
      throwException("Cannot write image to console. Redirect stdout to prShow, another image filter or a file");
    }

    PixRectDevice device;
    device.attach(Console::getWindow());
    PixRect *src = PixRect::load(device, ByteInputFile(input));

    PixRect *result = PixRect::scaleImage(src,param);

    result->writeAsBMP(ByteOutputFile(stdout));

  } catch(Exception e) {
    _ftprintf(stderr, _T("prScale:%s\n"), e.what());
    return -1;
  }

  return 0;
}
