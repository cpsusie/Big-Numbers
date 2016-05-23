#include "stdafx.h"
#include <MyUtil.h>
#include <Tokenizer.h>
#include "TestTokenizer.h"

static bool testTokenizer(int testid, const TCHAR *str, TCHAR *del, TCHAR tq, int flags, ...) {
  va_list argptr;
  Array<String> expected;
  TCHAR *ex;
  for(va_start(argptr,flags), ex = va_arg(argptr,TCHAR*); ex; ex = va_arg(argptr,TCHAR*)) {
    expected.add(ex);
  }
  
  int i = 0;
  bool ok = true;
  
  Tokenizer assignTok(str,del,tq,flags);

  for(Tokenizer tok(str,del,tq,flags); tok.hasNext(); i++) {
    assignTok = tok;
    Tokenizer copyConstTok(tok);
    const String nextStr      = tok.next(); 
    const String assignStr    = assignTok.next();
    const String copyConstStr = copyConstTok.next();

    if(i >= expected.size()) {
      _tprintf(_T("Test %d. Too many field in String(%s). expected %d. got %s number %d\n"), testid, str, expected.size(), nextStr.cstr(), i);
      ok = false;
    } else {
      if(nextStr != expected[i]) {
        _tprintf(_T("Test %d. Wrong field in String(%s). field %d =<%s>. expected <%s>\n"), testid, str, i, nextStr.cstr(), expected[i].cstr()); 
        ok = false;
      }
      if(assignStr != nextStr) {
        _tprintf(_T("Test %d. Wrong field in assignStr(%s). field %d =<%s>. expected <%s>\n"), testid, str, i, assignStr.cstr(), expected[i].cstr()); 
        ok = false;
      }
      if(copyConstStr != nextStr) {
        _tprintf(_T("Test %d. Wrong field in copyConstStr(%s). field %d =<%s>. expected <%s>\n"), testid, str, i, copyConstStr.cstr(), expected[i].cstr()); 
        ok = false;
      }
    }
    if(assignTok.hasNext() != tok.hasNext()) {
      _tprintf(_T("Test %d. assignTok.hasNext()(=%s) != tok.hasNext()(=%s)\n"), testid, boolToStr(assignTok.hasNext()), boolToStr(tok.hasNext()));
      ok = false;
    }
    if(copyConstTok.hasNext() != tok.hasNext()) {
      _tprintf(_T("Test %d. copyConstStr.hasNext()(=%s) != tok.hasNext()(=%s)\n"), testid, boolToStr(copyConstTok.hasNext()), boolToStr(tok.hasNext()));
      ok = false;
    }
  }
  if(i != expected.size()) {
    _tprintf(_T("Test %d. Too few fields in String(%s). Got %d expected %d\n"), testid, str, i, expected.size());
    ok = false;
  }
  return ok;
}

void testTokenizer() {
  const static TCHAR *objectToTest = _T("Tokenizer");

  _tprintf(_T("Testing %s%s"),objectToTest,spaceString(15-_tcslen(objectToTest),'.').cstr());

  bool ok = true;
  ok &= testTokenizer(1,_T("a,b,c")      ,_T(",")       ,0  ,0                   ,_T("a")  ,_T("b") ,_T("c")     ,NULL);
  ok &= testTokenizer(2,_T("a,b, c")     ,_T(",")       ,0  ,0                   ,_T("a")  ,_T("b") ,_T(" c")    ,NULL);
  ok &= testTokenizer(3,_T("a,b\t c")    ,_T("\t")      ,0  ,0                   ,_T("a,b"),_T(" c")             ,NULL);
  ok &= testTokenizer(4,_T("a,b\t c")    ,_T("\t, ")    ,0  ,0                   ,_T("a")  ,_T("b") ,_T("c")     ,NULL);
  ok &= testTokenizer(5,_T("a,b\t\" c\""),_T("\t, ")    ,_T('"'),0               ,_T("a")  ,_T("b") ,_T("\" c\""),NULL);
  ok &= testTokenizer(6,_T("a,b\t\tc\t") ,_T("\t")      ,_T('"'),TOK_SINGLEDELIMITERS,_T("a,b"),_T("")  ,_T("c")     ,NULL);

  if(ok) {
    _tprintf(_T("%s ok!\n"),objectToTest);
  } else {
    _tprintf(_T("%s failed!\n"),objectToTest);
    abort();
  }
}

