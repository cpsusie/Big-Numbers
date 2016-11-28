#include "stdafx.h"
#include <ByteFile.h>
#include "AdaptiveHuffmantree.h"
#include "ArithmeticCoder.h"
#include "ZLibCompressor.h"

#ifdef _USE_DCT
#include "DCT.h"
#endif

static void usage() {
  _ftprintf(stderr,_T("usage:compress C|E [-h|a|p|z] [-qQuality] [input [output]]\n"
                      "       C:compress\n"
                      "       E:Expand\n"
                      "      -h:Adaptiv Huffman encoding. (default)\n"
                      "      -a:Arithmetic encoding\n"
                      "      -z:zlib compression")
#ifdef _USE_DCT
                 "      -p:Picture encoding\n"
#endif
         );
  exit(-1);
}

typedef enum {
  COMPRESS
 ,EXPAND
} Command;

int main(int argc, char **argv) {
  Compressor *compressor = NULL;
  int quality = 3;

  char *cp;
  Command cmd;

  argv++;

  if(!*argv) usage();
  switch(**argv) {
  case 'C': cmd = COMPRESS; break;
  case 'E': cmd = EXPAND;   break;
  default : usage();
  }

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'h': 
        compressor = new AdaptivHuffmanTree();
        continue;
      case 'a':
        compressor = new ArithmeticCoder();
        continue;
      case 'z':
        compressor = new ZLibCompressor();
        continue;
#ifdef _USE_DCT
      case 'p':
        compressor = new PictureCoder;
        continue;
#endif
      case 'q':
        if(sscanf(cp+1, "%d", &quality) != 1)
          usage();
        if(quality < 0 || quality > 50)
          usage();
        break;
      default :
        usage();
      }
      break;
    }
  }

  if(compressor == NULL) {
    compressor = new AdaptivHuffmanTree();
  }

  try {
    FILE *input  = stdin;
    FILE *output = stdout;

    if(*argv) {
      char *inputname = *argv++;
      input = FOPEN(inputname,_T("rb"));
      if(*argv) {
        char *outputname = *argv++;
        output = FOPEN(outputname,_T("wb"));
      }
    }

    switch(cmd) {
    case COMPRESS:
      compressor->compress(ByteInputFile(input), ByteOutputFile(output), quality);
      break;
    case EXPAND  :
      compressor->expand(ByteInputFile(input), ByteOutputFile(output));
      break;
    }

    compressor->printRatios(stderr);

    if(input  != stdin ) fclose(input);
    if(output != stdout) fclose(output);

  } catch(Exception e) {
    _ftprintf(stderr,_T("%s\n"),e.what());
    exit(-1);
  }

  delete compressor;

  return 0;
}
