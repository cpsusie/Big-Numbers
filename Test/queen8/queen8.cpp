#include "stdafx.h"

#define SIZE 16

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
//  Console::clear();
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

  _tprintf(_T("\nLøsning %s. gennemgået %s kombinationer\n")
          ,format1000(solutioncounter).cstr(),format1000(trycount).cstr());
//  pause();
}

#define DIAG1(r,c) diag1[r+c]
#define DIAG2(r,c) diag2[r-c+SIZE-1]

static const bool *colend = col + SIZE;

static void tryrow(int r) {
  trycount++;
  if(r == SIZE) {
    if(++solutioncounter % 5000 == 0) {
      printsolution();
    }
  } else {
    bool *d1 = &DIAG1(r, 0), *d2 = &DIAG2(r,0);
    for(bool *cp = col; cp < colend; cp++, d1++, d2--) {
      if(!*cp && !*d1 && !*d2) {
        *cp = *d1 = *d2 = true;
        row[r] = (int)(cp - col);
        tryrow(r+1);
        *cp = *d1 = *d2 = false;
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
