#include "stdafx.h"
#include <time.h>
#include <ProcessTools.h>
#include <Math/BigReal/MRisprime.h>

static BigInt pow(int a, __int64 r) { // compute pow(a,r)
  BigInt p(1);
  BigInt tmpa(a);
  int i = 1;
  while(r != 0) {
    if((r % 2) == 1) {
      p *= tmpa;
      r--;
    } else {
      tmpa *= tmpa;
      r /= 2;
    }
  }
  return p;
}

bool LLisprime(INT64 p) { // Lucas-Lehmer primality test. is 2^p - 1 prime
  BigInt P(p);
  if(!MRisprime(P)) return false;
  BigInt s(4);
  BigInt n(pow(2,p)-1);
  for(UINT i = 2; i < p; i++) {
    s = (s*s - 2) % n;
  }
  return s == 0;
}

double _poly(int degree, double *p, double x) {
  double res = p[degree];
  for(int i = degree - 1; i >= 0; i--) {
    res = res * x + p[i];
  }
  return res;
}
static double _c[] = {
  +4.0241072885007365e+005,
  -1.1055302022908791e+003,
  +1.4973445739421787e+000,
  -4.7692676532712396e-004,
  +1.0289282737041579e-007,
  -5.5373859663653090e-012
};
double timeestimate(double x) {
  return _poly(5,_c,x);
}

int main(int argc, char **argv) {
  FILE *fdone = fopen("done.dat","r");
  __int64 i(2);
  if(fdone) {
    _ftscanf(fdone,_T("%I64d"),&i);
    fclose(fdone);
  }
  for(;; i++) {
    const double starttime = getProcessTime();
    const bool   prime     = LLisprime(i);
    const double endtime   = getProcessTime();
    FILE *tf = fopen("tm.dat","a");
    _ftprintf(tf,_T("%I64d %lf\n"),i,endtime-starttime);
    fclose(tf);

    _tprintf(_T("%I64d:%s processtime:%.3lf estimate:%.3lf\r")
            ,i,(prime ? _T("primtal") : _T("ikke primtal")),(endtime-starttime)/1000000,timeestimate((double)i)/1000000);
    if(prime) {
      FILE *f = FOPEN("mprimes.dat","a");
      _ftprintf(f,_T("i:%I64d\n"),i);
      fclose(f);
      f = FOPEN("mprimes1.dat","a");
      BigInt n(pow(2,i)-1);
      _ftprintf(f, _T("%s\n"), toString(n).cstr());
      fclose(f);
    }
    FILE *fdone = FOPEN("done.dat","w");
    _ftprintf( fdone,_T("%I64d\n"),i);
    fclose(fdone);
  }
  return 0;
}
