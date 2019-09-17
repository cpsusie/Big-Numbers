#include "StdAfx.h"
#include <MyAssert.h>
#include <DebugLog.h>
#include <Unicode.h>
#include <StreamParameters.h>
#include "IntStreamTest.h"

using namespace std;

inline istream &operator>>(istream &s, const StreamParameters &p) {
  s.precision(p.precision());
  s.width(p.width());
  s.flags(p.flags());
  s.fill((char)p.fill());
  return s;
}

inline wistream &operator>>(wistream &s, const StreamParameters &p) {
  s.precision(p.precision());
  s.width(p.width());
  s.flags(p.flags());
  s.fill(p.fill());
  return s;
}

#define MINIVALUE       _I64_MIN
#define MAXIVALUE       _I64_MAX
#define MAXUIVALUE      _UI64_MAX

template<class Int64, class Int128> void testIntIO(CompactArray<Int64> &a) {
  try {

#define MINI_LOOPVALUE  (MINIVALUE / 16)
#define MAXI_LOOPVALUE  (MAXIVALUE >>2 )
#define MAXUI_LOOPVALUE (MAXUIVALUE>>4 )
    int formatCounter = 0;
    for(Iterator<StreamParameters> it = StreamParameters::getIntParamIterator(30,ios::skipws, ITERATOR_INTFORMATMASK|ios::skipws, 0); it.hasNext();) {
      const StreamParameters &param = it.next();
      const int    radix = StreamParameters::radix(param.flags());
      const String prmStr = param.toString();
      formatCounter++;
      debugLog(_T("formatCount:%ld:%s\n"), formatCounter, prmStr.cstr());
      ostringstream  costr64, costr128;
      wostringstream wostr64, wostr128;

      for(size_t i = 0; i < a.size(); i++) {
        const Int64 &x = a[i];
        if((x < 0) && (radix != 10)) {
          continue;
        }
        costr64  << param << x << endl;
        wostr64  << param << x << endl;
        costr128 << param << x << endl;
        wostr128 << param << x << endl;
      }

      string  cstr64  = costr64.str();
      wstring wstr64  = wostr64.str();
      string  cstr128 = costr128.str();
      wstring wstr128 = wostr128.str();

      String ctmp64( cstr64.c_str() );
      String wtmp64( wstr64.c_str() );
      String ctmp128(cstr128.c_str());
      String wtmp128(wstr128.c_str());

      verify(wtmp64  == ctmp64);
      verify(ctmp128 == ctmp64);
      verify(wtmp128 == ctmp64);

      istringstream  cistr64,cistr128;
      wistringstream wistr64,wistr128;

      cistr64.str( cstr64 );
      wistr64.str( wstr64 );
      cistr128.str(cstr128);
      wistr128.str(wstr128);

      for(int i = 0;;i++) {
        const char *next = cstr64.c_str() + cistr64.tellg();
        debugLog(_T("%d "), i);
        Int64  c64 , w64;
        Int128 c128, w128;
        cistr64  >> param >> c64;
        bool cistr64Failed = cistr64.fail();
        bool cistr64eof    = cistr64.eof();
        if( (formatCounter == 1147) && (i == 0)) {
          int fisk = 1;
        }
        wistr64  >> param >> w64;
        cistr128 >> param >> c128;
        wistr128 >> param >> w128;

        const char *next1 = cstr64.c_str() + cistr64.tellg();

        if(cistr64eof) {
          verify(wistr64.eof());
          verify(cistr128.eof());
          verify(wistr128.eof());
        }
        if(cistr64Failed) {
          verify(wistr64.fail());
          verify(cistr128.fail());
          verify(wistr128.fail());
        } else {
          if(wistr64.fail()) {
            wistr64 >> param >> w64;
          }
          if(cistr128.fail()) {
            cistr128 >> param >> c128;
          }
          if(wistr128.fail()) {
            wistr128 >> param >> w128;
          }

          verify(w64  == c64);
          verify(c128 == c64);
          verify(w128 == c64);
        }
        debugLog(_T("\n"));
        if(cistr64eof || cistr64Failed) break;
      } // for(;;)
    } // for(iterator
  } catch (Exception e) {
    throwException(_T("Exception:%s"), e.what());
  }
}

void testSignedIntStream() {
  CompactArray<INT64> a;
  for(INT64 x = 0; x <= MAXI_LOOPVALUE; x = (x + 1) * 11) { // add some positive test-numbers
    a.add(x);
  }
  a.add(MAXIVALUE);
  for(INT64 x = 0; x >= MINI_LOOPVALUE; x = (x - 1) * 11) { // add some negative test-numbers
    a.add(x);
  }
  a.add(MINIVALUE);
  testIntIO<INT64, _int128>(a);
}

void testUnsgnedIntStream() {
  CompactArray<UINT64> a;

  for(UINT64 x = 0; x <= MAXUI_LOOPVALUE; x = (x + 1) * 11) { // add unsigned test-numbers
    a.add(x);
  }
  a.add(MAXUIVALUE);
  testIntIO<UINT64, _uint128>(a);
}

void testIntStream() {
  testSignedIntStream();
  testUnsgnedIntStream();
}
