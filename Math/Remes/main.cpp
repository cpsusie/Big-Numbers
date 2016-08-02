// remes.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Remes2.h"
#include <fstream>
#include <String>
#include <math.h>
#include <Math/Double80.h>

/*
#include "gamma.h"

static Real targetfunc_gamma(const Real &x) {
  return getDouble(gamma80(x+1.5));
}
*/
static Real targetfunc_log(const Real &x) {
  return log(x+2);
}

static Real targetfunc_sqrt(const Real &x) {
  return sqrt(x+2);
}

static Real targetfunc_exp(const Real &x) {
  return exp(x);
}

static Real targetfunc_id(const Real &x) {
  return x;
}

class FunctionFromFunc : public Function {
private:
  Real (*m_f)(const Real &);

public:
  Real operator()(const Real &x) { return m_f(x); }
  FunctionFromFunc(Real (*f)(const Real &)) { m_f = f; }
};

// -----------------------------------------------------

static void usage() {
  _ftprintf(stderr,_T("Usage:remes [options]\n"
                      "  options:\n"
                      "    -mnumber : Degree of a default = 2\n"
                      "    -knumber : Degree of b default = 2\n"
                      "    -r       : Minimize relative error, ie abs(E(x)/f(x))\n"
                      "    -j       : Generate Java function\n"
                      "    -c       : Generate C++ function\n"
                      "    -p       : Generate error-plot\n"
                      "    -h       : High precision (80 bit doubles)\n"
                      "    -v       : Verbose\n")
           );
  exit(-1);
}

int main(int argc, char **argv) {
  char *cp;
  int  m               = 2;
  int  k               = 2;
  bool genCppFunction  = false;
  bool genJavaFunction = false;
  bool genPlot         = false;
  bool verbose         = false;
  bool relativeError   = false;
  bool highPrecision   = false;

  FPU::setPrecisionMode(FPU_HIGH_PRECISION);

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++;*cp; cp++) {
      switch(*cp) {
      case 'm': 
        m = atoi(cp+1); 
        if(m < 0 || m > 10) usage();
        break;
      case 'k': 
        k = atoi(cp+1); 
        if(k < 0 || k > 10) usage();
        break;
      case 'r':
        relativeError = true;
        continue;
      case 'c':
        genCppFunction = true;
        continue;
      case 'j':
        genJavaFunction  = true;
        continue;
      case 'p':
        genPlot = true;
        continue;
      case 'h':
        highPrecision = true;
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

  try {
//  RealFunctionFromFunc rf(targetfunc_log);
  //Remes rem(-0.5,0.5,rf,relativeError,_T("log"),verbose);
  //  remes rem(-1,1,targetfunc_sqrt,relativeError,_T("sqrt"),verbose);
  //  remes rem(-1,1,targetfunc_exp,relativeError,_T("exp"),verbose);

    FunctionFromFunc rf(targetfunc_log);
    Remes rem(-1,8,rf,relativeError,_T("log"),verbose);
  //  remes rem(-0.5,0.5,targetfunc_id,relativeError,_T("id"),verbose);
    rem.solve(m,k);
    rem.genOutput(tcout);
    if(genCppFunction) {
      FILE *f = FOPEN(format(_T("rem%02d%02d.cpp"),m,k).cstr(),_T("w"));
      rem.genFunction(f,highPrecision?CPP80:CPP);
      fclose(f);
    }
    if(genJavaFunction) {
      FILE *f = FOPEN(format(_T("rem%02d%02d.java"),m,k).cstr(),_T("w"));
      rem.genFunction(f,JAVA);
      fclose(f);
    }
    if(genPlot) {
      tofstream s(format(_T("rem%02d%02d.plo"),m,k).cstr());
      rem.genPlot(s);
    }
  } catch(Exception e) {
    _tprintf(_T("Exception:%s\n"),e.what());
    return -1;
  }

  return 0;
}
