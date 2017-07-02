#include "stdafx.h"

#define SIZE 20

static int  row[SIZE];
static bool col[SIZE];
static bool diag1[2*SIZE];
static bool diag2[2*SIZE];

static void init() {
  for(int i = 0; i < SIZE; i++) {
    col[i] = false;
  }
  for(int i = 0; i < SIZE*2; i++) {
    diag1[i] = diag2[i] = false;
  }
}

static ULONG solutioncounter = 0;
static UINT  trycount = 0;

static void printsolution() {
  int r,c;
  Console::clear();
  Console::setCursorPos(0,0);
  _tprintf(_T(" "));
  for(c = 0; c < SIZE; c++) {
    _tprintf(_T(" %c"),'a'+c);
  }
  _tprintf(_T("\n"));
  for(r = 0; r < SIZE; r++) {
    _tprintf(_T("%2d%*.*s%-2d\n"),SIZE-r,2*SIZE-1,2*SIZE-1,_T(" "),SIZE-r);
  }
  _tprintf(_T(" "));
  for(c = 0; c < SIZE; c++) {
    _tprintf(_T(" %c"),'a'+c);
  }
  _tprintf(_T("\n"));

  for(r = 0; r < SIZE; r++) {
    Console::printf(2*r+2,row[r]+1,_T("x"));
  }

  _tprintf(_T("\nLøsning %lu. gennemgået %lu kombinationer\n"),solutioncounter,trycount);
//  pause();
}

#define DIAG1(r,c) diag1[r+c]
#define DIAG2(r,c) diag2[r-c+SIZE-1]

static void tryrow(int r) {
  trycount++;
  if(r == SIZE) {
    solutioncounter++;
    if(solutioncounter % 1000 == 0)
      printsolution();
  } else {
    for(int c = 0; c < SIZE; c++) {
      if(!col[c] && !DIAG1(r,c) && !DIAG2(r,c)) {
        col[c]     = true;
        DIAG1(r,c) = true;
        DIAG2(r,c) = true;
        row[r] = c;
        tryrow(r+1);
        col[c]     = false;
        DIAG1(r,c) = false;
        DIAG2(r,c) = false;
      }
    }
  }
}

static double kombinations(int size) {
  double r = 1;
  int n = size*size;
  for(int i = 0; i < size; i++) {
    r *= n--;
  }
  return r;
}

int _tmain(int argc, TCHAR **argv) {
  init();
  tryrow(0);
  Console::setCursorPos(1,20);
  _tprintf(_T("%lu løsninger fundet. Gennemgået %lu kombinationer\n"),solutioncounter,trycount);
  _tprintf(_T("%.0lg mulige kombinationer\n"),kombinations(SIZE));
  return 0;
}
