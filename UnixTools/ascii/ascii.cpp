#include "stdafx.h"
#include <stdlib.h>
#include <String.h>
#include <stdio.h>

static void printch3(int ch) {
  if(ch > 255) {
    printf("   ");
  } else {
    switch(ch) {
    case 7 : printf("bel"); break;
    case 8 : printf("bs "); break;
    case 9 : printf("tab"); break;
    case 10: printf("lf "); break;
    case 13: printf("cr "); break;
    default: printf( "%c  ", ch );
    }
  }
}

static void printasciidec() {
  const char *header = "    0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9";
  printf("%s\n",header);
  for(int i = 0, ch = 0; ch < 256; i++) {
    int l = ch;
    printf("%3d ", l);
    for(int j = 0; j < 20; j++)
      printch3(ch++);
    printf(" %3d\n", l);
  }
  printf("%s\n",header);
}

static void printasciihex() {
  const char *header = "    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F";
  printf("%s\n",header);
  for(int i = 0, ch = 0; ch < 256; i++ ) {
    int l = ch;
    printf("%3x ", l);
    for( int j = 0; j < 16; j++,ch++ )
      printch3(ch);
    printf(" %3x\n", l);
  }
  printf("%s\n",header);
}

static void usage() {
  fprintf(stderr,"usage:ascii [-d|-h]\n");
  exit(-1);
}

int main(int argc, char **argv) {
  argv++;
  if(!*argv)
    printasciidec();
  else if(strcmp(*argv,"-d") == 0)
    printasciidec();
  else if(strcmp(*argv,"-h") == 0)
    printasciihex();
  else
    usage();
  return 0;
}
