#include "stdafx.h"
#include <Math/Polynomial.h>

static void usage() {
  tcerr <<  _T("Usage: polyroot [options] -Cc0,c1...cn\n"
               "  Options:\n"
               "    -eeps : x is considered a root of p, if |p(x)| <= eps. Default is 1e-15\n"
               "    -v    : verbose. Print debug information.\n"
               "    -c    : Show coefficients for polynomial p.\n"
               "    -f    : Show p(x) for each root x.\n"
               "    -Cc0,c1...cn: Specify the coefficients for p.\n")
  ;
  exit(-1);
}

int main(int argc,char **argv) {
  char *cp;
  Polynomial *poly = NULL;
  Real        eps                 = 1e-15;
  bool        verbose             = false;
  bool        displayCoefficients = false;
  bool        displayValues       = false;

  for(argv++; *argv && ((*(cp = *argv) == '-')); argv++) {
    for(cp++;*cp;cp++) {
      switch(*cp) {
      case 'e':
        if(sscanf(cp+1,"%le",&eps) != 1)
          usage();
        if(eps <= 0) {
          tcerr << _T("Eps must be > 0. (=") << eps << _T(").") << endl;
          exit(-1);
        }
        break;
      case 'v':
        verbose = true;
        continue;
      case 'c':
        displayCoefficients = true;
        continue;
      case 'f':
        displayValues = true;
        continue;
      case 'C':
        { String tmp(cp+1);
          tistrstream s(tmp.cstr());
          Array<Complex> a;
          while(!s.eof() && !s.bad()) {
            Complex coef;
            TCHAR ch;
            s >> coef;
            a.add(coef);
            if(!s.eof()) {
              s >> ch;
              if(ch != ',')
                usage();
            }
          }
          if(s.bad()) {
            usage();
          }
          poly = new Polynomial(a);
        }
        break;
      default :
        usage();
      }
      break;
    }
  }

  if(poly == NULL) {
    _ftprintf(stderr, _T("No coefficients specified\n"));
    usage();
  }

  try {
    if(displayCoefficients) {
      tcout << *poly;
    }

    ComplexVector roots = poly->findRoots(verbose, eps);

    for(size_t i = 0; i < roots.getDimension(); i++ ) {
      tcout << StreamParameters(15,0,ios::scientific|ios::left|ios::showpos) << roots[i];
      if(displayValues) {
        tcout << _T("  f(x) = ") << (*poly)(roots[i]);
      }
      tcout << endl;
    }
  } catch(Exception e) {
    tcerr << _T("Exception:") << e.what() << endl;
    return -1;
  }
  return 0;
}

