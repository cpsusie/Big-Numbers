#include "stdafx.h"
#include <MyUtil.h>
#include <Tokenizer.h>
#include "TestTokenizer.h"

int _tmain(int argc, TCHAR **argv) {
  testTokenizer();
  for(;;) {
    TCHAR line[100];
    _tprintf(_T("Indtast text:"));
    GETS(line);
    
    for(Tokenizer tok(line,_T(" \t\n,"),_T('"')); tok.hasNext(); ) {
      _tprintf(_T("<%s>\n"),tok.next().cstr());
    }
  }
  return 0;
}
