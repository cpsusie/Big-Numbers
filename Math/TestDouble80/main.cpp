#include "stdafx.h"
#include <ostream>
#include <Console.h>
#include <Math.h>
#include <float.h>
#include <limits.h>
#include <Math/Double80.h>
#include "TestDouble80.h"

template <class T> class TestFloat {

  String format(float x) {
    return ::format(_T("%+.17e"),x);
  }

  String format(double x) {
    return ::format(_T("%+.17le"),x);
  }

  String format(const Double80 &x) {
    return x.toString();
  }

  String m_typeName;

public:
  TestFloat(const char *typeName) {
    m_typeName = typeName;
  }

  void test(double a,double b) {
    T x      = (T)a;
    T y      = (T)b;
    T sum    = x + y;
    T dif    = x - y;
    T prod   = x * y;
    T quot   = x / y;
    T minus  = -x;

    T xaddy  = x;
    xaddy += y;
    T xsuby  = x;
    xsuby -= y;
    T xprody = x;
    xprody *= y;
    T xdivy  = x;
    xdivy /= y;

    _tprintf(_T("type:<%s>\n"),m_typeName.cstr());
    _tprintf(_T("x:%s\n"),format(x).cstr());
    _tprintf(_T("a:%.17lf\n"),a);

    _tprintf(_T("x          :%s y           :%s\n"),format(x).cstr()          ,format(y).cstr());
    _tprintf(_T("x+y        :%s x-y         :%s\n"),format(sum).cstr()        ,format(dif).cstr());
    _tprintf(_T("x*y        :%s x/y         :%s\n"),format(prod).cstr()       ,format(quot).cstr());
    _tprintf(_T("-x         :%s fmod(x,y)   :%s\n"),format(minus).cstr()      ,format(fmod(x,y)).cstr());
    _tprintf(_T("x+=y       :%s x-=y        :%s\n"),format(xaddy).cstr()      ,format(xsuby).cstr());
    _tprintf(_T("x*=y       :%s x/=y        :%s\n"),format(xprody).cstr()     ,format(xdivy).cstr());
    _tprintf(_T("floor(x)   :%s ceil(x)     :%s\n"),format(floor(x)).cstr()   ,format(ceil(x)).cstr());
    _tprintf(_T("abs(x)     :%s sqrt(x)     :%s\n"),format(fabs(x)).cstr()    ,format(sqrt(x)).cstr());
    _tprintf(_T("sin(x)     :%s cos(x)      :%s\n"),format(sin(x)).cstr()     ,format(cos(x)).cstr());
    _tprintf(_T("asin(x)    :%s acos(x)     :%s\n"),format(asin(x)).cstr()    ,format(acos(x)).cstr());
    _tprintf(_T("tan(x)     :%s atan(x)     :%s\n"),format(tan(x)).cstr()     ,format(atan(x)).cstr());
    _tprintf(_T("atan2(x,y) :%s                \n"),format(atan2(x,y)).cstr());
    _tprintf(_T("exp(x)     :%s pow(x,y)    :%s\n"),format(exp(x)).cstr()     ,format(pow(x,y)).cstr());
    _tprintf(_T("log(x)     :%s log10(x)    :%s\n"),format(log(x)).cstr()     ,format(log10(x)).cstr());
    _tprintf(_T("round(x,5) :%s round(x,-5) :%s\n"),format(round(x,5)).cstr() ,format(round(x,-5)).cstr());
    _tprintf(_T("fraction(x):%s fraction(y) :%s\n"),format(fraction(x)).cstr(),format(fraction(y)).cstr());
    _tprintf(_T("sign(x)    :%d                \n"),sign(x));
          
    if(x <  y) _tprintf(_T("x < y\n"));
    if(x <= y) _tprintf(_T("x <= y\n"));
    if(x >  y) _tprintf(_T("x > y\n"));
    if(x >= y) _tprintf(_T("x >= y\n"));
    if(x == y) _tprintf(_T("x == y\n"));
    if(x != y) _tprintf(_T("x != y\n"));
    fflush(stdout);
    tcout << _T("x:") << x << _T("\n") << _T("y:") << y << _T("\n");
    tcout.precision(4); tcout.flags(std::ios::scientific | std::ios::uppercase);
    tcout << _T("x:") << x << _T("\n") << _T("y:") << y << _T("\n");
    tcout.flush();
  }
};

static void testio() {
  for(;;) {
    Double80 x;
    tcout << _T("Indtast tal:");
    tcin >> x;
//  Double80 x = "1.1234567890123456789e-006";
    double   y = getDouble(x);
    tcout << "1234567890123456789012345678901234567890123456789012345678901234567890\n";
    for(int prec = 0; prec <= 20; prec++) {
      tcout << "prec:";
      tcout.flags(std::ios::right);
      tcout.width(3);
      tcout << prec << ":";

      tcout << "Double80:";
      tcout.precision(prec); 
  //    tcout.flags(std::ios::fixed | std::ios::left);
      tcout.flags(std::ios::scientific | std::ios::left | std::ios::uppercase);
      tcout.width(24);
      tcout << x;

      tcout << " double:";
      tcout.precision(prec); 
  //    tcout.flags(std::ios::fixed | std::ios::left);
      tcout.flags(std::ios::scientific | std::ios::left | std::ios::uppercase);
      tcout.width(24);
      tcout << y;

      tcout << "\n";
      tcout.flush();

    }
  }
}

int main(int argc, char **argv) {
  try {
    testDouble80();
  } catch(Exception e) {
    if(FPU::stackOverflow()) {
      _tprintf(_T("fpu-stack overflow\n"));
    } else if(FPU::stackUnderflow()) {
      _tprintf(_T("fpu-stack underflow\n"));
    } else {
      _tprintf(_T("%s\n"),e.what());
    }
    return -1;
  }
  return 0;

  int it = 0;
  FPU::setPrecisionMode(FPU_HIGH_PRECISION);
  FPU::enableExceptions(true,FPU_OVERFLOW_EXCEPTION | FPU_UNDERFLOW_EXCEPTION);

  Double80 one = 1;
  Double80 onePlusEps = one + Double80::DBL80_EPSILON;
  Double80 diff = onePlusEps - one;
  tcout << diff << "\n";

  tcout.precision(19);
  tcout << "dbl80_Eps:" << Double80::DBL80_EPSILON.toString() << std::endl;
  tcout.precision(19);
  tcout << "dbl80_Max:" << Double80::DBL80_MAX.toString() << std::endl;
  tcout.precision(19);
  tcout << "dbl80_Min:" << Double80::DBL80_MIN.toString() << std::endl;
  tcout.flush();
//  exit(0);


//  Double80::FPUEnableExceptions(true,FPU_INVALID_OPERATION_EXCEPTION);

  double   x   = 2;
  Double80 x80 = 2;

#ifdef TESTPRECISION

  for(;;) {
    _tprintf("it:%2d:x:%.16lem,x80:%s\n",it++,x,x80.toString().cstr());
    if(x80 == 1) 
      break;
    x   = (1+x)/2;
    x80 = (1+x80)/2;
  }

  x80 = 1;
  x = 1;
  for(it = 0;;it++) {
    _tprintf("it:%2d:x:%.16lem,x80:%s\n",it++,x,x80.toString().cstr());
    x80 /= 2;
    x /= 2;
  }
#endif

#ifdef TESTTIME
  String str;
  x80 = _I64_MAX;
  str = x80.toString();
  unsigned __int64 ui64 = getUint64(x80);
  __int64           i64 = getInt64(x80);
  if(ui64 != _I64_MAX)
    _tprintf("fejl");

  x80  = _UI64_MAX;
  str  = x80.toString();
  ui64 = getUint64(x80);
  if(ui64 != _UI64_MAX)
    _tprintf("fejl");

  x80 = _I64_MIN;
  str = x80.toString();
  i64 = getInt64(x80);
  if(i64 != _I64_MIN)
    _tprintf("fejl");

  x   = 2394786.234e-40;
  x80 = x;
  str = x80.toString();

  double startTime = getProcessTime();
  for(it = 0; it < 100000; it++)
    String s = x80.toString();
  _tprintf("Double80::toString:%lf\n",getProcessTime()-startTime);

  startTime = getProcessTime();
  for(it = 0; it < 100000; it++) {
    int dec,sign;
    char *s = ecvt(x,18,&dec,&sign);
  }
  _tprintf("ecvt:%lf\n",getProcessTime()-startTime);
#endif

  for(;;) {
    try {
      _tprintf(_T("FPU-stackHeight:%d\n"),FPU::getStackHeight());
      FPU::clearStatusWord();
      _tprintf(_T("Enter x y:"));
      TCHAR line[100];
      GETS(line);
      double a,b;
      if(_stscanf(line,_T("%lf %lf"),&a,&b) != 2) {
        _tprintf(_T("Illegal input"));
        continue;
      }
      TestFloat<float> t1("float");
      t1.test(a,b);
      TestFloat<double> t2("double");
      t2.test(a,b);
      TestFloat<Double80> t3("Double80");
      t3.test(a,b);

    } catch(Exception e) {
      if(FPU::stackOverflow()) {
        _tprintf(_T("fpu-stack overflow\n"));
      } else if(FPU::stackUnderflow()) {
        _tprintf(_T("fpu-stack underflow\n"));
      } else {
        _tprintf(_T("%s\n"), e.what());
      }
    }
  }

  return 0;
}
