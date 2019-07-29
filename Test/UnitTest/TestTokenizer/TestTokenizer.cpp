#include "stdafx.h"
#include <StringArray.h>
#include <Tokenizer.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestTokenizer {		

#include <UnitTestTraits.h>

	TEST_CLASS(TestTokenizer) {
  private:
    void testTokenizer(int testid, const TCHAR *str, TCHAR *del, TCHAR tq, int flags, ...) {
      va_list argptr;
      StringArray expected;
      TCHAR *ex;
      for (va_start(argptr, flags), ex = va_arg(argptr, TCHAR*); ex; ex = va_arg(argptr, TCHAR*)) {
        expected.add(ex);
      }

      size_t i = 0;

      Tokenizer assignTok(str, del, tq, flags);

      for (Tokenizer tok(str, del, tq, flags); tok.hasNext(); i++) {
        assignTok = tok;
        Tokenizer copyConstTok(tok);
        const String nextStr = tok.next();
        const String assignStr = assignTok.next();
        const String copyConstStr = copyConstTok.next();

        if(i >= expected.size()) {
          OUTPUT(_T("Test %d. Too many field in String(%s). expected %d. got %s number %d\n"), testid, str, expected.size(), nextStr.cstr(), i);
          verify(false);
        }
        else {
          if(nextStr != expected[i]) {
            OUTPUT(_T("Test %d. Wrong field in String(%s). field %d =<%s>. expected <%s>\n"), testid, str, i, nextStr.cstr(), expected[i].cstr());
            verify(false);
          }
          if(assignStr != nextStr) {
            OUTPUT(_T("Test %d. Wrong field in assignStr(%s). field %d =<%s>. expected <%s>\n"), testid, str, i, assignStr.cstr(), expected[i].cstr());
            verify(false);
          }
          if(copyConstStr != nextStr) {
            OUTPUT(_T("Test %d. Wrong field in copyConstStr(%s). field %d =<%s>. expected <%s>\n"), testid, str, i, copyConstStr.cstr(), expected[i].cstr());
            verify(false);
          }
        }
        if(assignTok.hasNext() != tok.hasNext()) {
          OUTPUT(_T("Test %d. assignTok.hasNext()(=%s) != tok.hasNext()(=%s)\n"), testid, boolToStr(assignTok.hasNext()), boolToStr(tok.hasNext()));
          verify(false);
        }
        if(copyConstTok.hasNext() != tok.hasNext()) {
          OUTPUT(_T("Test %d. copyConstStr.hasNext()(=%s) != tok.hasNext()(=%s)\n"), testid, boolToStr(copyConstTok.hasNext()), boolToStr(tok.hasNext()));
          verify(false);
        }
      }
      if(i != expected.size()) {
        OUTPUT(_T("Test %d. Too few fields in String(%s). Got %d expected %d\n"), testid, str, i, expected.size());
        verify(false);
      }
    }

public:

    TEST_METHOD(testTokenizer) {
      testTokenizer(1, _T("a,b,c"), _T(","), 0, 0, _T("a"), _T("b"), _T("c"), NULL);
      testTokenizer(2, _T("a,b, c"), _T(","), 0, 0, _T("a"), _T("b"), _T(" c"), NULL);
      testTokenizer(3, _T("a,b\t c"), _T("\t"), 0, 0, _T("a,b"), _T(" c"), NULL);
      testTokenizer(4, _T("a,b\t c"), _T("\t, "), 0, 0, _T("a"), _T("b"), _T("c"), NULL);
      testTokenizer(5, _T("a,b\t\" c\""), _T("\t, "), _T('"'), 0, _T("a"), _T("b"), _T("\" c\""), NULL);
      testTokenizer(6, _T("a,b\t\tc\t"), _T("\t"), _T('"'), TOK_SINGLEDELIMITERS, _T("a,b"), EMPTYSTRING, _T("c"), NULL);
    }
  };
}
