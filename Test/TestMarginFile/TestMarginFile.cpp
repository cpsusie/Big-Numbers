#include "stdafx.h"
#include <Random.h>
#include <MarginFile.h>

TCHAR *randstr(TCHAR *s, int len) {
  int i;
  for(i = 0; i < len; i++)
    s[i] = ' ' + rand() % ('~' - ' ');
  s[i] = 0;
  return s;
}

int main(int argc, char **argv) {
  randomize();
  TCHAR str[40];
  for(int i = 0; i < 10; i++) {
    tostdout->setLeftMargin(i);
    fprintf(tostdout,_T("%s\n123456\n"),_T("jesper"));
    fprintf(tostdout,_T("sdfgs%s%c"),randstr(str,rand()%(ARRAYSIZE(str)-1)),(rand()%2)?'\n':' ');
  }
  return 0;
}
