#include "stdafx.h"
#include <math.h>
#include <Random.h>
#include <Math/MathLib.h>
#include <Math/Rational.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define VERIFYOP(op, maxError) {                                                                     \
  const Rational rBinResult = (r1) op (r2);                                                          \
  const double   dBinResult = (d1) op (d2);                                                          \
  const double   dFromR     = getDouble(rBinResult);                                                 \
  const double   error      = fabs(dFromR - dBinResult);                                             \
  if(error > maxError) {                                                                             \
    throwException(_T("%s %s %s = %s = %23.16le. %23.16le %s %23.16le = %23.16le. Error:%le > %le")  \
                  ,toString(r1).cstr(), _T(#op), toString(r2).cstr()                                 \
                  ,toString(rBinResult).cstr(), dFromR                                               \
                  ,d1, _T(#op), d2, dBinResult                                                       \
                  ,error, maxError                                                                   \
                  );                                                                                 \
  }                                                                                                  \
}

namespace TestRational {		

#include <UnitTestTraits.h>

  template<class OUTSTREAM> OUTSTREAM &setFormat(OUTSTREAM &os, ios::_Fmtflags baseFlag, unsigned int width, int showPos, int showBase, int uppercase, ios::_Fmtflags adjustFlag) {
    os.setf(baseFlag  , ios::basefield);
    os.setf(adjustFlag, ios::adjustfield);
    os.width(width);
    if(showBase) {
      os.setf(ios::showbase);
    }
    if(showPos) {
      os.setf(ios::showpos);
    }
    if(uppercase) {
      os.setf(ios::uppercase);
    }
    return os;
  }

	TEST_CLASS(TestRational) {
    public:

    TEST_METHOD(BasicOperations) {
      randomize();

      //  double maxTotalError = 0;
      for(int i = 0; i < 100000; i++) {
        /*
        if(i % 100 == 99) {
        printf("i:%10d. maxTotalError:%le\r", i, maxTotalError);
        }
        */
        const int nm1 = randInt(-1000, 1000);
        const int dn1 = randInt(1, 1000000);
        const int nm2 = randInt(-1000, 1000);
        const int dn2 = randInt(1, 1000000);

        const Rational r1(nm1, dn1);
        const Rational r2(nm2, dn2);

        const double d1 = getDouble(r1);
        const double d2 = getDouble(r2);

        const bool rgt = r1 >  r2, dgt = d1 >  d2;
        const bool rge = r1 >= r2, dge = d1 >= d2;
        const bool rlt = r1 <  r2, dlt = d1 <  d2;
        const bool rle = r1 <= r2, dle = d1 <= d2;
        const bool req = r1 == r2, deq = d1 == d2;
        const bool rne = r1 != r2, dne = d1 != d2;

        verify(rgt == dgt);
        verify(rge == dge);
        verify(rlt == dlt);
        verify(rle == dle);
        verify(req == deq);
        verify(rne == dne);

        VERIFYOP(+, 1e-13)
        VERIFYOP(-, 1e-13)
        VERIFYOP(*, 1e-14)

        if (!r2.isZero()) {
          VERIFYOP(/ , 3e-9)
        }

        const int      expo = r1.isZero() ? randInt(0, 4) : randInt(-3, 3);
        const Rational r1Pe = pow(r1, expo);
        const Real     d1Pe = mypow(d1, expo);
        Real           error = fabs(getReal(r1Pe) - d1Pe);
        if (d1Pe != 0) error /= d1Pe;
        verify(error < 1e-15);

        const Rational rfd(d1);
        const double   dfr = getDouble(rfd);
        error = fabs(dfr - d1);
        verify(error < 1e-13);
      }
    }

    TEST_METHOD(RationalIO) {
      try {
        CompactArray<Rational> sa;

        for(INT64 d = 1; d >= 0; d = (d + 1) * 31) {
          for(INT64 n = 0; n >= 0; n = (n + 1) * 29) {
            sa.add(Rational(n,d));
            sa.add(-sa.last());
          }
        }

        const ios::_Fmtflags baseFlags[] = {
          ios::dec
         ,ios::hex
         ,ios::oct
        };
        const ios::_Fmtflags adjustFlags[] = {
          ios::left
         ,ios::right
        };

        // try all(almost) combinations of output format flags
        for(int b = 0; b < ARRAYSIZE(baseFlags); b++) {
          const ios::_Fmtflags baseFlag   = baseFlags[b];
          int maxShowPos, maxShowBase, maxUpper;
          switch(baseFlag) {
          case ios::dec: maxShowPos = 1; maxShowBase = 0; maxUpper = 0; break;
          case ios::hex: maxShowPos = 0; maxShowBase = 1; maxUpper = 1; break;
          case ios::oct: maxShowPos = 0; maxShowBase = 1; maxUpper = 0; break;
          }
          for(int showPos = 1; showPos <= maxShowPos; showPos++) {
            for(int showBase = 0; showBase <= maxShowBase; showBase++) {
              for(int uppercase = 0; uppercase <= maxUpper; uppercase++) {
                for(int a = 0; a < ARRAYSIZE(adjustFlags); a++) {
                  for(UINT width = 0; width < 20; width += 3) {
                    ostringstream  ostr;
                    wostringstream wostr;
                    const ios::_Fmtflags adjustFlag = adjustFlags[a];

                    for(size_t i = 0; i < sa.size(); i++) { // write signed
                      setFormat<ostream>(ostr, baseFlag, width, showPos, showBase, uppercase, adjustFlag);
                      setFormat<wostream>(wostr, baseFlag, width, showPos, showBase, uppercase, adjustFlag);
                      ostr << sa[i] << "\n";
                      wostr << sa[i] << "\n";
                    }

                    string  str  = ostr.str();
                    wstring wstr = wostr.str();

                    for(Tokenizer tok(wstr.c_str(), _T("\n")); tok.hasNext();) {
                      const String s = tok.next();
                      verify(s.length() >= width);
                      const TCHAR *np;

                      if(adjustFlag == ios::right) {
                        for(np = s.cstr(); *np == _T(' '); np++);
                        verify(s.last() != _T(' '));
                      } else {
                        np = s.cstr();
                        verify(s[0] != _T(' '));
                      }
                      if(baseFlag == ios::dec) {
                        if(showPos) {
                          verify((np[0] == _T('-')) || (np[0] == _T('+')));
                        }
                      } else if (showBase) {
                        verify(np[0] == _T('0'));
                        if(baseFlag == ios::hex) {
                          verify(np[1] == _T('x'));
                        }
                      }
                      if(uppercase) {
                        for(const TCHAR *cp = np; *cp; cp++) {
                          verify(!_istlower(*cp));
                        }
                      } else {
                        for(const TCHAR *cp = np; *cp; cp++) {
                          verify(!_istupper(*cp));
                        }
                      }
                    }

                    istringstream  istr;
                    wistringstream wistr;

                    istr.str(str);
                    wistr.str(wstr);

                    for(size_t i = 0; i < sa.size(); i++) { // read signed
                      Rational x;
                      istr.setf(baseFlag, ios::basefield);
                      istr >> x;
                      verify(x == sa[i]);

                      Rational wx;
                      wistr.setf(baseFlag, ios::basefield);
                      wistr >> wx;
                      verify(wx == sa[i]);
                    }
                  } // for width=[0..20]
                } // for all AdjustFlags
              } // for lower/uppercase
            } // for all showBase
          } // for all showPos
        } // for all baseFlags
      } catch(Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

    template<class T> void verifyRationalNanTypes(const T &f) {
      const bool fisNan  = isnan(      f);
      const bool fisInf  = isinf(      f);
      const bool fisPInf = isPInfinity(f);
      const bool fisNInf = isNInfinity(f);
      const Rational r(f);
      verify(isnan(      r)  == fisNan );
      verify(isinf(      r)  == fisInf );
      verify(isPInfinity(r)  == fisPInf);
      verify(isNInfinity(r)  == fisNInf);

      const float    f1  = getFloat(   r);
      const double   d1  = getDouble(  r);
      const Double80 d80 = getDouble80(r);

      verify(isnan(      f1 ) == fisNan );
      verify(isinf(      f1 ) == fisInf );
      verify(isPInfinity(f1 ) == fisPInf);
      verify(isNInfinity(f1 ) == fisNInf);

      verify(isnan(      d1 ) == fisNan );
      verify(isinf(      d1 ) == fisInf );
      verify(isPInfinity(d1 ) == fisPInf);
      verify(isNInfinity(d1 ) == fisNInf);

      verify(isnan(      d80) == fisNan );
      verify(isinf(      d80) == fisInf );
      verify(isPInfinity(d80) == fisPInf);
      verify(isNInfinity(d80) == fisNInf);
    }

#pragma warning(disable : 4723) // Potential divide by 0
#pragma warning(disable : 4756) // overflow in constant arithmetic

    template<class T> void rationalNanTest(const T m) {
      T f = (T)sqrt(-1);
      verifyRationalNanTypes(f);

      f = m;
      f *= 2;
      verifyRationalNanTypes(f);

      f = m;
      f *= -2;
      verifyRationalNanTypes(f);

      f = 1;
      T g = 0;
      f /= g;
      verifyRationalNanTypes(f);

      f = -1;
      f /= g;
      verifyRationalNanTypes(f);

      f =  0;
      f /= g;
      verifyRationalNanTypes(f);
    }

    TEST_METHOD(RationalNaN) {
      try {
        rationalNanTest(FLT_MAX  );
        rationalNanTest(DBL_MAX  );
        rationalNanTest(DBL80_MAX);
      } catch(Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

    TEST_METHOD(RationalPowers) {
      try {
        const Rational b(46656,12167000);
        const Rational e(2,3);
        Rational p;
        verify(Rational::isRationalPow(b,e,&p));
        const double pd = pow(getDouble(b),getDouble(e));
        const double error = pd - getDouble(p);
        const double relError = fabs(error / pd);
        verify(relError < 1e-15);
      } catch(Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

  };
}
