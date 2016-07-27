// testdollar.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <String.h>
#include <stdio.h>

typedef struct {
  char s[1000];
} block;

void f(block b) {
  printf("%s\n",b.s);
  int i = 3;
  char *p = "fisk";
  *p = '\0';
  throw 2;
}

typedef void (*funcp)();

int main(int argc, char* argv[])
{
  block b;
  strcpy(b.s,"JESPER");
  int blocksize        = sizeof(b);
  int loopcounter      = blocksize / 4;
  funcp funcaddress    = (funcp)f;
  void *parameterblock = &b;
  double x = 1;
  double y = 2;
  double z = x + y * (y+1);

  try {
    __asm {
      sub         esp,blocksize
      mov         ecx,loopcounter
      mov         esi,parameterblock
      mov         edi,esp
      rep movs    dword ptr [edi],dword ptr [esi]
      call        dword ptr [funcaddress]
      add         esp,blocksize
    }
  } catch (int i) {
    printf("caught %d\n",i);
  } catch(...) {
    printf("caught unknown Exception!\n");
  }

  return 0;
}

