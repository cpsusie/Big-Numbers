#include "stdafx.h"
#include <time.h>
#include <Math/MRisprime.h>

static BigInt pow(int a, __int64 r) { // compute pow(a,r)
  BigInt p(1);
  BigInt tmpa(a);
  int i = 1;
  while(r != 0) {
    if((r % 2) == 1) {
      p = p * tmpa;
      r--;
    } else {
      tmpa = tmpa * tmpa;
      r /= 2;
    }
  }
  return p;
}

bool LLisprime(INT64 p) { // Lucas-Lehmer primality test. is 2^p - 1 prime
  BigInt P(p);
  if(!MRisprime(GetCurrentThreadId(), P)) return false;
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
    fscanf(fdone,"%I64d",&i);
    fclose(fdone);
  }
  double starttime,endtime;
  for(;; i++) {
    starttime = getProcessTime();
    bool prime = LLisprime(i);
    endtime = getProcessTime();
    FILE *tf = fopen("tm.dat","a");
    fprintf(tf,"%I64d %lf\n",i,endtime-starttime);
    fclose(tf);

    printf("%I64d:%s processtime:%.3lf estimate:%.3lf\n",i,(prime ? "primtal\n" : "ikke primtal\n"),(endtime-starttime)/1000000,timeestimate((double)i)/1000000);
    if(prime) {
      FILE *f = fopen("mprimes.dat","a");
      fprintf(f,"i:%I64d\n",i);
      fclose(f);
      f = fopen("mprimes1.dat","a");
      BigInt n(pow(2,i)-1);
      n.print(f,false);
      fprintf(f,"\n");
      fclose(f);
    }
    FILE *fdone = fopen("done.dat","w");
    fprintf( fdone,"%I64d\n",i);
    fclose(fdone);
  }
  return 0;
}
