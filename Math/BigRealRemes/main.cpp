#include "stdafx.h"
#include "BigRealRemes2.h"
#include <fstream>
#include <math.h>

// -------------------------------------------------------------------------

class LogFunction : public RemesTargetFunction {
private:
  UINT                          m_digits;
  const NumberInterval<BigReal> m_interval;
public:
  BigReal operator()(const BigReal &x);

  String getName() const {
    return _T("lnxp2");
  }

  const NumberInterval<BigReal> &getInterval() const {
    return m_interval;
  }

  int getDigits() const {
    return m_digits;
  }

  LogFunction(UINT digits) : m_interval(-1,8) {
    m_digits = digits;
  }
};

BigReal LogFunction::operator()(const BigReal &x) {
  BigReal result = rLn(x+2,m_digits);
  return result;
}

// -------------------------------------------------------------------------

class ExpFunction : public RemesTargetFunction {
private:
  UINT                          m_digits;
  const NumberInterval<BigReal> m_interval;
public:
  BigReal operator()(const BigReal &x);

  String getName() const {
    return _T("exp");
  }

  const NumberInterval<BigReal> &getInterval() const {
    return m_interval;
  }

  int getDigits() const {
    return m_digits;
  }

  ExpFunction(UINT digits) : m_interval(0,0.5) {
    m_digits = digits;
  }
};

BigReal ExpFunction::operator()(const BigReal &x) {
  BigReal result = rExp(x,m_digits);
  return result;
}

// -------------------------------------------------------------------------

class RatioanalFunction1 : public RemesTargetFunction {
private:
  UINT                          m_digits;
  const NumberInterval<BigReal> m_interval;
public:
  BigReal operator()(const BigReal &x);

  String getName() const {
    return _T("Rational1");
  }

  const NumberInterval<BigReal> &getInterval() const {
    return m_interval;
  }

  int getDigits() const {
    return m_digits;
  }

  RatioanalFunction1(UINT digits) : m_interval(-1,1) {
    m_digits = digits;
  }
};

BigReal RatioanalFunction1::operator()(const BigReal &x) {
  static const BigReal c1(0.5);
  static const BigReal c2(2);
  static const BigReal c3(3);

  BigReal x2 = rProd(x,x,m_digits);
  BigReal x3 = rProd(x,x2,m_digits);
  BigReal p  = c1*x3 - x2 - x + c3;
  BigReal q  = c1*x3 + x2 - c2*x + BIGREAL_1;
  BigReal result = rQuot(p, rSqrt(q,m_digits), m_digits);

  return result;
}

// -------------------------------------------------------------------------
/*
BigReal RemesFunction::operator()(const BigReal &x) {
  return sqrt(x+2);
}

BigReal RemesFunction::operator()(const BigReal &x) {
  return x;
}
*/

// -----------------------------------------------------

static void doJob(Remes &remes, int m, int k, bool genCFunction, bool useDouble80, bool genJavaFunction, bool genPlot) {
  try {
    remes.solve(m,k);
  } catch(Exception e) {
    if(remes.getMaxError() == 0) {
      _tprintf(_T("\nThis seems to be a rational function...\n"));
    } else {
      throw;
    }
  }

  String name = remes.getTargetFunction().getName();

  if(genCFunction) {
    FILE *f = FOPEN(format(_T("%s%02d%02d.cpp"),name.cstr(),m,k).cstr(),_T("w"));
    remes.generateCFunction(f,useDouble80);
    fclose(f);
  }
  if(genJavaFunction) {
    FILE *f = FOPEN(format(_T("%s%02d%02d.java"),name.cstr(),m,k).cstr(),_T("w"));
    remes.generateJavaFunction(f);
    fclose(f);
  }
  if(genPlot) {
    FILE *f = FOPEN(format(_T("%s%02d%02d.plot"),name.cstr(),m,k).cstr(),_T("w"));
    remes.generatePlot(f);
    fclose(f);
  }
}

static void usage() {
  _ftprintf(stderr,_T("Usage:remes [options]\n"
                      "  Options:\n"
                      "    -mfrom[-to] : Degree of a. Default m = 2.\n"
                      "                  If to is specified, m will run through the interval [from..to]\n"
                      "    -kfrom[-to] : Degree of b. Default k = 2.\n"
                      "                  If to is specified, k will run through the interval [from..to]\n"
                      "    -ddigits    : BigReal of digits in function-evalutaion. Default = 40.\n"
                      "    -r          : Minimize relative error, ie abs(E(x)/f(x)).\n"
                      "    -c[80]      : Generate C-function. If 80 specified, use Double80.\n"
                      "    -j          : Generate Java-function.\n"
                      "    -p          : Generate error-plot.\n"
                      "    -v          : Verbose.\n")
           );
  exit(-1);
}

int _tmain(int argc, TCHAR **argv) {
  TCHAR *cp;
  IntInterval M,K;
  int from,to;
  bool genCFunction    = false;
  bool genJavaFunction = false;
  bool useDouble80     = false;
  bool genPlot         = false;
  bool verbose         = false;
  bool relativeError   = false;
  int  digits          = 40;

  M.setFrom(2).setTo(2);
  K.setFrom(2).setTo(2);

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++;*cp; cp++) {
      switch(*cp) {
      case 'm':
        if(_stscanf(cp+1,_T("%u-%u"), &from,&to) != 2) {
          if(_stscanf(cp+1,_T("%u"), &from) != 1) {
            usage();
          }
          to = from;
        }
        if(from > to) {
          _ftprintf(stderr,_T("Illegal interval [%d-%d]\n"),from,to);
          exit(-1);
        }
        if(from < 1 || to > 20) {
          usage();
        }
        M.setFrom(from).setTo(to);
        break;

      case 'k':
        if(_stscanf(cp+1, _T("%u-%u"), &from,&to) != 2) {
          if(_stscanf(cp+1, _T("%u"), &from) != 1) {
            usage();
          }
          to = from;
        }
        if(from > to) {
          _ftprintf(stderr,_T("Illegal interval [%d-%d]\n"), from, to);
          exit(-1);
        }
        if(from < 0 || to > 20) {
          usage();
        }
        K.setFrom(from).setTo(to);
        break;

      case 'd':
        if(_stscanf(cp+1,_T("%u"), &digits) != 1) {
          usage();
        }
        break;

      case 'r':
        relativeError = true;
        continue;

      case 'c':
        genCFunction = true;
        if(_tcscmp(cp+1,_T("80")) == 0) {
          useDouble80 = true;
          break;
        }
        continue;

      case 'j':
        genJavaFunction = true;
        continue;

      case 'p':
        genPlot = true;
        continue;

      case 'v':
        verbose = true;
        continue;

      default:
        usage();
      }
      break;
    }
  }

  LogFunction f(digits);
//  ExpFunction f(digits);
//  RatioanalFunction1 f(digits);
  Remes remes(f,relativeError,verbose);
  remes.loadExtremaFromFile();

  try {
    for(int m = M.getFrom(); m <= M.getTo(); m++) {
      for(int k = K.getFrom(); k <= K.getTo(); k++) {
        doJob(remes, m, k, genCFunction, useDouble80, genJavaFunction, genPlot);
      }
    }
  } catch(Exception e) {
    _tprintf(_T("Exception:%s\n"), e.what());
    return -1;
  }
  return 0;
}
